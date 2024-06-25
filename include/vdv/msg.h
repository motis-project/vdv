#pragma once

#include "net/http/client/url.h"

#include "vdv/subscription.h"
#include "vdv/types.h"

#include <vector>

namespace vdv {

enum msg_type {
  kAboAnfrage,
  kAboLoeschen,
  kAboAntwort,
  kDatenBereitAnfrage,
  kDatenBereitAntwort,
  kDatenAbrufenAnfrage,
  kDatenAbrufenAntwort,
  kStatusAnfrage,
  kStatusAntwort,
  kClientStatusAnfrage,
  kClientStatusAntwort
};

struct msg {
  msg(net::http::client::url from, timestamp_t);
  virtual ~msg() = 0;
  constexpr virtual msg_type get_msg_type() const = 0;

  net::http::client::url from_;
  timestamp_t t_;
};

struct abo_anfrage_msg : public msg {
  abo_anfrage_msg(net::http::client::url from,
                  timestamp_t,
                  std::string const& sender,
                  abo_id_t const,
                  std::chrono::seconds const hysteresis,
                  std::chrono::minutes const look_ahead);

  constexpr msg_type get_msg_type() const override {
    return msg_type::kAboAnfrage;
  }

  std::string const& sender_;
  abo_id_t abo_id_;
  std::chrono::seconds hysteresis_;
  std::chrono::minutes look_ahead_;
};

struct abo_loeschen_anfrage_msg : public msg {
  abo_loeschen_anfrage_msg(net::http::client::url from,
                           timestamp_t,
                           std::string const& sender,
                           std::vector<abo_id_t>& abo_ids);

  constexpr msg_type get_msg_type() const override {
    return msg_type::kAboLoeschen;
  }

  std::string const& sender_;
  std::vector<abo_id_t> abo_ids_;
};

struct abo_antwort_msg : public msg {
  abo_antwort_msg(net::http::client::url from,
                  timestamp_t,
                  bool success,
                  error_code_t);

  constexpr msg_type get_msg_type() const override {
    return msg_type::kAboAntwort;
  }

  bool success_;
  error_code_t error_code_;
};

struct daten_bereit_anfrage_msg : public msg {
  daten_bereit_anfrage_msg(net::http::client::url from,
                           timestamp_t,
                           std::string const& sender);

  constexpr msg_type get_msg_type() const override {
    return msg_type::kDatenBereitAnfrage;
  }

  std::string const& sender_;
};

struct daten_bereit_antwort_msg : public msg {
  daten_bereit_antwort_msg(net::http::client::url from,
                           timestamp_t,
                           bool success,
                           error_code_t);

  constexpr msg_type get_msg_type() const override {
    return msg_type::kDatenBereitAntwort;
  }

  bool success_;
  error_code_t error_code_;
};

struct daten_abrufen_anfrage_msg : public msg {
  daten_abrufen_anfrage_msg(net::http::client::url from,
                            timestamp_t,
                            std::string const& sender,
                            bool all_datasets);

  constexpr msg_type get_msg_type() const override {
    return msg_type::kDatenAbrufenAnfrage;
  }

  std::string const& sender_;
  bool all_datasets_;
};

struct daten_abrufen_antwort_msg : public msg {
  daten_abrufen_antwort_msg(net::http::client::url from)

      constexpr msg_type get_msg_type() const override {
    return msg_type::kDatenAbrufenAntwort;
  }

  // TODO
};

struct status_anfrage_msg : public msg {
  constexpr msg_type get_msg_type() const override {
    return msg_type::kStatusAnfrage;
  }

  std::string sender_;
};

struct status_antwort_msg : public msg {
  constexpr msg_type get_msg_type() const override {
    return msg_type::kStatusAntwort;
  }

  bool success_;
  bool data_rdy_;
  timestamp_t start_;
};

struct client_status_anfrage_msg : public msg {
  constexpr msg_type get_msg_type() const override {
    return msg_type::kClientStatusAnfrage;
  }

  std::string sender_;
  bool req_active_abos_;
};

struct client_status_antwort_msg : public msg {
  client_status_antwort_msg(net::http::client::url from_,
                            timestamp_t t,
                            status status,
                            timestamp_t start,
                            std::vector<subscription> const& subscriptions);

  constexpr msg_type get_msg_type() const override {
    return msg_type::kClientStatusAntwort;
  }

  status status_;
  timestamp_t start_;
  std::vector<vdv::subscription> const& subscriptions_;
};

}  // namespace vdv