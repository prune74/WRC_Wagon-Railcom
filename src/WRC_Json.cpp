#include "WRC_Json.h"
#include "WRC_Settings.h"
#include "WRC_Debug.h"

/* ---------------------------------------------------------------------------
 * JSON → renvoie tous les paramètres du wagon
 * ------------------------------------------------------------------------- */
String WRC_Json::construireJsonParametres()
{
    JsonDocument doc;

    doc["adresse"]    = WRC_Settings::ADRESSE;
    doc["wifi_actif"] = WRC_Settings::WIFI_ACTIF;

    // ArduinoJson V7 : création explicite de l'objet fx
    doc["fx"] = JsonObject();
    JsonObject fx = doc["fx"].as<JsonObject>();

    fx["feu_arriere"]        = WRC_Settings::FX_FEU_ARRIERE;
    fx["lumiere_interieure"] = WRC_Settings::FX_LUMIERE_INTERIEURE;
    fx["servo_porte"]        = WRC_Settings::FX_SERVO_PORTE;

    String out;
    serializeJson(doc, out);
    return out;
}

/* ---------------------------------------------------------------------------
 * JSON → parse adresse
 * ------------------------------------------------------------------------- */
bool WRC_Json::lireAdresse(const String &json, uint16_t &adresse)
{
    JsonDocument doc;
    if (deserializeJson(doc, json))
        return false;

    if (!doc["adresse"].is<uint16_t>())
        return false;

    adresse = doc["adresse"].as<uint16_t>();
    return true;
}

/* ---------------------------------------------------------------------------
 * JSON → parse wifi_actif
 * ------------------------------------------------------------------------- */
bool WRC_Json::lireWifi(const String &json, bool &wifi_actif)
{
    JsonDocument doc;
    if (deserializeJson(doc, json))
        return false;

    if (!doc["wifi_actif"].is<bool>())
        return false;

    wifi_actif = doc["wifi_actif"].as<bool>();
    return true;
}

/* ---------------------------------------------------------------------------
 * JSON → parse FX
 * ------------------------------------------------------------------------- */
bool WRC_Json::lireFx(const String &json,
                      bool &feu_arriere,
                      bool &lumiere_interieure,
                      bool &servo_porte)
{
    JsonDocument doc;
    if (deserializeJson(doc, json))
        return false;

    if (!doc["fx"].is<JsonObject>())
        return false;

    JsonObject fx = doc["fx"].as<JsonObject>();

    feu_arriere        = fx["feu_arriere"]        | false;
    lumiere_interieure = fx["lumiere_interieure"] | false;
    servo_porte        = fx["servo_porte"]        | false;

    return true;
}
