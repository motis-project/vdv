#include "vdv/xml.h"
#include <vector>
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

void add_sender_zst_attr(pugi::xml_node& node,
                         std::string const& sender,
                         timestamp_t const t) {
  node.append_attribute("Sender") = sender.c_str();
  node.append_attribute("Zst") = timestamp_to_string(t).c_str();
}

void add_start_dienst_zst_node(pugi::xml_node& node, timestamp_t const start) {
  auto start_dienst_zst_node = node.append_child("StartDienstZst");
  start_dienst_zst_node.append_child(pugi::node_pcdata)
      .set_value(timestamp_to_string(start).c_str());
}

pugi::xml_node add_abo_anfrage_node(pugi::xml_node& node,
                                    std::string const& sender,
                                    timestamp_t const t) {
  auto abo_anfrage_node = node.append_child("AboAnfrage");
  add_sender_zst_attr(abo_anfrage_node, sender, t);
  return abo_anfrage_node;
}

void add_abo_loeschen_node(pugi::xml_node& node, std::uint32_t const abo_id) {
  auto abo_loeschen_node = node.append_child("AboLoeschen");
  abo_loeschen_node.append_child(pugi::node_pcdata)
      .set_value(std::to_string(abo_id).c_str());
}

void add_bestaetigung_node(pugi::xml_node& node,
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
  auto abo_anfrage_node = add_abo_anfrage_node(doc, sender, t);
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

std::string abo_loeschen_anfrage_xml_str(
    std::string const& sender,
    timestamp_t const t,
    std::vector<std::uint32_t> const& abo_ids) {
  auto doc = make_xml_doc();
  auto abo_anfrage_node = add_abo_anfrage_node(doc, sender, t);
  for (auto abo_id : abo_ids) {
    add_abo_loeschen_node(abo_anfrage_node, abo_id);
  }
  return xml_to_str(doc);
}

std::string abo_antwort_xml_str(timestamp_t const t,
                                bool const success,
                                std::uint32_t const error_code) {
  auto doc = make_xml_doc();
  auto abo_antwort_node = doc.append_child("AboAntwort");
  add_bestaetigung_node(abo_antwort_node, t, success, error_code);
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
  add_bestaetigung_node(daten_bereit_antwort_node, t, success, error_code);
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
  if (all_datasets) {
    auto datensatz_alle_node =
        daten_abrufen_anfrage_node.append_child("DatensatzAlle");
    datensatz_alle_node.append_child(pugi::node_pcdata).set_value("true");
  }
  return xml_to_str(doc);
}

std::string status_anfrage_xml_str(std::string const& sender,
                                   timestamp_t const t) {
  auto doc = make_xml_doc();
  auto status_anfrage_node = doc.append_child("StatusAnfrage");
  add_sender_zst_attr(status_anfrage_node, sender, t);
  return xml_to_str(doc);
}

std::string status_antwort_xml_str(timestamp_t const t,
                                   bool const success,
                                   bool const data_rdy,
                                   timestamp_t const start) {
  auto doc = make_xml_doc();
  auto status_antwort_node = doc.append_child("StatusAntwort");
  auto status_node = status_antwort_node.append_child("Status");
  status_node.append_attribute("Zst") = timestamp_to_string(t).c_str();
  status_node.append_attribute("Ergebnis") = success ? "ok" : "notok";
  auto daten_bereit_node = status_antwort_node.append_child("DatenBereit");
  daten_bereit_node.append_child(pugi::node_pcdata)
      .set_value(data_rdy ? "true" : "false");
  add_start_dienst_zst_node(status_antwort_node, start);
  return xml_to_str(doc);
}

std::string client_status_anfrage_xml_str(std::string const& sender,
                                          timestamp_t const t,
                                          bool const req_active_abos) {
  auto doc = make_xml_doc();
  auto client_status_anfrage_node = doc.append_child("ClientStatusAnfrage");
  add_sender_zst_attr(client_status_anfrage_node, sender, t);
  if (req_active_abos) {
    client_status_anfrage_node.append_attribute("MitAbos") = "true";
  }
  return xml_to_str(doc);
}

}  // namespace vdv
