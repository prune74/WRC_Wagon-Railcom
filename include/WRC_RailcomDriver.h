#pragma once
#include <Arduino.h>
#include "WRC_RailcomFrame.h"

class WRC_RailcomDriver
{
public:
    static void Begin();
    static void Loop();

    static bool hasFrame();
    static WRC_RailcomFrame getFrame();
    static void clearFrame();

private:
    static void IRAM_ATTR isrRailcom(void* arg);

    static volatile uint32_t dernierFront;
    static volatile uint16_t transitions[128];
    static volatile uint8_t  nbTransitions;

    static void decoderTrame();

    static WRC_RailcomFrame frameCourante;
    static bool frameDisponible;

    // Ajout pour adresses longues RailCom (CH1 + CH2)
    static uint16_t s_ch1Adresse;
    static uint16_t s_ch2Adresse;
};
