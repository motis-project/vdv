#include "vdv/xml.h"

namespace vdv {

pugi::xml_document make_xml_doc() {
  auto doc = pugi::xml_document{};
  return doc;
}

void add_root_attr(pugi::xml_node& node) {
  node.append_attribute("xmlns:xsd") = "http://www.w3.org/2001/XMLSchema";
  node.append_attribute("xmlns:xsi") =
      "http://www.w3.org/2001/XMLSchema-instance";
  node.append_attribute("Sender") = "motis";
}

void xml_to_stream(std::basic_ostream<char, std::char_traits<char> >& stream,
                   pugi::xml_document const& doc) {
  doc.save(stream, "  ", pugi::format_default,
           pugi::xml_encoding::encoding_latin1);
}

}  // namespace vdv
