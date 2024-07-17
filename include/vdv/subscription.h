#pragma once

#include "nigiri/types.h"
#include "net/http/client/url.h"

#include "vdv/timestamp.h"

namespace vdv {

struct subscription {
  bool is_valid() const {
    auto const now = std::chrono::system_clock::now();
    return start_ <= now && now <= end_;
  }

  std::chrono::time_point<std::chrono::system_clock> start_;
  std::chrono::time_point<std::chrono::system_clock> end_;
  std::chrono::seconds hysteresis_;
  nigiri::source_idx_t src_idx_;

  std::string server_name_;
  net::http::client::url server_addr_;
  net::http::client::url status_addr_;
  net::http::client::url manage_sub_addr_;
  net::http::client::url fetch_data_addr_;
};

}  // namespace vdv