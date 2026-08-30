// CST9217 capacitive touch backend for Waveshare ESP32-S3 Touch AMOLED 1.75.
//
// The controller shares the board I2C bus (SDA 15, SCL 14) with the AXP2101.
// GPIO 11 is a pulsed active-low interrupt, not a continuous finger-down level,
// so each interrupt must be followed by a 0xD000 report read. The report ACK and
// finger-state parsing below follow Waveshare's bundled SensorLib CST92xx driver.
#include "button_touch_backend.h"

#if defined(USE_CST9217)

#include <Wire.h>

namespace {

constexpr uint8_t  CST9217_ADDR             = 0x5A;
constexpr uint16_t CST9217_REPORT_REG       = 0xD000;
constexpr uint8_t  CST9217_REPORT_ACK       = 0xAB;
constexpr uint8_t  CST9217_FINGER_DOWN      = 0x06;
constexpr uint8_t  CST9217_MAX_POINTS       = 2;
constexpr size_t   CST9217_REPORT_BYTES     = CST9217_MAX_POINTS * 5 + 5;
constexpr int16_t  CST9217_COORD_MAX        = 466;
// The CST9217 repeats its pulsed interrupt about once per second while held.
// This fallback prevents a lost release report from leaving BambuHelper held
// forever, while still allowing deliberate long presses.
constexpr unsigned long CST9217_RELEASE_FALLBACK_MS = 1600;

bool busReady = false;
bool seen = false;
bool held = false;
volatile uint32_t irqCount = 0;
uint32_t irqSeen = 0;
unsigned long lastReportMs = 0;
bool lastPointValid = false;
int16_t lastPointX = 0;
int16_t lastPointY = 0;

void IRAM_ATTR cst9217Isr() {
  irqCount++;
}

bool writeBytes(const uint8_t* bytes, size_t count, bool sendStop = true) {
  Wire.beginTransmission(CST9217_ADDR);
  if (Wire.write(bytes, count) != count) {
    Wire.endTransmission(sendStop);
    return false;
  }
  return Wire.endTransmission(sendStop) == 0;
}

bool readBytes(uint16_t reg, uint8_t* out, size_t count) {
  const uint8_t command[2] = {
    static_cast<uint8_t>(reg >> 8),
    static_cast<uint8_t>(reg & 0xFF),
  };
  if (!writeBytes(command, sizeof(command), false)) return false;
  if (Wire.requestFrom(CST9217_ADDR, static_cast<uint8_t>(count)) != count) {
    while (Wire.available()) Wire.read();
    return false;
  }
  for (size_t i = 0; i < count; i++) out[i] = Wire.read();
  return true;
}

bool probe() {
  Wire.beginTransmission(CST9217_ADDR);
  return Wire.endTransmission(true) == 0;
}

// Read and acknowledge one pending report. A successful bus transaction with
// no active point is a real release; an I2C failure is reported separately so
// button.cpp can preserve its current debounce/hold state.
bool readReport(bool& isDown, uint8_t& pointCount,
                bool& hasPoint, int16_t& screenX, int16_t& screenY) {
  uint8_t report[CST9217_REPORT_BYTES] = {};
  if (!readBytes(CST9217_REPORT_REG, report, sizeof(report))) return false;

  const uint8_t ack[3] = {
    static_cast<uint8_t>(CST9217_REPORT_REG >> 8),
    static_cast<uint8_t>(CST9217_REPORT_REG & 0xFF),
    CST9217_REPORT_ACK,
  };
  if (!writeBytes(ack, sizeof(ack))) return false;

  isDown = false;
  pointCount = 0;
  hasPoint = false;
  if (report[6] != CST9217_REPORT_ACK) return true;

  pointCount = report[5] & 0x7F;
  if (pointCount == 0 || pointCount > CST9217_MAX_POINTS) return true;

  for (uint8_t i = 0; i < pointCount; i++) {
    // SensorLib places point 0 at byte 0 and point 1 at byte 7. The low nibble
    // is 0x06 only while that finger is present; IDs occupy the high nibble.
    const size_t offset = (i == 0) ? 0 : 7;
    if ((report[offset] & 0x0F) == CST9217_FINGER_DOWN) {
      const uint16_t rawX = (static_cast<uint16_t>(report[offset + 1]) << 4) |
                            (report[offset + 3] >> 4);
      const uint16_t rawY = (static_cast<uint16_t>(report[offset + 2]) << 4) |
                            (report[offset + 3] & 0x0F);

      // Waveshare's 466x466 example calls setMirrorXY(true, true). SensorLib
      // implements that as max-coordinate minus the raw coordinate. Clamp the
      // upper endpoint to BambuHelper's valid framebuffer range (0..465).
      int16_t mirroredX = CST9217_COORD_MAX - static_cast<int16_t>(rawX);
      int16_t mirroredY = CST9217_COORD_MAX - static_cast<int16_t>(rawY);
      if (mirroredX < 0) mirroredX = 0;
      if (mirroredX >= CST9217_COORD_MAX) mirroredX = CST9217_COORD_MAX - 1;
      if (mirroredY < 0) mirroredY = 0;
      if (mirroredY >= CST9217_COORD_MAX) mirroredY = CST9217_COORD_MAX - 1;
      screenX = mirroredX;
      screenY = mirroredY;
      hasPoint = true;
      isDown = true;
      break;
    }
  }
  return true;
}

}  // namespace

