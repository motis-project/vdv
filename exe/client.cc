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
#include "nigiri/timetable.h"
#include "nigiri/types.h"

#include "vdv/vdv_client.h"

namespace fs = std::filesystem;
namespace bpo = boost::program_options;
using namespace std::literals::chrono_literals;
using namespace std::string_literals;
using namespace net::http::client;
using namespace vdv;
using namespace nigiri;

int main(int argc, char* argv[]) {

  auto tt_path = std::filesystem::path{};
  auto client_name = "client"s;
  auto client_ip = "0.0.0.0"s;
  auto client_port = "80"s;
  auto server_name = "server"s;
  auto server_url = "http://0.0.0.0:80"s;

  bpo::options_description desc("Allowed options");
  desc.add_options()("help,h", "produce this help message")  //
      ("tt_path,p", bpo::value(&tt_path)->required())  //
      ("client_name", bpo::value(&client_name)->required())  //
      ("client_ip", bpo::value(&client_ip)->required())  //
      ("client_port", bpo::value(&client_port)->required())  //
      ("server_name", bpo::value(&server_name)->required())  //
      ("server_url", bpo::value(&server_url)->required());

  bpo::variables_map vm;
  bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(), vm);

  // process program options - begin
  if (vm.count("help") != 0U) {
    std::cout << desc << "\n";
    return 0;
  }

  bpo::notify(vm);

  auto tt = *nigiri::timetable::read(cista::memory_holder{
      cista::file{fs::path{"tt.bin"}.generic_string().c_str(), "r"}.content()});
  tt.locations_.resolve_timezones();

  auto rtt =
      rt::create_rt_timetable(tt, std::chrono::time_point_cast<date::days>(
                                      std::chrono::system_clock::now()));

  auto ioc = boost::asio::io_context{};

  auto client = vdv_client{ioc,         client_name, client_ip,
                           client_port, server_name, server_url,
                           &tt,         &rtt,        nigiri::source_idx_t{0}};

  std::thread t(&vdv_client::run, &client);
  t.detach();

  client.subscribe(std::chrono::system_clock::now(),
                   std::chrono::system_clock::now() + 10min, 30s, 60min);

  auto stop_time = std::chrono::system_clock::now() + 10min;

  while (std::chrono::system_clock::now() < stop_time) {
    std::this_thread::sleep_for(60s);
    client.check_server_status();
  }

  client.cancel_sub();

  int unsupported_additional_run{0};
  int unsupported_cancelled_run{0};
  int unsupported_additional_stop_{0};
  int unmatchable_run_{0};
  int matched_stops_{0};
  int propagated_delay_{0};
  for (auto const& stat : client.stats_) {
    unsupported_additional_run += stat.unsupported_additional_run;
    unsupported_cancelled_run += stat.unsupported_cancelled_run;
    // TODO
  }
}
