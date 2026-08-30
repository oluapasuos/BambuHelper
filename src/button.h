#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

enum class TouchGestureType : uint8_t {
  None,
  Tap,
  SwipeLeft,
  SwipeRight,
};

struct TouchGesture {
  TouchGestureType type;
  int16_t x;       // contact start, in screen coordinates
  int16_t y;
  int16_t deltaX;  // final minus start
  int16_t deltaY;
  uint32_t durationMs;
};

void initButton();
bool wasButtonPressed();  // returns true once per press (edge-detected, debounced)
void sanitizeButtonPin();  // zero buttonPin if it conflicts with a reserved
                           // subsystem (backlight, touch bus, buzzer). No-op
                           // for touchscreen type.

// Hold-state polling. Pure getters - they reflect whatever the most recent
// wasButtonPressed() call observed and do NOT consume edge events. The main
// loop calls wasButtonPressed() once per iteration, so these stay fresh.
bool isButtonHeld();              // post-debounce stable pressed state
uint32_t buttonHoldDurationMs();  // 0 if not held, else millis() - press start

// Returns and consumes the latest coordinate gesture. Button-only touch
// backends always return None and retain their established edge behaviour.
TouchGesture takeTouchGesture();

#endif // BUTTON_H
