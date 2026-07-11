#pragma once
#include <stdint.h>
#include <vector>

class WRC_Settings
{
public:
    static bool WIFI_ACTIF;
    static uint16_t ADRESSE;

    struct FxItem
    {
        const char *jsonName;
        bool *value;
    };

    // ---------------------------------------------------------
    // Déclarations générées automatiquement
    // ---------------------------------------------------------
    #define FX_ENTRY(NAME, JSON, FUNC) \
        static bool FX_##NAME;         \
        static FxItem FX_ITEM_##NAME;

    #include "WRC_FX.inc"
    #undef FX_ENTRY

    // ---------------------------------------------------------
    // Tableau FX_LIST
    // ---------------------------------------------------------
    static FxItem *FX_LIST[];
    static size_t FX_COUNT;

    static std::vector<uint16_t> ADRESSES_CONNUES;

    static void Begin();
    static void readFile();
    static void writeFile();

    static uint16_t getAdresse();
};
