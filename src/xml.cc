#include "vdv/xml.h"
#include "vdv/timestamp.h"
#include "vdv/types.h"

namespace vdv {

pugi::xml_document make_xml_doc() {
  auto doc = pugi::xml_document{};
  auto decl = doc.prepend_child(pugi::node_declaration);
  decl.append_attribute("version") = "1.0";
  decl.append_attribute("encoding") = "iso-8859-1";
  return doc;
}

void add_root_attr(pugi::xml_node& node,
                   std::string const& sender,
                   timestamp_t const t_now) {
  node.append_attribute("xmlns:xsd") = "http://www.w3.org/2001/XMLSchema";
  node.append_attribute("xmlns:xsi") =
      "http://www.w3.org/2001/XMLSchema-instance";
  node.append_attribute("Sender") = sender.c_str();
  node.append_attribute("Zst") = timestamp_to_string(t_now).c_str();
}

std::string xml_to_str(pugi::xml_document const& doc) {
  std::stringstream ss{};
  doc.save(ss, "  ", pugi::format_default, pugi::xml_encoding::encoding_latin1);
  return ss.str();
}

}  // namespace vdv
