#pragma once
#include <hardware/spi.h>


/* DISPLAY */

#define DISP_RES_W 128
#define DISP_RES_H 32
#define PIN_DISP_MISO 8
#define PIN_DISP_RES 6
#define PIN_DISP_DC 7
#define PIN_DISP_CS 9
#define PIN_DISP_SCK 10
#define PIN_DISP_MOSI 11
#define DISP_EN_12V 24

/* BUTTONS */

#define JOY_BU 12
#define JOY_BB 13
#define JOY_BL 14
#define JOY_BR 15
#define JOY_BP 20

/* SD */
#define SD_MISO 0
#define SD_CS 1
#define SD_SCK 2
#define SD_MOSI 3
#define SD_SPEED 64'000'000
#define SD_SPI spi0