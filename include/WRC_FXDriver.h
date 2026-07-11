#pragma once
#include <Arduino.h>
#include "WRC_Settings.h"
#include "WRC_Pins.h"

class WRC_FXDriver
{
public:
    static void Begin();
    static void Loop();

    // Applique les FX selon WRC_Settings
    static void mettreAJourFx();

private:
    // Fonctions FX: à ajouter à chaque fonction nouvelle
    static void appliquerLedArriere(bool actif);
    static void appliquerLedInterieure(bool actif);
    static void appliquerServoPorte(bool actif);

    // Structure pour associer un FX à une fonction
    struct FxBehavior {
        void (*func)(bool actif);
    };

    static FxBehavior FX_BEHAVIORS[];
};
