#include "vdv/vdv_client.h"

#include "net/run.h"
#include "net/web_server/query_router.h"
#include "net/web_server/responses.h"
#include "net/web_server/web_server.h"

#include "nigiri/rt/vdv/vdv_update.h"

#include "vdv/xml_in.h"
#include "vdv/xml_out.h"

namespace vdv {

using namespace net;
using namespace net::http::client;

url get_endpoint_addr(std::string_view server_addr,
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

web_server::string_res_t xml_response(
    web_server::http_req_t const& req,
    std::string_view text,
    boost::beast::http::status status = boost::beast::http::status::ok,
    std::string_view content_type = "text/xml");

std::ostream& operator<<(std::ostream& out, net::route_request const& rr) {
  out << rr.body() << "\n";
  return out;
}

std::ostream& operator<<(std::ostream& out, response const& r) {
  out << "STATUS " << r.status_code << "\n";
  for (auto const& [k, v] : r.headers) {
    out << k << ": " << v << "\n";
  }
  out << "\n" << r.body << "\n\n";
  return out;
}

vdv_client::vdv_client(std::string_view client_name,
                       std::string_view client_port,
                       std::string_view server_name,
                       url const& server_addr,
                       std::chrono::seconds const hysteresis,
                       std::chrono::minutes const look_ahead,
                       nigiri::timetable const* tt,
                       nigiri::rt_timetable* rtt,
                       nigiri::source_idx_t const src_idx)
    : client_name_{client_name},
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
      look_ahead_{look_ahead},
      hysteresis_{hysteresis},
      tt_{tt},
      rtt_{rtt},
      src_idx_{src_idx},
      http_client_{make_http(ioc_, server_addr_)} {}

void vdv_client::fetch() {
  auto req = net::http::client::request(
      fetch_data_addr_, net::http::client::request::method::POST,
      {{"Content-Type", "text/xml"}},
      daten_abrufen_anfrage_xml_str(client_name_,
                                    std::chrono::system_clock::now(), false));
  http_client_->query(req, [&]([[maybe_unused]] auto&& shrd_ptr, auto&& r,
                               [[maybe_unused]] auto&& ec) {
    if (r.status_code == 200) {
      auto doc = pugi::xml_document{};
      [[maybe_unused]] auto result = doc.load_string(r.body.c_str());
      if (doc.select_node("DatenAbrufenAntwort/AUSNachricht")) {
        nigiri::rt::vdv::vdv_update(*tt_, *rtt_, src_idx_, doc);
      }
    } else {
      std::cout << "data fetch failed, server replied:\n" << r;
    }
  });
}

void vdv_client::run() {
  auto const xml =
      vdv::abo_anfrage_xml_str(client_name_, std::chrono::system_clock::now(),
                               1, hysteresis_, look_ahead_);

  auto s = net::web_server{ioc_};
  auto qr =
      net::query_router{}
          .route("POST", client_status_path_,
                 [&](net::route_request const& req,
                     net::web_server::http_res_cb_t res_cb, bool) {
                   auto const msg_in = parse(req.body());
                   if (holds_alternative<client_status_anfrage_msg>(msg_in)) {
                     res_cb(xml_response(
                         req,
                         client_status_antwort_xml_str(
                             std::chrono::system_clock::now(), true, start_)));
                     std::cout << "Received request on client status endpoint, "
                                  "response: OK\n";
                   } else {
                     res_cb(net::bad_request_response(req));
                     std::cout << "Received bad request on client status "
                                  "endpoint:\n"
                               << req << "\n";
                   }
                 })
          .route("POST", data_ready_path_,
                 [this](net::route_request const& req,
                        net::web_server::http_res_cb_t res_cb, bool) {
                   auto const msg_in = parse(req.body());
                   if (holds_alternative<daten_bereit_anfrage_msg>(msg_in)) {
                     res_cb(xml_response(
                         req, daten_bereit_antwort_xml_str(
                                  std::chrono::system_clock::now(), true, 0)));
                     std::cout << "Received request on data ready endpoint, "
                                  "fetching...\n";
                     fetch();
                   } else {
                     res_cb(net::bad_request_response(req));
                     std::cout << "Received bad request on data ready "
                                  "endpoint:\n"
                               << req << "\n";
                   }
                 });
  qr.enable_cors();
  s.on_http_request(std::move(qr));

  auto ec = boost::system::error_code{};
  s.init("0.0.0.0", client_port_, ec);
  s.run();
  if (ec) {
    std::cerr << "error: " << ec << "\n";
    return;
  }
  std::cout << "listening on 0.0.0.0: << " << client_port_ << "\n";

  net::run(ioc_)();
}

}  // namespace vdv
