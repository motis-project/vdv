#pragma once

#include "net/http/client/request.h"
#include "net/web_server/web_server.h"

#include "nigiri/types.h"

#include "vdv/types.h"

namespace nigiri {
struct timetable;
struct rt_timetable;
}  // namespace nigiri

namespace nigiri::rt::vdv {
struct statistics;
}  // namespace nigiri::rt::vdv

namespace boost::asio {
struct io_context;
}  // namespace boost::asio

namespace vdv {

struct vdv_config;

struct vdv_client {

  explicit vdv_client(vdv_config&);

  void run(boost::asio::io_context& ioc);

  void stop();

  void subscribe(boost::asio::io_context&,
                 sys_time start,
                 sys_time end,
                 std::chrono::seconds hysteresis,
                 std::chrono::minutes look_ahead);

  void unsubscribe(boost::asio::io_context&);

  net::http::client::request make_sub_req(
      sys_time start,
      sys_time end,
      std::uint64_t abo_id,
      std::chrono::seconds hysteresis,
      std::chrono::minutes look_ahead) const;
  net::http::client::request make_unsub_req(sys_time) const;
  net::http::client::request make_fetch_req() const;
  net::http::client::request make_server_status_req(sys_time) const;

  vdv_config& cfg_;
  sys_time start_;
  std::unique_ptr<net::web_server> svr_;
};

}  // namespace vdv