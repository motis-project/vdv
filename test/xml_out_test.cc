#include "gtest/gtest.h"

#include "vdv/timestamp.h"
#include "vdv/xml.h"

#include <chrono>

using namespace vdv;
using namespace std::literals::chrono_literals;

std::string result_str(std::string const& str) {
  std::stringstream ss;
  ss << "\n" << str;
  return ss.str();
}

auto const t = vdv::timestamp_from_string("2024-06-21T13:37:23");

constexpr auto const abo_anfrage_expected = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<AboAnfrage Sender="motis" Zst="2024-06-21T13:37:23">
  <AboAUS AboID="1" VerfallZst="2024-06-22T13:37:23">
    <Hysterese>30</Hysterese>
    <Vorschauzeit>1440</Vorschauzeit>
  </AboAUS>
</AboAnfrage>
)";

TEST(xml_out, abo_anfrage) {
  EXPECT_EQ(abo_anfrage_expected,
            result_str(abo_anfrage_xml_str("motis", t, 1, 30s, 1440min)));
}

constexpr auto const abo_antwort_expected = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<AboAntwort>
  <Bestaetigung Zst="2024-06-21T13:37:23" Ergebnis="ok" Fehlernummer="0" />
</AboAntwort>
)";

TEST(xml_out, abo_antwort) {
  EXPECT_EQ(abo_antwort_expected, result_str(abo_antwort_xml_str(t, true, 0)));
}

constexpr auto const daten_bereit_anfrage_expected = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<DatenBereitAnfrage Sender="motis" Zst="2024-06-21T13:37:23" />
)";

TEST(xml_out, daten_bereit_anfrage) {
  EXPECT_EQ(daten_bereit_anfrage_expected,
            result_str(daten_bereit_anfrage_xml_str("motis", t)));
}

constexpr auto const daten_bereit_antwort_expected = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<DatenBereitAntwort>
  <Bestaetigung Zst="2024-06-21T13:37:23" Ergebnis="ok" Fehlernummer="0" />
</DatenBereitAntwort>
)";

TEST(xml_out, daten_bereit_antwort) {
  EXPECT_EQ(daten_bereit_antwort_expected,
            result_str(daten_bereit_antwort_xml_str(t, true, 0)));
}

constexpr auto const daten_abrufen_anfrage_expected = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<DatenAbrufenAnfrage Sender="motis" Zst="2024-06-21T13:37:23">
  <DatensatzAlle>true</DatensatzAlle>
</DatenAbrufenAnfrage>
)";

TEST(xml_out, daten_abrufen_anfrage) {
  EXPECT_EQ(daten_abrufen_anfrage_expected,
            result_str(daten_abrufen_anfrage_xml_str("motis", t, true)));
}
