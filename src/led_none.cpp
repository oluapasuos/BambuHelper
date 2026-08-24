#include "led.h"

void initLed() {}
void ledTick() {}

void applyLedDuty(uint8_t) {}

void sanitizeLedPin() {}

bool isLedPinAllowed(uint8_t) {
    return false;
}

bool isRgbLedPinAllowed(uint8_t) {
    return false;
}

bool onboardRgbPins(uint8_t&, uint8_t&, uint8_t&, bool&) {
    return false;
}

void previewLed(bool, uint8_t, uint8_t, uint8_t, uint8_t,
                bool, uint8_t, uint32_t) {}

void ledSetActivity(LedActivity) {}

void ledStartFinishEffect() {}
void ledStopFinishEffect() {}

#if HAS_HMS_UI
void ledStartErrorEpisode() {}
void ledStopErrorEpisode() {}
#endif

bool ledTriggerTestEffect(uint8_t, uint16_t, uint8_t, uint32_t) {
    return false;
}

void ledOnUserInteraction() {}

void ledSetSuspended(bool) {}

bool ledHoldDimUpdate(bool, uint32_t, bool) {
    return false;
}
