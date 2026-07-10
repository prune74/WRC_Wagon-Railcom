#include "WRC_FXDriver.h"
#include "WRC_Debug.h"

void WRC_FXDriver::Begin()
{
    // Servo porte
    gpio_set_direction(WRC_FxPins::SERVO_DOOR, GPIO_MODE_OUTPUT);

    gpio_set_direction(WRC_FxPins::LED_INTERIEURE, GPIO_MODE_OUTPUT);

    gpio_set_direction(WRC_FxPins::LED_ARRIERE, GPIO_MODE_OUTPUT);

    // LED statut interne
    gpio_set_direction(WRC_Pins::LED_STATUS, GPIO_MODE_OUTPUT);

    // État initial
    mettreAJourFx();

    LOG_INFO("FXDriver initialisé");
}

void WRC_FXDriver::Loop()
{
    // Pour l’instant, pas d’animations complexes
    // Tu pourras ajouter des effets ici plus tard
}

/* ---------------------------------------------------------------------------
 * APPLIQUER FX
 * ------------------------------------------------------------------------- */
void WRC_FXDriver::mettreAJourFx()
{
    appliquerLedArriere(WRC_Settings::FX_FEU_ARRIERE);
    appliquerLedInterieure(WRC_Settings::FX_LUMIERE_INTERIEURE);
    appliquerServoPorte(WRC_Settings::FX_SERVO_PORTE);

    LOG_VERBOSE("FX mis à jour : arrière=%d, intérieur=%d, servo=%d",
                WRC_Settings::FX_FEU_ARRIERE,
                WRC_Settings::FX_LUMIERE_INTERIEURE,
                WRC_Settings::FX_SERVO_PORTE);
}

/* ---------------------------------------------------------------------------
 * FX : LED ARRIÈRE
 * ------------------------------------------------------------------------- */
void WRC_FXDriver::appliquerLedArriere(bool actif)
{
    // À compléter quand tu ajoutes la LED arrière
    gpio_set_level(WRC_FxPins::LED_ARRIERE, actif ? 1 : 0);
}

/* ---------------------------------------------------------------------------
 * FX : LED INTÉRIEURE
 * ------------------------------------------------------------------------- */
void WRC_FXDriver::appliquerLedInterieure(bool actif)
{
    // À compléter quand tu ajoutes la LED intérieure
    gpio_set_level(WRC_FxPins::LED_INTERIEURE, actif ? 1 : 0);
}

/* ---------------------------------------------------------------------------
 * FX : SERVO PORTE
 * ------------------------------------------------------------------------- */
void WRC_FXDriver::appliquerServoPorte(bool actif)
{
    // Pour l’instant : ON/OFF simple
    gpio_set_level(WRC_FxPins::SERVO_DOOR, actif ? 1 : 0);
}
