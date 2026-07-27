#pragma once
#include <Arduino.h>
#include "WRC_Settings.h"
#include "WRC_Pins.h"

// ⭐ États dynamiques pilotés par le DCC
extern bool FX_ACTIVE_FEU_ARRIERE;
extern bool FX_ACTIVE_LUMIERE_INTERIEURE;
extern bool FX_ACTIVE_SERVO_PORTE;

class WRC_FXDriver
{
public:
    static void Begin();
    static void Loop();

    // Applique les FX selon WRC_Settings
    static void mettreAJourFx();

private:
    // ⭐ 3 FX fixes — plus de structure dynamique
    static void appliquerLedArriere(bool actif);
    static void appliquerLedInterieure(bool actif);
    static void appliquerServoPorte(bool actif);
};
