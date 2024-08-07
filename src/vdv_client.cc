#include "vdv/vdv_client.h"

#include "net/http/client/http_client.h"
#include "net/http/client/request.h"
#include "net/http/client/response.h"
#include "net/run.h"
#include "net/web_server/query_router.h"
#include "net/web_server/responses.h"
#include "net/web_server/web_server.h"

#include "nigiri/rt/vdv/vdv_update.h"

#include "vdv/vdv_config.h"
#include "vdv/xml_in.h"
#include "vdv/xml_out.h"

namespace vdv {

namespace http = boost::beast::http;
namespace nhc = net::http::client;
using namespace net;

auto const vdv_headers = std::map<std::string, std::string>{
    {"Content-Type", "text/xml"}, {"Accept", "text/xml"}};

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

vdv_client::vdv_client(vdv_config& cfg, boost::asio::io_context& ioc)
    : cfg_{cfg}, ioc_{ioc}, start_{std::chrono::system_clock::now()} {}

void vdv_client::run() {
  auto s = net::web_server{ioc_};
  auto qr =
      net::query_router{}
          .route("POST", cfg_.client_status_path_,
                 [&](net::route_request const& req,
                     net::web_server::http_res_cb_t const& res_cb, bool) {
                   std::cout << "client status request:\n" << req << "\n\n";
                   try {
                     auto const msg_in = parse(req.body());
                     if (holds_alternative<client_status_anfrage_msg>(msg_in)) {
                       auto const res_body = client_status_antwort_xml_str(
                           std::chrono::system_clock::now(), true, start_);
                       std::cout << "client status response:\n"
                                 << res_body << "\n\n";
                       res_cb(string_response(req, res_body, http::status::ok,
                                              "text/xml"));
                     }
                   } catch (std::runtime_error const& e) {
                     res_cb(net::bad_request_response(req));
                     std::cout << "Received bad request on client status "
                                  "endpoint:\n"
                               << e.what() << req << "\n";
                   }
                 })
          .route("POST", cfg_.data_ready_path_,
                 [](net::route_request const& req,
                    net::web_server::http_res_cb_t const& res_cb, bool) {
                   std::cout << "data ready request:\n" << req << "\n\n";
                   try {
                     auto const msg_in = parse(req.body());
                     if (holds_alternative<daten_bereit_anfrage_msg>(msg_in)) {
                       auto const res_body = daten_bereit_antwort_xml_str(
                           std::chrono::system_clock::now(), true, 0);
                       std::cout << "data ready response:\n"
                                 << res_body << "\n";
                       res_cb(string_response(req, res_body, http::status::ok,
                                              "text/xml"));
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
  s.init(cfg_.client_ip_, cfg_.client_port_, ec);
  s.run();
  if (ec) {
    std::cerr << "error: " << ec << "\n";
    return;
  }
  std::cout << "listening on " << cfg_.client_ip_ << ":" << cfg_.client_port_
            << "\n";
  ioc_.run();
}

void vdv_client::subscribe(sys_time start,
                           sys_time end,
                           std::chrono::seconds hysteresis,
                           std::chrono::minutes look_ahead) {
  unsubscribe();

  auto const req_body = abo_anfrage_xml_str(cfg_.client_name_, start, end, 1,
                                            hysteresis, look_ahead);
  std::cout << "subscription request:\n" << req_body << "\n\n";
  auto req = nhc::request(cfg_.manage_sub_addr_, nhc::request::method::POST,
                          vdv_headers, req_body);
  make_http(ioc_, cfg_.manage_sub_addr_)
      ->query(req, [&]([[maybe_unused]] auto&& shrd_ptr, auto&& r,
                       [[maybe_unused]] auto&& ec) {
        std::cout << "subscription response:\n" << r << "\n\n";
        if (r.status_code == 200) {
          try {
            auto const msg_in = parse(r.body);
            if (holds_alternative<abo_antwort_msg>(msg_in) &&
                get<abo_antwort_msg>(msg_in).success_) {
              std::cout << "--> successfully subscribed\n\n";
            }
          } catch (std::runtime_error const& e) {
            std::cout << e.what() << "\n";
          }
        }
      });
}

void vdv_client::unsubscribe() {
  auto const req_body = abo_loeschen_anfrage_xml_str(
      cfg_.client_name_, std::chrono::system_clock::now());
  std::cout << "cancel sub request:\n" << req_body << "\n\n";
  auto req = nhc::request(cfg_.manage_sub_addr_, nhc::request::method::POST,
                          vdv_headers, req_body);
  make_http(ioc_, cfg_.manage_sub_addr_)
      ->query(req, [&]([[maybe_unused]] auto&& shrd_ptr, auto&& r,
                       [[maybe_unused]] auto&& ec) {
        std::cout << "cancel sub response:\n" << r << "\n\n";
        if (r.status_code == 200) {
          try {
            auto const msg_in = parse(r.body);
            if (holds_alternative<abo_antwort_msg>(msg_in) &&
                get<abo_antwort_msg>(msg_in).success_) {
              std::cout << "--> successfully unsubscribed\n\n";
            }
          } catch (std::runtime_error const& e) {
            std::cout << e.what() << "\n";
          }
        }
      });
}

nigiri::rt::vdv::statistics vdv_client::fetch() {
  auto const req_body = daten_abrufen_anfrage_xml_str(
      cfg_.client_name_, std::chrono::system_clock::now(), false);
  auto req = nhc::request(cfg_.fetch_data_addr_, nhc::request::method::POST,
                          vdv_headers, req_body);
  std::cout << "fetch data request:\n" << req_body << "\n\n";
  auto stats = nigiri::rt::vdv::statistics{};
  make_http(ioc_, cfg_.fetch_data_addr_)
      ->query(req, [&]([[maybe_unused]] auto&& shrd_ptr, auto&& r,
                       [[maybe_unused]] auto&& ec) {
        std::cout << "fetch data response:\n" << r << "\n" << std::endl;
        if (r.status_code == 200) {
          try {
            auto doc = pugi::xml_document{};
            doc.load_string(r.body.c_str());
            if (doc.select_node("DatenAbrufenAntwort/AUSNachricht")) {
              stats = nigiri::rt::vdv::vdv_update(*cfg_.tt_, *cfg_.rtt_,
                                                  cfg_.src_idx_, doc);
            }
          } catch (std::runtime_error const& e) {
            std::cout << e.what() << "\n";
          }
        }
      });
  return stats;
}

void vdv_client::check_server_status() {
  auto const req_body = status_anfrage_xml_str(
      cfg_.client_name_, std::chrono::system_clock::now());
  std::cout << "status request:\n" << req_body << "\n\n";
  auto req = nhc::request(cfg_.status_addr_, nhc::request::method::POST,
                          vdv_headers, req_body);
  make_http(ioc_, cfg_.status_addr_)
      ->query(req, [&]([[maybe_unused]] auto&& shrd_ptr, auto&& r,
                       [[maybe_unused]] auto&& ec) {
        std::cout << "status response:\n" << r << "\n\n";
      });
}

}  // namespace vdv
