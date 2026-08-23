#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// Waveshare ESP32-S3-Touch-AMOLED-1.75
// Official pin map
static constexpr int LCD_SDIO0 = 4;
static constexpr int LCD_SDIO1 = 5;
static constexpr int LCD_SDIO2 = 6;
static constexpr int LCD_SDIO3 = 7;
static constexpr int LCD_SCLK  = 38;
static constexpr int LCD_CS    = 12;
static constexpr int LCD_RESET = 39;

static constexpr int LCD_WIDTH  = 466;
static constexpr int LCD_HEIGHT = 466;

// The CO5300 panel is QSPI.
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS,
    LCD_SCLK,
    LCD_SDIO0,
    LCD_SDIO1,
    LCD_SDIO2,
    LCD_SDIO3
);

Arduino_CO5300 *gfx = new Arduino_CO5300(
    bus,
    LCD_RESET,
    0,              // rotation
    LCD_WIDTH,
    LCD_HEIGHT,
    6,              // col_offset1 (matches Waveshare example)
    0, 0, 0
);

static void draw_test_screen()
{
    gfx->fillScreen(RGB565_BLACK);

    // Border and simple geometry make orientation/cropping easy to verify.
    gfx->drawRect(8, 8, LCD_WIDTH - 16, LCD_HEIGHT - 16, RGB565_WHITE);
    gfx->drawCircle(LCD_WIDTH / 2, LCD_HEIGHT / 2, 190, RGB565_BLUE);

    gfx->setTextColor(RGB565_GREEN);
    gfx->setTextSize(3);
    gfx->setCursor(70, 135);
    gfx->println("BambuHelper");

    gfx->setTextColor(RGB565_WHITE);
    gfx->setTextSize(2);
    gfx->setCursor(72, 190);
    gfx->println("AMOLED 1.75 TEST");

    gfx->setTextColor(RGB565_YELLOW);
    gfx->setCursor(105, 235);
    gfx->println("466 x 466");

    gfx->setTextColor(RGB565_CYAN);
    gfx->setCursor(86, 285);
    gfx->println("CO5300 QSPI");

    gfx->setTextColor(RGB565_MAGENTA);
    gfx->setCursor(92, 335);
    gfx->println("DISPLAY OK");
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("====================================");
    Serial.println("WS ESP32-S3-Touch-AMOLED-1.75 test");
    Serial.println("CO5300 / 466x466 / QSPI");
    Serial.println("====================================");

    if (!gfx->begin(40000000UL)) {
        Serial.println("ERROR: gfx->begin() failed");
        while (true) {
            delay(1000);
        }
    }

    Serial.println("Display initialised.");

    // AMOLED brightness is controlled by the panel itself; there is no
    // conventional LED backlight pin on this board.
    gfx->setBrightness(160);

    draw_test_screen();

    Serial.println("Test screen drawn.");
    Serial.println("Expected: BambuHelper / AMOLED 1.75 TEST / DISPLAY OK");
}

void loop()
{
    static uint32_t last = 0;
    if (millis() - last >= 2000) {
        last = millis();
        Serial.println("alive");
    }
}
