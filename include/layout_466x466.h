#ifndef LAYOUT_466X466_H
#define LAYOUT_466X466_H

// Layout profile: Waveshare ESP32-S3-Touch-AMOLED-1.75
// CO5300, 466x466 pixels.
//
// The controller exposes a square framebuffer, but the visible panel is round.
// Coordinates therefore keep meaningful text and controls inside the circle
// instead of merely scaling the rectangular 480x480 profile.

// --- Screen dimensions ---
#define LY_W    466
#define LY_H    466

// --- LED progress bar (top arc; pixels outside the circle are harmless) ---
#define LY_BAR_W   458
#define LY_BAR_H   10

// --- Header bar ---
// At y=44 the circular chord begins near x=96, so both edges are pulled inward.
#define LY_HDR_Y        24
#define LY_HDR_H        40
#define LY_HDR_NAME_X   100
#define LY_HDR_CY       44
#define LY_HDR_BADGE_RX 100
#define LY_HDR_DOT_CY   48

// --- Printing: 2x3 gauge grid ---
#define LY_GAUGE_R   58
#define LY_GAUGE_T   11
#define LY_TEMP_GAUGE_T 8
#define LY_GAUGE_VALUE_FONT FONT_LARGE
#define LY_GAUGE_VALUE_NUDGE_Y 0
#define LY_COL1      82
#define LY_COL2      233
#define LY_COL3      384
#define LY_ROW1      130
#define LY_ROW2      280

// --- AMS tray visualization ---
#define LY_AMS_Y                205
#define LY_AMS_H                105
#define LY_AMS_BAR_H            64
#define LY_AMS_BAR_GAP          4
#define LY_AMS_GROUP_GAP        14
#define LY_AMS_LABEL_OFFY       7
#define LY_AMS_MARGIN           42
#define LY_AMS_BAR_MAX_W        54
#define LY_AMS_BAR_MAX_W_EXTRAS 46

// --- Printing: ETA / info zone ---
#define LY_ETA_Y        338
#define LY_ETA_H        44
#define LY_ETA_TEXT_Y   360

// --- Printing: bottom status bar ---
// Kept above the narrow bottom cap of the round panel.
#define LY_BOT_Y    390
#define LY_BOT_H    50
#define LY_BOT_CY   415

// --- Printing: WiFi signal indicator ---
#define LY_WIFI_X    193
#define LY_WIFI_Y    390

// --- Battery indicator placeholders ---
#define LY_BAT_W       16
#define LY_BAT_H       32
#define LY_BAT_TEXT_X  24
#define LY_BAT_SHIFT_X 28

// --- Idle screen (with printer) ---
#define LY_IDLE_NAME_Y       72
#define LY_IDLE_STATE_Y      96
#define LY_IDLE_STATE_H      42
#define LY_IDLE_STATE_TY    117
#define LY_IDLE_DOT_Y       158
#define LY_IDLE_GAUGE_R      58
#define LY_IDLE_GAUGE_Y     270
#define LY_IDLE_G_OFFSET    108

// --- Idle screen (no printer) ---
#define LY_IDLE_NP_TITLE_Y   78
#define LY_IDLE_NP_WIFI_Y   145
#define LY_IDLE_NP_DOT_Y    184
#define LY_IDLE_NP_MSG_Y    252
#define LY_IDLE_NP_OPEN_Y   310
#define LY_IDLE_NP_IP_Y     374

// --- Finished screen ---
#define LY_FIN_GAUGE_R   60
#define LY_FIN_GL       143
#define LY_FIN_GR       323
#define LY_FIN_GY       155
#define LY_FIN_TEXT_Y   286
#define LY_FIN_FILE_Y   335
#define LY_FIN_BOT_Y    390
#define LY_FIN_BOT_H     50
#define LY_FIN_WIFI_Y   415

// --- AP mode screen ---
#define LY_AP_TITLE_Y      78
#define LY_AP_SSID_LBL_Y  140
#define LY_AP_SSID_Y      194
#define LY_AP_PASS_LBL_Y  250
#define LY_AP_PASS_Y      286
#define LY_AP_OPEN_Y      340
#define LY_AP_IP_Y        390

// --- Simple clock ---
#define LY_CLK_CLEAR_Y   105
#define LY_CLK_CLEAR_H   260
#define LY_CLK_TIME_Y    215
#define LY_CLK_AMPM_Y    270
#define LY_CLK_DATE_Y    310

// --- Pong/Breakout clock ---
#define LY_ARK_BRICK_ROWS   5
#define LY_ARK_COLS          10
#define LY_ARK_BRICK_W       42
#define LY_ARK_BRICK_H       16
#define LY_ARK_BRICK_GAP      4
#define LY_ARK_START_X       10
#define LY_ARK_START_Y       66
#define LY_ARK_PADDLE_Y     420
#define LY_ARK_PADDLE_W      60
#define LY_ARK_TIME_Y       255
#define LY_ARK_DATE_Y        34
#define LY_ARK_DIGIT_W       62
#define LY_ARK_DIGIT_H       92
#define LY_ARK_COLON_W       22
#define LY_ARK_DATE_CLR_X    78
#define LY_ARK_DATE_CLR_W   310

#endif // LAYOUT_466X466_H
