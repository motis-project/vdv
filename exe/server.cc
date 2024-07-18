#include "net/run.h"
#include "net/web_server/query_router.h"
#include "net/web_server/responses.h"
#include "net/web_server/web_server.h"

#include "vdv/xml_in.h"
#include "vdv/xml_out.h"

namespace asio = boost::asio;
namespace http = boost::beast::http;
using net::web_server;
using namespace std::string_literals;
using namespace vdv;

constexpr auto const response =
    R"(<?xml version="1.0" encoding="ISO-8859-1"?>
<DatenBereitAntwort>
  <Bestaetigung Zst="2024-06-05T20:56:54Z" Ergebnis="ok" Fehlernummer="0" />
</DatenBereitAntwort>)";

int main(int ac, char** av) {

  auto const server_ip = "0.0.0.0";
  auto server_port = "80";

  auto const start = std::chrono::system_clock::now();

  auto ioc = asio::io_context{};
  auto s = net::web_server{ioc};
  auto qr =
      net::query_router{}
          .route(
              "POST", "/client/aus/status.xml",
              [&](net::route_request const& req,
                  web_server::http_res_cb_t const& res_cb, bool) {
                auto const msg_in = parse(req.body());
                if (holds_alternative<status_anfrage_msg>(msg_in)) {
                  res_cb(string_response(
                      req,
                      status_antwort_xml_str(std::chrono::system_clock::now(),
                                             true, false, start),
                      http::status::ok, "text/xml"));
                  std::cout << "Received request on server status endpoint, "
                               "response: OK"
                            << std::endl;
                } else {
                  res_cb(bad_request_response(req));
                }
              })
          .route("POST", "/client/aus/aboverwalten.xml",
                 [&](net::route_request const& req,
                     web_server::http_res_cb_t const& res_cb, bool) {
                   auto const msg_in = parse(req.body());
                   if (holds_alternative<abo_anfrage_msg>(msg_in)) {
                     res_cb(string_response(
                         req,
                         abo_antwort_xml_str(std::chrono::system_clock::now(),
                                             true, 0),
                         http::status::ok, "text/xml"));
                     std::cout << "Received request on aboverwalten endpoint, "
                                  "response: OK"
                               << std::endl;
                   } else {
                     res_cb(bad_request_response(req));
                   }
                 })
          .route("*", ".*",
                 [&](net::route_request const& req,
                     web_server::http_res_cb_t const& res_cb, bool) {
                   std::cout << "REQ: " << req.base().method() << ", "
                             << req.base().target() << "\n";
                   std::cout << "HEADERS:\n";
                   for (auto const& h : req.base()) {
                     std::cout << h.name() << ": " << h.value() << "\n";
                   }
                   std::cout << "BODY: " << req.body() << "\n" << std::endl;
                   res_cb(bad_request_response(req));
                 });

  qr.enable_cors();
  s.on_http_request(std::move(qr));

  auto ec = boost::system::error_code{};
  s.init(server_ip, server_port, ec);
  s.run();
  if (ec) {
    std::cerr << "error: " << ec << "\n";
    return 1;
  }

  std::cout << "listening on " << server_ip << ":" << server_port << "\n";
  // ioc.run();
  net::run(ioc)();
}
