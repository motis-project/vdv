#pragma once

#include "vdv/types.h"

namespace vdv {

timestamp_t timestamp_now();

std::string to_string(const timestamp_t);

timestamp_t parse_timestamp(std::string const&);

}  // namespace vdv