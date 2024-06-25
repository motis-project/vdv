#include "vdv/client.h"
#include "vdv/xml_out.h"

namespace vdv {

bool client::subscribe(net::http::client::url server_addr,
                       std::chrono::seconds hysteresis,
                       std::chrono::minutes look_ahead) {
  auto const xml = vdv::abo_anfrage_xml_str(sender_, timestamp_now(), next_id++,
                                            hysteresis, look_ahead);
  // TODO send xml to server_addr and parse response

  return false;
}

}  // namespace vdv