#pragma once

#include "pugixml.hpp"
#include "vdv/types.h"

namespace vdv {

bool parse(std::string const&, msg*);

}  // namespace vdv