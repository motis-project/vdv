#include "vdv/requests.h"

#include "vdv/timestamp.h"
#include "vdv/xml.h"

#include <sstream>

namespace vdv {

using namespace pugi;
using namespace net::http::client;

xml_document make_xml_abo_anfrage(std::string const& sender,
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
  return doc;
}

request make_abo_anfrage(net::http::client::url const& addr,
                         std::string const& sender,
                         timestamp_t const t,
                         std::uint32_t const abo_id,
                         std::chrono::seconds const hysteresis,
                         std::chrono::minutes const look_ahead) {
  auto xml = make_xml_abo_anfrage(sender, t, abo_id, hysteresis, look_ahead);
  return request{addr, request::method::POST, request::str_map(),
                 xml_to_str(xml)};
}

}  // namespace vdv