#include <random>
#include <thread>

#include "net/http/client/http_client.h"
#include "net/run.h"

#include "nigiri/types.h"

#include "vdv/vdv_client.h"

using namespace std::literals::chrono_literals;
using namespace net::http::client;
using namespace vdv;

int main() {

  auto client = vdv_client{"client",
                           "192.168.2.220",
                           "8080",
                           "server",
                           url{"http://192.168.2.206:80"},
                           nullptr,
                           nullptr,
                           nigiri::source_idx_t{0}};

  std::thread t(&vdv_client::run, &client);
  t.detach();

  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(5, 15);

  for (auto i = 0U; i != 1000; ++i) {
    std::this_thread::sleep_for(std::chrono::seconds{dist(rng)});

    client.check_server_status();

    std::this_thread::sleep_for(std::chrono::seconds{dist(rng)});

    client.subscribe(std::chrono::system_clock::now(),
                     std::chrono::system_clock::now() + 5min, 30s, 60min);
  }
}
