#pragma once

#include <forward_list>
#include <mutex>
#include <string>
#include <vector>

#include "net/http/client/http_client.h"
#include "net/http/client/url.h"

#include "nigiri/rt/vdv/vdv_update.h"
#include "nigiri/types.h"

#include "vdv/types.h"

namespace nigiri {
struct timetable;
struct rt_timetable;
}  // namespace nigiri

namespace vdv {

struct vdv_client {

  vdv_client(boost::asio::io_context&,
             std::string_view client_name,
             std::string_view client_ip,
             std::string_view client_port,
             std::string_view server_name,
             net::http::client::url const& server_addr,
             nigiri::timetable const*,
             nigiri::rt_timetable*,
             nigiri::source_idx_t);

  void run();

  void subscribe(sys_time start,
                 sys_time end,
                 std::chrono::seconds hysteresis,
                 std::chrono::minutes look_ahead);

  void cancel_sub();

  void fetch();

  void check_server_status();

  boost::asio::io_context& ioc_;
  std::string client_name_;
  std::string client_ip_;
  std::string client_port_;
  std::string server_name_;
  net::http::client::url server_addr_;
  net::http::client::url status_addr_;
  net::http::client::url manage_sub_addr_;
  net::http::client::url fetch_data_addr_;

  std::string client_status_path_;
  std::string data_ready_path_;

  sys_time start_;

  nigiri::timetable const* tt_;
  nigiri::rt_timetable* rtt_;
  nigiri::source_idx_t src_idx_;
  std::vector<nigiri::rt::vdv::statistics> stats_;
};

}  // namespace vdv