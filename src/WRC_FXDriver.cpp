#include "WRC_FXDriver.h"
#include "WRC_ServoDriver.h"
#include "WRC_Debug.h"
#include "WRC_Settings.h"

// ---------------------------------------------------------------------------
// VARIABLES DYNAMIQUES PILOTÉES PAR LE DCC
// ---------------------------------------------------------------------------
bool FX_ACTIVE_FEU_ARRIERE = false;
bool FX_ACTIVE_LUMIERE_INTERIEURE = false;
bool FX_ACTIVE_SERVO_PORTE = false;

// ---------------------------------------------------------------------------
// INITIALISATION
// ---------------------------------------------------------------------------
void WRC_FXDriver::Begin()
{
    gpio_set_direction(WRC_FxPins::SERVO_DOOR, GPIO_MODE_OUTPUT);
    gpio_set_direction(WRC_FxPins::LED_INTERIEURE, GPIO_MODE_OUTPUT);
    gpio_set_direction(WRC_FxPins::LED_ARRIERE, GPIO_MODE_OUTPUT);

    gpio_set_direction(WRC_Pins::LED_STATUS, GPIO_MODE_OUTPUT);

    WRC_ServoDriver::Begin();

    mettreAJourFx();

    LOG_INFO("FXDriver initialisé");
}

void WRC_FXDriver::Loop()
{
    WRC_ServoDriver::update();
}

// ---------------------------------------------------------------------------
// METTRE À JOUR FX — VERSION DYNAMIQUE (pilotée par le DCC)
// ---------------------------------------------------------------------------
void WRC_FXDriver::mettreAJourFx()
{
    appliquerLedArriere(FX_ACTIVE_FEU_ARRIERE);
    appliquerLedInterieure(FX_ACTIVE_LUMIERE_INTERIEURE);
    appliquerServoPorte(FX_ACTIVE_SERVO_PORTE);
}

/* ---------------------------------------------------------------------------
 * FX : LED ARRIÈRE
 * ------------------------------------------------------------------------- */
void WRC_FXDriver::appliquerLedArriere(bool actif)
{
    gpio_set_level(WRC_FxPins::LED_ARRIERE, actif ? 1 : 0);
}

/* ---------------------------------------------------------------------------
 * FX : LED INTÉRIEURE
 * ------------------------------------------------------------------------- */
void WRC_FXDriver::appliquerLedInterieure(bool actif)
{
    gpio_set_level(WRC_FxPins::LED_INTERIEURE, actif ? 1 : 0);
}

/* ---------------------------------------------------------------------------
 * FX : SERVO PORTE
 * ------------------------------------------------------------------------- */
void WRC_FXDriver::appliquerServoPorte(bool actif)
{
    if (actif)
        WRC_ServoDriver::ouvrirPorte();
    else
        WRC_ServoDriver::fermerPorte();
}
