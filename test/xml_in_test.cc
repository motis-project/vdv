#include "gtest/gtest.h"

#include "vdv/xml_in.h"

#include <chrono>

#include "xml_test_data.h"

using namespace vdv;
using namespace std::literals::chrono_literals;

TEST(xml_in, empty) {
  EXPECT_FALSE(parse("").has_value());
  EXPECT_FALSE(parse(empty_xml_str).has_value());
}

TEST(xml_in, unsupported) { EXPECT_FALSE(parse(unsupported_str).has_value()); }

TEST(xml_in, abo_anfrage) {
  auto const msg = parse(abo_anfrage_str);
  ASSERT_TRUE(msg.has_value());
  ASSERT_TRUE(holds_alternative<abo_anfrage_msg>(msg.value()));
  auto const abo_anfrage_actual = get<abo_anfrage_msg>(msg.value());
  EXPECT_EQ(abo_anfrage_actual.sender_, "motis");
  EXPECT_EQ(abo_anfrage_actual.abo_id_, 1);
  EXPECT_EQ(abo_anfrage_actual.t_, t);
  EXPECT_EQ(abo_anfrage_actual.expiration_t_,
            timestamp_from_string("2024-06-22T13:37:23"));
  EXPECT_EQ(abo_anfrage_actual.hysteresis_, 30s);
  EXPECT_EQ(abo_anfrage_actual.look_ahead_, 1440min);
}

TEST(xml_in, abo_antwort) {
  auto const msg = parse(abo_antwort_str);
  ASSERT_TRUE(msg.has_value());
  ASSERT_TRUE(holds_alternative<abo_antwort_msg>(msg.value()));
  auto const abo_antwort_actual = get<abo_antwort_msg>(msg.value());
  EXPECT_EQ(abo_antwort_actual.t_, t);
  EXPECT_EQ(abo_antwort_actual.success_, true);
  EXPECT_EQ(abo_antwort_actual.error_code_, 0);
}

TEST(xml_in, daten_bereit_anfrage) {
  auto const msg = parse(daten_bereit_anfrage_str);
  ASSERT_TRUE(msg.has_value());
  ASSERT_TRUE(holds_alternative<daten_bereit_anfrage_msg>(msg.value()));
  auto const daten_bereit_anfrage_actual =
      get<daten_bereit_anfrage_msg>(msg.value());
  EXPECT_EQ(daten_bereit_anfrage_actual.sender_, "motis");
  EXPECT_EQ(daten_bereit_anfrage_actual.t_, t);
}

TEST(xml_in, daten_bereit_antwort) {
  auto const msg = parse(daten_bereit_antwort_str);
  ASSERT_TRUE(msg.has_value());
  ASSERT_TRUE(holds_alternative<daten_bereit_antwort_msg>(msg.value()));
  auto const daten_bereit_antwort_actual =
      get<daten_bereit_antwort_msg>(msg.value());
  EXPECT_EQ(daten_bereit_antwort_actual.t_, t);
  EXPECT_EQ(daten_bereit_antwort_actual.success_, true);
  EXPECT_EQ(daten_bereit_antwort_actual.error_code_, 0);
}

TEST(xml_in, daten_abrufen_anfrage_all_datasets) {
  auto const msg = parse(daten_abrufen_anfrage_all_datasets_str);
  ASSERT_TRUE(msg.has_value());
  ASSERT_TRUE(holds_alternative<daten_abrufen_anfrage_msg>(msg.value()));
  auto const daten_abrufen_anfrage_actual =
      get<daten_abrufen_anfrage_msg>(msg.value());
  EXPECT_EQ(daten_abrufen_anfrage_actual.sender_, "motis");
  EXPECT_EQ(daten_abrufen_anfrage_actual.t_, t);
  EXPECT_EQ(daten_abrufen_anfrage_actual.all_datasets_, true);
}

