#include <Arduino.h>
#include <renderer.h>
#include <input.h>
#include <vector>
#include <apps/app.h>
#include <apps/testapp.h>

#define APP_COUNT sizeof(app_list)/sizeof(application)
application app_list[]=
{
  {.name="testapp", .open=testapp_open},
  {.name="ABC", .open=testapp_open},
  {.name="TESTER", .open=testapp_open},
  {.name="APP", .open=testapp_open},
  {.name="sdgfsdtestapp", .open=testapp_open},
  {.name="AfdgdfBC", .open=testapp_open},
  {.name="TEdgffSTER", .open=testapp_open},
  {.name="AdgfP", .open=testapp_open}  
};


void setup() {
  Serial.begin(115200);
  input_init();
  rend_init();
  rend_test();
}

void loop() {
  Serial.println("Test");
  static int i=0;
  i=i+1;
  rend_clear(false);
  
  // application *apps;
  // size_t app_count = get_apps(apps);

  const char *app_names[APP_COUNT];
  for(size_t i=0;i<APP_COUNT;i++)
  {
    app_names[i]=app_list[i].name;
  }
  size_t selection = rend_menu(app_names, APP_COUNT);
  app_list[selection].open();

  delay(500);


}

