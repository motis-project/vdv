#include <filesystem>
#include <random>
#include <string>
#include <thread>

#include "boost/program_options.hpp"
#include "date/date.h"
#include "net/http/client/http_client.h"
#include "net/run.h"

#include "nigiri/rt/create_rt_timetable.h"
#include "nigiri/rt/rt_timetable.h"
#include "nigiri/rt/vdv/vdv_update.h"
#include "nigiri/timetable.h"
#include "nigiri/types.h"

#include "vdv/vdv_client.h"
#include "vdv/vdv_config.h"

namespace fs = std::filesystem;
namespace bpo = boost::program_options;
using namespace std::literals::chrono_literals;
using namespace std::string_literals;
using namespace net::http::client;
using namespace vdv;
using namespace nigiri;

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

  auto tt = *nigiri::timetable::read(
      cista::memory_holder{cista::file{tt_path.c_str(), "r"}.content()});
  tt.locations_.resolve_timezones();

  auto rtt =
      rt::create_rt_timetable(tt, std::chrono::time_point_cast<date::days>(
                                      std::chrono::system_clock::now()));

  cfg.tt_ = &tt;
  cfg.rtt_ = &rtt;
  cfg.src_idx_ = source_idx_t{0};
  cfg.derive_endpoints();

  auto ioc = boost::asio::io_context{};

  auto client = vdv_client{cfg, ioc};

  std::thread t(&vdv_client::run, &client);
  t.detach();

  client.subscribe(std::chrono::system_clock::now(),
                   std::chrono::system_clock::now() + 10min, 30s, 60min);

  auto stop_time = std::chrono::system_clock::now() + 10min;

  auto stats = std::vector<rt::vdv::statistics>{};

  while (std::chrono::system_clock::now() < stop_time) {
    std::this_thread::sleep_for(30s);
    client.check_server_status();
    std::this_thread::sleep_for(30s);
    stats.emplace_back(client.fetch());
  }

  client.unsubscribe();

  auto acc_stats = rt::vdv::statistics{};
  std::accumulate(begin(stats), end(stats), acc_stats);
  std::cout << "Statistics:\n" << acc_stats << "\n";
}
