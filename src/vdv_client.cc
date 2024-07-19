#include "vdv/vdv_client.h"

#include "net/run.h"
#include "net/web_server/query_router.h"
#include "net/web_server/responses.h"
#include "net/web_server/web_server.h"

#include "nigiri/rt/vdv/vdv_update.h"

#include "vdv/xml_in.h"
#include "vdv/xml_out.h"

namespace vdv {

namespace http = boost::beast::http;
namespace nhc = net::http::client;
using namespace net;

nhc::url get_endpoint_addr(std::string_view server_addr,
                           std::string_view client_name,
                           std::string_view path) {
  auto ss = std::stringstream{};
  ss << server_addr << "/" << client_name << "/" << path;
  return {ss.str()};
}

std::string get_endpoint_path(std::string_view server_name,
                              std::string_view request) {
  auto ss = std::stringstream{};
  ss << "/" << server_name << "/" << request;
  return ss.str();
}

std::ostream& operator<<(std::ostream& out, net::route_request const& rr) {
  out << rr.body() << "\n";
  return out;
}

std::ostream& operator<<(std::ostream& out, nhc::response const& r) {
  out << "STATUS " << r.status_code << "\n";
  for (auto const& [k, v] : r.headers) {
    out << k << ": " << v << "\n";
  }
  out << "\n" << r.body << "\n\n";
  return out;
}

vdv_client::vdv_client(std::string_view client_name,
                       std::string_view client_ip,
                       std::string_view client_port,
                       std::string_view server_name,
                       nhc::url const& server_addr,
                       nigiri::timetable const* tt,
                       nigiri::rt_timetable* rtt,
                       nigiri::source_idx_t const src_idx)
    : client_name_{client_name},
      client_ip_{client_ip},
      client_port_{client_port},
      server_name_{server_name},
      server_addr_{server_addr},
      status_addr_{
          get_endpoint_addr(server_addr.str(), client_name_, "aus/status.xml")},
      manage_sub_addr_{get_endpoint_addr(
          server_addr.str(), client_name_, "aus/aboverwalten.xml")},
      fetch_data_addr_{get_endpoint_addr(
          server_addr.str(), client_name_, "aus/datenabrufen.xml")},
      client_status_path_{
          get_endpoint_path(server_name, "aus/clientstatus.xml")},
      data_ready_path_{get_endpoint_path(server_name, "aus/datenbereit.xml")},
      start_{std::chrono::system_clock::now()},
      tt_{tt},
      rtt_{rtt},
      src_idx_{src_idx},
      http_client_{make_http(ioc_, server_addr_)} {}

void vdv_client::run() {
  auto s = net::web_server{ioc_};
  auto qr =
      net::query_router{}
          .route("POST", client_status_path_,
                 [&](net::route_request const& req,
                     net::web_server::http_res_cb_t const& res_cb, bool) {
                   try {
                     auto const msg_in = parse(req.body());
                     if (holds_alternative<client_status_anfrage_msg>(msg_in)) {
                       res_cb(string_response(
                           req,
                           client_status_antwort_xml_str(
                               std::chrono::system_clock::now(), true, start_),
                           http::status::ok, "text/xml"));
                       std::cout
                           << "Received request on client status endpoint, "
                              "response: OK\n";
                     } else {
                       res_cb(net::bad_request_response(req));
                       std::cout << "Received bad request on client status "
                                    "endpoint:\n"
                                 << req << "\n";
                     }
                   } catch (std::runtime_error const& e) {
                     res_cb(net::bad_request_response(req));
                     std::cout << "Received bad request on client status "
                                  "endpoint:\n"
                               << e.what() << req << "\n";
                   }
                 })
          .route("POST", data_ready_path_,
                 [this](net::route_request const& req,
                        net::web_server::http_res_cb_t const& res_cb, bool) {
                   try {
                     auto const msg_in = parse(req.body());
                     if (holds_alternative<daten_bereit_anfrage_msg>(msg_in)) {
                       res_cb(string_response(
                           req,
                           daten_bereit_antwort_xml_str(
                               std::chrono::system_clock::now(), true, 0),
                           http::status::ok, "text/xml"));
                       std::cout << "Received request on data ready endpoint, "
                                    "fetching...\n";
                       fetch();
                     } else {
                       res_cb(net::bad_request_response(req));
                       std::cout << "Received bad request on data ready "
                                    "endpoint:\n"
                                 << req << "\n";
                     }
                   } catch (std::runtime_error const& e) {
                     res_cb(net::bad_request_response(req));
                     std::cout << "Received bad request on data ready "
                                  "endpoint:\n"
                               << e.what() << req << "\n";
                   }
                 });
  qr.enable_cors();
  s.on_http_request(std::move(qr));

  auto ec = boost::system::error_code{};
  s.init(client_ip_, client_port_, ec);
  s.run();
  if (ec) {
    std::cerr << "error: " << ec << "\n";
    return;
  }
  std::cout << "listening on " << client_ip_ << ":" << client_port_ << "\n";

  ioc_.run();
}

void vdv_client::subscribe(sys_time start,
                           sys_time end,
                           std::chrono::seconds hysteresis,
                           std::chrono::minutes look_ahead) {
  clean_up_subs();

  std::cout << "sending subscription request to " << manage_sub_addr_ << ":";
  auto const id = next_id_++;
  auto req = nhc::request(manage_sub_addr_, nhc::request::method::POST,
                          {{"Content-Type", "text/xml"}},
                          abo_anfrage_xml_str(client_name_, start, end, id,
                                              hysteresis, look_ahead));
  auto const client = make_http(ioc_, server_addr_);
  client->query(req, [&]([[maybe_unused]] auto&& shrd_ptr, auto&& r,
                         [[maybe_unused]] auto&& ec) {
    if (r.status_code == 200) {
      try {
        auto const msg_in = parse(r.body);
        if (holds_alternative<abo_antwort_msg>(msg_in) &&
            get<abo_antwort_msg>(msg_in).success_) {
          std::cout << "successfully subscribed\n" << std::endl;
          auto const lock = std::scoped_lock{subs_mutex_};
          subs_.emplace_front(id, start, end, hysteresis, look_ahead);
        } else {
          std::cout << "invalid response body to subscription request:\n"
                    << r << std::endl;
        }
      } catch (std::runtime_error const& e) {
        std::cout << "invalid response to subscription request: " << e.what()
                  << ", response: " << r << std::endl;
      }
    } else {
      std::cout << "subscription failed, server replied:\n" << r;
    }
  });
  std::cout << "Client subs are: ";
  for (auto const& sub : subs_) {
    std::cout << sub.id_ << " ";
  }
  std::cout << std::endl;
}

void vdv_client::clean_up_subs() {
  auto const lock = std::scoped_lock{subs_mutex_};
  std::erase_if(subs_, [](auto&& sub) { return sub.is_stale(); });
}

void vdv_client::fetch() {
  auto req =
      nhc::request(fetch_data_addr_, nhc::request::method::POST,
                   {{"Content-Type", "text/xml"}},
                   daten_abrufen_anfrage_xml_str(
                       client_name_, std::chrono::system_clock::now(), false));
  auto const client = make_http(ioc_, server_addr_);
  client->query(req, [&]([[maybe_unused]] auto&& shrd_ptr, auto&& r,
                         [[maybe_unused]] auto&& ec) {
    if (r.status_code == 200) {
      auto doc = pugi::xml_document{};
      doc.load_string(r.body.c_str());
      if (doc.select_node("DatenAbrufenAntwort/AUSNachricht")) {
        std::cout << "successfully fetched data\n";
        // nigiri::rt::vdv::vdv_update(*tt_, *rtt_, src_idx_, doc);
      }
    } else {
      std::cout << "data fetch failed, server replied:\n" << r;
    }
  });
}

void vdv_client::check_server_status() {
  std::cout << "sending status request to " << status_addr_ << "\n";
  auto req = nhc::request(
      status_addr_, nhc::request::method::POST, {{"Content-Type", "text/xml"}},
      status_anfrage_xml_str(client_name_, std::chrono::system_clock::now()));
  auto const client = make_http(ioc_, server_addr_);
  client->query(req, [&]([[maybe_unused]] auto&& shrd_ptr, auto&& r,
                         [[maybe_unused]] auto&& ec) {
    if (r.status_code == 200) {
      auto doc = pugi::xml_document{};
      doc.load_string(r.body.c_str());
      if (doc.select_node("StatusAntwort")) {
        try {
          auto msg_in = parse(r.body);
          if (holds_alternative<status_antwort_msg>(msg_in)) {
            auto const& status_msg = get<status_antwort_msg>(msg_in);
            if (status_msg.success_) {
              std::cout << "server check: OK" << std::endl;
              if (status_msg.data_rdy_) {
                std::cout << ", data ready, fetching...";
                fetch();
              }
              std::cout << std::endl;
            } else {
              std::cout << "server check: BAD\n";
            }
          }
        } catch (std::runtime_error const& e) {
          std::cout << "server status check failed: " << e.what()
                    << ",server replied:\n"
                    << r;
        }
      }
    } else {
      std::cout << "server status check failed, server replied:\n" << r;
    }
  });
}

}  // namespace vdv