void touchInit() {
  // Battery::begin() already initialized this exact shared bus for the AXP2101.
  // Reuse it rather than installing or starting a second I2C driver.
  Wire.setClock(400000);

  pinMode(CST9217_RST, OUTPUT);
  digitalWrite(CST9217_RST, LOW);
  delay(10);
  digitalWrite(CST9217_RST, HIGH);
  delay(50);

  pinMode(CST9217_IRQ, INPUT_PULLUP);
  irqCount = 0;
  irqSeen = 0;
  held = false;
  lastPointValid = false;
  lastPointX = 0;
  lastPointY = 0;
  lastReportMs = millis();
  attachInterrupt(digitalPinToInterrupt(CST9217_IRQ), cst9217Isr, FALLING);

  busReady = true;
  seen = probe();
  if (seen) {
    Serial.printf("CST9217 touch initialized (I2C addr 0x%02X, SDA=%d SCL=%d IRQ=%d RST=%d)\n",
                  CST9217_ADDR, CST9217_SDA, CST9217_SCL,
                  CST9217_IRQ, CST9217_RST);
  } else {
    Serial.printf("CST9217 touch did not answer at init (addr 0x%02X); will retry on interrupts\n",
                  CST9217_ADDR);
  }
}

TouchPoll touchPoll() {
  if (!busReady) return {TouchEvent::Unavailable, false, false, 0, 0};

  const uint32_t count = irqCount;
  const unsigned long now = millis();
  if (count == irqSeen) {
    if (held && (now - lastReportMs) > CST9217_RELEASE_FALLBACK_MS) {
      TouchPoll release = {
        TouchEvent::Released, false, lastPointValid, lastPointX, lastPointY
      };
      held = false;
      lastPointValid = false;
      return release;
    }
    return {TouchEvent::None, held,
            held && lastPointValid, lastPointX, lastPointY};
  }
  irqSeen = count;

  bool down = false;
  uint8_t points = 0;
  bool hasPoint = false;
  int16_t pointX = 0;
  int16_t pointY = 0;
  if (!readReport(down, points, hasPoint, pointX, pointY)) {
    return {TouchEvent::Unavailable, false, false, 0, 0};
  }
  lastReportMs = now;

  if (hasPoint) {
    lastPointValid = true;
    lastPointX = pointX;
    lastPointY = pointY;
  }

  if (!seen) {
    Serial.printf("CST9217 touch became responsive at runtime (addr 0x%02X)\n",
                  CST9217_ADDR);
    seen = true;
  }

  if (down && !held) {
    held = true;
    return {TouchEvent::Pressed, true,
            lastPointValid, lastPointX, lastPointY};
  }
  if (!down && held) {
    TouchPoll release = {
      TouchEvent::Released, false, lastPointValid, lastPointX, lastPointY
    };
    held = false;
    lastPointValid = false;
    return release;
  }
  return {TouchEvent::None, held,
          held && lastPointValid, lastPointX, lastPointY};
}

#endif  // USE_CST9217
