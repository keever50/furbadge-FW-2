#pragma once
#include <Arduino.h>
#include <stdlib.h>
#include <vector>

typedef void(*app_open_func)(void);

struct application
{
  const char *name;
  app_open_func open;
};


size_t get_apps(application* apps);