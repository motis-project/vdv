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

bool get_bool_with_fallback(pugi::xml_node const& node,
                            std::string const& str,
                            bool fallback) {
  auto const xpath = get_opt(node, str);
  return xpath ? parse_bool(xpath.node().child_value()) : fallback;
}

bool get_bool_attr_with_fallback(pugi::xml_node const& node,
                                 std::string const& str,
                                 bool fallback) {
  auto const xpath = get_opt(node, str);
  return xpath ? parse_bool(xpath.attribute().value()) : fallback;
}

std::optional<bool> get_opt_bool(pugi::xml_node const& node,
                                 std::string const& str) {
  auto const xpath = get_opt(node, str);
  return xpath ? std::optional{parse_bool(xpath.node().child_value())}
               : std::nullopt;
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
          .all_datasets_ = get_bool_with_fallback(
              doc, "DatenAbrufenAnfrage/DatensatzAlle", false)};
}

daten_abrufen_antwort_msg parse_daten_abrufen_antwort(
    pugi::xml_document const& doc) {
  auto const get_runs = [&]() {
    auto const get_run = [](pugi::xml_node const& run_node) -> vdv_run {
      auto const get_stops = [&]() {
        auto const get_stop = [](pugi::xml_node const& stop_node) -> vdv_stop {
          auto const pass_through = get_opt_bool(stop_node, "Durchfahrt");
          auto const no_enter = get_opt_bool(stop_node, "Einsteigeverbot");
          auto const no_exit = get_opt_bool(stop_node, "Aussteigeverbot");
          return {
              .stop_id_ = get_req(stop_node, "HaltID").node().child_value(),
              .platform_arr_ = get_opt_str(stop_node, "AnkunftssteigText"),
              .platform_dep_ = get_opt_str(stop_node, "AbfahrtssteigText"),
              .t_arr_ = get_opt_timestamp(stop_node, "Ankunftszeit"),
              .t_dep_ = get_opt_timestamp(stop_node, "Abfahrtszeit"),
              .t_arr_rt_ = get_opt_timestamp(stop_node, "IstAnkunftPrognose"),
              .t_dep_rt_ = get_opt_timestamp(stop_node, "IstAbfahrtPrognose"),
              .in_allowed_ =
                  pass_through.has_value() && pass_through.value() ? false
                  : no_enter.has_value() ? std::optional{!no_enter.value()}
                                         : std::nullopt,
              .out_allowed_ =
                  pass_through.has_value() && pass_through.value() ? false
                  : no_exit.has_value() ? std::optional{!no_exit.value()}
                                        : std::nullopt,
              .additional_stop_ =
                  get_bool_with_fallback(stop_node, "Zusatzhalt", false)};
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
          .canceled_ = get_bool_with_fallback(run_node, "FaelltAus", false),
          .additional_run_ =
              get_bool_with_fallback(run_node, "Zusatzfahrt", false),
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

status_anfrage_msg parse_status_anfrage(pugi::xml_document const& doc) {
  return {.t_ = parse_timestamp(
              get_req(doc, "StatusAnfrage/@Zst").attribute().value()),
          .sender_ = get_req(doc, "StatusAnfrage/@Sender").attribute().value()};
}

status_antwort_msg parse_status_antwort(pugi::xml_document const& doc) {
  return {
      .t_ = parse_timestamp(
          get_req(doc, "StatusAntwort/Status/@Zst").attribute().value()),
      .success_ = parse_success(
          get_req(doc, "StatusAntwort/Status/@Ergebnis").attribute().value()),
      .data_rdy_ =
          get_bool_with_fallback(doc, "StatusAntwort/DatenBereit", false),
      .start_ = parse_timestamp(
          get_req(doc, "StatusAntwort/StartDienstZst").node().child_value())};
}

client_status_anfrage_msg parse_client_status_anfrage(
    pugi::xml_document const& doc) {
  return {.t_ = parse_timestamp(
              get_req(doc, "ClientStatusAnfrage/@Zst").attribute().value()),
          .sender_ =
              get_req(doc, "ClientStatusAnfrage/@Sender").attribute().value(),
          .req_active_abos_ = get_bool_attr_with_fallback(
              doc, "ClientStatusAnfrage/@MitAbos", false)};
}

client_status_antwort_msg parse_client_status_antwort(
    pugi::xml_document const& doc) {
  return {
      .t_ = parse_timestamp(
          get_req(doc, "ClientStatusAntwort/Status/@Zst").attribute().value()),
      .success_ =
          parse_success(get_req(doc, "ClientStatusAntwort/Status/@Ergebnis")
                            .attribute()
                            .value()),
      .start_ =
          parse_timestamp(get_req(doc, "ClientStatusAntwort/StartDienstZst")
                              .node()
                              .child_value()),
      .subscriptions_ = std::vector<subscription>{}};
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