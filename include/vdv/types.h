#pragma once

#include <chrono>

#include "date/date.h"

namespace vdv {

using timestamp_t = std::chrono::time_point<std::chrono::system_clock>;

inline timestamp_t timestamp_now() { return std::chrono::system_clock::now(); }

inline std::string to_string(const timestamp_t t) {
  return std::format("{0:%F}T{0:%T}",
                     std::chrono::time_point_cast<std::chrono::seconds>(t));
}

inline timestamp_t parse_timestamp(std::string const& str) {
  timestamp_t parsed;
  auto ss = std::stringstream{str};
  ss >> date::parse("%FT%T", parsed);
  return parsed;
}

using abo_id_t = std::uint32_t;
using error_code_t = std::uint32_t;

}  // namespace vdv