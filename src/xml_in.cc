#include "vdv/xml_in.h"

#include <functional>
#include <iostream>
#include <unordered_map>

#include "date/date.h"

#include "utl/verify.h"
#include "vdv/types.h"

namespace vdv {

pugi::xpath_node get_opt(pugi::xml_node const& node, std::string const& str) {
  return node.select_node(str.c_str());
}

std::optional<std::string> get_opt_str(pugi::xml_node const& node,
                                       std::string const& str) {
  auto const xpath = get_opt(node, str);
  return xpath ? std::optional{xpath.node().child_value()} : std::nullopt;
}

std::optional<timestamp_t> get_opt_timestamp(pugi::xml_node const& node,
                                             std::string const& str) {
  auto const xpath = get_opt(node, str);
  return xpath ? std::optional{parse_timestamp(xpath.node().child_value())}
               : std::nullopt;
}

pugi::xpath_node get_req(pugi::xml_node const& node, std::string const& str) {
  auto const xpath = get_opt(node, str);
  utl::verify(xpath, "required xml node not found: {}", str);
  return xpath;
}

bool parse_success(std::string_view str) { return str == "ok"; }

bool parse_bool(std::string const& str) {
  auto result = false;
  std::istringstream{str} >> std::boolalpha >> result;
  return result;
}

bool get_opt_bool(pugi::xml_node const& node,
                  std::string const& str,
                  bool fallback) {
  auto const xpath = get_opt(node, str);
  return xpath ? parse_bool(xpath.node().child_value()) : fallback;
}

date::sys_days parse_date(std::string const& str) {
  auto result = date::sys_days{};
  auto ss = std::stringstream{str};
  ss >> date::parse("%F", result);
  return result;
}

abo_anfrage_msg parse_abo_anfrage(pugi::xml_document const& doc) {
  return {
      .t_ =
          parse_timestamp(get_req(doc, "AboAnfrage/@Zst").attribute().value()),
      .sender_ = get_req(doc, "AboAnfrage/@Sender").attribute().value(),
      .abo_id_ = get_req(doc, "AboAnfrage/AboAUS/@AboID").attribute().as_uint(),
      .expiration_t_ = parse_timestamp(
          get_req(doc, "AboAnfrage/AboAUS/@VerfallZst").attribute().value()),
      .hysteresis_ = std::chrono::seconds{std::stol(
          get_req(doc, "AboAnfrage/AboAUS/Hysterese").node().child_value())},
      .look_ahead_ = std::chrono::minutes{
          std::stol(get_req(doc, "AboAnfrage/AboAUS/Vorschauzeit")
                        .node()
                        .child_value())}};
}

abo_antwort_msg parse_abo_antwort(pugi::xml_document const& doc) {
  return {.t_ = parse_timestamp(
              get_req(doc, "AboAntwort/Bestaetigung/@Zst").attribute().value()),
          .success_ =
              std::string_view{get_req(doc, "AboAntwort/Bestaetigung/@Ergebnis")
                                   .attribute()
                                   .value()} == "ok",
          .error_code_ = get_req(doc, "AboAntwort/Bestaetigung/@Fehlernummer")
                             .attribute()
                             .as_uint()};
}

daten_bereit_anfrage_msg parse_daten_bereit_anfrage(
    pugi::xml_document const& doc) {
  return {.t_ = parse_timestamp(
              get_req(doc, "DatenBereitAnfrage/@Zst").attribute().value()),
          .sender_ =
              get_req(doc, "DatenBereitAnfrage/@Sender").attribute().value()};
}

daten_bereit_antwort_msg parse_daten_bereit_antwort(
    pugi::xml_document const& doc) {
  return {
      .t_ = parse_timestamp(get_req(doc, "DatenBereitAntwort/Bestaetigung/@Zst")
                                .attribute()
                                .value()),
      .success_ = parse_success(
          get_req(doc, "DatenBereitAntwort/Bestaetigung/@Ergebnis")
              .attribute()
              .value()),
      .error_code_ =
          get_req(doc, "DatenBereitAntwort/Bestaetigung/@Fehlernummer")
              .attribute()
              .as_uint()};
}

daten_abrufen_anfrage_msg parse_daten_abrufen_anfrage(
    pugi::xml_document const& doc) {
  return {.t_ = parse_timestamp(
              get_req(doc, "DatenAbrufenAnfrage/@Zst").attribute().value()),
          .sender_ =
              get_req(doc, "DatenAbrufenAnfrage/@Sender").attribute().value(),
          .all_datasets_ =
              get_opt_bool(doc, "DatenAbrufenAnfrage/DatensatzAlle", false)};
}

daten_abrufen_antwort_msg parse_daten_abrufen_antwort(
    pugi::xml_document const& doc) {
  auto const get_runs = [&]() {
    auto const get_run = [](pugi::xml_node const& run_node) -> vdv_run {
      auto const get_stops = [&]() {
        auto const get_stop = [](pugi::xml_node const& stop_node) -> vdv_stop {
          auto const pass_through =
              parse_bool(get_req(stop_node, "Durchfahrt").node().child_value());
          return {
              .stop_id_ = get_req(stop_node, "HaltID").node().child_value(),
              .platform_arr_ = get_opt_str(stop_node, "AnkunftssteigText"),
              .platform_dep_ = get_opt_str(stop_node, "AbfahrtssteigText"),
              .t_arr_ = get_opt_timestamp(stop_node, "Ankunftszeit"),
              .t_dep_ = get_opt_timestamp(stop_node, "Abfahrtszeit"),
              .t_arr_rt_ = get_opt_timestamp(stop_node, "IstAnkunftPrognose"),
              .t_dep_rt_ = get_opt_timestamp(stop_node, "IstAbfahrtPrognose"),
              .in_allowed_ = !pass_through &&
                             !parse_bool(get_req(stop_node, "Einsteigeverbot")
                                             .node()
                                             .child_value()),
              .out_allowed_ = !pass_through &&
                              !parse_bool(get_req(stop_node, "Aussteigeverbot")
                                              .node()
                                              .child_value()),
              .additional_stop_ = parse_bool(get)};
        };
        auto stops = std::vector<vdv_stop>{};
        auto const stops_xpath = run_node.select_nodes("IstHalt");
        for (auto const& stop_xpath : stops_xpath) {
          stops.emplace_back(get_stop(stop_xpath.node()));
        }
        return stops;
      };
      return {
          .t_ = parse_timestamp(get_req(run_node, "@Zst").attribute().value()),
          .route_id_ = get_req(run_node, "LinienID").node().child_value(),
          .route_text_ = get_req(run_node, "LinienText").node().child_value(),
          .direction_id_ =
              get_req(run_node, "RichtungsID").node().child_value(),
          .direction_text_ =
              get_req(run_node, "RichtungsText").node().child_value(),
          .vehicle_ = get_req(run_node, "ProduktID").node().child_value(),
          .trip_ref_ = get_req(run_node, "FahrtRef/FahrtID/FahrtBezeichner")
                           .node()
                           .child_value(),
          .operator_ = get_req(run_node, "BetreiberID").node().child_value(),
          .date_ = parse_date(get_req(run_node, "FahrtRef/FahrtID/Betriebstag")
                                  .node()
                                  .child_value()),
          .complete_ = parse_bool(
              get_req(run_node, "Komplettfahrt").node().child_value()),
          .canceled_ = get_opt_bool(run_node, "FaelltAus", false),
          .additional_run_ = get_opt_bool(run_node, "Zusatzfahrt", false),
          .stops_ = get_stops()};
    };

    auto runs = std::vector<vdv_run>{};
    auto const runs_xpath =
        doc.select_nodes("DatenAbrufenAntwort/AUSNachricht/IstFahrt");
    for (auto const& run_xpath : runs_xpath) {
      runs.emplace_back(get_run(run_xpath.node()));
    }
    return runs;
  };

  return {.t_ = parse_timestamp(
              get_req(doc, "DatenAbrufenAntwort/Bestaetigung/@Zst")
                  .attribute()
                  .value()),
          .success_ = parse_success(
              get_req(doc, "DatenAbrufenAntwort/Bestaetigung/@Ergebnis")
                  .attribute()
                  .value()),
          .error_code_ =
              get_req(doc, "DatenAbrufenAntwort/Bestaetigung/@Fehlernummer")
                  .attribute()
                  .as_uint(),
          .abo_id_ = get_req(doc, "DatenAbrufenAntwort/AUSNachricht/@AboID")
                         .attribute()
                         .as_uint(),
          .runs_ = get_runs()};
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
  status_anfrage.t_ = parse_timestamp(timestamp_xpath.attribute().value());

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
  status_antwort.t_ = parse_timestamp(timestamp_xpath.attribute().value());

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
      parse_timestamp(start_time_xpath.node().child_value());

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
      parse_timestamp(timestamp_xpath.attribute().value());

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
      parse_timestamp(timestamp_xpath.attribute().value());

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
      parse_timestamp(start_time_xpath.node().child_value());

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