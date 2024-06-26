#pragma once

#include "vdv/subscription.h"
#include "vdv/types.h"

#include "net/http/client/url.h"

#include <string>
#include <vector>

namespace vdv {

struct client {

  bool subscribe(net::http::client::url server_addr,
                 std::chrono::seconds hysteresis,
                 std::chrono::minutes look_ahead);

  std::string sender_{"motis"};
  std::vector<vdv::subscription> subscriptions_;
  std::uint64_t next_id{0U};
};

}  // namespace vdv