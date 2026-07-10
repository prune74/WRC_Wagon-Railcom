#include "WRC_CutoutDetector.h"
#include "WRC_RailcomTiming.h"
#include "WRC_Debug.h"

uint32_t WRC_CutoutDetector::dernierCutout = 0;

void WRC_CutoutDetector::Begin()
{
    dernierCutout = 0;
}

bool WRC_CutoutDetector::detecterCutout(uint32_t dernierFront)
{
    uint32_t silence = micros() - dernierFront;

    if (silence > WRC_RailcomTiming::CUTOUT_US)
    {
        dernierCutout = micros();
        LOG_VERBOSE("Cutout détecté (%u µs)", silence);
        return true;
    }

    return false;
}
