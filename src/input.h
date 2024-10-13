#pragma once
#include <Arduino.h>
struct input_directions
{
  char up, right, down, left, pressed;
  uint32_t debug;
};

void input_init();
bool input_get(struct input_directions *inputs);