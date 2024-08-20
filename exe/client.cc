#include <filesystem>
#include <random>
#include <string>
#include <thread>

#include "boost/program_options.hpp"
#include "date/date.h"
#include "net/http/client/http_client.h"
#include "net/run.h"

#include "vdv/vdv_client.h"
#include "vdv/vdv_config.h"

namespace fs = std::filesystem;
namespace bpo = boost::program_options;
using namespace std::literals::chrono_literals;
using namespace std::string_literals;
using namespace net::http::client;
using namespace vdv;

void run(boost::asio::io_context& ioc) { ioc.run(); }

int main(int argc, char* argv[]) {

  auto tt_path = std::filesystem::path{};
  auto cfg = vdv_config{};

  bpo::options_description desc("Allowed options");
  desc.add_options()("help,h", "produce this help message")  //
      ("tt_path,p", bpo::value(&tt_path)->required())  //
      ("client_name", bpo::value(&cfg.client_name_)->required())  //
      ("client_ip", bpo::value(&cfg.client_ip_)->required())  //
      ("client_port", bpo::value(&cfg.client_port_)->required())  //
      ("server_name", bpo::value(&cfg.server_name_)->required())  //
      ("server_addr", bpo::value(&cfg.server_addr_)->required());

  bpo::variables_map vm;
  bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);

  // process program options - begin
  if (vm.count("help") != 0U) {
    std::cout << desc << "\n";
    return 0;
  }

  bpo::notify(vm);

  cfg.derive_endpoints();

  auto ioc = boost::asio::io_context{};
  auto work_guard = boost::asio::make_work_guard(ioc);
  auto t = std::thread{run, std::ref(ioc)};

  auto client = vdv_client{cfg};

  client.run(ioc);

  auto stop_time = std::chrono::system_clock::now() + 10min;

  while (std::chrono::system_clock::now() < stop_time) {
    client.subscribe(ioc, std::chrono::system_clock::now(),
                     std::chrono::system_clock::now() + 10min, 30s, 60min);
    std::this_thread::sleep_for(30s);
    client.unsubscribe(ioc);
    std::this_thread::sleep_for(30s);
  }

  client.stop();
  t.join();
}
