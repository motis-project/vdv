#include "vdv/timestamp.h"
#include "date/date.h"

namespace vdv {

timestamp_t timestamp_now() { return std::chrono::system_clock::now(); }

std::string timestamp_to_string(timestamp_t const t) {
  return std::format("{0:%F}T{0:%T}",
                     std::chrono::time_point_cast<std::chrono::seconds>(t));
}

timestamp_t timestamp_from_string(std::string const& str) {
  timestamp_t parsed;
  auto ss = std::stringstream{str};
  ss >> date::parse("%FT%T", parsed);
  return parsed;
}

}  // namespace vdv