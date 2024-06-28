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

constexpr auto const daten_abrufen_antwort_str = R"(
<?xml version="1.0" encoding="iso-8859-1"?>
<DatenAbrufenAntwort>
  <Bestaetigung Zst="2024-06-21T13:37:23" Ergebnis="ok" Fehlernummer="0" />
  <AUSNachricht AboID="1">
    <IstFahrt Zst="2024-06-21T13:37:23">
      <LinienID>RB 37</LinienID>
      <RichtungsID>2</RichtungsID>
      <FahrtRef>
        <FahrtID>
          <FahrtBezeichner>83972#WBA_DDS-LB</FahrtBezeichner>
          <Betriebstag>2024-06-05</Betriebstag>
        </FahrtID>
      </FahrtRef>
      <Komplettfahrt>true</Komplettfahrt>
      <BetreiberID>DDS-LB</BetreiberID>
      <IstHalt>
        <HaltID>de:09276:5025</HaltID>
        <Abfahrtszeit>2024-06-21T13:37:23</Abfahrtszeit>
        <AbfahrtssteigText>4</AbfahrtssteigText>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
        <Besetztgrad>Unbekannt</Besetztgrad>
      </IstHalt>
      <IstHalt>
        <HaltID>de:09276:80503</HaltID>
        <Abfahrtszeit>2024-06-21T13:37:23</Abfahrtszeit>
        <Ankunftszeit>2024-06-21T13:37:23</Ankunftszeit>
        <AbfahrtssteigText>1</AbfahrtssteigText>
        <AnkunftssteigText>1</AnkunftssteigText>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
        <Besetztgrad>Unbekannt</Besetztgrad>
      </IstHalt>
      <LinienText>RB 37</LinienText>
      <ProduktID>WBA</ProduktID>
      <RichtungsText>Bodenmais</RichtungsText>
      <Zusatzfahrt>false</Zusatzfahrt>
      <FaelltAus>false</FaelltAus>
    </IstFahrt>
    <IstFahrt Zst="2024-06-21T13:37:23">
      <LinienID>RB5</LinienID>
      <RichtungsID>2</RichtungsID>
      <FahrtRef>
        <FahrtID>
          <FahrtBezeichner>80188#VBG_DDS-LB</FahrtBezeichner>
          <Betriebstag>2024-06-05</Betriebstag>
        </FahrtID>
      </FahrtRef>
      <Komplettfahrt>true</Komplettfahrt>
      <BetreiberID>DDS-LB</BetreiberID>
      <IstHalt>
        <HaltID>de:14523:379</HaltID>
        <Abfahrtszeit>2024-06-21T13:37:23</Abfahrtszeit>
        <AbfahrtssteigText>2</AbfahrtssteigText>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
        <Besetztgrad>Unbekannt</Besetztgrad>
      </IstHalt>
      <IstHalt>
        <HaltID>de:14523:1100</HaltID>
        <Abfahrtszeit>2024-06-21T13:37:23</Abfahrtszeit>
        <Ankunftszeit>2024-06-21T13:37:23</Ankunftszeit>
        <AbfahrtssteigText>1</AbfahrtssteigText>
        <AnkunftssteigText>1</AnkunftssteigText>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
        <Besetztgrad>Unbekannt</Besetztgrad>
      </IstHalt>
      <IstHalt>
        <HaltID>de:14523:309</HaltID>
        <Abfahrtszeit>2024-06-21T13:37:23</Abfahrtszeit>
        <Ankunftszeit>2024-06-21T13:37:23</Ankunftszeit>
        <IstAbfahrtPrognose>2024-06-21T13:37:23</IstAbfahrtPrognose>
        <IstAnkunftPrognose>2024-06-21T13:37:23</IstAnkunftPrognose>
        <AbfahrtssteigText>1</AbfahrtssteigText>
        <AnkunftssteigText>1</AnkunftssteigText>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
        <Besetztgrad>Unbekannt</Besetztgrad>
      </IstHalt>
      <IstHalt>
        <HaltID>de:14523:517</HaltID>
        <Abfahrtszeit>2024-06-21T13:37:23</Abfahrtszeit>
        <Ankunftszeit>2024-06-21T13:37:23</Ankunftszeit>
        <AbfahrtssteigText>1</AbfahrtssteigText>
        <AnkunftssteigText>1</AnkunftssteigText>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
        <Besetztgrad>Unbekannt</Besetztgrad>
      </IstHalt>
      <IstHalt>
        <HaltID>de:14523:66</HaltID>
        <Abfahrtszeit>2024-06-21T13:37:23</Abfahrtszeit>
        <Ankunftszeit>2024-06-21T13:37:23</Ankunftszeit>
        <AbfahrtssteigText>4</AbfahrtssteigText>
        <AnkunftssteigText>4</AnkunftssteigText>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
        <Besetztgrad>Unbekannt</Besetztgrad>
      </IstHalt>
      <IstHalt>
        <HaltID>de:14523:1132</HaltID>
        <Abfahrtszeit>2024-06-21T13:37:23</Abfahrtszeit>
        <Ankunftszeit>2024-06-21T13:37:23</Ankunftszeit>
        <AbfahrtssteigText>2</AbfahrtssteigText>
        <AnkunftssteigText>2</AnkunftssteigText>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
        <Besetztgrad>Unbekannt</Besetztgrad>
      </IstHalt>
      <IstHalt>
        <HaltID>de:14523:1117</HaltID>
        <Abfahrtszeit>2024-06-21T13:37:23</Abfahrtszeit>
        <Ankunftszeit>2024-06-21T13:37:23</Ankunftszeit>
        <AbfahrtssteigText>2</AbfahrtssteigText>
        <AnkunftssteigText>2</AnkunftssteigText>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
        <Besetztgrad>Unbekannt</Besetztgrad>
      </IstHalt>
      <IstHalt>
        <HaltID>de:14523:1118</HaltID>
        <Abfahrtszeit>2024-06-21T13:37:23</Abfahrtszeit>
        <Ankunftszeit>2024-06-21T13:37:23</Ankunftszeit>
        <AbfahrtssteigText>2</AbfahrtssteigText>
        <AnkunftssteigText>2</AnkunftssteigText>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
        <Besetztgrad>Unbekannt</Besetztgrad>
      </IstHalt>
      <IstHalt>
        <HaltID>de:14523:1144</HaltID>
        <Ankunftszeit>2024-06-21T13:37:23</Ankunftszeit>
        <AnkunftssteigText>5</AnkunftssteigText>
        <Einsteigeverbot>false</Einsteigeverbot>
        <Aussteigeverbot>false</Aussteigeverbot>
        <Durchfahrt>false</Durchfahrt>
        <Zusatzhalt>false</Zusatzhalt>
        <Besetztgrad>Unbekannt</Besetztgrad>
      </IstHalt>
      <LinienText>RB5</LinienText>
      <ProduktID>VBG</ProduktID>
      <RichtungsText>Plauen(Vogtl)ob Bf</RichtungsText>
      <Zusatzfahrt>false</Zusatzfahrt>
      <FaelltAus>false</FaelltAus>
    </IstFahrt>
  </AUSNachricht>
</DatenAbrufenAntwort>
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