TEST(xml_in, daten_abrufen_anfrage) {
  auto const msg = parse(daten_abrufen_anfrage_str);
  ASSERT_TRUE(msg.has_value());
  ASSERT_TRUE(holds_alternative<daten_abrufen_anfrage_msg>(msg.value()));
  auto const daten_abrufen_anfrage_actual =
      get<daten_abrufen_anfrage_msg>(msg.value());
  EXPECT_EQ(daten_abrufen_anfrage_actual.sender_, "motis");
  EXPECT_EQ(daten_abrufen_anfrage_actual.t_, t);
  EXPECT_EQ(daten_abrufen_anfrage_actual.all_datasets_, false);
}

TEST(xml_in, daten_abrufen_antwort) {
  auto const msg = parse(daten_abrufen_antwort_str);
  ASSERT_TRUE(msg.has_value());
  ASSERT_TRUE(holds_alternative<daten_abrufen_antwort_msg>(msg.value()));
  auto const daten_abrufen_antwort_actual =
      get<daten_abrufen_antwort_msg>(msg.value());

  EXPECT_EQ(daten_abrufen_antwort_actual.t_, t);
  EXPECT_EQ(daten_abrufen_antwort_actual.success_, true);
  EXPECT_EQ(daten_abrufen_antwort_actual.error_code_, 0);
  EXPECT_EQ(daten_abrufen_antwort_actual.abo_id_, 1);
  ASSERT_EQ(daten_abrufen_antwort_actual.runs_.size(), 2);

  auto const& run0 = daten_abrufen_antwort_actual.runs_[0];
  EXPECT_EQ(run0.t_, t);
  EXPECT_EQ(run0.route_id_, "RB 37");
  EXPECT_EQ(run0.route_text_, "RB 37");
  EXPECT_EQ(run0.direction_id_, "2");
  EXPECT_EQ(run0.direction_text_, "Bodenmais");
  EXPECT_EQ(run0.vehicle_, "WBA");
  EXPECT_EQ(run0.trip_ref_, "83972#WBA_DDS-LB");
  EXPECT_EQ(run0.operator_, "DDS-LB");
  EXPECT_EQ(run0.date_, date::sys_days{std::chrono::June / 5 / 2024});
  EXPECT_EQ(run0.complete_, true);
  EXPECT_EQ(run0.canceled_, false);
  EXPECT_EQ(run0.additional_run_, false);

  ASSERT_EQ(run0.stops_.size(), 2);

  auto const& stop0 = run0.stops_[0];
  EXPECT_EQ(stop0.stop_id_, "de:09276:5025");
  EXPECT_EQ(stop0.platform_dep_, "4");
  EXPECT_FALSE(stop0.t_arr_.has_value());
  ASSERT_TRUE(stop0.t_dep_.has_value());
  EXPECT_EQ(stop0.t_dep_.value(), t);
  EXPECT_FALSE(stop0.t_arr_rt_.has_value());
  EXPECT_FALSE(stop0.t_dep_rt_.has_value());
  EXPECT_EQ(stop0.in_allowed_, true);
  EXPECT_EQ(stop0.out_allowed_, true);
  EXPECT_EQ(stop0.additional_stop_, false);

  auto const& stop1 = run0.stops_[1];
  EXPECT_EQ(stop1.stop_id_, "de:09276:80503");
  EXPECT_EQ(stop1.platform_arr_, "1");
  EXPECT_EQ(stop1.platform_dep_, "1");
  ASSERT_TRUE(stop1.t_arr_.has_value());
  EXPECT_EQ(stop1.t_arr_, t);
  ASSERT_TRUE(stop1.t_dep_.has_value());
  EXPECT_EQ(stop1.t_dep_, t);
  EXPECT_FALSE(stop1.t_arr_rt_.has_value());
  EXPECT_FALSE(stop1.t_dep_rt_.has_value());
  EXPECT_EQ(stop1.in_allowed_, true);
  EXPECT_EQ(stop1.out_allowed_, true);
  EXPECT_EQ(stop1.additional_stop_, false);

  auto const& run1 = daten_abrufen_antwort_actual.runs_[1];
  EXPECT_EQ(run1.t_, t);
  EXPECT_EQ(run1.route_id_, "RB5");
  EXPECT_EQ(run1.route_text_, "RB5");
  EXPECT_EQ(run1.direction_id_, "2");
  EXPECT_EQ(run1.direction_text_, "Plauen(Vogtl)ob Bf");
  EXPECT_EQ(run1.vehicle_, "VBG");
  EXPECT_EQ(run1.trip_ref_, "80188#VBG_DDS-LB");
  EXPECT_EQ(run1.operator_, "DDS-LB");
  EXPECT_EQ(run1.date_,
            std::chrono::year_month_day{std::chrono::June / 5 / 2024});
  EXPECT_EQ(run1.complete_, true);
  EXPECT_EQ(run1.canceled_, false);
  EXPECT_EQ(run1.additional_run_, false);

  ASSERT_EQ(run1.stops_.size(), 9);

  auto const& stop2 = run1.stops_[2];
  EXPECT_EQ(stop2.stop_id_, "de:14523:309");
  EXPECT_EQ(stop2.platform_arr_, "1");
  EXPECT_EQ(stop2.platform_dep_, "1");
  ASSERT_TRUE(stop2.t_arr_.has_value());
  EXPECT_EQ(stop2.t_arr_.value(), t);
  ASSERT_TRUE(stop2.t_dep_.has_value());
  EXPECT_EQ(stop2.t_dep_.value(), t);
  ASSERT_TRUE(stop2.t_arr_rt_.has_value());
  EXPECT_EQ(stop2.t_arr_rt_.value(), t);
  ASSERT_TRUE(stop2.t_dep_rt_.has_value());
  EXPECT_EQ(stop2.t_dep_rt_.value(), t);
  EXPECT_EQ(stop2.in_allowed_, true);
  EXPECT_EQ(stop2.out_allowed_, true);
  EXPECT_EQ(stop2.additional_stop_, false);
}

