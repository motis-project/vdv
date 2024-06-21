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
<AboAnfrage xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" Sender="motis" Zst="2024-06-21T13:37:23">
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
<AboAntwort xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <Bestaetigung Zst="2024-06-21T13:37:23" Ergebnis="ok" Fehlernummer="0" />
</AboAntwort>
)";

TEST(xml_out, abo_antwort) {
  EXPECT_EQ(abo_antwort_expected, result_str(abo_antwort_xml_str(t, true, 0)));
}