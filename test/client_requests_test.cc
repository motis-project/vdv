#include "gtest/gtest.h"

#include "vdv/client/requests.h"
#include "vdv/timestamp.h"

using namespace vdv::client;

constexpr auto const abo_anfrage_expected = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<AboAnfrage xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" Sender="motis" Zst="2024-06-05T01:45:14Z">
  <AboAUS AboID="1" VerfallZst="2024-06-06T01:45:14.026405Z">
    <Hysterese>30</Hysterese>
    <Vorschauzeit>1200</Vorschauzeit>
    <MitFormation>true</MitFormation>
  </AboAUS>
</AboAnfrage>
)";

TEST(vdv_client, abo_anfrage) {
  auto const req_abo_anfrage =
      make_req_abo_anfrage(net::http::client::url{"http://0.0.0.0:80"});

  std::cout << "timestamp_current: " << vdv::timestamp_current() << "\n";

  std::cout << req_abo_anfrage.body << "\n";
  EXPECT_EQ(abo_anfrage_expected, req_abo_anfrage.body);
}