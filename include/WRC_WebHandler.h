#pragma once
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class WRC_WebHandler
{
public:
    static void Begin(uint16_t port = 80);
    static void Loop();

private:
    static AsyncWebServer *serveur;
    static AsyncWebSocket *ws;

    static void definirRoutes();
    static void evenementWs(AsyncWebSocket *server,
                            AsyncWebSocketClient *client,
                            AwsEventType type,
                            void *arg,
                            uint8_t *data,
                            size_t len);
};
