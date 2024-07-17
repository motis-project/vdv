#pragma once

#include <string>
#include <vector>

#include "net/http/client/http_client.h"
#include "net/http/client/url.h"

#include "vdv/subscription.h"
#include "vdv/types.h"

namespace nigiri {
struct rt_timetable;
}  // namespace nigiri

namespace vdv {

struct client {

  explicit client(net::http::client::url server_addr,
                  std::string_view server_name,
                  std::chrono::seconds hysteresis,
                  std::chrono::minutes look_ahead,
                  nigiri::rt_timetable*,
                  nigiri::source_idx_t);

  void run();

  void fetch();

  bool is_sub_valid() const {
    auto const now = std::chrono::system_clock::now();
    return start_ <= now && now <= start_ + look_ahead_;
  }

  std::string client_name_{"motis"};
  std::string server_name_;
  net::http::client::url server_addr_;
  net::http::client::url status_addr_;
  net::http::client::url manage_sub_addr_;
  net::http::client::url fetch_data_addr_;

  std::string client_status_path_;
  std::string data_ready_path_;

  std::chrono::time_point<std::chrono::system_clock> start_;
  std::chrono::minutes look_ahead_;
  std::chrono::seconds hysteresis_;

  nigiri::rt_timetable* rtt_;
  nigiri::source_idx_t src_idx_;

  boost::asio::io_context ioc_;
  std::shared_ptr<net::http::client::basic_http_client<net::tcp>> http_client_;
};

}  // namespace vdv