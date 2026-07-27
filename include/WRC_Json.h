#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

class WRC_Json
{
public:
    // Construction du JSON complet
    static String construireJsonParametres();

    // Lecture des paramètres simples
    static bool lireAdresse(const String &json, uint16_t &adresse);
    static bool lireWifi(const String &json, bool &wifi_actif);

    // ⭐ Lecture des FX (une fonction par FX)
    static bool lireFeuArriere(const String &json, bool &FxFeuArriere_actif);
    static bool lireLumiereInterieure(const String &json, bool &FxLumiereInterieure_actif);
    static bool lireServoPorteFx(const String &json, bool &FxServoPorte_actif);
};
