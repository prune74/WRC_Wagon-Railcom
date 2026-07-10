#pragma once
#include <stdint.h>
#include <vector>

class WRC_Settings
{
public:
    static bool     WIFI_ACTIF;
    static uint16_t ADRESSE;

    static bool FX_FEU_ARRIERE;
    static bool FX_LUMIERE_INTERIEURE;
    static bool FX_SERVO_PORTE;

    // Liste des adresses connues
    static std::vector<uint16_t> ADRESSES_CONNUES;

    static void Begin();
    static void readFile();
    static void writeFile();

    static uint16_t getAdresse();

    // Vérification doublon
    static bool adresseExiste(uint16_t adresse);
};
