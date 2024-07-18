#pragma once

#include <forward_list>
#include <mutex>
#include <string>
#include <vector>

#include "net/http/client/http_client.h"
#include "net/http/client/url.h"

#include "nigiri/types.h"

#include "vdv/types.h"

namespace nigiri {
struct timetable;
struct rt_timetable;
}  // namespace nigiri

namespace vdv {

struct vdv_client {

  struct subscription {
    bool is_stale() const { return end_ < std::chrono::system_clock::now(); }

    std::uint64_t id_;
    sys_time start_;
    sys_time end_;
    std::chrono::seconds hysteresis_;
    std::chrono::minutes look_ahead_;
  };

  vdv_client(std::string_view client_name,
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

  void clean_up_subs();

  void fetch() const;

  void check_server_status();

  std::string client_name_;
  std::string client_port_;
  std::string server_name_;
  net::http::client::url server_addr_;
  net::http::client::url status_addr_;
  net::http::client::url manage_sub_addr_;
  net::http::client::url fetch_data_addr_;

  std::string client_status_path_;
  std::string data_ready_path_;

  sys_time start_;

  std::uint8_t next_id_{0U};
  std::forward_list<subscription> subs_{};
  std::mutex subs_mutex_;

  nigiri::timetable const* tt_;
  nigiri::rt_timetable* rtt_;
  nigiri::source_idx_t src_idx_;

  boost::asio::io_context ioc_;
  std::shared_ptr<net::http::client::basic_http_client<net::tcp>> http_client_;
};

}  // namespace vdv