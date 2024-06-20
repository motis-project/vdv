#pragma once

#include "net/http/client/request.h"
#include "pugixml.hpp"

namespace vdv::client {

net::http::client::request make_req_abo_anfrage(net::http::client::url addr);

}  // namespace vdv::client