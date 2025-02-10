#pragma once

#include <string>

namespace vdv {

std::string get_client_status_path(std::string_view server_name);

std::string get_data_ready_path(std::string_view server_name);

std::string get_status_addr(std::string_view server_addr,
                            std::string_view client_name);

std::string get_manage_sub_addr(std::string_view server_addr,
                                std::string_view client_name);

std::string get_fetch_data_addr(std::string_view server_addr,
                                std::string_view client_name);

}  // namespace vdv