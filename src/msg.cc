#include "vdv/msg.h"

namespace vdv {

msg::msg(net::http::client::url from, timestamp_t t) : from_{from}, t_{t} {}

abo_anfrage_msg::abo_anfrage_msg(net::http::client::url from,
                                 timestamp_t t,
                                 std::string const& sender,
                                 abo_id_t const abo_id,
                                 std::chrono::seconds const hysteresis,
                                 std::chrono::minutes const look_ahead)
    : msg{from, t},
      sender_{sender},
      abo_id_{abo_id},
      hysteresis_{hysteresis},
      look_ahead_{look_ahead} {}

abo_loeschen_anfrage_msg::abo_loeschen_anfrage_msg(
    net::http::client::url from,
    timestamp_t t,
    std::string const& sender,
    std::vector<abo_id_t>& abo_ids)
    : msg{from, t}, sender_{sender}, abo_ids_{abo_ids} {}

abo_antwort_msg::abo_antwort_msg(net::http::client::url from,
                                 timestamp_t t,
                                 bool success,
                                 error_code_t error_code)
    : msg{from, t}, success_{success}, error_code_{error_code} {}

daten_bereit_anfrage_msg::daten_bereit_anfrage_msg(net::http::client::url from,
                                                   timestamp_t t,
                                                   const std::string& sender)
    : msg{from, t}, sender_{sender} {}

daten_bereit_antwort_msg::daten_bereit_antwort_msg(net::http::client::url from,
                                                   timestamp_t t,
                                                   bool success,
                                                   error_code_t error_code)
    : msg{from, t}, success_{success}, error_code_{error_code} {}

daten_abrufen_anfrage_msg::daten_abrufen_anfrage_msg(
    net::http::client::url from,
    vdv::timestamp_t t,
    const std::string& sender,
    bool all_datasets)
    : msg{from, t}, sender_{sender}, all_datasets_{all_datasets} {}

client_status_antwort_msg::client_status_antwort_msg(
    net::http::client::url from,
    timestamp_t t,
    status status,
    timestamp_t start,
    std::vector<subscription> const& subscriptions)
    : msg{from, t},
      status_{status},
      start_{start},
      subscriptions_{subscriptions} {}

}  // namespace vdv