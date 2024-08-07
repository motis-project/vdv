#include "vdv/vdv_config.h"

namespace vdv {

net::http::client::url get_endpoint_addr(std::string_view server_addr,
                                         std::string_view client_name,
                                         std::string_view path) {
  auto ss = std::stringstream{};
  ss << server_addr << "/" << client_name << "/" << path;
  return {ss.str()};
}

std::string get_endpoint_path(std::string_view server_name,
                              std::string_view request) {
  auto ss = std::stringstream{};
  ss << "/" << server_name << "/" << request;
  return ss.str();
}

void vdv_config::derive_endpoints() {
  client_status_path_ = get_endpoint_path(server_name_, "aus/clientstatus.xml");
  data_ready_path_ = get_endpoint_path(server_name_, "aus/datenbereit.xml");
  status_addr_ =
      get_endpoint_addr(server_addr_, client_name_, "aus/status.xml");
  manage_sub_addr_ =
      get_endpoint_addr(server_addr_, client_name_, "aus/aboverwalten.xml");
  fetch_data_addr_ =
      get_endpoint_addr(server_addr_, client_name_, "aus/datenabrufen.xml");
}

}  // namespace vdv