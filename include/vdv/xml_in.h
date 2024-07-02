#pragma once

#include "pugixml.hpp"
#include "vdv/msg.h"
#include "vdv/types.h"

namespace vdv {

vdv_msg parse(std::string const&);

}  // namespace vdv