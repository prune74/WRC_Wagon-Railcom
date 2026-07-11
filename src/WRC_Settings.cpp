#include "WRC_Settings.h"
#include "WRC_Debug.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>

bool     WRC_Settings::WIFI_ACTIF = true;
uint16_t WRC_Settings::ADRESSE    = 4001;

// ---------------------------------------------------------
// Définitions générées automatiquement
// ---------------------------------------------------------
#define FX_ENTRY(NAME, JSON, FUNC) \
    bool WRC_Settings::FX_##NAME = false; \
    WRC_Settings::FxItem WRC_Settings::FX_ITEM_##NAME = { JSON, &WRC_Settings::FX_##NAME };

#include "WRC_FX.inc"
#undef FX_ENTRY

// ---------------------------------------------------------
// Tableau FX_LIST généré automatiquement
// ---------------------------------------------------------
#define FX_ENTRY(NAME, JSON, FUNC) &WRC_Settings::FX_ITEM_##NAME,

WRC_Settings::FxItem* WRC_Settings::FX_LIST[] = {
    #include "WRC_FX.inc"
};

#undef FX_ENTRY

size_t WRC_Settings::FX_COUNT = sizeof(WRC_Settings::FX_LIST) / sizeof(FxItem*);

// ---------------------------------------------------------
// JSON + SPIFFS
// ---------------------------------------------------------
void WRC_Settings::Begin()
{
    if (!SPIFFS.begin(true))
    {
        LOG_ERROR("SPIFFS → échec du montage");
        return;
    }

    LOG_INFO("SPIFFS monté avec succès");
    readFile();
}

void WRC_Settings::readFile()
{
    File file = SPIFFS.open("/Settings.json", "r");
    if (!file)
    {
        LOG_WARN("Settings.json introuvable → valeurs par défaut");
        return;
    }

    JsonDocument doc;
    if (deserializeJson(doc, file))
    {
        LOG_ERROR("Erreur JSON → valeurs par défaut");
        file.close();
        return;
    }
    file.close();

    WIFI_ACTIF = doc["wifi_actif"] | WIFI_ACTIF;
    ADRESSE    = doc["adresse"]    | ADRESSE;

    JsonObject fx = doc["fx"];
    if (!fx.isNull())
    {
        for (size_t i = 0; i < FX_COUNT; i++)
            *(FX_LIST[i]->value) = fx[FX_LIST[i]->jsonName] | *(FX_LIST[i]->value);
    }
}

void WRC_Settings::writeFile()
{
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    JsonObject fx   = root["fx"].to<JsonObject>();

    root["wifi_actif"] = WIFI_ACTIF;
    root["adresse"]    = ADRESSE;

    for (size_t i = 0; i < FX_COUNT; i++)
        fx[FX_LIST[i]->jsonName] = *(FX_LIST[i]->value);

    File file = SPIFFS.open("/Settings.json", "w");
    if (!file)
    {
        LOG_ERROR("SPIFFS → impossible d’ouvrir Settings.json");
        return;
    }

    serializeJsonPretty(doc, file);
    file.close();
}

uint16_t WRC_Settings::getAdresse()
{
    return ADRESSE;
}
