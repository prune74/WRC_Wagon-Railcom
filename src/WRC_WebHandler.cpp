#include "WRC_WebHandler.h"
#include "WRC_Debug.h"
#include "WRC_Settings.h"
#include "WRC_Json.h"
#include "WRC_FXDriver.h"
#include <SPIFFS.h>

AsyncWebServer *WRC_WebHandler::serveur = nullptr;
AsyncWebSocket *WRC_WebHandler::ws = nullptr;

/* ---------------------------------------------------------------------------
 * DÉMARRAGE DU SERVEUR WEB
 * ------------------------------------------------------------------------- */
void WRC_WebHandler::Begin(uint16_t port)
{
    serveur = new AsyncWebServer(port);
    ws = new AsyncWebSocket("/ws");

    ws->onEvent([](AsyncWebSocket *server,
                   AsyncWebSocketClient *client,
                   AwsEventType type,
                   void *arg,
                   uint8_t *data,
                   size_t len)
                { WRC_WebHandler::evenementWs(server, client, type, arg, data, len); });

    serveur->addHandler(ws);

    definirRoutes();
    serveur->begin();

    LOG_INFO("Serveur Web démarré sur port %u", port);
}

/* ---------------------------------------------------------------------------
 * ROUTES HTTP
 * ------------------------------------------------------------------------- */
