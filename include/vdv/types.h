#pragma once

#include <chrono>

#include "date/date.h"

namespace vdv {

using sys_time = std::chrono::time_point<std::chrono::system_clock>;

inline std::string timestamp(const sys_time t) {
  return date::format("%FT%T",
                      std::chrono::time_point_cast<std::chrono::seconds>(t));
}

inline sys_time parse_timestamp(std::string const& str) {
  sys_time parsed;
  auto ss = std::stringstream{str};
  ss >> date::parse("%FT%T", parsed);
  return parsed;
}

using abo_id_t = std::uint32_t;
using error_code_t = std::uint32_t;

struct subscription {
  bool is_stale() const { return end_ < std::chrono::system_clock::now(); }

  abo_id_t id_;
  sys_time start_;
  sys_time end_;
  std::chrono::seconds hysteresis_;
  std::chrono::minutes look_ahead_;
};

}  // namespace vdv