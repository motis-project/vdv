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
                                std::uint64_t abo_id,
                                std::chrono::seconds hysteresis,
                                std::chrono::minutes look_ahead);

std::string abo_loeschen_anfrage_xml_str(std::string const& sender,
                                         timestamp_t,
                                         std::vector<abo_id_t> const& abo_ids);

std::string abo_antwort_xml_str(timestamp_t,
                                bool success,
                                error_code_t error_code);

std::string daten_bereit_anfrage_xml_str(std::string const& sender,
                                         timestamp_t);

std::string daten_bereit_antwort_xml_str(timestamp_t,
                                         bool success,
                                         std::uint32_t error_code);

std::string daten_abrufen_anfrage_xml_str(std::string const& sender,
                                          timestamp_t,
                                          bool all_datasets);

// daten_abrufen_antwort_xml_str

std::string status_anfrage_xml_str(std::string const& sender, timestamp_t);

std::string status_antwort_xml_str(timestamp_t,
                                   bool success,
                                   bool data_rdy,
                                   timestamp_t start);

std::string client_status_anfrage_xml_str(std::string const& sender,
                                          timestamp_t,
                                          bool req_active_abos);

}  // namespace vdv
