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

    root["adresse"] = WRC_Settings::ADRESSE;
    root["wifi_actif"] = WRC_Settings::WIFI_ACTIF;

    // Servo
    root["servo_porte_angle_ouvert"] = WRC_Settings::SERVO_PORTE_ANGLE_OUVERT;
    root["servo_porte_angle_ferme"] = WRC_Settings::SERVO_PORTE_ANGLE_FERME;
    root["servo_porte_vitesse"] = WRC_Settings::SERVO_PORTE_VITESSE;

    // FX
    root["feu_arriere"] = WRC_Settings::FEU_ARRIERE;
    root["lumiere_interieure"] = WRC_Settings::LUMIERE_INTERIEURE;
    root["servo_porte"] = WRC_Settings::SERVO_PORTE;

    // Wagon
    root["essieux"] = WRC_Settings::ESSIEUX;
    root["type_wagon"] = WRC_Settings::TYPE_WAGON;
    root["longueur_mm"] = WRC_Settings::LONGUEUR_MM;

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
 * JSON → parse FX feu arrière
 * ------------------------------------------------------------------------- */
bool WRC_Json::lireFeuArriere(const String &json, bool &FxFeuArriere_actif)
{
    JsonDocument doc;
    if (deserializeJson(doc, json))
        return false;
    if (!doc["feu_arriere"].is<bool>())
        return false;

    FxFeuArriere_actif = doc["feu_arriere"].as<bool>();
    return true;
}

/* ---------------------------------------------------------------------------
 * JSON → parse FX lumière intérieure
 * ------------------------------------------------------------------------- */
bool WRC_Json::lireLumiereInterieure(const String &json, bool &FxLumiereInterieure_actif)
{
    JsonDocument doc;
    if (deserializeJson(doc, json))
        return false;
    if (!doc["lumiere_interieure"].is<bool>())
        return false;

    FxLumiereInterieure_actif = doc["lumiere_interieure"].as<bool>();
    return true;
}

/* ---------------------------------------------------------------------------
 * JSON → parse FX servo porte
 * ------------------------------------------------------------------------- */
bool WRC_Json::lireServoPorteFx(const String &json, bool &FxServoPorte_actif)
{
    JsonDocument doc;
    if (deserializeJson(doc, json))
        return false;
    if (!doc["servo_porte"].is<bool>())
        return false;

    FxServoPorte_actif = doc["servo_porte"].as<bool>();
    return true;
}

/* ---------------------------------------------------------------------------
 * JSON → parse ESSIEUX
 * ------------------------------------------------------------------------- */
bool WRC_Json::lireEssieux(const String &json, bool &essieux)
{
    JsonDocument doc;
    if (deserializeJson(doc, json))
        return false;
    if (!doc["essieux"].is<bool>())
        return false;

    essieux = doc["essieux"].as<bool>();
    return true;
}

