#pragma once

#include "net/http/client/url.h"

#include "nigiri/types.h"

namespace nigiri {
struct timetable;
struct rt_timetable;
}  // namespace nigiri

namespace vdv {

struct vdv_config {
  void derive_endpoints();

  std::string tag_;

  std::string client_name_;
  std::string client_ip_;
  std::string client_port_;
  std::string server_name_;
  std::string server_addr_;

  std::string client_status_path_;
  std::string data_ready_path_;
  net::http::client::url status_addr_;
  net::http::client::url manage_sub_addr_;
  net::http::client::url fetch_data_addr_;
};

}  // namespace vdv