#include "WRC_DCCDecoder.h"
#include "WRC_DCCHandler.h"
#include "WRC_Settings.h"
#include "WRC_RailcomEmitter.h"
#include "WRC_FXDriver.h"
#include "WRC_Debug.h"
#include "WRC_Pins.h"

extern bool FX_ACTIVE_FEU_ARRIERE;
extern bool FX_ACTIVE_LUMIERE_INTERIEURE;
extern bool FX_ACTIVE_SERVO_PORTE;

volatile uint32_t lastEdge = 0;
volatile uint16_t bitBuffer = 0;
volatile uint8_t  bitCount = 0;

void WRC_DCCDecoder::Begin()
{
    pinMode(WRC_Pins::TRACK_ADC, INPUT);

    gpio_set_direction(WRC_Pins::TRACK_ADC, GPIO_MODE_INPUT);
    gpio_set_intr_type(WRC_Pins::TRACK_ADC, GPIO_INTR_ANYEDGE);
    gpio_isr_handler_add(WRC_Pins::TRACK_ADC, isrDcc, nullptr);

    lastEdge = micros();
}

void WRC_DCCDecoder::Loop()
{
    uint32_t now = micros();

    // ---------------------------------------------------------------------
    // 1. Détection de perte de signal DCC
    // ---------------------------------------------------------------------
    static bool dccPresent = false;

    if ((now - lastEdge) > 20000)   // 20 ms sans front → plus de DCC
    {
        if (dccPresent)
        {
            dccPresent = false;
            LOG_WARN("DCC perdu → désactivation RailCom + reset FX");

            WRC_RailcomEmitter::setEnabled(false);

            // ⭐ Reset des FX dynamiques
            FX_ACTIVE_FEU_ARRIERE        = false;
            FX_ACTIVE_LUMIERE_INTERIEURE = false;
            FX_ACTIVE_SERVO_PORTE        = false;

            WRC_FXDriver::mettreAJourFx();
        }
    }
    else
    {
        if (!dccPresent)
        {
            dccPresent = true;
            LOG_INFO("Signal DCC détecté");
        }
    }

    // ---------------------------------------------------------------------
    // 2. Watchdog du décodeur DCC
    // ---------------------------------------------------------------------
    static uint32_t lastCheck = now;

    if (now - lastCheck > 100000)  // toutes les 100 ms
    {
        lastCheck = now;

        if (bitCount > 8)
        {
            LOG_WARN("Reset buffer DCC (bitCount incohérent)");
            bitCount = 0;
            bitBuffer = 0;
        }

        // Adresse invalide → reset
        if (WRC_Settings::ADRESSE == 0)
        {
            LOG_WARN("Adresse invalide → reset");
            WRC_Settings::ADRESSE = 3;
        }
    }

    // ---------------------------------------------------------------------
    // 3. Sécurité RailCom
    // ---------------------------------------------------------------------
    if (WRC_RailcomEmitter::isEnabled() && !dccPresent)
    {
        LOG_WARN("RailCom actif sans DCC → désactivation");
        WRC_RailcomEmitter::setEnabled(false);
    }

    // ---------------------------------------------------------------------
    // 4. Sécurité FX (2 secondes sans DCC)
    // ---------------------------------------------------------------------
    if ((now - lastEdge) > 2000000)
    {
        // ⭐ Reset des FX dynamiques
        FX_ACTIVE_FEU_ARRIERE        = false;
        FX_ACTIVE_LUMIERE_INTERIEURE = false;
        FX_ACTIVE_SERVO_PORTE        = false;

        WRC_FXDriver::mettreAJourFx();
    }
}

void IRAM_ATTR WRC_DCCDecoder::isrDcc(void* arg)
{
    uint32_t now = micros();
    uint32_t delta = now - lastEdge;
    lastEdge = now;

    bool bit = (delta < 100);   // 58 µs = 1, 116 µs = 0

    processBit(bit);
}

void WRC_DCCDecoder::processBit(bool bit)
{
    bitBuffer = (bitBuffer << 1) | (bit ? 1 : 0);
    bitCount++;

    if (bitCount == 8)
    {
        processByte(bitBuffer & 0xFF);
        bitCount = 0;
        bitBuffer = 0;
    }
}

void WRC_DCCDecoder::processByte(uint8_t b)
{
    static uint8_t addr = 0;
    static uint8_t fn   = 0;

    // ---------------------------------------------------------------------
    // Adresse courte (DCC)
    // ---------------------------------------------------------------------
    if (addr == 0)
    {
        addr = b;
        return;
    }

    // ---------------------------------------------------------------------
    // Groupe F0..F4
    // ---------------------------------------------------------------------
    if ((b & 0xE0) == 0x80)
    {
        fn = b & 0x1F;
        WRC_DCCHandler::onFunction(addr, fn, true);
        return;
    }

    // ---------------------------------------------------------------------
    // Groupe F5..F8
    // ---------------------------------------------------------------------
    if ((b & 0xF0) == 0xB0)
    {
        fn = (b & 0x0F) + 5;
        WRC_DCCHandler::onFunction(addr, fn, true);
        return;
    }

    // ---------------------------------------------------------------------
    // Groupe F9..F12
    // ---------------------------------------------------------------------
    if ((b & 0xF0) == 0xA0)
    {
        fn = (b & 0x0F) + 9;
        WRC_DCCHandler::onFunction(addr, fn, true);
        return;
    }

    // ---------------------------------------------------------------------
    // Groupe F13..F20
    // ---------------------------------------------------------------------
    if ((b & 0xF0) == 0xDE)
    {
        fn = (b & 0x1F) + 13;
        WRC_DCCHandler::onFunction(addr, fn, true);
        return;
    }

    // ---------------------------------------------------------------------
    // Groupe F21..F28
    // ---------------------------------------------------------------------
    if ((b & 0xF0) == 0xDF)
    {
        fn = (b & 0x1F) + 21;

        if (fn == 27)
            WRC_DCCHandler::onFunction(addr, 27, true);

        if (fn == 28)
            WRC_DCCHandler::onFunction(addr, 28, false);

        return;
    }
}
