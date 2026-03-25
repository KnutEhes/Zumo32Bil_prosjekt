# Zumo32Bil_prosjekt
FabianPipi
Arsenal123

--------------------------------------------------------------------------------
PROSJEKTPLAN: SMART-CITY (ZUMO & IOT-STACK)
--------------------------------------------------------------------------------

1. FYSISK LAG OG BIL (ZUMO & ESP32)
===================================
[x] Mekanisk: Designe/lage brett/feste for ESP32 på Zumo-bilen.
[x] Strøm: Sikre stabil strømforsyning til både ESP32 og Zumo.
[x] Kommunikasjon: Sette opp Seriell (UART) mellom Zumo og ESP32.
[x] Linjefølging: Teste og finjustere sensorer for stabil kjøring.
[ ] Ladestasjon: Implementere deteksjon (f.eks. IR-sensor eller ultralyd).
[/] Lyskryss: Programmere stopp- og start-logikk basert på signaler.

2. INFRASTRUKTUR OG NETTVERK (GATEWAY & MQTT)
==============================================
[x] WiFi: Kode ESP32 til å koble seg automatisk til lokalt nett.
[x] Broker: Installere og konfigurere Mosquitto på Raspberry Pi.
[x] Topic-struktur: Definere meldingsstier (f.eks. smartcity/bil1/status).
[x] Sikkerhet: Sette opp brukernavn og passord for MQTT-klienter.

3. STYRING OG VISUALISERING (NODE-RED & DASHBOARD)
==================================================
[x] Node-RED: Installere på Raspberry Pi og sette opp flows.
[x] Oppstrøms: Tolke JSON-data fra ESP32 og vise i Dashboard.
[x] Nedstrøms: Lage knapper i Dashboard som sender MQTT-kommandoer.
[x] Styring: Implementere valg for sving (Venstre/Høyre) fra Dashboard.

4. SKY OG EKSTERN TILGANG
=========================
[x] Sky: Sette opp en "Cloud Broker" eller VPN for tilgang utenfor huset.
[x] Dashboard: Gjøre brukergrensesnittet tilgjengelig via nettleser.

--------------------------------------------------------------------------------
5. BYGRID-SYSTEM (AVANSERT NAVIGASJON)
======================================
[ ] Kartlegging: Definere et koordinatsystem eller noder for byen i Node-RED.
[ ] Posisjonering: Implementere metode for at bilen vet hvilket kryss den er i:
    - Alternativ A: RFID-tags under banen og leser på bilen.
    - Alternativ B: Fargekoder i kryssene som Zumo-sensoren leser.
    - Alternativ C: Telling av krysninger/linjebrudd.
[ ] Ruteplanlegging: Logikk i Node-RED som finner korteste vei (f.eks. A*).
[ ] Grid-Dashboard: Lage et visuelt kart i Dashboard som viser bilens posisjon.

--------------------------------------------------------------------------------
DATAFLYT (SYSTEMARKITEKTUR)
--------------------------------------------------------------------------------

OPPSTRØMS (Data fra bil til sky)
-------------------------------------------------------------------------
Zumo Robot -> (Seriell) -> ESP32 -> (WiFi/MQTT) -> RPi -> Node-RED -> Dashboard
(Sensorer)                (Node)                 (Gateway)           (Visning)

NEDSTRØMS (Kommando fra sky til bil)
-------------------------------------------------------------------------
Dashboard -> Node-RED -> (MQTT) -> ESP32 -> (Seriell) -> Zumo Robot
(Knapper)                (Broker)  (Node)                (Aktuatorer)

--------------------------------------------------------------------------------
UTSTYRSLISTE
--------------------------------------------------------------------------------

ENHETER:
[x] Zumo Robot (Pololu)      - Kjøretøy og sensorer
[x] ESP32 Mikrokontroller    - WiFi-kommunikasjon (Node)
[x] Raspberry Pi             - Server og Gateway
[ ] MicroSD-kort             - OS for Raspberry Pi

KOBLING:
[ ] Prototypebrett           - For montering på Zumo
[x] Jumper-kabler            - For seriell kobling (TX/RX)
[ ] Batterier / Powerbank    - Strømforsyning

PROGRAMVARE:
[x] Arduino IDE / VS Code    - Koding av ESP32
[ ] Mosquitto                - MQTT Broker
[ ] Node-RED                 - Logikk og Dashboard
--------------------------------------------------------------------------------