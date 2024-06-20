#pragma once

#include "pugixml.hpp"

namespace vdv {

constexpr auto const kEncoding = "encoding_latin-1";

pugi::xml_document make_xml_doc();

void add_root_attr(pugi::xml_node&);

void xml_to_stream(std::basic_ostream<char, std::char_traits<char> >&,
                   pugi::xml_document const&);

}  // namespace vdv
