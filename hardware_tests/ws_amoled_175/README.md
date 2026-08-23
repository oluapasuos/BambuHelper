# Waveshare ESP32-S3-Touch-AMOLED-1.75 — hardware test

This is the first validation step for a BambuHelper port.

## Expected result

After flashing the generated full image, the AMOLED should show:

- BambuHelper
- AMOLED 1.75 TEST
- 466 x 466
- CO5300 QSPI
- DISPLAY OK

The serial port at 115200 should also print `alive` every 2 seconds.

## Hardware definition

- ESP32-S3R8
- 16 MB flash
- 8 MB PSRAM
- CO5300 QSPI AMOLED
- 466 × 466
- LCD pins:
  - SDIO0 GPIO4
  - SDIO1 GPIO5
  - SDIO2 GPIO6
  - SDIO3 GPIO7
  - SCLK GPIO38
  - CS GPIO12
  - RESET GPIO39

## GitHub repository placement

Copy this project to:

`hardware_tests/ws_amoled_175/`

Copy the workflow file to:

`.github/workflows/build-ws-amoled-175-test.yml`

Then open GitHub → Actions → **Build AMOLED 1.75 test** → **Run workflow**.

Download the artifact `ws-amoled-175-test-firmware`.

## Flashing

Use the full image at offset 0x0:

```powershell
py -m esptool --chip esp32s3 --port COM3 --baud 460800 write_flash --flash_mode qio --flash_size 16MB --flash_freq 80m 0x0 BambuHelper-WS-AMOLED-175-Test-Full.bin
```

Change COM3 if necessary.

If the display remains black, open a serial monitor at 115200 and capture the boot log.
