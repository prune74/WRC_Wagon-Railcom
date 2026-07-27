#include "WRC_RailcomEmitter.h"
#include "WRC_RailcomTiming.h"
#include "WRC_Pins.h"

uint16_t WRC_RailcomEmitter::s_adresse = 0;
bool     WRC_RailcomEmitter::s_enabled = false;

void WRC_RailcomEmitter::Begin(uint16_t adresse)
{
    s_adresse = adresse;
    s_enabled = false;

    pinMode(WRC_Pins::RAILCOM_OUT, OUTPUT);
    digitalWrite(WRC_Pins::RAILCOM_OUT, LOW);
}

void WRC_RailcomEmitter::setEnabled(bool enable)
{
    s_enabled = enable;
}

bool WRC_RailcomEmitter::isEnabled()
{
    return s_enabled;
}

/*
 * Manchester :
 * 0 → 122 µs HIGH + 122 µs LOW
 * 1 → 244 µs HIGH + 244 µs LOW
 */
void WRC_RailcomEmitter::emitBit(bool bit)
{
    uint16_t duration = bit ? WRC_RailcomTiming::BIT_US : WRC_RailcomTiming::DEMI_BIT_US;

    digitalWrite(WRC_Pins::RAILCOM_OUT, HIGH);
    delayMicroseconds(duration);

    digitalWrite(WRC_Pins::RAILCOM_OUT, LOW);
    delayMicroseconds(duration);
}

/*
 * Trame CH1 Locoduino :
 * canal = 0
 * adresse = 7 bits
 * flags = 0
 */
void WRC_RailcomEmitter::emitFrame()
{
    emitBit(0); // canal

    for (int i = 6; i >= 0; i--)
    {
        bool bit = (s_adresse >> i) & 0x01;
        emitBit(bit);
    }

    emitBit(0); // flags
}

/*
 * Émission continue :
 * LaBox capture pendant le cutout
 */
void WRC_RailcomEmitter::Loop()
{
    if (!s_enabled)
        return;

    emitFrame();
}