TEST(xml_in, status_anfrage) {
  auto const msg = parse(status_anfrage_str);
  ASSERT_TRUE(msg.has_value());
  ASSERT_TRUE(holds_alternative<status_anfrage_msg>(msg.value()));
  auto const status_anfrage_actual = get<status_anfrage_msg>(msg.value());
  EXPECT_EQ(status_anfrage_actual.sender_, "motis");
  EXPECT_EQ(status_anfrage_actual.t_, t);
}

TEST(xml_in, status_antwort) {
  auto const msg = parse(status_antwort_str);
  ASSERT_TRUE(msg.has_value());
  ASSERT_TRUE(holds_alternative<status_antwort_msg>(msg.value()));
  auto const status_antwort_actual = get<status_antwort_msg>(msg.value());
  EXPECT_EQ(status_antwort_actual.t_, t);
  EXPECT_EQ(status_antwort_actual.success_, true);
  EXPECT_EQ(status_antwort_actual.data_rdy_, true);
  EXPECT_EQ(status_antwort_actual.start_, t);
}

TEST(xml_in, client_status_anfrage) {
  auto const msg = parse(client_status_anfrage_str);
  ASSERT_TRUE(msg.has_value());
  ASSERT_TRUE(holds_alternative<client_status_anfrage_msg>(msg.value()));
  auto const client_status_anfrage_actual =
      get<client_status_anfrage_msg>(msg.value());
  EXPECT_EQ(client_status_anfrage_actual.sender_, "motis");
  EXPECT_EQ(client_status_anfrage_actual.t_, t);
  EXPECT_EQ(client_status_anfrage_actual.req_active_abos_, true);
}

TEST(xml_in, client_status_antwort) {
  auto const msg = parse(client_status_antwort_str);
  ASSERT_TRUE(msg.has_value());
  ASSERT_TRUE(holds_alternative<client_status_antwort_msg>(msg.value()));
  auto const client_status_antwort_actual =
      get<client_status_antwort_msg>(msg.value());
  EXPECT_EQ(client_status_antwort_actual.t_, t);
  EXPECT_EQ(client_status_antwort_actual.success_, true);
  EXPECT_EQ(client_status_antwort_actual.start_, t);
  EXPECT_TRUE(client_status_antwort_actual.subscriptions_.empty());
}
