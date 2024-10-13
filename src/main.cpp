#include <Arduino.h>
#include <renderer.h>
#include <input.h>
#include <vector>
#include <fs_init.h>
#include <badgeio.h>

#include <apps/app.h>
#include <apps/testapp.h>
#include <apps/imageviewer.h>

#define APP_COUNT sizeof(app_list) / sizeof(application)
application app_list[] =
    {
        {.name = "imageviewer", .open = app_imageviewer},
        {.name = "ABC", .open = testapp_open},
        {.name = "TESTER", .open = testapp_open},
        {.name = "APP", .open = testapp_open},
        {.name = "sdgfsdtestapp", .open = testapp_open},
        {.name = "AfdgdfBC", .open = testapp_open},
        {.name = "TEdgffSTER", .open = testapp_open},
        {.name = "AdgfP", .open = testapp_open}};

void do_halting_error(const char *msg)
{
  b_printf("SD/FS Error");
  while (true)
    ;
}

void setup()
{
  Serial.begin(115200);
  input_init();
  rend_init();
  if (!fs_init())
    do_halting_error("SD/FS Error");
}

void loop()
{
  Serial.println("Test");
  static int i = 0;
  i = i + 1;
  rend_clear(false);

  // application *apps;
  // size_t app_count = get_apps(apps);

  String app_names[APP_COUNT];
  for (size_t i = 0; i < APP_COUNT; i++)
  {
    app_names[i] = app_list[i].name;
  }
  size_t selection = rend_menu(app_names, APP_COUNT);
  app_list[selection].open();

  delay(500);
}
