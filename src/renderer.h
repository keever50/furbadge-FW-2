#pragma once
#include <Arduino.h>

#define REND_MAX_LINES 4
#define REND_LINE_SIZE 8

void rend_init();
void rend_test();
void rend_printf(const char *format, ...);
void rend_clear(bool update);
void rend_draw_pixel_line(uint8_t y, uint8_t *pixels, uint8_t width);
void rend_draw_monopixel(uint8_t x, uint8_t y, uint8_t bit);
void rend_display();
size_t rend_menu(String *list, size_t size);