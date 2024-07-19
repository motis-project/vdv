#include "vdv/xml_in.h"

#include <functional>
#include <iostream>
#include <unordered_map>

#include "date/date.h"

#include "utl/parser/arg_parser.h"
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

std::optional<sys_time> get_opt_timestamp(pugi::xml_node const& node,
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
                         .as_uint()};
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
          .query_active_subs_ = get_bool_attr_with_fallback(
              doc, "ClientStatusAnfrage/@MitAbos", false)};
}

client_status_antwort_msg parse_client_status_antwort(
    pugi::xml_document const& doc) {
  auto const get_subs = [&]() {
    auto subs = std::vector<abo_id_t>{};
    for (auto const abo_xpath :
         doc.select_nodes("ClientStatusAntwort/AktiveAbos/AboAUS")) {
      subs.emplace_back(abo_xpath.node().attribute("AboID").as_uint());
    }
    return subs;
  };

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
      .active_subs_ = get_subs()};
}

vdv_msg parse(std::string const& str) {
  auto doc = pugi::xml_document{};
  auto result = doc.load_string(str.c_str());

  utl::verify(result, "XML [{}] parsed with errors: {}\n", str,
              result.description());

  static auto const parse_fun_map =
      std::unordered_map<std::string,
                         std::function<vdv_msg(pugi::xml_document const&)>>{
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
  utl::verify(parse_fun_map.contains(msg_type_node.name()),
              "Can not parse message type {}", msg_type_node.name());

  return parse_fun_map.at(msg_type_node.name())(doc);
}

}  // namespace vdv