#include "vdv/client/requests.h"
#include "vdv/xml.h"

#include <sstream>

namespace vdv::client {

using namespace pugi;
using namespace net::http::client;

xml_document make_xml_abo_anfrage() {
  auto doc = make_xml_doc();
  auto abo_anfrage_node = doc.append_child("AboAnfrage");
  add_root_attr(abo_anfrage_node);
  return doc;
}

request make_req_abo_anfrage(url addr) {
  auto xml = make_xml_abo_anfrage();
  std::stringstream ss;
  xml.save(ss);
  auto req = request{addr, request::method::POST, request::str_map(), ss.str()};
  return req;
}

}  // namespace vdv::client