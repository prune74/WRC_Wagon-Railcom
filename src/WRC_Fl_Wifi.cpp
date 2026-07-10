#include "WRC_Fl_Wifi.h"
#include "WRC_Settings.h"
#include "WRC_Config.h"
#include "WRC_Debug.h"
#include <WiFi.h>

// ---------------------------------------------------------------------------
// DÉMARRAGE DU WIFI (toujours en mode AP)
// ---------------------------------------------------------------------------
void WRC_Fl_Wifi::start()
{
    // -----------------------------------------------------------------------
    // SSID dynamique basé sur l'adresse RailCom
    // -----------------------------------------------------------------------
    uint16_t addr = WRC_Settings::getAdresse();

    String ssid;
    if (WRC_Settings::WIFI_ACTIF)
    {
        ssid = String(WRC_Config::WIFI_AP_PREFIX) + addr;   // Mode normal
    }
    else
    {
        ssid = String("WRC_SAFE_") + addr;                  // Mode minimal
    }

    const char *password = WRC_Config::WIFI_AP_PASSWORD;

    // -----------------------------------------------------------------------
    // Mode AP (toujours)
    // -----------------------------------------------------------------------
    LOG_INFO("WiFi → Mode AP");
    LOG_INFO("WiFi AP → SSID=%s", ssid.c_str());

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str(), password);

    LOG_INFO("WiFi AP → IP=%s", WiFi.softAPIP().toString().c_str());

    // -----------------------------------------------------------------------
    // Mode minimal : WebHandler réduit
    // -----------------------------------------------------------------------
    if (!WRC_Settings::WIFI_ACTIF)
    {
        LOG_WARN("WiFi → Mode minimal (WIFI_ON=false)");
        LOG_WARN("WebHandler → uniquement 'Réactiver WiFi normal'");
    }
}
