#include "net/run.h"
#include "net/web_server/query_router.h"
#include "net/web_server/web_server.h"
#include "net/web_server/responses.h"

#include "boost/program_options.hpp"

namespace asio = boost::asio;
using net::web_server;
using namespace net;
using namespace std::string_literals;

constexpr auto const response =
    R"(<?xml version="1.0" encoding="ISO-8859-1"?>
<DatenBereitAntwort>
  <Bestaetigung Zst="2024-06-05T20:56:54Z" Ergebnis="ok" Fehlernummer="0" />
</DatenBereitAntwort>)";

int main(int ac, char** av) {
  namespace bpo = boost::program_options;

  auto server_ip = "0.0.0.0"s;
  auto server_port = "80"s;
  auto client_ip = "0.0.0.0"s;
  auto client_port = "80"s;

  auto desc = bpo::options_description{"Options"};
  desc.add_options()  //
      ("help,h", "produce this help message")  //
      ("server_ip", bpo::value(&server_ip)->default_value(server_ip),
       "the ip of this vdv server")  //
      ("server_port", bpo::value(&server_port)->default_value(server_port),
       "the listening port of this vdv server")  //
      ("client_ip", bpo::value(&client_ip)->default_value(client_ip),
       "the ip of the vdv client to talk to")  //
      ("client_port", bpo::value(&client_port)->default_value(client_port),
       "the listening port of the vdv client to talk to");
  bpo::variables_map vm;
  bpo::store(bpo::command_line_parser(ac, av).options(desc).run(), vm);
  if (vm.count("help") != 0U) {
    std::cout << desc << "\n";
    return 0;
  }
  bpo::notify(vm);

  auto ioc = asio::io_context{};
  auto s = net::web_server{ioc};
  auto qr = net::query_router{}.route("*", ".*", [](route_request const& req, web_server::http_res_cb_t res_cb, bool) {
    std::cout << "REQ: " << req.base().method() << ", " << req.base().target() << "\n";
    std::cout << "HEADERS:\n";
    for (auto const& h : req.base()) {
      std::cout << h.name() << ": " << h.value() << "\n";
    }
    std::cout << "BODY: " << req.body() << "\n\n";
    std::cout << std::flush;
    res_cb(server_error_response(req, "not implemented yet"));
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
  net::run(ioc)();
}
