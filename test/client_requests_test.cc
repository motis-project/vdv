#include "gtest/gtest.h"

#include "vdv/requests.h"
#include "vdv/timestamp.h"

#include <chrono>

using namespace vdv;
using namespace std::literals::chrono_literals;

std::string result_str(std::string const& str) {
  std::stringstream ss;
  ss << "\n" << str;
  return ss.str();
}

constexpr auto const abo_anfrage_expected = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<AboAnfrage xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" Sender="motis" Zst="2024-06-21T13:37:23">
  <AboAUS AboID="1" VerfallZst="2024-06-22T13:37:23">
    <Hysterese>30</Hysterese>
    <Vorschauzeit>1440</Vorschauzeit>
  </AboAUS>
</AboAnfrage>
)";

TEST(vdv_client, abo_anfrage) {
  auto const t = vdv::timestamp_from_string("2024-06-21T13:37:23.00000");

  auto const req_abo_anfrage = make_abo_anfrage(
      net::http::client::url{"http://0.0.0.0:80"}, "motis", t, 1, 30s, 1440min);

  EXPECT_EQ(abo_anfrage_expected, result_str(req_abo_anfrage.body));
}