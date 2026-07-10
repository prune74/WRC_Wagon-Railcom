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
    // FICHIERS STATIQUES (interface web)
    // -----------------------------------------------------------------------
    serveur->serveStatic("/", SPIFFS, "/index.html");
    serveur->serveStatic("/index.html", SPIFFS, "/index.html");
    serveur->serveStatic("/style.css", SPIFFS, "/style.css");
    serveur->serveStatic("/script.js", SPIFFS, "/script.js");
    serveur->serveStatic("/Settings.json", SPIFFS, "/Settings.json");

    // Fallback : toute URL inconnue renvoie index.html
    serveur->onNotFound([](AsyncWebServerRequest *req)
                        { req->send(SPIFFS, "/index.html", "text/html"); });

    /* -----------------------------------------------------------------------
     * GET /obtenirParametres
     * --------------------------------------------------------------------- */
    serveur->on("/obtenirParametres", HTTP_GET, [](AsyncWebServerRequest *req)
                {
        String json = WRC_Json::construireJsonParametres();
        req->send(200, "application/json", json); });

    /* -----------------------------------------------------------------------
     * POST /definirAdresse
     * --------------------------------------------------------------------- */
    serveur->on("/definirAdresse", HTTP_POST, [](AsyncWebServerRequest *req) {}, nullptr, [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t)
                {
        String body = String((char *)data).substring(0, len);

        uint16_t adresse;
        if (WRC_Json::lireAdresse(body, adresse))
        {
            // Vérification plage 4001–4999
            if (adresse < 4001 || adresse > 4999)
            {
                req->send(400, "text/plain", "Adresse hors plage autorisée (4001–4999)");
                LOG_WARN("Adresse rejetée : hors plage (%u)", adresse);
                return;
            }

            // OK → on enregistre
            WRC_Settings::ADRESSE = adresse;
            WRC_Settings::writeFile();
            req->send(200, "text/plain", "OK");
            LOG_INFO("Adresse mise à jour : %u", adresse);
        }
        else
        {
            req->send(400, "text/plain", "JSON invalide");
        } });

    /* -----------------------------------------------------------------------
     * POST /definirWifi
     * --------------------------------------------------------------------- */
    serveur->on("/definirWifi", HTTP_POST, [](AsyncWebServerRequest *req) {}, nullptr, [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t)
                {
        String body = String((char*)data).substring(0, len);

        bool wifi_actif;
        if (WRC_Json::lireWifi(body, wifi_actif))
        {
            WRC_Settings::WIFI_ACTIF = wifi_actif;
            WRC_Settings::writeFile();
            req->send(200, "text/plain", "OK");
            LOG_INFO("WiFi actif = %s", wifi_actif ? "true" : "false");
        }
        else
        {
            req->send(400, "text/plain", "JSON invalide");
        } });

    /* -----------------------------------------------------------------------
     * POST /definirFx
     * --------------------------------------------------------------------- */
    serveur->on("/definirFx", HTTP_POST, [](AsyncWebServerRequest *req) {}, nullptr, [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t, size_t)
                {
        String body = String((char*)data).substring(0, len);

        bool feu_arriere, lumiere_interieure, servo_porte;

        if (WRC_Json::lireFx(body, feu_arriere, lumiere_interieure, servo_porte))
        {
            WRC_Settings::FX_FEU_ARRIERE        = feu_arriere;
            WRC_Settings::FX_LUMIERE_INTERIEURE = lumiere_interieure;
            WRC_Settings::FX_SERVO_PORTE        = servo_porte;

            WRC_Settings::writeFile();
            WRC_FXDriver::mettreAJourFx();

            req->send(200, "text/plain", "OK");

            LOG_INFO("FX mis à jour : arrière=%s, intérieur=%s, servo=%s",
                     feu_arriere ? "true" : "false",
                     lumiere_interieure ? "true" : "false",
                     servo_porte ? "true" : "false");
        }
        else
        {
            req->send(400, "text/plain", "JSON invalide");
        } });
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
    {
        LOG_INFO("Client WebSocket connecté : %u", client->id());
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        LOG_INFO("Client WebSocket déconnecté : %u", client->id());
    }
}

/* ---------------------------------------------------------------------------
 * Loop
 * ------------------------------------------------------------------------- */
void WRC_WebHandler::Loop()
{
    if (ws)
        ws->cleanupClients();
}
