#pragma once

#include <optional>
#include <string>

#include "date/date.h"
#include "pugixml.hpp"

#include "timestamp.h"

namespace vdv {

struct vdv_stop {
  std::string stop_id_;
  std::optional<std::string> platform_arr_;
  std::optional<std::string> platform_dep_;
  std::optional<timestamp_t> t_arr_;
  std::optional<timestamp_t> t_dep_;
  std::optional<timestamp_t> t_arr_rt_;
  std::optional<timestamp_t> t_dep_rt_;
  std::optional<bool> in_allowed_;
  std::optional<bool> out_allowed_;
  std::optional<bool> additional_stop_;
};

struct vdv_run {
  timestamp_t t_;
  std::string route_id_;
  std::string route_text_;
  std::string direction_id_;
  std::string direction_text_;
  std::string vehicle_;
  std::string trip_ref_;
  std::string operator_;
  date::sys_days date_;
  bool complete_;  // if false only stops with updates will be transmitted
  bool canceled_{false};
  bool additional_run_{false};
  std::vector<vdv_stop> stops_;
};

vdv_run get_run(pugi::xml_node const& run_node) {
  auto const get_stops = [&]() {
    auto const get_stop = [](pugi::xml_node const& stop_node) -> vdv_stop {
      auto const pass_through = get_opt_bool(stop_node, "Durchfahrt");
      auto const no_enter = get_opt_bool(stop_node, "Einsteigeverbot");
      auto const no_exit = get_opt_bool(stop_node, "Aussteigeverbot");
      return {.stop_id_ = get_req(stop_node, "HaltID").node().child_value(),
              .platform_arr_ = get_opt_str(stop_node, "AnkunftssteigText"),
              .platform_dep_ = get_opt_str(stop_node, "AbfahrtssteigText"),
              .t_arr_ = get_opt_timestamp(stop_node, "Ankunftszeit"),
              .t_dep_ = get_opt_timestamp(stop_node, "Abfahrtszeit"),
              .t_arr_rt_ = get_opt_timestamp(stop_node, "IstAnkunftPrognose"),
              .t_dep_rt_ = get_opt_timestamp(stop_node, "IstAbfahrtPrognose"),
              .in_allowed_ =
                  pass_through.has_value() && pass_through.value() ? false
                  : no_enter.has_value() ? std::optional{!no_enter.value()}
                                         : std::nullopt,
              .out_allowed_ =
                  pass_through.has_value() && pass_through.value() ? false
                  : no_exit.has_value() ? std::optional{!no_exit.value()}
                                        : std::nullopt,
              .additional_stop_ =
                  get_bool_with_fallback(stop_node, "Zusatzhalt", false)};
    };

    auto stops = std::vector<vdv_stop>{};
    auto const stops_xpath = run_node.select_nodes("IstHalt");
    for (auto const& stop_xpath : stops_xpath) {
      stops.emplace_back(get_stop(stop_xpath.node()));
    }
    return stops;
  };

  return {
      .t_ = parse_timestamp(get_req(run_node, "@Zst").attribute().value()),
      .route_id_ = get_req(run_node, "LinienID").node().child_value(),
      .route_text_ = get_req(run_node, "LinienText").node().child_value(),
      .direction_id_ = get_req(run_node, "RichtungsID").node().child_value(),
      .direction_text_ =
          get_req(run_node, "RichtungsText").node().child_value(),
      .vehicle_ = get_req(run_node, "ProduktID").node().child_value(),
      .trip_ref_ = get_req(run_node, "FahrtRef/FahrtID/FahrtBezeichner")
                       .node()
                       .child_value(),
      .operator_ = get_req(run_node, "BetreiberID").node().child_value(),
      .date_ = parse_date(get_req(run_node, "FahrtRef/FahrtID/Betriebstag")
                              .node()
                              .child_value()),
      .complete_ =
          parse_bool(get_req(run_node, "Komplettfahrt").node().child_value()),
      .canceled_ = get_bool_with_fallback(run_node, "FaelltAus", false),
      .additional_run_ = get_bool_with_fallback(run_node, "Zusatzfahrt", false),
      .stops_ = get_stops()};
}

}  // namespace vdv