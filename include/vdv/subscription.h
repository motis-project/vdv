#pragma once

#include "vdv/timestamp.h"

#include "net/http/client/url.h"

namespace vdv {

struct subscription {
  bool is_valid() {
    return start_ <= timestamp_now() && timestamp_now() <= end_;
  }

  std::uint64_t id_;
  net::http::client::url server_addr_;
  timestamp_t start_;
  timestamp_t end_;
  std::chrono::seconds hysteresis_;
};

}  // namespace vdv