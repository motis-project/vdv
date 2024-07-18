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
                                sys_time start,
                                sys_time end,
                                std::uint64_t abo_id,
                                std::chrono::seconds hysteresis,
                                std::chrono::minutes look_ahead);

std::string abo_loeschen_anfrage_xml_str(std::string const& sender,
                                         sys_time,
                                         std::vector<abo_id_t> const& abo_ids);

std::string abo_antwort_xml_str(sys_time,
                                bool success,
                                error_code_t error_code);

std::string daten_bereit_anfrage_xml_str(std::string const& sender, sys_time);

std::string daten_bereit_antwort_xml_str(sys_time,
                                         bool success,
                                         std::uint32_t error_code);

std::string daten_abrufen_anfrage_xml_str(std::string const& sender,
                                          sys_time,
                                          bool all_datasets);

// daten_abrufen_antwort_xml_str

std::string status_anfrage_xml_str(std::string const& sender, sys_time);

std::string status_antwort_xml_str(sys_time,
                                   bool success,
                                   bool data_rdy,
                                   sys_time start);

std::string client_status_anfrage_xml_str(std::string const& sender,
                                          sys_time,
                                          bool req_active_abos);

std::string client_status_antwort_xml_str(sys_time,
                                          bool success,
                                          sys_time start);

}  // namespace vdv
