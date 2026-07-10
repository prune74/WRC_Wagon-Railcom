#include "WRC_RailcomDriver.h"
#include "WRC_RailcomTiming.h"
#include "WRC_CutoutDetector.h"
#include "WRC_Debug.h"
#include "WRC_Pins.h"

volatile uint32_t WRC_RailcomDriver::dernierFront = 0;
volatile uint16_t WRC_RailcomDriver::transitions[128];
volatile uint8_t  WRC_RailcomDriver::nbTransitions = 0;

WRC_RailcomFrame WRC_RailcomDriver::frameCourante;
bool             WRC_RailcomDriver::frameDisponible = false;

/* ---------------------------------------------------------------------------
 * INIT
 * ------------------------------------------------------------------------- */
void WRC_RailcomDriver::Begin()
{
    frameCourante.reset();
    frameDisponible = false;

    nbTransitions = 0;
    dernierFront  = micros();

    WRC_CutoutDetector::Begin();

    // ⚠️ OBLIGATOIRE : installer le service ISR
    // Compatible ESP32 classique et ESP32‑C3
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);

    gpio_set_direction(WRC_Pins::RAILCOM_IN, GPIO_MODE_INPUT);
    gpio_set_intr_type(WRC_Pins::RAILCOM_IN, GPIO_INTR_ANYEDGE);

    gpio_isr_handler_add(WRC_Pins::RAILCOM_IN, isrRailcom, nullptr);

    LOG_INFO("RailComDriver initialisé (GPIO %d)", WRC_Pins::RAILCOM_IN);
}


/* ---------------------------------------------------------------------------
 * ISR GPIO → capture transitions RailCom
 * ------------------------------------------------------------------------- */
void IRAM_ATTR WRC_RailcomDriver::isrRailcom(void* arg)
{
    uint32_t maintenant = micros();
    uint32_t delta = maintenant - dernierFront;
    dernierFront = maintenant;

    // Anti-spam : ignore les pulses trop rapides
    if (delta < 40) return;

    // Anti-overflow
    if (nbTransitions >= 128) return;

    transitions[nbTransitions++] = delta;
}

/* ---------------------------------------------------------------------------
 * LOOP
 * ------------------------------------------------------------------------- */
void WRC_RailcomDriver::Loop()
{
    if (WRC_CutoutDetector::detecterCutout(dernierFront))
    {
        decoderTrame();
    }
}

/* ---------------------------------------------------------------------------
 * DÉCODAGE TRAME RAILCOM
 * ------------------------------------------------------------------------- */
void WRC_RailcomDriver::decoderTrame()
{
    frameCourante.reset();

    if (nbTransitions < 10)
    {
        nbTransitions = 0;
        return;
    }

    uint8_t bits[32];
    uint8_t nbBits = 0;

    for (uint8_t i = 0; i < nbTransitions; i++)
    {
        uint16_t t = transitions[i];

        if (WRC_RailcomTiming::isShortPulse(t))
            bits[nbBits++] = 0;
        else if (WRC_RailcomTiming::isLongPulse(t))
            bits[nbBits++] = 1;
    }

    nbTransitions = 0;

    if (nbBits < 8)
        return;

    uint8_t canal = bits[0];

    if (canal == 0)
    {
        frameCourante.canal = WRC_RailcomFrame::CANAL_1;

        uint16_t adresse = 0;
        for (int i = 1; i < 8; i++)
            adresse = (adresse << 1) | bits[i];

        frameCourante.adresse = adresse;
        frameCourante.flags   = bits[8];
        frameCourante.valide  = true;
    }
    else
    {
        frameCourante.canal = WRC_RailcomFrame::CANAL_2;

        uint16_t fx = 0;
        for (int i = 1; i < 16 && i < nbBits; i++)
            fx = (fx << 1) | bits[i];

        frameCourante.fxBits = fx;
        frameCourante.valide = true;
    }

    frameDisponible = true;
}

/* ---------------------------------------------------------------------------
 * FRAME DISPONIBLE ?
 * ------------------------------------------------------------------------- */
bool WRC_RailcomDriver::hasFrame()
{
    return frameDisponible;
}

WRC_RailcomFrame WRC_RailcomDriver::getFrame()
{
    frameDisponible = false;
    return frameCourante;
}

void WRC_RailcomDriver::clearFrame()
{
    frameCourante.reset();
    frameDisponible = false;
}
