#include "gtest/gtest.h"

#include <chrono>

#include "vdv/msg.h"
#include "vdv/xml_in.h"

#include "xml_test_data.h"

using namespace vdv;
using namespace std::literals::chrono_literals;

TEST(xml_in, empty) {
  EXPECT_THROW(parse(""), std::runtime_error);
  EXPECT_THROW(parse(empty_xml_str), std::runtime_error);
}

TEST(xml_in, unsupported) {
  EXPECT_THROW(parse(unsupported_str), std::runtime_error);
}

TEST(xml_in, abo_anfrage) {
  ASSERT_NO_THROW(parse(abo_anfrage_str));
  auto const msg = parse(abo_anfrage_str);
  ASSERT_TRUE(holds_alternative<abo_anfrage_msg>(msg));
  auto const abo_anfrage_actual = get<abo_anfrage_msg>(msg);
  EXPECT_EQ(abo_anfrage_actual.sender_, "motis");
  EXPECT_EQ(abo_anfrage_actual.abo_id_, 1);
  EXPECT_EQ(abo_anfrage_actual.t_, t);
  EXPECT_EQ(abo_anfrage_actual.expiration_t_,
            parse_timestamp("2024-06-22T13:37:23"));
  EXPECT_EQ(abo_anfrage_actual.hysteresis_, 30s);
  EXPECT_EQ(abo_anfrage_actual.look_ahead_, 1440min);
}

TEST(xml_in, abo_antwort) {
  ASSERT_NO_THROW(parse(abo_antwort_str));
  auto const msg = parse(abo_antwort_str);
  ASSERT_TRUE(holds_alternative<abo_antwort_msg>(msg));
  auto const abo_antwort_actual = get<abo_antwort_msg>(msg);
  EXPECT_EQ(abo_antwort_actual.t_, t);
  EXPECT_EQ(abo_antwort_actual.success_, true);
  EXPECT_EQ(abo_antwort_actual.error_code_, 0);
}

TEST(xml_in, daten_bereit_anfrage) {
  ASSERT_NO_THROW(parse(daten_bereit_anfrage_str));
  auto const msg = parse(daten_bereit_anfrage_str);
  ASSERT_TRUE(holds_alternative<daten_bereit_anfrage_msg>(msg));
  auto const daten_bereit_anfrage_actual = get<daten_bereit_anfrage_msg>(msg);
  EXPECT_EQ(daten_bereit_anfrage_actual.sender_, "motis");
  EXPECT_EQ(daten_bereit_anfrage_actual.t_, t);
}

TEST(xml_in, daten_bereit_antwort) {
  ASSERT_NO_THROW(parse(daten_bereit_antwort_str));
  auto const msg = parse(daten_bereit_antwort_str);
  ASSERT_TRUE(holds_alternative<daten_bereit_antwort_msg>(msg));
  auto const daten_bereit_antwort_actual = get<daten_bereit_antwort_msg>(msg);
  EXPECT_EQ(daten_bereit_antwort_actual.t_, t);
  EXPECT_EQ(daten_bereit_antwort_actual.success_, true);
  EXPECT_EQ(daten_bereit_antwort_actual.error_code_, 0);
}

TEST(xml_in, daten_abrufen_anfrage_all_datasets) {
  ASSERT_NO_THROW(parse(daten_abrufen_anfrage_all_datasets_str));
  auto const msg = parse(daten_abrufen_anfrage_all_datasets_str);
  ASSERT_TRUE(holds_alternative<daten_abrufen_anfrage_msg>(msg));
  auto const daten_abrufen_anfrage_actual = get<daten_abrufen_anfrage_msg>(msg);
  EXPECT_EQ(daten_abrufen_anfrage_actual.sender_, "motis");
  EXPECT_EQ(daten_abrufen_anfrage_actual.t_, t);
  EXPECT_EQ(daten_abrufen_anfrage_actual.all_datasets_, true);
}

TEST(xml_in, daten_abrufen_anfrage) {
  ASSERT_NO_THROW(parse(daten_abrufen_anfrage_str));
  auto const msg = parse(daten_abrufen_anfrage_str);
  ASSERT_TRUE(holds_alternative<daten_abrufen_anfrage_msg>(msg));
  auto const daten_abrufen_anfrage_actual = get<daten_abrufen_anfrage_msg>(msg);
  EXPECT_EQ(daten_abrufen_anfrage_actual.sender_, "motis");
  EXPECT_EQ(daten_abrufen_anfrage_actual.t_, t);
  EXPECT_EQ(daten_abrufen_anfrage_actual.all_datasets_, false);
}

TEST(xml_in, daten_abrufen_antwort) {
  ASSERT_NO_THROW(parse(daten_abrufen_antwort_str));
  auto const msg = parse(daten_abrufen_antwort_str);
  ASSERT_TRUE(holds_alternative<daten_abrufen_antwort_msg>(msg));
  auto const daten_abrufen_antwort_actual = get<daten_abrufen_antwort_msg>(msg);

  EXPECT_EQ(daten_abrufen_antwort_actual.t_, t);
  EXPECT_EQ(daten_abrufen_antwort_actual.success_, true);
  EXPECT_EQ(daten_abrufen_antwort_actual.error_code_, 0);
  EXPECT_EQ(daten_abrufen_antwort_actual.abo_id_, 1);
}

TEST(xml_in, status_anfrage) {
  ASSERT_NO_THROW(parse(status_anfrage_str));
  auto const msg = parse(status_anfrage_str);
  ASSERT_TRUE(holds_alternative<status_anfrage_msg>(msg));
  auto const status_anfrage_actual = get<status_anfrage_msg>(msg);
  EXPECT_EQ(status_anfrage_actual.sender_, "motis");
  EXPECT_EQ(status_anfrage_actual.t_, t);
}

TEST(xml_in, status_antwort) {
  ASSERT_NO_THROW(parse(status_antwort_str));
  auto const msg = parse(status_antwort_str);
  ASSERT_TRUE(holds_alternative<status_antwort_msg>(msg));
  auto const status_antwort_actual = get<status_antwort_msg>(msg);
  EXPECT_EQ(status_antwort_actual.t_, t);
  EXPECT_EQ(status_antwort_actual.success_, true);
  EXPECT_EQ(status_antwort_actual.data_rdy_, true);
  EXPECT_EQ(status_antwort_actual.start_, t);
}

TEST(xml_in, client_status_anfrage) {
  ASSERT_NO_THROW(parse(client_status_anfrage_str));
  auto const msg = parse(client_status_anfrage_str);
  ASSERT_TRUE(holds_alternative<client_status_anfrage_msg>(msg));
  auto const client_status_anfrage_actual = get<client_status_anfrage_msg>(msg);
  EXPECT_EQ(client_status_anfrage_actual.sender_, "motis");
  EXPECT_EQ(client_status_anfrage_actual.t_, t);
  EXPECT_EQ(client_status_anfrage_actual.req_active_abos_, true);
}

TEST(xml_in, client_status_antwort) {
  ASSERT_NO_THROW(parse(client_status_antwort_str));
  auto const msg = parse(client_status_antwort_str);
  ASSERT_TRUE(holds_alternative<client_status_antwort_msg>(msg));
  auto const client_status_antwort_actual = get<client_status_antwort_msg>(msg);
  EXPECT_EQ(client_status_antwort_actual.t_, t);
  EXPECT_EQ(client_status_antwort_actual.success_, true);
  EXPECT_EQ(client_status_antwort_actual.start_, t);
  EXPECT_TRUE(client_status_antwort_actual.subscriptions_.empty());
}
