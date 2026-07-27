#pragma once
#include <Arduino.h>

class WRC_RailcomEmitter
{
public:
    static void Begin(uint16_t adresse);
    static void setEnabled(bool enable);
    static bool isEnabled();
    static void Loop();

private:
    static uint16_t s_adresse;
    static bool     s_enabled;

    static void emitBit(bool bit);
    static void emitFrame();
};
