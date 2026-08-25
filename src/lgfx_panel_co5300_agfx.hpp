#pragma once

#include <LovyanGFX.hpp>
#include <Arduino_GFX_Library.h>

namespace lgfx {
inline namespace v1 {

// -----------------------------------------------------------------------------
// Panel_CO5300_AGFX
//
// LovyanGFX Panel_Device wrapper around Arduino_GFX's Arduino_CO5300 driver.
//
// Target hardware:
//   Waveshare ESP32-S3-Touch-AMOLED-1.75
//   CO5300 QSPI AMOLED
//   466 x 466
//
// Verified display pins:
//   CS    = GPIO12
//   SCLK  = GPIO38
//   SDIO0 = GPIO4
//   SDIO1 = GPIO5
//   SDIO2 = GPIO6
//   SDIO3 = GPIO7
//   RESET = GPIO39
//
// Arduino_GFX is used because LovyanGFX has no native CO5300 panel driver.
// -----------------------------------------------------------------------------

class Panel_CO5300_AGFX : public Panel_Device {
public:
  Panel_CO5300_AGFX() {
    _cfg.memory_width  = _cfg.panel_width  = 466;
    _cfg.memory_height = _cfg.panel_height = 466;

    _cfg.offset_x = 0;
    _cfg.offset_y = 0;
    _cfg.offset_rotation = 0;

    _cfg.dummy_read_pixel = 0;
    _cfg.dummy_read_bits  = 0;

    _cfg.readable   = false;
    _cfg.invert     = false;
    _cfg.rgb_order  = false;
    _cfg.dlen_16bit = false;
    _cfg.bus_shared = false;

    _write_depth = color_depth_t::rgb565_2Byte;
    _read_depth  = color_depth_t::rgb565_2Byte;
  }

  ~Panel_CO5300_AGFX() {
    if (_agfx) {
      delete _agfx;
      _agfx = nullptr;
    }

    if (_agfx_bus) {
      delete _agfx_bus;
      _agfx_bus = nullptr;
    }
  }

  // ---------------------------------------------------------------------------
  // Initialisation
  // ---------------------------------------------------------------------------

  bool init(bool /*use_reset*/) override {
    if (_init_done) return true;

    _agfx_bus = new Arduino_ESP32QSPI(
        12,  // CS
        38,  // SCLK
        4,   // SDIO0
        5,   // SDIO1
        6,   // SDIO2
        7    // SDIO3
    );

    // Constructor parameters match Waveshare's official Arduino example:
    //
    // bus, reset, rotation, width, height,
    // col_offset1, row_offset1, col_offset2, row_offset2
    _agfx = new Arduino_CO5300(
        _agfx_bus,
        39,   // RESET
        0,    // rotation
        466,
        466,
        6,
        0,
        0,
        0
    );

    if (!_agfx->begin(40000000UL)) {
      delete _agfx;
      _agfx = nullptr;

      delete _agfx_bus;
      _agfx_bus = nullptr;

      return false;
    }

    // CO5300 AMOLED brightness is controlled by the panel itself.
    // There is no conventional GPIO LED backlight on this board.
    _agfx->setBrightness(160);

    // Leave the AMOLED black until the UI draws its first frame.

    _init_done = true;
    _width  = _cfg.panel_width;
    _height = _cfg.panel_height;

    return true;
  }

  // Arduino_GFX owns the QSPI bus.
  void initBus(void) override {}
  void releaseBus(void) override {}

  // ---------------------------------------------------------------------------
  // Transactions
  // ---------------------------------------------------------------------------

  void beginTransaction(void) override {
    if (_agfx && !_in_transaction) {
      _agfx->startWrite();
      _in_transaction = true;
    }
  }

  void endTransaction(void) override {
    if (_agfx && _in_transaction) {
      _agfx->endWrite();
      _in_transaction = false;
    }
  }

  // ---------------------------------------------------------------------------
  // Panel configuration
  // ---------------------------------------------------------------------------

  color_depth_t setColorDepth(color_depth_t) override {
    _write_depth = color_depth_t::rgb565_2Byte;
    _read_depth  = color_depth_t::rgb565_2Byte;
    return _write_depth;
  }

  void setRotation(uint_fast8_t r) override {
    r &= 3;

    _rotation = r;
    _internal_rotation = r;

    if (_agfx) {
      _agfx->setRotation(r);
    }

    // Square panel, but retain the generic rotation logic.
    _width  = (r & 1) ? _cfg.panel_height : _cfg.panel_width;
    _height = (r & 1) ? _cfg.panel_width  : _cfg.panel_height;
  }

  void setInvert(bool /*invert*/) override {}
  void setSleep(bool /*flg*/) override {}
  void setPowerSave(bool /*flg*/) override {}

  void waitDisplay(void) override {}
  bool displayBusy(void) override { return false; }

  // Arduino_GFX writes immediately; there is no LovyanGFX DMA queue here.
  void initDMA(void) override {}
  void waitDMA(void) override {}
  bool dmaBusy(void) override { return false; }

  void display(uint_fast16_t,
               uint_fast16_t,
               uint_fast16_t,
               uint_fast16_t) override {}

  // ---------------------------------------------------------------------------
  // Drawing primitives
  // ---------------------------------------------------------------------------

  void setWindow(uint_fast16_t xs,
                 uint_fast16_t ys,
                 uint_fast16_t xe,
                 uint_fast16_t ye) override {
    if (!_agfx) return;

    _agfx->writeAddrWindow(
        xs,
        ys,
        xe - xs + 1,
        ye - ys + 1
    );
  }

  void drawPixelPreclipped(uint_fast16_t x,
                           uint_fast16_t y,
                           uint32_t rawcolor) override {
    if (!_agfx) return;

    _agfx->writePixelPreclipped(
        x,
        y,
        static_cast<uint16_t>(rawcolor)
    );
  }

  void writeFillRectPreclipped(uint_fast16_t x,
                               uint_fast16_t y,
                               uint_fast16_t w,
                               uint_fast16_t h,
                               uint32_t rawcolor) override {
    if (!_agfx) return;

    _agfx->writeFillRectPreclipped(
        x,
        y,
        w,
        h,
        static_cast<uint16_t>(rawcolor)
    );
  }

  void writeBlock(uint32_t rawcolor,
                  uint32_t length) override {
    if (!_agfx || length == 0) return;

    _agfx->writeRepeat(
        static_cast<uint16_t>(rawcolor),
        length
    );
  }

  void writePixels(pixelcopy_t* pc,
                   uint32_t length,
                   bool /*use_dma*/) override {
    if (!_agfx || length == 0) return;

    static constexpr uint32_t BUF_PIXELS = 4096;
    static uint16_t buf[BUF_PIXELS];

    while (length > 0) {
      uint32_t n =
          length > BUF_PIXELS
              ? BUF_PIXELS
              : length;

      pc->fp_copy(buf, 0, n, pc);
      _agfx->writePixels(buf, n);

      length -= n;
    }
  }

  void writeImage(uint_fast16_t x,
                  uint_fast16_t y,
                  uint_fast16_t w,
                  uint_fast16_t h,
                  pixelcopy_t* pc,
                  bool use_dma) override {
    if (!_agfx || w == 0 || h == 0) return;

    _agfx->writeAddrWindow(x, y, w, h);
    writePixels(pc, static_cast<uint32_t>(w) * h, use_dma);
  }

  // Optional fast path for a contiguous RGB565 framebuffer.
  void pushRawPixels(uint16_t* data,
                     uint32_t length) {
    if (!_agfx || !data || length == 0) return;

    _agfx->startWrite();
    _agfx->writeAddrWindow(
        0,
        0,
        _cfg.panel_width,
        _cfg.panel_height
    );

    _agfx->writeBytes(
        reinterpret_cast<uint8_t*>(data),
        length * sizeof(uint16_t)
    );

    _agfx->endWrite();
  }

  // ---------------------------------------------------------------------------
  // Read operations
  // ---------------------------------------------------------------------------

  uint32_t readCommand(uint_fast16_t,
                       uint_fast8_t,
                       uint_fast8_t) override {
    return 0;
  }

  uint32_t readData(uint_fast8_t,
                    uint_fast8_t) override {
    return 0;
  }

  void readRect(uint_fast16_t,
                uint_fast16_t,
                uint_fast16_t,
                uint_fast16_t,
                void*,
                pixelcopy_t*) override {}

  int32_t getScanLine(void) override {
    return 0;
  }

  // Not used because Arduino_GFX owns the physical bus.
  void writeCommand(uint32_t,
                    uint_fast8_t) override {}

  void writeData(uint32_t,
                 uint_fast8_t) override {}

  void writeImageARGB(uint_fast16_t,
                      uint_fast16_t,
                      uint_fast16_t,
                      uint_fast16_t,
                      pixelcopy_t*) override {}

  void copyRect(uint_fast16_t,
                uint_fast16_t,
                uint_fast16_t,
                uint_fast16_t,
                uint_fast16_t,
                uint_fast16_t) override {}

private:
  bool _init_done = false;
  bool _in_transaction = false;

  Arduino_DataBus* _agfx_bus = nullptr;
  Arduino_CO5300*  _agfx     = nullptr;
};

} // namespace v1
} // namespace lgfx
