#pragma once

#include "pugixml.hpp"
#include "vdv/aus_msg.h"
#include "vdv/types.h"

namespace vdv {

aus_msg parse_aus(std::string const&);

}  // namespace vdv