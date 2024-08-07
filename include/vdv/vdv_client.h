#pragma once

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

  vdv_client(vdv_config&, boost::asio::io_context&);

  void run();

  void subscribe(sys_time start,
                 sys_time end,
                 std::chrono::seconds hysteresis,
                 std::chrono::minutes look_ahead);

  void unsubscribe();

  nigiri::rt::vdv::statistics fetch();

  void check_server_status();

  vdv_config& cfg_;
  boost::asio::io_context& ioc_;
  sys_time start_;
};

}  // namespace vdv