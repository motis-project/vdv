#include "gtest/gtest.h"

#include "vdv/timestamp.h"
#include "vdv/xml_out.h"

#include <chrono>

#include "xml_test_data.h"

using namespace vdv;
using namespace std::literals::chrono_literals;

std::string result_str(std::string const& str) {
  std::stringstream ss;
  ss << "\n" << str;
  return ss.str();
}

TEST(xml_out, abo_anfrage) {
  EXPECT_EQ(abo_anfrage_str,
            result_str(abo_anfrage_xml_str("motis", t, 1, 30s, 1440min)));
}

TEST(xml_out, abo_antwort) {
  EXPECT_EQ(abo_antwort_str, result_str(abo_antwort_xml_str(t, true, 0)));
}

TEST(xml_out, daten_bereit_anfrage) {
  EXPECT_EQ(daten_bereit_anfrage_str,
            result_str(daten_bereit_anfrage_xml_str("motis", t)));
}

TEST(xml_out, daten_bereit_antwort) {
  EXPECT_EQ(daten_bereit_antwort_str,
            result_str(daten_bereit_antwort_xml_str(t, true, 0)));
}

TEST(xml_out, daten_abrufen_anfrage_all_datasets_str) {
  EXPECT_EQ(daten_abrufen_anfrage_all_datasets_str,
            result_str(daten_abrufen_anfrage_xml_str("motis", t, true)));
}

TEST(xml_out, daten_abrufen_anfrage) {
  EXPECT_EQ(daten_abrufen_anfrage_str,
            result_str(daten_abrufen_anfrage_xml_str("motis", t, false)));
}

TEST(xml_out, status_anfrage) {
  EXPECT_EQ(status_anfrage_str, result_str(status_anfrage_xml_str("motis", t)));
}

TEST(xml_out, status_antwort) {
  EXPECT_EQ(status_antwort_str,
            result_str(status_antwort_xml_str(t, true, true, t)));
}

TEST(xml_out, client_status_anfrage) {
  EXPECT_EQ(client_status_anfrage_str,
            result_str(client_status_anfrage_xml_str("motis", t, true)));
}

TEST(xml_out, client_status_antwort) {
  EXPECT_EQ(client_status_antwort_str,
            result_str(client_status_antwort_xml_str(t, true, t)));
}