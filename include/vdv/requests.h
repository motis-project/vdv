#pragma once

#include "net/http/client/request.h"
#include "pugixml.hpp"
#include "vdv/types.h"

namespace vdv {

/*
 * abo_id: id of the subscription, chosen and managed by the client
 * hysteresis: threshold in seconds at which deviations from the static
 *              timetable shall be transmitted
 * look_ahead: maximum look ahead time in minutes
 */
net::http::client::request make_abo_anfrage(net::http::client::url const&,
                                            std::string const& sender,
                                            timestamp_t,
                                            std::uint32_t abo_id,
                                            std::chrono::seconds hysteresis,
                                            std::chrono::minutes look_ahead);

}  // namespace vdv