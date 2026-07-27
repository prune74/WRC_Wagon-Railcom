#include <Arduino.h>
#include "WRC_Settings.h"
#include "WRC_Fl_Wifi.h"
#include "WRC_FXDriver.h"
#include "WRC_WebHandler.h"
#include "WRC_RailcomDriver.h"
#include "WRC_RailcomEmitter.h"
#include "WRC_DCCDecoder.h"
#include "WRC_DCCHandler.h"

static uint32_t lastLog = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("BOOT");

    WRC_Settings::Begin();
    Serial.println("Settings OK");

    WRC_Fl_Wifi::start();
    Serial.println("WiFi AP OK");

    WRC_FXDriver::Begin();
    Serial.println("FXDriver OK");

    WRC_WebHandler::Begin();
    Serial.println("WebHandler OK");

    WRC_RailcomDriver::Begin();
    Serial.println("RailcomDriver OK");

    WRC_RailcomEmitter::Begin(WRC_Settings::ADRESSE);
    Serial.println("RailcomEmitter OK");

    WRC_DCCDecoder::Begin();
    Serial.println("DCCDecoder OK");

    Serial.println("SETUP FINI");
}

void loop()
{
    // Web interface
    WRC_WebHandler::Loop();

    // FX animations
    WRC_FXDriver::Loop();

    // DCC decoder
    WRC_DCCDecoder::Loop();

    // RailCom emitter (activé par F27/F28)
    WRC_RailcomEmitter::Loop();

    // RailCom receiver
    WRC_RailcomDriver::Loop();

    // Log non bloquant toutes les 1000 ms
    uint32_t now = millis();
    if (now - lastLog >= 1000)
    {
        Serial.println("OK");
        lastLog = now;
    }

    // Nourrit le watchdog sans bloquer
    vTaskDelay(1);
}
