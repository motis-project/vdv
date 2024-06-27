#pragma once

#include <chrono>

namespace vdv {

using timestamp_t = std::chrono::time_point<std::chrono::system_clock>;
using abo_id_t = std::uint32_t;
using error_code_t = std::uint32_t;

struct rt_stop {
  std::string stop_id_;
  std::string platform_arr_;
  std::string platform_dep_;
  std::optional<timestamp_t> t_arr_;
  std::optional<timestamp_t> t_dep_;
  std::optional<timestamp_t> t_arr_rt_;
  std::optional<timestamp_t> t_dep_rt_;
  bool in_allowed_;
  bool out_allowed_;
  bool additional_stop_;
};

struct rt_run {
  timestamp_t t_;
  std::string route_id_;
  std::string route_text_;
  std::string direction_id_;
  std::string direction_text_;
  std::string vehicle_;
  std::string trip_ref_;
  std::string operator_;
  std::chrono::year_month_day date_;
  bool complete_;  // if false only stops with updates will be transmitted
  bool canceled_{false};
  bool additional_run_{false};
  std::vector<rt_stop> stops_;
};

}  // namespace vdv