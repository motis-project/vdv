#include "vdv/xml_out.h"
#include <vector>
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
                         sys_time const t) {
  node.append_attribute("Sender") = sender.c_str();
  node.append_attribute("Zst") = timestamp(t).c_str();
}

void add_start_dienst_zst_node(pugi::xml_node& node, sys_time const start) {
  auto start_dienst_zst_node = node.append_child("StartDienstZst");
  start_dienst_zst_node.append_child(pugi::node_pcdata)
      .set_value(timestamp(start).c_str());
}

pugi::xml_node add_abo_anfrage_node(pugi::xml_node& node,
                                    std::string const& sender,
                                    sys_time const t) {
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
                           sys_time const t,
                           bool const success,
                           std::uint32_t const error_code) {
  auto bestaetigung_node = node.append_child("Bestaetigung");
  bestaetigung_node.append_attribute("Zst") = timestamp(t).c_str();
  bestaetigung_node.append_attribute("Ergebnis") = success ? "ok" : "notok";
  bestaetigung_node.append_attribute("Fehlernummer") =
      std::to_string(error_code).c_str();
}

void add_status_node(pugi::xml_node& node,
                     sys_time const t,
                     bool const success) {
  auto status_node = node.append_child("Status");
  status_node.append_attribute("Zst") = timestamp(t).c_str();
  status_node.append_attribute("Ergebnis") = success ? "ok" : "notok";
}

std::string xml_to_str(pugi::xml_document const& doc) {
  std::stringstream ss{};
  doc.save(ss, "  ", pugi::format_default, pugi::xml_encoding::encoding_latin1);
  return ss.str();
}

std::string abo_anfrage_xml_str(std::string const& sender,
                                sys_time const start,
                                sys_time const end,
                                std::uint64_t const abo_id,
                                std::chrono::seconds const hysteresis,
                                std::chrono::minutes const look_ahead) {
  auto doc = make_xml_doc();
  auto abo_anfrage_node = add_abo_anfrage_node(doc, sender, start);
  auto abo_aus_node = abo_anfrage_node.append_child("AboAUS");
  abo_aus_node.append_attribute("AboID") = std::to_string(abo_id).c_str();
  abo_aus_node.append_attribute("VerfallZst") = timestamp(end).c_str();
  auto hysterese_node = abo_aus_node.append_child("Hysterese");
  hysterese_node.append_child(pugi::node_pcdata)
      .set_value(std::to_string(hysteresis.count()).c_str());
  auto vorschauzeit_node = abo_aus_node.append_child("Vorschauzeit");
  vorschauzeit_node.append_child(pugi::node_pcdata)
      .set_value(std::to_string(look_ahead.count()).c_str());
  return xml_to_str(doc);
}

std::string abo_loeschen_anfrage_xml_str(std::string const& sender,
                                         sys_time const t) {
  auto doc = make_xml_doc();
  add_abo_anfrage_node(doc, sender, t)
      .append_child("AboLoeschenAlle")
      .append_child(pugi::node_pcdata)
      .set_value("true");
  return xml_to_str(doc);
}

std::string abo_antwort_xml_str(sys_time const t,
                                bool const success,
                                std::uint32_t const error_code) {
  auto doc = make_xml_doc();
  auto abo_antwort_node = doc.append_child("AboAntwort");
  add_bestaetigung_node(abo_antwort_node, t, success, error_code);
  return xml_to_str(doc);
}

std::string daten_bereit_anfrage_xml_str(std::string const& sender,
                                         sys_time const t) {
  auto doc = make_xml_doc();
  auto daten_bereit_anfrage_node = doc.append_child("DatenBereitAnfrage");
  daten_bereit_anfrage_node.append_attribute("Sender") = sender.c_str();
  daten_bereit_anfrage_node.append_attribute("Zst") = timestamp(t).c_str();
  return xml_to_str(doc);
}

