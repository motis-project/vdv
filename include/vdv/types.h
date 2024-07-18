#pragma once

#include <chrono>

#include "date/date.h"

namespace vdv {

using sys_time = std::chrono::time_point<std::chrono::system_clock>;

inline std::string to_string(const sys_time t) {
  return std::format("{0:%F}T{0:%T}",
                     std::chrono::time_point_cast<std::chrono::seconds>(t));
}

inline sys_time parse_sys_time(std::string const& str) {
  sys_time parsed;
  auto ss = std::stringstream{str};
  ss >> date::parse("%FT%T", parsed);
  return parsed;
}

using abo_id_t = std::uint32_t;
using error_code_t = std::uint32_t;

}  // namespace vdv