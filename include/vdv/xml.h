#pragma once

#include "pugixml.hpp"
#include "vdv/types.h"

namespace vdv {

constexpr auto const kEncoding = "encoding_latin-1";

pugi::xml_document make_xml_doc();

void add_root_attr(pugi::xml_node&, std::string const& sender, timestamp_t);

std::string xml_to_str(pugi::xml_document const&);

}  // namespace vdv
