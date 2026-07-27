#include "WRC_DCCHandler.h"
#include "WRC_Settings.h"
#include "WRC_RailcomEmitter.h"
#include "WRC_ServoDriver.h"
#include "WRC_FXDriver.h"
#include "WRC_Debug.h"

// Variables dynamiques FX (pilotées par le DCC)
extern bool FX_ACTIVE_FEU_ARRIERE;
extern bool FX_ACTIVE_LUMIERE_INTERIEURE;
extern bool FX_ACTIVE_SERVO_PORTE;

/*
 * Réception d’une fonction DCC (FN)
 * fn = numéro de la fonction (F0..F28)
 * actif = ON/OFF
 */
void WRC_DCCHandler::onFunction(uint16_t adresse, uint8_t fn, bool actif)
{
    // Vérifie que la commande est pour CE wagon
    if (adresse != WRC_Settings::ADRESSE)
        return;

    switch (fn)
    {
    // ---------------------------------------------------------------------
    // FX : FEU ARRIÈRE (F1)
    // ---------------------------------------------------------------------
    case 1:
        if (WRC_Settings::FEU_ARRIERE)   // La fonction existe ?
        {
            FX_ACTIVE_FEU_ARRIERE = actif;
            WRC_FXDriver::mettreAJourFx();
            LOG_INFO("F1 → FEU_ARRIERE %s", actif ? "ON" : "OFF");
        }
        else
        {
            LOG_WARN("F1 ignorée : FEU_ARRIERE désactivé dans settings.json");
        }
        return;

    // ---------------------------------------------------------------------
    // FX : LUMIÈRE INTÉRIEURE (F2)
    // ---------------------------------------------------------------------
    case 2:
        if (WRC_Settings::LUMIERE_INTERIEURE)
        {
            FX_ACTIVE_LUMIERE_INTERIEURE = actif;
            WRC_FXDriver::mettreAJourFx();
            LOG_INFO("F2 → LUMIERE_INTERIEURE %s", actif ? "ON" : "OFF");
        }
        else
        {
            LOG_WARN("F2 ignorée : LUMIERE_INTERIEURE désactivée dans settings.json");
        }
        return;

    // ---------------------------------------------------------------------
    // FX : SERVO PORTE (F3)
    // ---------------------------------------------------------------------
    case 3:
        if (WRC_Settings::SERVO_PORTE)
        {
            FX_ACTIVE_SERVO_PORTE = actif;
            WRC_FXDriver::mettreAJourFx();
            LOG_INFO("F3 → SERVO_PORTE %s", actif ? "OUVERT" : "FERMÉ");
        }
        else
        {
            LOG_WARN("F3 ignorée : SERVO_PORTE désactivé dans settings.json");
        }
        return;

    // ---------------------------------------------------------------------
    // RailCom ON (F27)
    // ---------------------------------------------------------------------
    case 27:
        LOG_INFO("F27 → RailCom ON");
        WRC_RailcomEmitter::setEnabled(true);
        return;

    // ---------------------------------------------------------------------
    // RailCom OFF (F28)
    // ---------------------------------------------------------------------
    case 28:
        LOG_INFO("F28 → RailCom OFF");
        WRC_RailcomEmitter::setEnabled(false);
        return;

    // ---------------------------------------------------------------------
    // Non géré
    // ---------------------------------------------------------------------
    default:
        LOG_WARN("FN %d non gérée", fn);
        return;
    }
}
