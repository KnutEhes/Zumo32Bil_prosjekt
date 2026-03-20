#pragma once

// nextDir: 'F' = rett frem, 'L' = venstre, 'R' = høyre, 'U' = snu (180°)
// Returnerer true når svingen er fullført (klar for neste instruksjon fra server)
bool driveTo(char nextDir);
