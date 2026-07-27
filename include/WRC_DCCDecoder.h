#pragma once
#include <Arduino.h>

class WRC_DCCDecoder
{
public:
    static void Begin();
    static void Loop();

private:
    static void IRAM_ATTR isrDcc(void* arg);
    static void processBit(bool bit);
    static void processByte(uint8_t b);
};
