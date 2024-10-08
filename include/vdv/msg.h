#pragma once

#include "net/http/client/url.h"

#include "vdv/types.h"

#include <variant>
#include <vector>

namespace vdv {

struct abo_anfrage_msg {
  sys_time t_;
  std::string sender_;
  abo_id_t abo_id_;
  sys_time expiration_t_;
  std::chrono::seconds hysteresis_;
  std::chrono::minutes look_ahead_;
};

struct abo_antwort_msg {
  sys_time t_;
  bool success_;
  error_code_t error_code_;
};

struct daten_bereit_anfrage_msg {
  sys_time t_;
  std::string sender_;
};

struct daten_bereit_antwort_msg {
  sys_time t_;
  bool success_;
  error_code_t error_code_;
};

struct daten_abrufen_anfrage_msg {
  sys_time t_;
  std::string sender_;
  bool all_datasets_{false};
};

struct daten_abrufen_antwort_msg {
  sys_time t_;
  bool success_;
  error_code_t error_code_;
  abo_id_t abo_id_;
};

struct status_anfrage_msg {
  sys_time t_;
  std::string sender_;
};

struct status_antwort_msg {
  sys_time t_;
  bool success_;
  bool data_rdy_;
  sys_time start_;
};

struct client_status_anfrage_msg {
  sys_time t_;
  std::string sender_;
  bool query_active_subs_{false};
};

struct client_status_antwort_msg {
  sys_time t_;
  bool success_;
  sys_time start_;
  std::vector<abo_id_t> active_subs_;
};

using vdv_msg = std::variant<abo_anfrage_msg,
                             abo_antwort_msg,
                             daten_bereit_anfrage_msg,
                             daten_bereit_antwort_msg,
                             daten_abrufen_anfrage_msg,
                             daten_abrufen_antwort_msg,
                             status_anfrage_msg,
                             status_antwort_msg,
                             client_status_anfrage_msg,
                             client_status_antwort_msg>;

}  // namespace vdv