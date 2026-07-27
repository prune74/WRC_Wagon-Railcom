#pragma once
#include <Arduino.h>

class WRC_DCCHandler
{
public:
    // Appelé par le décodeur DCC quand une fonction est reçue
    static void onFunction(uint16_t adresse, uint8_t fn, bool actif);
};
