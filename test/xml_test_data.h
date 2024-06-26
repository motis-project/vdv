#pragma once

auto const t = vdv::timestamp_from_string("2024-06-21T13:37:23");

constexpr auto const abo_anfrage_str = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<AboAnfrage Sender="motis" Zst="2024-06-21T13:37:23">
  <AboAUS AboID="1" VerfallZst="2024-06-22T13:37:23">
    <Hysterese>30</Hysterese>
    <Vorschauzeit>1440</Vorschauzeit>
  </AboAUS>
</AboAnfrage>
)";

constexpr auto const abo_antwort_str = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<AboAntwort>
  <Bestaetigung Zst="2024-06-21T13:37:23" Ergebnis="ok" Fehlernummer="0" />
</AboAntwort>
)";

constexpr auto const daten_bereit_anfrage_str = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<DatenBereitAnfrage Sender="motis" Zst="2024-06-21T13:37:23" />
)";

constexpr auto const daten_bereit_antwort_str = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<DatenBereitAntwort>
  <Bestaetigung Zst="2024-06-21T13:37:23" Ergebnis="ok" Fehlernummer="0" />
</DatenBereitAntwort>
)";

constexpr auto const daten_abrufen_anfrage_all_datasets_str = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<DatenAbrufenAnfrage Sender="motis" Zst="2024-06-21T13:37:23">
  <DatensatzAlle>true</DatensatzAlle>
</DatenAbrufenAnfrage>
)";

constexpr auto const daten_abrufen_anfrage_str = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<DatenAbrufenAnfrage Sender="motis" Zst="2024-06-21T13:37:23" />
)";

constexpr auto const status_anfrage_str = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<StatusAnfrage Sender="motis" Zst="2024-06-21T13:37:23" />
)";

constexpr auto const status_antwort_str = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<StatusAntwort>
  <Status Zst="2024-06-21T13:37:23" Ergebnis="ok" />
  <DatenBereit>true</DatenBereit>
  <StartDienstZst>2024-06-21T13:37:23</StartDienstZst>
</StatusAntwort>
)";

constexpr auto const client_status_anfrage_str = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<ClientStatusAnfrage Sender="motis" Zst="2024-06-21T13:37:23" MitAbos="true" />
)";

constexpr auto const client_status_antwort_str = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<ClientStatusAntwort>
  <Status Zst="2024-06-21T13:37:23" Ergebnis="ok" />
  <StartDienstZst>2024-06-21T13:37:23</StartDienstZst>
</ClientStatusAntwort>
)";