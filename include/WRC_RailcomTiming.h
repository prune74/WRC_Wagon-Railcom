#pragma once
#include <Arduino.h>

/*
 * 🎯 WRC_RailcomTiming
 * Module header-only contenant les timings RailCom
 * et les helpers pour classifier les pulses Manchester.
 */

namespace WRC_RailcomTiming
{
    // Durées RailCom officielles
    static const uint16_t DEMI_BIT_US = 122;   // demi-bit Manchester
    static const uint16_t BIT_US      = 244;   // bit complet
    static const uint16_t CUTOUT_US   = 488;   // fenêtre RailCom

    // Tolérance (±40 µs)
    static const uint16_t TOLERANCE   = 40;

    // Pulse court = 122 µs
    inline bool isShortPulse(uint16_t t)
    {
        return (t > (DEMI_BIT_US - TOLERANCE) &&
                t < (DEMI_BIT_US + TOLERANCE));
    }

    // Pulse long = 244 µs
    inline bool isLongPulse(uint16_t t)
    {
        return (t > (BIT_US - TOLERANCE) &&
                t < (BIT_US + TOLERANCE));
    }
}
