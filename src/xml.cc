#include "vdv/xml.h"
#include "vdv/timestamp.h"
#include "vdv/types.h"

namespace vdv {

pugi::xml_document make_xml_doc() {
  auto doc = pugi::xml_document{};
  auto decl = doc.prepend_child(pugi::node_declaration);
  decl.append_attribute("version") = "1.0";
  decl.append_attribute("encoding") = "iso-8859-1";
  return doc;
}

void add_bestaetigung(pugi::xml_node& node,
                      timestamp_t const t,
                      bool const success,
                      std::uint32_t const error_code) {
  auto bestaetigung_node = node.append_child("Bestaetigung");
  bestaetigung_node.append_attribute("Zst") = timestamp_to_string(t).c_str();
  bestaetigung_node.append_attribute("Ergebnis") = success ? "ok" : "notok";
  bestaetigung_node.append_attribute("Fehlernummer") =
      std::to_string(error_code).c_str();
}

std::string xml_to_str(pugi::xml_document const& doc) {
  std::stringstream ss{};
  doc.save(ss, "  ", pugi::format_default, pugi::xml_encoding::encoding_latin1);
  return ss.str();
}

std::string abo_anfrage_xml_str(std::string const& sender,
                                timestamp_t const t,
                                std::uint32_t const abo_id,
                                std::chrono::seconds const hysteresis,
                                std::chrono::minutes const look_ahead) {
  auto doc = make_xml_doc();
  auto abo_anfrage_node = doc.append_child("AboAnfrage");
  abo_anfrage_node.append_attribute("Sender") = sender.c_str();
  abo_anfrage_node.append_attribute("Zst") = timestamp_to_string(t).c_str();
  auto abo_aus_node = abo_anfrage_node.append_child("AboAUS");
  abo_aus_node.append_attribute("AboID") = std::to_string(abo_id).c_str();
  abo_aus_node.append_attribute("VerfallZst") =
      timestamp_to_string(t + look_ahead).c_str();
  auto hysterese_node = abo_aus_node.append_child("Hysterese");
  hysterese_node.append_child(pugi::node_pcdata)
      .set_value(std::to_string(hysteresis.count()).c_str());
  auto vorschauzeit_node = abo_aus_node.append_child("Vorschauzeit");
  vorschauzeit_node.append_child(pugi::node_pcdata)
      .set_value(std::to_string(look_ahead.count()).c_str());
  return xml_to_str(doc);
}

std::string abo_antwort_xml_str(timestamp_t const t,
                                bool const success,
                                std::uint32_t const error_code) {
  auto doc = make_xml_doc();
  auto abo_antwort_node = doc.append_child("AboAntwort");
  add_bestaetigung(abo_antwort_node, t, success, error_code);
  return xml_to_str(doc);
}

std::string daten_bereit_anfrage_xml_str(std::string const& sender,
                                         timestamp_t const t) {
  auto doc = make_xml_doc();
  auto daten_bereit_anfrage_node = doc.append_child("DatenBereitAnfrage");
  daten_bereit_anfrage_node.append_attribute("Sender") = sender.c_str();
  daten_bereit_anfrage_node.append_attribute("Zst") =
      timestamp_to_string(t).c_str();
  return xml_to_str(doc);
}

std::string daten_bereit_antwort_xml_str(timestamp_t const t,
                                         bool const success,
                                         std::uint32_t const error_code) {
  auto doc = make_xml_doc();
  auto daten_bereit_antwort_node = doc.append_child("DatenBereitAntwort");
  add_bestaetigung(daten_bereit_antwort_node, t, success, error_code);
  return xml_to_str(doc);
}

std::string daten_abrufen_anfrage_xml_str(std::string const& sender,
                                          timestamp_t const t,
                                          bool const all_datasets) {
  auto doc = make_xml_doc();
  auto daten_abrufen_anfrage_node = doc.append_child("DatenAbrufenAnfrage");
  daten_abrufen_anfrage_node.append_attribute("Sender") = sender.c_str();
  daten_abrufen_anfrage_node.append_attribute("Zst") =
      timestamp_to_string(t).c_str();
  auto datensatz_alle_node =
      daten_abrufen_anfrage_node.append_child("DatensatzAlle");
  datensatz_alle_node.append_child(pugi::node_pcdata)
      .set_value(all_datasets ? "true" : "false");
  return xml_to_str(doc);
}

}  // namespace vdv
