#include "WRC_Json.h"
#include "WRC_Settings.h"
#include "WRC_Debug.h"

/* ---------------------------------------------------------------------------
 * JSON → renvoie tous les paramètres du wagon
 * ------------------------------------------------------------------------- */
String WRC_Json::construireJsonParametres()
{
    JsonDocument doc;

    JsonObject root = doc.to<JsonObject>();
    JsonObject fx = root["fx"].to<JsonObject>();

    root["adresse"] = WRC_Settings::ADRESSE;
    root["wifi_actif"] = WRC_Settings::WIFI_ACTIF;

    // ⭐ FX dynamiques
    for (size_t i = 0; i < WRC_Settings::FX_COUNT; i++)
    {
        const char *name = WRC_Settings::FX_LIST[i]->jsonName;
        fx[name] = *(WRC_Settings::FX_LIST[i]->value);
    }

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

    feu_arriere = fx["feu_arriere"] | false;
    lumiere_interieure = fx["lumiere_interieure"] | false;
    servo_porte = fx["servo_porte"] | false;

    return true;
}
