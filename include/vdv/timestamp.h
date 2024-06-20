#pragma once

#include "date/date.h"

namespace vdv {

std::chrono::time_point<std::chrono::system_clock> timestamp_current();

std::chrono::time_point<std::chrono::system_clock> timestamp_future(
    std::chrono::hours);

}  // namespace vdv