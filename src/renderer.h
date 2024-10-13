#pragma once
#include <Arduino.h>

#define REND_MAX_LINES 4
#define REND_LINE_SIZE 8

void rend_init();
void rend_test();
void rend_printf(const char *format, ...);
void rend_clear(bool update);
size_t rend_menu(String *list, size_t size);