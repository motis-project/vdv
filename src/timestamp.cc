#include "vdv/timestamp.h"

namespace vdv {

std::chrono::time_point<std::chrono::system_clock> timestamp_current() {
  return std::chrono::system_clock::now();
}

std::chrono::time_point<std::chrono::system_clock> timestamp_future(
    std::chrono::hours offset) {
  return timestamp_current() + offset;
}

}  // namespace vdv