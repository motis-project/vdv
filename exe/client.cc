#include "vdv/requests.h"
#include "vdv/timestamp.h"

#include "net/http/client/http_client.h"
#include "net/run.h"

#include "boost/program_options.hpp"

namespace asio = boost::asio;
using namespace std::string_literals;
using namespace std::literals::chrono_literals;
using namespace vdv;

int main(int ac, char** av) {
  using namespace net::http::client;
  namespace bpo = boost::program_options;

  auto const http_str = "http://"s;

  auto client_ip = "0.0.0.0"s;
  auto client_port = "80"s;
  auto server_ip = "0.0.0.0"s;
  auto server_port = "8080"s;
  auto const sender = "motis"s;

  auto desc = bpo::options_description{"Options"};
  desc.add_options()  //
      ("help,h", "produce this help message")  //
      ("client_ip", bpo::value(&client_ip)->default_value(client_ip),
       "the ip of this vdv client")  //
      ("client_port", bpo::value(&client_port)->default_value(client_port),
       "the listening port of this vdv client")  //
      ("server_ip", bpo::value(&server_ip)->default_value(server_ip),
       "the ip of the vdv server to talk to")  //
      ("server_port", bpo::value(&server_port)->default_value(server_port),
       "the port of the vdv to talk to");
  bpo::variables_map vm;
  bpo::store(bpo::command_line_parser(ac, av).options(desc).run(), vm);
  if (vm.count("help") != 0U) {
    std::cout << desc << "\n";
    return 0;
  }
  bpo::notify(vm);

  auto const client_addr =
      net::http::client::url{http_str + client_ip + ":" + client_port};
  auto const server_addr =
      net::http::client::url{http_str + server_ip + ":" + server_port};

  boost::asio::io_service ios;
  auto client = net::http::client::make_http(ios, client_addr);

  auto abo_req =
      make_abo_anfrage(server_addr, sender, timestamp_now(), 1, 60s, 480min);

  std::cout << "request.body:" << abo_req.body << "\n";

  client->query(abo_req, [&](auto&& shrd_ptr, auto&& r, auto&& ec) {
    std::cout << "response: " << r.body << "\n";
  });

  ios.run();
}
