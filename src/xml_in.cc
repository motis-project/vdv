#include "vdv/xml_in.h"

#include "vdv/types.h"

#include "date/date.h"

#include <functional>
#include <iostream>
#include <unordered_map>

namespace vdv {

pugi::xpath_node find_node(pugi::xml_node const& doc,
                           std::string const& str,
                           bool warn) {
  auto const xpath_result = doc.select_node(str.c_str());
  if (!xpath_result && warn) {
    std::cout << "Warning: could not find " << str << "\n";
  }
  return xpath_result;
}

pugi::xpath_node find_required_node(pugi::xml_node const& doc,
                                    std::string const& str) {
  return find_node(doc, str, true);
}

pugi::xpath_node find_optional_node(pugi::xml_node const& doc,
                                    std::string const& str) {
  return find_node(doc, str, false);
}

std::optional<abo_anfrage_msg> parse_abo_anfrage(
    pugi::xml_document const& doc) {
  auto abo_anfrage = abo_anfrage_msg{};

  auto const sender_xpath = find_required_node(doc, "AboAnfrage/@Sender");
  if (!sender_xpath) {
    return std::nullopt;
  }
  abo_anfrage.sender_ = sender_xpath.attribute().value();

  auto const timestamp_xpath = find_required_node(doc, "AboAnfrage/@Zst");
  if (!timestamp_xpath) {
    return std::nullopt;
  }
  abo_anfrage.t_ = timestamp_from_string(timestamp_xpath.attribute().value());

  auto const abo_id_xpath = find_required_node(doc, "AboAnfrage/AboAUS/@AboID");
  if (!abo_id_xpath) {
    return std::nullopt;
  }
  abo_anfrage.abo_id_ = abo_id_xpath.attribute().as_uint();

  auto const expiration_t_xpath =
      find_required_node(doc, "AboAnfrage/AboAUS/@VerfallZst");
  if (!expiration_t_xpath) {
    return std::nullopt;
  }
  abo_anfrage.expiration_t_ =
      timestamp_from_string(expiration_t_xpath.attribute().value());

  auto const hysterese_xpath =
      find_required_node(doc, "AboAnfrage/AboAUS/Hysterese");
  if (!hysterese_xpath) {
    return std::nullopt;
  }
  abo_anfrage.hysteresis_ =
      std::chrono::seconds{std::stol(hysterese_xpath.node().child_value())};

  auto const look_ahead_xpath =
      find_required_node(doc, "AboAnfrage/AboAUS/Vorschauzeit");
  if (!look_ahead_xpath) {
    return std::nullopt;
  }
  abo_anfrage.look_ahead_ =
      std::chrono::minutes{std::stol(look_ahead_xpath.node().child_value())};

  return abo_anfrage;
}

std::optional<abo_antwort_msg> parse_abo_antwort(
    pugi::xml_document const& doc) {
  auto abo_antwort = abo_antwort_msg{};

  auto const timestamp_xpath =
      find_required_node(doc, "AboAntwort/Bestaetigung/@Zst");
  if (!timestamp_xpath) {
    return std::nullopt;
  }
  abo_antwort.t_ = timestamp_from_string(timestamp_xpath.attribute().value());

  auto const success_xpath =
      find_required_node(doc, "AboAntwort/Bestaetigung/@Ergebnis");
  if (!success_xpath) {
    return std::nullopt;
  }
  abo_antwort.success_ =
      std::string_view{success_xpath.attribute().value()} == "ok";

  auto const error_code_xpath =
      find_required_node(doc, "AboAntwort/Bestaetigung/@Fehlernummer");
  if (!error_code_xpath) {
    return std::nullopt;
  }
  abo_antwort.error_code_ = error_code_xpath.attribute().as_uint();

  return abo_antwort;
}

std::optional<daten_bereit_anfrage_msg> parse_daten_bereit_anfrage(
    pugi::xml_document const& doc) {
  auto daten_bereit_anfrage = daten_bereit_anfrage_msg{};

  auto const sender_xpath =
      find_required_node(doc, "DatenBereitAnfrage/@Sender");
  if (!sender_xpath) {
    return std::nullopt;
  }
  daten_bereit_anfrage.sender_ = sender_xpath.attribute().value();

  auto const timestamp_xpath =
      find_required_node(doc, "DatenBereitAnfrage/@Zst");
  if (!timestamp_xpath) {
    return std::nullopt;
  }
  daten_bereit_anfrage.t_ =
      timestamp_from_string(timestamp_xpath.attribute().value());

  return daten_bereit_anfrage;
}

std::optional<daten_bereit_antwort_msg> parse_daten_bereit_antwort(
    pugi::xml_document const& doc) {
  auto daten_bereit_antwort = daten_bereit_antwort_msg{};

  auto const timestamp_xpath =
      find_required_node(doc, "DatenBereitAntwort/Bestaetigung/@Zst");
  if (!timestamp_xpath) {
    return std::nullopt;
  }
  daten_bereit_antwort.t_ =
      timestamp_from_string(timestamp_xpath.attribute().value());

  auto const success_xpath =
      find_required_node(doc, "DatenBereitAntwort/Bestaetigung/@Ergebnis");
  if (!success_xpath) {
    return std::nullopt;
  }
  daten_bereit_antwort.success_ =
      std::string_view{success_xpath.attribute().value()} == "ok";

  auto const error_code_xpath =
      find_required_node(doc, "DatenBereitAntwort/Bestaetigung/@Fehlernummer");
  if (!error_code_xpath) {
    return std::nullopt;
  }
  daten_bereit_antwort.error_code_ = error_code_xpath.attribute().as_uint();

  return daten_bereit_antwort;
}

std::optional<daten_abrufen_anfrage_msg> parse_daten_abrufen_anfrage(
    pugi::xml_document const& doc) {
  auto daten_abrufen_anfrage = daten_abrufen_anfrage_msg{};

  auto const sender_xpath =
      find_required_node(doc, "DatenAbrufenAnfrage/@Sender");
  if (!sender_xpath) {
    return std::nullopt;
  }
  daten_abrufen_anfrage.sender_ = sender_xpath.attribute().value();

  auto const timestamp_xpath =
      find_required_node(doc, "DatenAbrufenAnfrage/@Zst");
  if (!timestamp_xpath) {
    return std::nullopt;
  }
  daten_abrufen_anfrage.t_ =
      timestamp_from_string(timestamp_xpath.attribute().value());

  auto const all_datasets_xpath =
      find_optional_node(doc, "DatenAbrufenAnfrage/DatensatzAlle");
  if (all_datasets_xpath) {
    std::istringstream{all_datasets_xpath.node().first_child().value()} >>
        std::boolalpha >> daten_abrufen_anfrage.all_datasets_;
  }

  return daten_abrufen_anfrage;
}

std::optional<daten_abrufen_antwort_msg> parse_daten_abrufen_antwort(
    pugi::xml_document const& doc) {
  auto daten_abrufen_antwort = daten_abrufen_antwort_msg{};

  auto const timestamp_xpath =
      find_required_node(doc, "DatenAbrufenAntwort/Bestaetigung/@Zst");
  if (!timestamp_xpath) {
    return std::nullopt;
  }
  daten_abrufen_antwort.t_ =
      timestamp_from_string(timestamp_xpath.attribute().value());

  auto const success_xpath =
      find_required_node(doc, "DatenAbrufenAntwort/Bestaetigung/@Ergebnis");
  if (!success_xpath) {
    return std::nullopt;
  }
  daten_abrufen_antwort.success_ =
      std::string_view{success_xpath.attribute().value()} == "ok";

  auto const error_code_xpath =
      find_required_node(doc, "DatenAbrufenAntwort/Bestaetigung/@Fehlernummer");
  if (!error_code_xpath) {
    return std::nullopt;
  }
  daten_abrufen_antwort.error_code_ = error_code_xpath.attribute().as_uint();

  auto const abo_id_xpath =
      find_required_node(doc, "DatenAbrufenAntwort/AUSNachricht/@AboID");
  if (!abo_id_xpath) {
    return std::nullopt;
  }
  daten_abrufen_antwort.abo_id_ = abo_id_xpath.attribute().as_uint();

  auto const runs_xpath =
      doc.select_nodes("DatenAbrufenAntwort/AUSNachricht/IstFahrt");
  for (auto const& run_xpath : runs_xpath) {
    auto run = vdv_run{};

    auto const run_t_xpath = find_required_node(run_xpath.node(), "@Zst");
    if (!run_t_xpath) {
      continue;
    }
    run.t_ = timestamp_from_string(run_t_xpath.attribute().value());

    auto const route_id_xpath =
        find_required_node(run_xpath.node(), "LinienID");
    if (!route_id_xpath) {
      continue;
    }
    run.route_id_ = route_id_xpath.node().child_value();

    auto const route_text_xpath =
        find_required_node(run_xpath.node(), "LinienText");
    if (!route_text_xpath) {
      continue;
    }
    run.route_text_ = route_text_xpath.node().child_value();

    auto const direction_id_xpath =
        find_required_node(run_xpath.node(), "RichtungsID");
    if (!direction_id_xpath) {
      continue;
    }
    run.direction_id_ = direction_id_xpath.node().child_value();

    auto const direction_text_xpath =
        find_required_node(run_xpath.node(), "RichtungsText");
    if (!direction_text_xpath) {
      continue;
    }
    run.direction_text_ = direction_text_xpath.node().child_value();

    auto const vehicle_xpath =
        find_required_node(run_xpath.node(), "ProduktID");
    if (!vehicle_xpath) {
      continue;
    }
    run.vehicle_ = vehicle_xpath.node().child_value();

    auto const trip_ref_xpath = find_required_node(
        run_xpath.node(), "FahrtRef/FahrtID/FahrtBezeichner");
    if (!trip_ref_xpath) {
      continue;
    }
    run.trip_ref_ = trip_ref_xpath.node().child_value();

    auto const operator_xpath =
        find_required_node(run_xpath.node(), "BetreiberID");
    if (!operator_xpath) {
      continue;
    }
    run.operator_ = operator_xpath.node().child_value();

    auto const date_xpath =
        find_required_node(run_xpath.node(), "FahrtRef/FahrtID/Betriebstag");
    if (!date_xpath) {
      continue;
    }
    auto ss = std::stringstream{date_xpath.node().child_value()};
    ss >> date::parse("%F", run.date_);

    auto const complete_xpath =
        find_required_node(run_xpath.node(), "Komplettfahrt");
    if (!complete_xpath) {
      continue;
    }
    std::istringstream{complete_xpath.node().child_value()} >> std::boolalpha >>
        run.complete_;

    auto const canceled_xpath =
        find_optional_node(run_xpath.node(), "FaelltAus");
    if (canceled_xpath) {
      std::istringstream{canceled_xpath.node().child_value()} >>
          std::boolalpha >> run.canceled_;
    }

    auto const additional_run_xpath =
        find_optional_node(run_xpath.node(), "Zusatzfahrt");
    if (additional_run_xpath) {
      std::istringstream{additional_run_xpath.node().child_value()} >>
          std::boolalpha >> run.additional_run_;
    }

    auto const stops_xpath = run_xpath.node().select_nodes("IstHalt");
    for (auto const& stop_xpath : stops_xpath) {
      auto stop = vdv_stop{};

      auto const stop_id_xpath =
          find_required_node(stop_xpath.node(), "HaltID");
      if (!stop_id_xpath) {
        continue;
      }
      stop.stop_id_ = stop_id_xpath.node().child_value();

      auto const platform_arr_xpath =
          find_optional_node(stop_xpath.node(), "AnkunftssteigText");
      if (platform_arr_xpath) {
        stop.platform_arr_ = platform_arr_xpath.node().child_value();
      }

      auto const platform_dep_xpath =
          find_optional_node(stop_xpath.node(), "AbfahrtssteigText");
      if (platform_dep_xpath) {
        stop.platform_dep_ = platform_dep_xpath.node().child_value();
      }

      auto const t_arr_xpath =
          find_optional_node(stop_xpath.node(), "Ankunftszeit");
      if (t_arr_xpath) {
        stop.t_arr_ = timestamp_from_string(t_arr_xpath.node().child_value());
      }

      auto const t_dep_xpath =
          find_optional_node(stop_xpath.node(), "Abfahrtszeit");
      if (t_dep_xpath) {
        stop.t_dep_ = timestamp_from_string(t_dep_xpath.node().child_value());
      }

      auto const t_arr_rt_xpath =
          find_optional_node(stop_xpath.node(), "IstAnkunftPrognose");
      if (t_arr_rt_xpath) {
        stop.t_arr_rt_ =
            timestamp_from_string(t_arr_rt_xpath.node().child_value());
      }

      auto const t_dep_rt_xpath =
          find_optional_node(stop_xpath.node(), "IstAbfahrtPrognose");
      if (t_dep_rt_xpath) {
        stop.t_dep_rt_ =
            timestamp_from_string(t_dep_rt_xpath.node().child_value());
      }

      auto const no_enter_xpath =
          find_required_node(stop_xpath.node(), "Einsteigeverbot");
      if (!no_enter_xpath) {
        continue;
      }
      std::istringstream{no_enter_xpath.node().child_value()} >>
          std::boolalpha >> stop.in_allowed_;
      stop.in_allowed_ = !stop.in_allowed_;

      auto const no_exit_xpath =
          find_required_node(stop_xpath.node(), "Aussteigeverbot");
      if (!no_exit_xpath) {
        continue;
      }
      std::istringstream{no_exit_xpath.node().child_value()} >>
          std::boolalpha >> stop.out_allowed_;
      stop.out_allowed_ = !stop.out_allowed_;

      auto const pass_through_xpath =
          find_required_node(stop_xpath.node(), "Durchfahrt");
      if (!pass_through_xpath) {
        continue;
      }
      auto pass_through = false;
      std::istringstream{pass_through_xpath.node().child_value()} >>
          std::boolalpha >> pass_through;
      if (pass_through) {
        stop.in_allowed_ = false;
        stop.out_allowed_ = false;
      }

      run.stops_.emplace_back(stop);
    }

    daten_abrufen_antwort.runs_.emplace_back(run);
  }

  return daten_abrufen_antwort;
}

std::optional<status_anfrage_msg> parse_status_anfrage(
    pugi::xml_document const& doc) {
  auto status_anfrage = status_anfrage_msg{};

  auto const sender_xpath = find_required_node(doc, "StatusAnfrage/@Sender");
  if (!sender_xpath) {
    return std::nullopt;
  }
  status_anfrage.sender_ = sender_xpath.attribute().value();

  auto const timestamp_xpath = find_required_node(doc, "StatusAnfrage/@Zst");
  if (!timestamp_xpath) {
    return std::nullopt;
  }
  status_anfrage.t_ =
      timestamp_from_string(timestamp_xpath.attribute().value());

  return status_anfrage;
}

std::optional<status_antwort_msg> parse_status_antwort(
    pugi::xml_document const& doc) {
  auto status_antwort = status_antwort_msg{};

  auto const timestamp_xpath =
      find_required_node(doc, "StatusAntwort/Status/@Zst");
  if (!timestamp_xpath) {
    return std::nullopt;
  }
  status_antwort.t_ =
      timestamp_from_string(timestamp_xpath.attribute().value());

  auto const success_xpath =
      find_required_node(doc, "StatusAntwort/Status/@Ergebnis");
  if (!success_xpath) {
    return std::nullopt;
  }
  status_antwort.success_ =
      std::string_view{success_xpath.attribute().value()} == "ok";

  auto const data_rdy_xpath =
      find_required_node(doc, "StatusAntwort/DatenBereit");
  if (!data_rdy_xpath) {
    return std::nullopt;
  }
  std::istringstream{data_rdy_xpath.node().child_value()} >> std::boolalpha >>
      status_antwort.data_rdy_;

  auto const start_time_xpath =
      find_required_node(doc, "StatusAntwort/StartDienstZst");
  if (!start_time_xpath) {
    return std::nullopt;
  }
  status_antwort.start_ =
      timestamp_from_string(start_time_xpath.node().child_value());

  return status_antwort;
}

std::optional<client_status_anfrage_msg> parse_client_status_anfrage(
    pugi::xml_document const& doc) {
  auto client_status_anfrage = client_status_anfrage_msg{};

  auto sender_xpath = find_required_node(doc, "ClientStatusAnfrage/@Sender");
  if (!sender_xpath) {
    return std::nullopt;
  }
  client_status_anfrage.sender_ = sender_xpath.attribute().value();

  auto timestamp_xpath = find_required_node(doc, "ClientStatusAnfrage/@Zst");
  if (!timestamp_xpath) {
    return std::nullopt;
  }
  client_status_anfrage.t_ =
      timestamp_from_string(timestamp_xpath.attribute().value());

  auto req_active_abos_xpath =
      find_optional_node(doc, "ClientStatusAnfrage/@MitAbos");
  if (req_active_abos_xpath) {
    std::istringstream{req_active_abos_xpath.attribute().value()} >>
        std::boolalpha >> client_status_anfrage.req_active_abos_;
  }

  return client_status_anfrage;
}

std::optional<client_status_antwort_msg> parse_client_status_antwort(
    pugi::xml_document const& doc) {
  auto client_status_antwort = client_status_antwort_msg{};

  auto const timestamp_xpath =
      find_required_node(doc, "ClientStatusAntwort/Status/@Zst");
  if (!timestamp_xpath) {
    return std::nullopt;
  }
  client_status_antwort.t_ =
      timestamp_from_string(timestamp_xpath.attribute().value());

  auto const success_xpath =
      find_required_node(doc, "ClientStatusAntwort/Status/@Ergebnis");
  if (!success_xpath) {
    return std::nullopt;
  }
  client_status_antwort.success_ =
      std::string_view{success_xpath.attribute().value()} == "ok";

  auto const start_time_xpath =
      find_required_node(doc, "ClientStatusAntwort/StartDienstZst");
  if (!start_time_xpath) {
    return std::nullopt;
  }
  client_status_antwort.start_ =
      timestamp_from_string(start_time_xpath.node().child_value());

  return client_status_antwort;
}

std::optional<vdv_msg> parse(std::string const& str) {
  auto doc = pugi::xml_document{};
  auto result = doc.load_string(str.c_str());

  // Error Handling from https://pugixml.org/docs/manual.html#loading
  if (!result) {
    std::cout << "XML [" << str << "] parsed with errors, attr value: ["
              << doc.child("node").attribute("attr").value()
              << "]\nError description: " << result.description()
              << "\nError offset: " << result.offset << " (error at [..."
              << (str.c_str() + result.offset) << "]\n\n";
    return std::nullopt;
  }

  static auto const parse_fun_map =
      std::unordered_map<std::string, std::function<std::optional<vdv_msg>(
                                          pugi::xml_document const&)>>{
          {"AboAnfrage", parse_abo_anfrage},
          {"AboAntwort", parse_abo_antwort},
          {"DatenBereitAnfrage", parse_daten_bereit_anfrage},
          {"DatenBereitAntwort", parse_daten_bereit_antwort},
          {"DatenAbrufenAnfrage", parse_daten_abrufen_anfrage},
          {"DatenAbrufenAntwort", parse_daten_abrufen_antwort},
          {"StatusAnfrage", parse_status_anfrage},
          {"StatusAntwort", parse_status_antwort},
          {"ClientStatusAnfrage", parse_client_status_anfrage},
          {"ClientStatusAntwort", parse_client_status_antwort}};

  auto const msg_type_node = doc.first_child();
  if (msg_type_node && parse_fun_map.contains(msg_type_node.name())) {
    return parse_fun_map.at(msg_type_node.name())(doc);
  }
  return std::nullopt;
}

}  // namespace vdv