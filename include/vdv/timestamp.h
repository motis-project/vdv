#pragma once

#include "vdv/types.h"

namespace vdv {

timestamp_t timestamp_now();

std::string timestamp_to_string(timestamp_t);

timestamp_t timestamp_from_string(std::string const&);

}  // namespace vdv