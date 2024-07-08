#pragma once

#include "date/date.h"

#include <chrono>
#include <vector>

namespace vdv {

using timestamp_t = std::chrono::time_point<std::chrono::system_clock>;
using abo_id_t = std::uint32_t;
using error_code_t = std::uint32_t;

}  // namespace vdv