void WRC_WebHandler::definirRoutes()
{
    // -----------------------------------------------------------------------
    // FICHIERS STATIQUES
    // -----------------------------------------------------------------------
    serveur->serveStatic("/", SPIFFS, "/index.html");
    serveur->serveStatic("/index.html", SPIFFS, "/index.html");
    serveur->serveStatic("/style.css", SPIFFS, "/style.css");
    serveur->serveStatic("/script.js", SPIFFS, "/script.js");
    serveur->serveStatic("/Settings.json", SPIFFS, "/Settings.json");

    serveur->onNotFound([](AsyncWebServerRequest *req)
                        { req->send(SPIFFS, "/index.html", "text/html"); });

    /* -----------------------------------------------------------------------
     * GET /obtenirParametres
     * --------------------------------------------------------------------- */
    serveur->on("/obtenirParametres", HTTP_GET, [](AsyncWebServerRequest *req)
                {
        JsonDocument doc;
        JsonObject root = doc.to<JsonObject>();

        root["wifi_actif"] = WRC_Settings::WIFI_ACTIF;
        root["adresse"]    = WRC_Settings::ADRESSE;

        root["servo_porte_angle_ouvert"] = WRC_Settings::SERVO_PORTE_ANGLE_OUVERT;
        root["servo_porte_angle_ferme"]  = WRC_Settings::SERVO_PORTE_ANGLE_FERME;
        root["servo_porte_vitesse"]      = WRC_Settings::SERVO_PORTE_VITESSE;

        root["feu_arriere"]        = WRC_Settings::FEU_ARRIERE;
        root["lumiere_interieure"] = WRC_Settings::LUMIERE_INTERIEURE;
        root["servo_porte"]        = WRC_Settings::SERVO_PORTE;

        root["essieux"] = WRC_Settings::ESSIEUX;

        String out;
        serializeJson(doc, out);
        req->send(200, "application/json", out); });

    /* -----------------------------------------------------------------------
     * POST /definirAdresse
     * --------------------------------------------------------------------- */
    serveur->on("/definirAdresse", HTTP_POST, [](AsyncWebServerRequest *req) {}, nullptr, [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t)
                {
        String body = String((char*)data).substring(0, len);

        uint16_t adresse;
        if (!WRC_Json::lireAdresse(body, adresse)) {
            req->send(400, "text/plain", "JSON invalide");
            return;
        }

        if (adresse < 4001 || adresse > 4999) {
            req->send(400, "text/plain", "Adresse hors plage (4001–4999)");
            return;
        }

        WRC_Settings::ADRESSE = adresse;
        WRC_Settings::writeFile();
        req->send(200, "text/plain", "OK"); });

    /* -----------------------------------------------------------------------
     * POST /definirWifi
     * --------------------------------------------------------------------- */
    serveur->on("/definirWifi", HTTP_POST, [](AsyncWebServerRequest *req) {}, nullptr, [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t)
                {
        String body = String((char*)data).substring(0, len);

        bool wifi_actif;
        if (!WRC_Json::lireWifi(body, wifi_actif)) {
            req->send(400, "text/plain", "JSON invalide");
            return;
        }

        WRC_Settings::WIFI_ACTIF = wifi_actif;
        WRC_Settings::writeFile();
        req->send(200, "text/plain", "OK"); });

    /* -----------------------------------------------------------------------
     * POST /definirFx
     * --------------------------------------------------------------------- */
    serveur->on("/definirFx", HTTP_POST, [](AsyncWebServerRequest *req) {}, nullptr, [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t)
                {
    String body = String((char*)data).substring(0, len);

    JsonDocument doc;
    if (deserializeJson(doc, body)) {
        req->send(400, "text/plain", "JSON invalide");
        return;
    }

    WRC_Settings::FEU_ARRIERE        = doc["feu_arriere"]        | WRC_Settings::FEU_ARRIERE;
    WRC_Settings::LUMIERE_INTERIEURE = doc["lumiere_interieure"] | WRC_Settings::LUMIERE_INTERIEURE;
    WRC_Settings::SERVO_PORTE        = doc["servo_porte"]        | WRC_Settings::SERVO_PORTE;

    WRC_Settings::writeFile();
    req->send(200, "text/plain", "OK"); });

    /* -----------------------------------------------------------------------
     * POST /definirServoPorteParametres
     * --------------------------------------------------------------------- */
    serveur->on("/definirServoPorteParametres", HTTP_POST, [](AsyncWebServerRequest *req) {}, nullptr, [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t)
                {
    String body = String((char*)data).substring(0, len);

    JsonDocument doc;
    if (deserializeJson(doc, body)) {
        req->send(400, "text/plain", "JSON invalide");
        return;
    }

    WRC_Settings::SERVO_PORTE_ANGLE_OUVERT = doc["servo_porte_angle_ouvert"] | WRC_Settings::SERVO_PORTE_ANGLE_OUVERT;
    WRC_Settings::SERVO_PORTE_ANGLE_FERME  = doc["servo_porte_angle_ferme"]  | WRC_Settings::SERVO_PORTE_ANGLE_FERME;
    WRC_Settings::SERVO_PORTE_VITESSE      = doc["servo_porte_vitesse"]      | WRC_Settings::SERVO_PORTE_VITESSE;

    WRC_Settings::writeFile();
    req->send(200, "text/plain", "OK"); });

    /* -----------------------------------------------------------------------
     * POST /definirEssieux
     * --------------------------------------------------------------------- */   
    serveur->on("/definirEssieux", HTTP_POST, [](AsyncWebServerRequest *req) {}, nullptr, [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t)
                {
    String body = String((char*)data).substring(0, len);

    JsonDocument doc;
    if (deserializeJson(doc, body)) {
        req->send(400, "text/plain", "JSON invalide");
        return;
    }

    bool essieuxBool = doc["essieux"] | false;

    // false = 2 essieux
    // true  = 4 essieux
    WRC_Settings::ESSIEUX = essieuxBool;

    WRC_Settings::writeFile();
    req->send(200, "text/plain", "OK"); });

    /* -----------------------------------------------------------------------
     * POST /sauvegardeGenerale
     * --------------------------------------------------------------------- */
    serveur->on("/sauvegardeGenerale", HTTP_POST, [](AsyncWebServerRequest *req) {}, nullptr, [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t)
                {
        String body = String((char*)data).substring(0, len);

        JsonDocument doc;
        if (deserializeJson(doc, body)) {
            req->send(400, "text/plain", "JSON invalide");
            return;
        }

        WRC_Settings::ADRESSE = doc["adresse"] | WRC_Settings::ADRESSE;
        WRC_Settings::WIFI_ACTIF = doc["wifi_actif"] | WRC_Settings::WIFI_ACTIF;

        WRC_Settings::SERVO_PORTE_ANGLE_OUVERT = doc["servo_porte_angle_ouvert"] | WRC_Settings::SERVO_PORTE_ANGLE_OUVERT;
        WRC_Settings::SERVO_PORTE_ANGLE_FERME  = doc["servo_porte_angle_ferme"]  | WRC_Settings::SERVO_PORTE_ANGLE_FERME;
        WRC_Settings::SERVO_PORTE_VITESSE      = doc["servo_porte_vitesse"]      | WRC_Settings::SERVO_PORTE_VITESSE;

        WRC_Settings::FEU_ARRIERE        = doc["feu_arriere"]        | WRC_Settings::FEU_ARRIERE;
        WRC_Settings::LUMIERE_INTERIEURE = doc["lumiere_interieure"] | WRC_Settings::LUMIERE_INTERIEURE;
        WRC_Settings::SERVO_PORTE        = doc["servo_porte"]        | WRC_Settings::SERVO_PORTE;

        WRC_Settings::ESSIEUX            = doc["essieux"]        | WRC_Settings::ESSIEUX;

        WRC_Settings::writeFile();
        req->send(200, "text/plain", "Sauvegarde OK"); });

    /* -----------------------------------------------------------------------
     * POST /resetParametres
     * --------------------------------------------------------------------- */
    serveur->on("/resetParametres", HTTP_POST, [](AsyncWebServerRequest *req)
                {
        WRC_Settings::WIFI_ACTIF = true;
        WRC_Settings::ADRESSE    = 4001;

        WRC_Settings::SERVO_PORTE_ANGLE_OUVERT = 160;
        WRC_Settings::SERVO_PORTE_ANGLE_FERME  = 20;
        WRC_Settings::SERVO_PORTE_VITESSE      = 3;

        WRC_Settings::FEU_ARRIERE        = false;
        WRC_Settings::LUMIERE_INTERIEURE = false;
        WRC_Settings::SERVO_PORTE        = false;

        WRC_Settings::ESSIEUX            = false;

        WRC_Settings::writeFile();
        req->send(200, "text/plain", "Paramètres réinitialisés"); });
}

/* ---------------------------------------------------------------------------
 * WEBSOCKET
 * ------------------------------------------------------------------------- */
void WRC_WebHandler::evenementWs(AsyncWebSocket *server,
                                 AsyncWebSocketClient *client,
                                 AwsEventType type,
                                 void *arg,
                                 uint8_t *data,
                                 size_t len)
{
    if (type == WS_EVT_CONNECT)
        LOG_INFO("Client WebSocket connecté : %u", client->id());
    else if (type == WS_EVT_DISCONNECT)
        LOG_INFO("Client WebSocket déconnecté : %u", client->id());
}

/* ---------------------------------------------------------------------------
 * BOUCLE
 * ------------------------------------------------------------------------- */
void WRC_WebHandler::Loop()
{
    if (ws)
        ws->cleanupClients();
}
