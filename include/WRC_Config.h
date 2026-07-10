#pragma once
#include <Arduino.h>

/*
 * 🎯 WRC_Config.h
 * Configuration globale du Wagon RailCom ESP32-C3
 *
 * Contient :
 *   • Paramètres par défaut
 *   • Timings RailCom
 *   • Options WiFi
 *   • Versions firmware/hardware
 *   • Constantes typées (pas de #define)
 */

namespace WRC_Config
{
    // -----------------------------------------------------------------------
    // 📌 Version du firmware / hardware
    // -----------------------------------------------------------------------
    static const char* FW_VERSION = "1.0.0";
    static const char* HW_VERSION = "WRC-ESP32C3-01";

    // -----------------------------------------------------------------------
    // 📌 Adresse RailCom par défaut
    // -----------------------------------------------------------------------
    static const uint16_t DEFAULT_ADDRESS = 4001;

    // -----------------------------------------------------------------------
    // 📌 RailCom : timings Manchester
    // -----------------------------------------------------------------------
    static const uint32_t RAILCOM_HALF_BIT_US = 4;   // ~8.68 µs par bit

    // -----------------------------------------------------------------------
    // 📌 WiFi : configuration AP
    // -----------------------------------------------------------------------
    static const bool WIFI_ON_DEFAULT = true;

    static const char* WIFI_AP_PASSWORD = "railcom123";   // mot de passe AP
    static const char* WIFI_AP_PREFIX   = "WagonRailcom_"; // SSID = prefix + adresse

    // -----------------------------------------------------------------------
    // 📌 WebServer
    // -----------------------------------------------------------------------
    static const uint16_t WEBSERVER_PORT = 80;

    // -----------------------------------------------------------------------
    // 📌 FX (éclairage, servo, etc.)
    // -----------------------------------------------------------------------
    static const bool FX_ENABLED_DEFAULT = false;
}
