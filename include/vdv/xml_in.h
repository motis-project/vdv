#pragma once

#include "pugixml.hpp"
#include "vdv/msg.h"
#include "vdv/types.h"

namespace vdv {

std::optional<vdv_msg> parse(std::string const&);

}  // namespace vdv