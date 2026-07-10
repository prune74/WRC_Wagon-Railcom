#pragma once
#include <Arduino.h>

class WRC_RailcomFrame
{
public:
    // Canal RailCom (1 = adresse, 2 = FX)
    enum Canal
    {
        CANAL_1 = 1,
        CANAL_2 = 2
    };

    // Données décodées
    bool valide;            // La trame est valide
    Canal canal;            // Canal 1 ou 2
    uint16_t adresse;       // Adresse loco (canal 1)
    uint16_t fxBits;        // Bits FX (canal 2)
    uint8_t flags;          // Flags simples (canal 1)

    // Constructeur
    WRC_RailcomFrame();

    // Réinitialise la trame
    void reset();

    // Débogage
    String toString() const;
};
