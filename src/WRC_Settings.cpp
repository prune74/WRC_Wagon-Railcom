#include "WRC_Settings.h"
#include "WRC_Debug.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>

bool WRC_Settings::WIFI_ACTIF                   = true;
uint16_t WRC_Settings::ADRESSE                  = 4001;

uint16_t WRC_Settings::SERVO_PORTE_ANGLE_OUVERT = 160;
uint16_t WRC_Settings::SERVO_PORTE_ANGLE_FERME  = 20;
uint16_t WRC_Settings::SERVO_PORTE_VITESSE      = 3;

bool WRC_Settings::FEU_ARRIERE                  = false;
bool WRC_Settings::LUMIERE_INTERIEURE           = false;
bool WRC_Settings::SERVO_PORTE                  = false;

bool WRC_Settings::ESSIEUX                      = false;

String WRC_Settings::TYPE_WAGON                 = "autre";

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

    WIFI_ACTIF               = doc["wifi_actif"]                | WIFI_ACTIF;
    ADRESSE                  = doc["adresse"]                   | ADRESSE;

    SERVO_PORTE_ANGLE_OUVERT = doc["servo_porte_angle_ouvert"]  | SERVO_PORTE_ANGLE_OUVERT;
    SERVO_PORTE_ANGLE_FERME  = doc["servo_porte_angle_ferme"]   | SERVO_PORTE_ANGLE_FERME;
    SERVO_PORTE_VITESSE      = doc["servo_porte_vitesse"]       | SERVO_PORTE_VITESSE;

    FEU_ARRIERE              = doc["feu_arriere"]               | FEU_ARRIERE;
    LUMIERE_INTERIEURE       = doc["lumiere_interieure"]        | LUMIERE_INTERIEURE;
    SERVO_PORTE              = doc["servo_porte"]               | SERVO_PORTE;

    ESSIEUX                  = doc["essieux"]                   | ESSIEUX;
    TYPE_WAGON               = doc["type_wagon"]                | TYPE_WAGON;

}

void WRC_Settings::writeFile()
{
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();

    root["wifi_actif"] = WIFI_ACTIF;
    root["adresse"] = ADRESSE;

    root["servo_porte_angle_ouvert"]    = SERVO_PORTE_ANGLE_OUVERT;
    root["servo_porte_angle_ferme"]     = SERVO_PORTE_ANGLE_FERME;
    root["servo_porte_vitesse"]         = SERVO_PORTE_VITESSE;

    root["feu_arriere"]                 = FEU_ARRIERE;
    root["lumiere_interieure"]          = LUMIERE_INTERIEURE;
    root["servo_porte"]                 = SERVO_PORTE;

    root["essieux"]                     = ESSIEUX;
    root["type_wagon"]                  = TYPE_WAGON;

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
