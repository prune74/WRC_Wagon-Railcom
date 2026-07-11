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
        JsonDocument doc;
        JsonObject root = doc.to<JsonObject>();
        JsonObject fx   = root["fx"].to<JsonObject>();

        root["wifi_actif"] = WRC_Settings::WIFI_ACTIF;
        root["adresse"]    = WRC_Settings::ADRESSE;

        for (size_t i = 0; i < WRC_Settings::FX_COUNT; i++)
            fx[WRC_Settings::FX_LIST[i]->jsonName] = *(WRC_Settings::FX_LIST[i]->value);

        String out;
        serializeJson(doc, out);
        req->send(200, "application/json", out); });

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

        JsonDocument doc;
        if (deserializeJson(doc, body))
        {
            req->send(400, "text/plain", "JSON invalide");
            return;
        }

        JsonObject fx = doc["fx"];
        if (!fx.isNull())
        {
            for (size_t i = 0; i < WRC_Settings::FX_COUNT; i++)
            {
                const char* name = WRC_Settings::FX_LIST[i]->jsonName;

                if (!fx[name].isNull())
                {
                    *(WRC_Settings::FX_LIST[i]->value) = fx[name] | *(WRC_Settings::FX_LIST[i]->value);
                }
            }

            WRC_Settings::writeFile();
        }

        req->send(200, "text/plain", "OK"); });

    /* -----------------------------------------------------------------------
     * SAUVEGARDE GENERALE
     * --------------------------------------------------------------------- */
    serveur->on("/sauvegardeGenerale", HTTP_POST, [](AsyncWebServerRequest *req) {}, NULL, 
        [](AsyncWebServerRequest *req, uint8_t *data, size_t len, size_t index, size_t total)
                {
        String body;
        body.reserve(total);
        body = String((char*)data).substring(0, len);

        JsonDocument doc;
        if (deserializeJson(doc, body))
        {
            req->send(400, "text/plain", "JSON invalide");
            return;
        }

        // Adresse
        WRC_Settings::ADRESSE = doc["adresse"] | WRC_Settings::ADRESSE;

        // WiFi
        WRC_Settings::WIFI_ACTIF = doc["wifi_actif"] | WRC_Settings::WIFI_ACTIF;

        // FX
        JsonObject fx = doc["fx"];
        if (!fx.isNull())
        {
            for (size_t i = 0; i < WRC_Settings::FX_COUNT; i++)
            {
                *(WRC_Settings::FX_LIST[i]->value) =
                    fx[WRC_Settings::FX_LIST[i]->jsonName] |
                    *(WRC_Settings::FX_LIST[i]->value);
            }
        }

        // Sauvegarde
        WRC_Settings::writeFile();

        req->send(200, "text/plain", "Sauvegarde OK"); });

    /* -----------------------------------------------------------------------
     * REMISE A ZERO
     * --------------------------------------------------------------------- */
    serveur->on("/resetParametres", HTTP_POST, [](AsyncWebServerRequest *req)
                {
        // Remettre les valeurs par défaut
        WRC_Settings::WIFI_ACTIF = true;
        WRC_Settings::ADRESSE    = 4001;

        for (size_t i = 0; i < WRC_Settings::FX_COUNT; i++)
            *(WRC_Settings::FX_LIST[i]->value) = false;

        // Sauvegarder dans Settings.json
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
    {
        LOG_INFO("Client WebSocket connecté : %u", client->id());
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        LOG_INFO("Client WebSocket déconnecté : %u", client->id());
    }
}

/* ---------------------------------------------------------------------------
 * BOUCLE
 * ------------------------------------------------------------------------- */
void WRC_WebHandler::Loop()
{
    if (ws)
        ws->cleanupClients();
}
