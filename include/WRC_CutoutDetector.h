#pragma once
#include <Arduino.h>

class WRC_CutoutDetector
{
public:
    static void Begin();
    static bool detecterCutout(uint32_t dernierFront);

private:
    static uint32_t dernierCutout;
};
