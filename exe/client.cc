#include "net/run.h"
#include "net/web_server/query_router.h"
#include "net/web_server/web_server.h"

namespace asio = boost::asio;

constexpr auto const response =
    R"(<?xml version="1.0" encoding="ISO-8859-1"?>
<DatenBereitAntwort>
  <Bestaetigung Zst="2024-06-05T20:56:54Z" Ergebnis="ok" Fehlernummer="0" />
</DatenBereitAntwort>)";

int main() {
  namespace bpo = boost::program_options;

  auto client_ip = "0.0.0.0";
  auto client_port = "80";
  auto server_ip = "0.0.0.0";
  auto server_port = "8080"

  auto desc = bpo::options_description{"Options"};
  desc.add_options()  //
      ("help,h", "produce this help message")  //
      ("client_ip", bpo::value(&client_ip)->default_value(client_ip),
       "the ip of this client") //
      ("client_port", bpo::value(&client_port)->default_value(client_port),
       "the listening port of this client") //
      ("server_ip", bpo::value(&fasta_path)->default_value(server_ip),
       "the ip of the vdv server") //
      ("server_port", bpo::value(&server_port)->default_value(server_port));
  bpo::variables_map vm;
  bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);

  if (vm.count("help") != 0U) {
    std::cout << desc << "\n";
    return 0;
  }

  bpo::notify(vm);

  auto ioc = asio::io_context{};
  auto s = net::web_server{ioc};
  auto qr = net::query_router{}.route("*", ".*", [](std::string_view body) {
    std::cout << "BODY: " << body << "\n\n";
    return std::string{response};
  });
  qr.enable_cors();
  s.on_http_request(std::move(qr));

  auto ec = boost::system::error_code{};
  s.init("0.0.0.0", "80", ec);
  s.run();
  if (ec) {
    std::cerr << "error: " << ec << "\n";
    return 1;
  }

  std::cout << "listening on 0.0.0.0:80\n";
  net::run(ioc)();
}
