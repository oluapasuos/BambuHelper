#ifndef LAYOUT_H
#define LAYOUT_H

// Layout profile dispatcher.
// Each display target defines LY_* constants for screen dimensions,
// gauge positions, text positions, etc.
// To add a new display: create layout_xxx.h and add an #elif here.

#if defined(DISPLAY_466x466)
  #include "layout_466x466.h"    // Waveshare AMOLED: round CO5300 466x466
#elif defined(DISPLAY_480x480)
  #include "layout_480x480.h"    // SenseCAP Indicator: ST7701S 480x480
#elif defined(DISPLAY_ROUND_240)
  #include "layout_round240.h"  // GC9A01 1.28" round 240x240
#elif defined(DISPLAY_320x480)
  #include "layout_320x480.h"   // 320x480 portrait (Guition JC3248W535)
#elif defined(DISPLAY_240x320)
  #include "layout_240x320.h"   // 240x320 portrait (CYD, Waveshare)
#else
  #include "layout_default.h"   // ESP32-S3 Mini: ST7789 240x240
#endif

// Distance from a temperature gauge's centre to its smaller target line.
// Individual layouts can increase it where a larger panel/font needs more
// optical separation; the historical value remains the default everywhere.
#ifndef LY_TEMP_TARGET_OFFSET_Y
  #define LY_TEMP_TARGET_OFFSET_Y 10
#endif

#endif // LAYOUT_H
