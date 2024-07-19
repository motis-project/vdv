#include <random>
#include <thread>

#include "net/http/client/http_client.h"
#include "net/http/client/url.h"
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
namespace nhc = net::http::client;

constexpr auto const server_name = "server";
constexpr auto const server_ip = "192.168.2.206";
constexpr auto const server_port = "80";
constexpr auto const client_ip = "192.168.2.220";
constexpr auto const client_port = "8080";

void run(asio::io_context& ioc) {

  auto const start = std::chrono::system_clock::now();

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
                     std::cout << "Received request on manage subs endpoint, "
                                  "response: OK"
                               << std::endl;
                   } else {
                     res_cb(bad_request_response(req));
                   }
                 })
          .route("POST", "/client/aus/datenabrufen.xml",
                 [&](net::route_request const& req,
                     web_server::http_res_cb_t const& res_cb, bool) {
                   auto const msg_in = parse(req.body());
                   if (holds_alternative<daten_abrufen_anfrage_msg>(msg_in)) {
                     res_cb(string_response(
                         req,
                         daten_abrufen_antwort_xml_str(
                             std::chrono::system_clock::now(), true, 0),
                         http::status::ok, "text/xml"));
                     std::cout << "Received request on data request endpoint, "
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
    return;
  }

  std::cout << "listening on " << server_ip << ":" << server_port << "\n";
  // ioc.run();
  net::run(ioc)();
}

void check_client_status(asio::io_context& ioc,
                         nhc::url const& client_status_addr) {
  std::cout << "sending client status request to " << client_status_addr
            << "\n";
  auto req =
      nhc::request(client_status_addr, nhc::request::method::POST,
                   {{"Content-Type", "text/xml"}},
                   client_status_anfrage_xml_str(
                       server_name, std::chrono::system_clock::now(), true));
  auto const client = make_http(ioc, client_status_addr);
  client->query(req, [&]([[maybe_unused]] auto&& shrd_ptr, auto&& r,
                         [[maybe_unused]] auto&& ec) {
    if (r.status_code == 200) {
      try {
        auto const msg_in = parse(r.body);
        if (holds_alternative<client_status_antwort_msg>(msg_in)) {
          std::cout << "client reports active subs: ";
          for (auto const id :
               get<client_status_antwort_msg>(msg_in).active_subs_) {
            std::cout << id << " ";
          }
          std::cout << std::endl;
        }
      } catch (std::runtime_error const& e) {
        std::cout << "error parsing xml response\n";
      }
    }
  });
}

void report_data_rdy(asio::io_context& ioc, nhc::url const& data_rdy_addr) {
  std::cout << "reporting data rdy to " << data_rdy_addr << "\n";
  auto req = nhc::request(data_rdy_addr, nhc::request::method::POST,
                          {{"Content-Type", "text/xml"}},
                          daten_bereit_anfrage_xml_str(
                              server_name, std::chrono::system_clock::now()));
  auto const client = make_http(ioc, data_rdy_addr);
  client->query(req, [&]([[maybe_unused]] auto&& shrd_ptr, auto&& r,
                         [[maybe_unused]] auto&& ec) {
    if (r.status_code == 200) {
      try {
        auto const msg_in = parse(r.body);
        if (holds_alternative<daten_bereit_antwort_msg>(msg_in) &&
            get<daten_bereit_antwort_msg>(msg_in).success_) {
          std::cout << "client copies" << std::endl;
        }
      } catch (std::runtime_error const& e) {
        std::cout << "error parsing xml response\n";
      }
    }
  });
}

int main(int ac, char** av) {

  auto ioc = asio::io_context{};

  std::thread t(run, std::ref(ioc));
  t.detach();

  auto ss = std::stringstream{};
  ss << "http://" << client_ip << ":" << client_port << "/" << server_name
     << "/aus/clientstatus.xml";
  auto const client_status_addr = nhc::url{ss.str()};
  ss.clear();
  ss << "http://" << client_ip << ":" << client_port << "/" << server_name
     << "/aus/datenbereit.xml";
  auto const data_rdy_addr = nhc::url{ss.str()};

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(5, 15);

  for (auto i = 0U; i != 1000; ++i) {
    std::this_thread::sleep_for(std::chrono::seconds{dist(rng)});

    check_client_status(ioc, client_status_addr);

    std::this_thread::sleep_for(std::chrono::seconds{dist(rng)});

    report_data_rdy(ioc, data_rdy_addr);
  }
}
