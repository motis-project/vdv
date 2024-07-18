#include "net/run.h"

#include "net/http/client/http_client.h"

#include "nigiri/types.h"

#include "vdv/vdv_client.h"

using namespace std::literals::chrono_literals;
using namespace net::http::client;
using namespace vdv;

int main() {

  auto client =
      vdv_client{"client", "8080",  "server", url{"http://0.0.0.0:80"}, 60s,
                 1440min,  nullptr, nullptr,  nigiri::source_idx_t{0}};

  client.run();
}
