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
                           "8080",
                           "server",
                           url{"http://0.0.0.0:80"},
                           nullptr,
                           nullptr,
                           nigiri::source_idx_t{0}};

  std::thread t(&vdv_client::run, &client);
  t.detach();

  while (true) {
    client.check_server_status();

    std::this_thread::sleep_for(1s);

    //    client.subscribe(std::chrono::system_clock::now(),
    //                     std::chrono::system_clock::now() + 5min, 30s, 60min);
    //    std::this_thread::sleep_for(10s);
    //    std::cout << "active subs: ";
    //    auto i = 0;
    //    for (auto const& sub : client.subs_) {
    //      std::cout << i++ << " ";
    //    }
    //    std::cout << "\n" << std::endl;
  }
}
