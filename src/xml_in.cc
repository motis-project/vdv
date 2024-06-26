#include "vdv/xml_in.h"

#include <functional>
#include <iostream>
#include <unordered_map>

namespace vdv {

std::optional<abo_anfrage_msg> parse_abo_anfrage(
    pugi::xml_document const& doc) {
  auto abo_anfrage = abo_anfrage_msg{};

  auto const sender_xpath = doc.select_node("AboAnfrage/@Sender");
  if (!sender_xpath) {
    std::cout << "Warning: could not find Sender attribute\n";
    return std::nullopt;
  }
  abo_anfrage.sender_ = sender_xpath.attribute().value();

  auto const timestamp_xpath = doc.select_node("AboAnfrage/@Zst");
  if (!timestamp_xpath) {
    std::cout << "Warning: could not find Zst attribute\n";
    return std::nullopt;
  }
  abo_anfrage.t_ = timestamp_from_string(timestamp_xpath.attribute().value());

  auto const abo_id_xpath = doc.select_node("AboAnfrage/AboAUS/@AboID");
  if (!abo_id_xpath) {
    std::cout << "Warning: could not find AboID attribute\n";
    return std::nullopt;
  }
  abo_anfrage.abo_id_ = abo_id_xpath.attribute().as_uint();

  auto const expiration_t_xpath =
      doc.select_node("AboAnfrage/AboAUS/@VerfallZst");
  if (!expiration_t_xpath) {
    std::cout << "Warning: could not find VerfallZst attribute\n";
    return std::nullopt;
  }
  abo_anfrage.expiration_t_ =
      timestamp_from_string(expiration_t_xpath.attribute().value());

  auto const hysterese_xpath = doc.select_node("AboAnfrage/AboAUS/Hysterese");
  if (!hysterese_xpath) {
    std::cout << "Warning: could not find Hysterese\n";
    return std::nullopt;
  }
  abo_anfrage.hysteresis_ =
      std::chrono::seconds{std::stol(hysterese_xpath.node().child_value())};

  auto const look_ahead_xpath =
      doc.select_node("AboAnfrage/AboAUS/Vorschauzeit");
  if (!look_ahead_xpath) {
    std::cout << "Warning: could not find Vorschauzeit\n";
    return std::nullopt;
  }
  abo_anfrage.look_ahead_ =
      std::chrono::minutes{std::stol(look_ahead_xpath.node().child_value())};

  return abo_anfrage;
}

std::optional<abo_antwort_msg> parse_abo_antwort(
    pugi::xml_document const& doc) {
  auto abo_antwort = abo_antwort_msg{};

  auto const timestamp_xpath = doc.select_node("AboAntwort/Bestaetigung/@Zst");
  if (!timestamp_xpath) {
    std::cout << "Warning: could not find Zst attribute\n";
    return std::nullopt;
  }
  abo_antwort.t_ = timestamp_from_string(timestamp_xpath.attribute().value());

  auto const success_xpath =
      doc.select_node("AboAntwort/Bestaetigung/@Ergebnis");
  if (!success_xpath) {
    std::cout << "Warning: could not find Ergebnis attribute\n";
    return std::nullopt;
  }
  abo_antwort.success_ =
      std::string_view{success_xpath.attribute().value()} == "ok";

  auto const error_code_xpath =
      doc.select_node("AboAntwort/Bestaetigung/@Fehlernummer");
  if (!error_code_xpath) {
    std::cout << "Warning: could not find Fehlernummer attribute\n";
    return std::nullopt;
  }
  abo_antwort.error_code_ = error_code_xpath.attribute().as_uint();

  return abo_antwort;
}

std::optional<daten_bereit_anfrage_msg> parse_daten_bereit_anfrage(
    pugi::xml_document const& doc) {
  auto daten_bereit_anfrage = daten_bereit_anfrage_msg{};

  auto const sender_xpath = doc.select_node("DatenBereitAnfrage/@Sender");
  if (!sender_xpath) {
    std::cout << "Warning: Could not find Sender attribute\n";
    return std::nullopt;
  }
  daten_bereit_anfrage.sender_ = sender_xpath.attribute().value();

  auto const timestamp_xpath = doc.select_node("DatenBereitAnfrage/@Zst");
  if (!timestamp_xpath) {
    std::cout << "Warning: Could not find Zst attribute\n";
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
      doc.select_node("DatenBereitAntwort/Bestaetigung/@Zst");
  if (!timestamp_xpath) {
    std::cout << "Warning: could not find Zst attribute\n";
    return std::nullopt;
  }
  daten_bereit_antwort.t_ =
      timestamp_from_string(timestamp_xpath.attribute().value());

  auto const success_xpath =
      doc.select_node("DatenBereitAntwort/Bestaetigung/@Ergebnis");
  if (!success_xpath) {
    std::cout << "Warning: could not find Ergebnis attribute\n";
    return std::nullopt;
  }
  daten_bereit_antwort.success_ =
      std::string_view{success_xpath.attribute().value()} == "ok";

  auto const error_code_xpath =
      doc.select_node("DatenBereitAntwort/Bestaetigung/@Fehlernummer");
  if (!error_code_xpath) {
    std::cout << "Warning: could not find Fehlernummer attribute\n";
    return std::nullopt;
  }
  daten_bereit_antwort.error_code_ = error_code_xpath.attribute().as_uint();

  return daten_bereit_antwort;
}

std::optional<daten_abrufen_anfrage_msg> parse_daten_abrufen_anfrage(
    pugi::xml_document const& doc) {
  auto daten_abrufen_anfrage = daten_abrufen_anfrage_msg{};

  auto const sender_xpath = doc.select_node("DatenAbrufenAnfrage/@Sender");
  if (!sender_xpath) {
    std::cout << "Warning: could not find Sender attribute\n";
    return std::nullopt;
  }
  daten_abrufen_anfrage.sender_ = sender_xpath.attribute().value();

  auto const timestamp_xpath = doc.select_node("DatenAbrufenAnfrage/@Zst");
  if (!timestamp_xpath) {
    std::cout << "Warning: could not find Zst attribute\n";
    return std::nullopt;
  }
  daten_abrufen_anfrage.t_ =
      timestamp_from_string(timestamp_xpath.attribute().value());

  auto const all_datasets_xpath =
      doc.select_node("DatenAbrufenAnfrage/DatensatzAlle");
  if (all_datasets_xpath) {
    std::istringstream{all_datasets_xpath.node().first_child().value()} >>
        std::boolalpha >> daten_abrufen_anfrage.all_datasets_;
  }

  return daten_abrufen_anfrage;
}

std::optional<status_anfrage_msg> parse_status_anfrage(
    pugi::xml_document const& doc) {
  auto status_anfrage = status_anfrage_msg{};

  auto const sender_xpath = doc.select_node("StatusAnfrage/@Sender");
  if (!sender_xpath) {
    std::cout << "Warning: could not find Sender attribute\n";
    return std::nullopt;
  }
  status_anfrage.sender_ = sender_xpath.attribute().value();

  auto const timestamp_xpath = doc.select_node("StatusAnfrage/@Zst");
  if (!timestamp_xpath) {
    std::cout << "Warning: could not find Zst attribute\n";
    return std::nullopt;
  }
  status_anfrage.t_ =
      timestamp_from_string(timestamp_xpath.attribute().value());

  return status_anfrage;
}

std::optional<status_antwort_msg> parse_status_antwort(
    pugi::xml_document const& doc) {
  auto status_antwort = status_antwort_msg{};

  auto const timestamp_xpath = doc.select_node("StatusAntwort/Status/@Zst");
  if (!timestamp_xpath) {
    std::cout << "Warning: could not find Zst attribute\n";
    return std::nullopt;
  }
  status_antwort.t_ =
      timestamp_from_string(timestamp_xpath.attribute().value());

  auto const success_xpath = doc.select_node("StatusAntwort/Status/@Ergebnis");
  if (!success_xpath) {
    std::cout << "Warning: could not find Ergebnis attribute\n";
    return std::nullopt;
  }
  status_antwort.success_ =
      std::string_view{success_xpath.attribute().value()} == "ok";

  auto const data_rdy_xpath = doc.select_node("StatusAntwort/DatenBereit");
  if (!data_rdy_xpath) {
    std::cout << "Warning: could not find DatenBereit\n";
    return std::nullopt;
  }
  std::istringstream{data_rdy_xpath.node().child_value()} >> std::boolalpha >>
      status_antwort.data_rdy_;

  auto const start_time_xpath = doc.select_node("StatusAntwort/StartDienstZst");
  if (!start_time_xpath) {
    std::cout << "Warning: could not find StartDienstZst\n";
    return std::nullopt;
  }
  status_antwort.start_ =
      timestamp_from_string(start_time_xpath.node().child_value());

  return status_antwort;
}

static auto parse_fun_map = std::unordered_map<
    std::string,
    std::function<std::optional<vdv_msg>(pugi::xml_document const&)>>{
    {"AboAnfrage", parse_abo_anfrage},
    {"AboAntwort", parse_abo_antwort},
    {"DatenBereitAnfrage", parse_daten_bereit_anfrage},
    {"DatenBereitAntwort", parse_daten_bereit_antwort},
    {"DatenAbrufenAnfrage", parse_daten_abrufen_anfrage},
    {"StatusAnfrage", parse_status_anfrage},
    {"StatusAntwort", parse_status_antwort}};

std::optional<vdv_msg> parse(std::string const& str) {
  auto doc = pugi::xml_document{};
  auto result = doc.load_string(str.c_str());

  // Error Handling from https://pugixml.org/docs/manual.html#loading
  if (!result) {
    std::cout << "XML [" << str << "] parsed with errors, attr value: ["
              << doc.child("node").attribute("attr").value() << "]\n";
    std::cout << "Error description: " << result.description() << "\n";
    std::cout << "Error offset: " << result.offset << " (error at [..."
              << (str.c_str() + result.offset) << "]\n\n";
    return std::nullopt;
  }

  if (!doc.empty() && parse_fun_map.contains(doc.first_child().name())) {
    return parse_fun_map[doc.first_child().name()](doc);
  }
  std::cout << "Warning: no applicable parsing function found\n";
  return std::nullopt;
}

}  // namespace vdv