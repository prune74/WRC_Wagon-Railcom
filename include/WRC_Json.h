#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

class WRC_Json
{
public:
    static String construireJsonParametres();

    static bool lireAdresse(const String &json, uint16_t &adresse);
    static bool lireWifi(const String &json, bool &wifi_actif);
    static bool lireFx(const String &json, bool &feu_arriere, bool &lumiere_interieure, bool &servo_porte);
};
