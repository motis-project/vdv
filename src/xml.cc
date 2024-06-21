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

std::string abo_anfrage_xml_str(std::string const& sender,
                                timestamp_t const t,
                                std::uint32_t const abo_id,
                                std::chrono::seconds const hysteresis,
                                std::chrono::minutes const look_ahead) {
  auto doc = make_xml_doc();
  auto abo_anfrage_node = doc.append_child("AboAnfrage");
  add_root_attr(abo_anfrage_node, sender, t);
  auto abo_aus_node = abo_anfrage_node.append_child("AboAUS");
  abo_aus_node.append_attribute("AboID") = std::to_string(abo_id).c_str();
  abo_aus_node.append_attribute("VerfallZst") =
      timestamp_to_string(t + look_ahead).c_str();
  auto hysterese_node = abo_aus_node.append_child("Hysterese");
  hysterese_node.append_child(pugi::node_pcdata)
      .set_value(std::to_string(hysteresis.count()).c_str());
  auto vorschauzeit_node = abo_aus_node.append_child("Vorschauzeit");
  vorschauzeit_node.append_child(pugi::node_pcdata)
      .set_value(std::to_string(look_ahead.count()).c_str());
  return xml_to_str(doc);
}

}  // namespace vdv