std::string daten_bereit_antwort_xml_str(sys_time const t,
                                         bool const success,
                                         std::uint32_t const error_code) {
  auto doc = make_xml_doc();
  auto daten_bereit_antwort_node = doc.append_child("DatenBereitAntwort");
  add_bestaetigung_node(daten_bereit_antwort_node, t, success, error_code);
  return xml_to_str(doc);
}

std::string daten_abrufen_anfrage_xml_str(std::string const& sender,
                                          sys_time const t,
                                          bool const all_datasets) {
  auto doc = make_xml_doc();
  auto daten_abrufen_anfrage_node = doc.append_child("DatenAbrufenAnfrage");
  daten_abrufen_anfrage_node.append_attribute("Sender") = sender.c_str();
  daten_abrufen_anfrage_node.append_attribute("Zst") = timestamp(t).c_str();
  if (all_datasets) {
    auto datensatz_alle_node =
        daten_abrufen_anfrage_node.append_child("DatensatzAlle");
    datensatz_alle_node.append_child(pugi::node_pcdata).set_value("true");
  }
  return xml_to_str(doc);
}

// mock
std::string daten_abrufen_antwort_xml_str(sys_time const t,
                                          bool const success,
                                          std::uint32_t const error_code) {
  auto doc = make_xml_doc();
  auto daten_bereit_antwort_node = doc.append_child("DatenAbrufenAntwort");
  add_bestaetigung_node(daten_bereit_antwort_node, t, success, error_code);
  auto aus_nachricht_node =
      daten_bereit_antwort_node.append_child("AUSNachricht");
  aus_nachricht_node.append_attribute("AboID") = "1";
  return xml_to_str(doc);
}

std::string status_anfrage_xml_str(std::string const& sender,
                                   sys_time const t) {
  auto doc = make_xml_doc();
  auto status_anfrage_node = doc.append_child("StatusAnfrage");
  add_sender_zst_attr(status_anfrage_node, sender, t);
  return xml_to_str(doc);
}

std::string status_antwort_xml_str(sys_time const t,
                                   bool const success,
                                   bool const data_rdy,
                                   sys_time const start) {
  auto doc = make_xml_doc();
  auto status_antwort_node = doc.append_child("StatusAntwort");
  add_status_node(status_antwort_node, t, success);
  auto daten_bereit_node = status_antwort_node.append_child("DatenBereit");
  daten_bereit_node.append_child(pugi::node_pcdata)
      .set_value(data_rdy ? "true" : "false");
  add_start_dienst_zst_node(status_antwort_node, start);
  return xml_to_str(doc);
}

std::string client_status_anfrage_xml_str(std::string const& sender,
                                          sys_time const t,
                                          bool const req_active_abos) {
  auto doc = make_xml_doc();
  auto client_status_anfrage_node = doc.append_child("ClientStatusAnfrage");
  add_sender_zst_attr(client_status_anfrage_node, sender, t);
  if (req_active_abos) {
    client_status_anfrage_node.append_attribute("MitAbos") = "true";
  }
  return xml_to_str(doc);
}

std::string client_status_antwort_xml_str(
    sys_time const t,
    bool const success,
    sys_time const start,
    std::forward_list<subscription> const* subs) {

  auto doc = make_xml_doc();
  auto client_status_antwort_node = doc.append_child("ClientStatusAntwort");
  add_status_node(client_status_antwort_node, t, success);
  add_start_dienst_zst_node(client_status_antwort_node, start);
  if (subs) {
    auto active_subs_node =
        client_status_antwort_node.append_child("AktiveAbos");
    for (auto const& sub : *subs) {
      if (!sub.is_stale()) {
        auto sub_node = active_subs_node.append_child("AboAUS");
        sub_node.append_attribute("AboID") = std::to_string(sub.id_).c_str();
        sub_node.append_attribute("VerfallZst") = timestamp(sub.end_).c_str();
      }
    }
  }
  return xml_to_str(doc);
}

}  // namespace vdv
