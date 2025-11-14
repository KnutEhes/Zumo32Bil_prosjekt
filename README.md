# Zumo32Bil_prosjekt
Prosjekt i IELS Zumo32u4 bil

Obligatoriske moduler:

1. Hovedmodul
- Modulen skal styre bilens funksjoner og koordinere alle andre moduler.

Ekstrapoeng for:
- God bruk av bilens sensorer.
- Oversiktlig og robust programstruktur.
- Implementerte løsninger for å tjene penger.


2. SW-batteri og energihåndtering
- Emulerer et batteri som lades ut ved kjøring og lades opp ved ladestasjoner.
- Skal varsle om lavt batterinivå.
- Rapporterer batteristatus til resten av systemet.

Ekstrapoeng for:
- Realistiske batterifunksjoner (f.eks. degradering over tid, påvirkning av kjøremønster og hurtiglading).
- Gode metoder for beregning av batteriforbruk basert på kjøremønster.


3. Ladeløsning
- Bilen skal kunne detektere en "ladestasjon".
- Skal stoppe ved stasjonen, lade, og fullføre en enkel betalingsprosess.
- Ladeløsningen trenger ikke være fysisk.

Ekstrapoeng for:
- Realistiske lademekanismer med meny og ladestatus (type lading, prosent, osv.).
- Effektiv håndtering av kundesaldo (penger inn/ut og transaksjoner).


4. Linjefølging og trafikklys
- Bilen skal følge en teipbane.
- Skal stoppe ved rødt lys og kjøre ved grønt (uten kamera).

Ekstrapoeng for:
- Utfordringer i banen (krappe svinger, blindgate, rasteplass, manglende teip, osv.) som bilen håndterer korrekt.
- Bruk av flere sensorer for forbedret navigasjon.