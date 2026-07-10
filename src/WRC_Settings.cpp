#include "WRC_Settings.h"
#include "WRC_Debug.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>

// ---------------------------------------------------------------------------
// VARIABLES STATIQUES
// ---------------------------------------------------------------------------
bool     WRC_Settings::WIFI_ACTIF        = true;
uint16_t WRC_Settings::ADRESSE           = 4001;

bool WRC_Settings::FX_FEU_ARRIERE        = false;
bool WRC_Settings::FX_LUMIERE_INTERIEURE = false;
bool WRC_Settings::FX_SERVO_PORTE        = false;

// ---------------------------------------------------------------------------
// INITIALISATION SPIFFS
// ---------------------------------------------------------------------------
void WRC_Settings::Begin()
{
    if (!SPIFFS.begin(true))
    {
        LOG_ERROR("SPIFFS → échec du montage");
        return;
    }

    LOG_INFO("SPIFFS monté avec succès");
}

// ---------------------------------------------------------------------------
// LECTURE DU FICHIER Settings.json
// ---------------------------------------------------------------------------
void WRC_Settings::readFile()
{
    File file = SPIFFS.open("/Settings.json", "r");
    if (!file)
    {
        LOG_WARN("Settings.json introuvable → valeurs par défaut utilisées");
        return;
    }

    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        LOG_ERROR("Erreur JSON → valeurs par défaut");
        file.close();
        return;
    }

    WIFI_ACTIF = doc["wifi_actif"] | true;
    ADRESSE    = doc["adresse"]    | 4001;

    if (doc["fx"].is<JsonObject>())
    {
        FX_FEU_ARRIERE        = doc["fx"]["feu_arriere"]        | false;
        FX_LUMIERE_INTERIEURE = doc["fx"]["lumiere_interieure"] | false;
        FX_SERVO_PORTE        = doc["fx"]["servo_porte"]        | false;
    }
    else
    {
        FX_FEU_ARRIERE        = false;
        FX_LUMIERE_INTERIEURE = false;
        FX_SERVO_PORTE        = false;
    }

    file.close();
}

// ---------------------------------------------------------------------------
// ÉCRITURE DU FICHIER Settings.json
// ---------------------------------------------------------------------------
void WRC_Settings::writeFile()
{
    JsonDocument doc;

    doc["wifi_actif"] = WIFI_ACTIF;
    doc["adresse"]    = ADRESSE;

    // ArduinoJson V7 : création correcte du bloc fx
    doc["fx"] = JsonObject();               // ← crée l'objet
    JsonObject fx = doc["fx"].as<JsonObject>();  // ← récupère l'objet

    fx["feu_arriere"]        = FX_FEU_ARRIERE;
    fx["lumiere_interieure"] = FX_LUMIERE_INTERIEURE;
    fx["servo_porte"]        = FX_SERVO_PORTE;

    File file = SPIFFS.open("/Settings.json", "w");
    if (!file)
    {
        LOG_ERROR("SPIFFS → impossible d’ouvrir Settings.json en écriture");
        return;
    }

    serializeJsonPretty(doc, file);
    file.close();
}

// ---------------------------------------------------------------------------
// GETTERS
// ---------------------------------------------------------------------------
uint16_t WRC_Settings::getAdresse()
{
    return ADRESSE;
}
