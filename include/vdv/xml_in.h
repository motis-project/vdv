#pragma once

#include "pugixml.hpp"
#include "vdv/msg.h"
#include "vdv/types.h"

namespace vdv {

msg_envelope parse(std::string const&);

}  // namespace vdv