#include "WRC_Debug.h"

// Niveau de debug global
DebugLevel DEBUG_LEVEL = DEBUG_INFO;

/* ---------------------------------------------------------------------------
 * 🔒 Applique un niveau de debug
 * ------------------------------------------------------------------------- */
static void Debug_setLevel(DebugLevel lvl)
{
    DEBUG_LEVEL = lvl;
}

/* ---------------------------------------------------------------------------
 * 🎮 Commande série pour changer le niveau de debug
 * ------------------------------------------------------------------------- */
void WRC_Debug::handleSerialCommand()
{
    if (!Serial.available())
        return;

    char c = Serial.read();

    switch (c)
    {
    case '0':
        Debug_setLevel(DEBUG_NONE);
        Serial.println("[DEBUG] Niveau = 0 (NONE)");
        break;

    case '1':
        Debug_setLevel(DEBUG_ERROR);
        Serial.println("[DEBUG] Niveau = 1 (ERROR)");
        break;

    case '2':
        Debug_setLevel(DEBUG_WARN);
        Serial.println("[DEBUG] Niveau = 2 (WARN)");
        break;

    case '3':
        Debug_setLevel(DEBUG_INFO);
        Serial.println("[DEBUG] Niveau = 3 (INFO)");
        break;

    case '4':
        Debug_setLevel(DEBUG_VERBOSE);
        Serial.println("[DEBUG] Niveau = 4 (VERBOSE)");
        break;

    default:
        Serial.println("[DEBUG] Commande inconnue. Utilise 0-4.");
        break;
    }
}
