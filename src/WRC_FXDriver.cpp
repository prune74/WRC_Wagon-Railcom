#include "WRC_FXDriver.h"
#include "WRC_Debug.h"

// ---------------------------------------------------------------------------
// Définition de la table FX_BEHAVIORS
// ---------------------------------------------------------------------------
#define FX_ENTRY(NAME, JSON, FUNC) { &WRC_FXDriver::FUNC },

WRC_FXDriver::FxBehavior WRC_FXDriver::FX_BEHAVIORS[] = {
    #include "WRC_FX.inc"
};
#undef FX_ENTRY

// ---------------------------------------------------------------------------
// INITIALISATION
// ---------------------------------------------------------------------------
void WRC_FXDriver::Begin()
{
    gpio_set_direction(WRC_FxPins::SERVO_DOOR, GPIO_MODE_OUTPUT);
    gpio_set_direction(WRC_FxPins::LED_INTERIEURE, GPIO_MODE_OUTPUT);
    gpio_set_direction(WRC_FxPins::LED_ARRIERE, GPIO_MODE_OUTPUT);

    gpio_set_direction(WRC_Pins::LED_STATUS, GPIO_MODE_OUTPUT);

    mettreAJourFx();

    LOG_INFO("FXDriver initialisé");
}

void WRC_FXDriver::Loop()
{
    // Pour l’instant, pas d’animations complexes
    // Tu pourras ajouter des effets ici plus tard
}

// ---------------------------------------------------------------------------
// METTRE À JOUR FX (DYNAMIQUE)
// ---------------------------------------------------------------------------
void WRC_FXDriver::mettreAJourFx()
{
    for (size_t i = 0; i < WRC_Settings::FX_COUNT; i++)
    {
        bool actif = *(WRC_Settings::FX_LIST[i]->value);
        FX_BEHAVIORS[i].func(actif);
    }
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
    // Pour l’instant : ON/OFF simple
    gpio_set_level(WRC_FxPins::SERVO_DOOR, actif ? 1 : 0);
}
