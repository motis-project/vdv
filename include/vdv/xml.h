#pragma once

#include "pugixml.hpp"
#include "vdv/types.h"

namespace vdv {

/*
 * abo_id: id of the subscription, chosen and managed by the client
 * hysteresis: threshold in seconds at which deviations from the static
 *              timetable shall be transmitted
 * look_ahead: maximum look ahead time in minutes
 */
std::string abo_anfrage_xml_str(std::string const& sender,
                                timestamp_t,
                                std::uint32_t abo_id,
                                std::chrono::seconds hysteresis,
                                std::chrono::minutes look_ahead);

std::string abo_antwort_xml_str(timestamp_t,
                                bool success,
                                std::uint32_t error_code);

}  // namespace vdv
