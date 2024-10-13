#include <renderer.h>
#include <Adafruit_SSD1305.h>
#include <hardware.h>
#include <hardware/spi.h>
#include <Wire.h>
#include <SPI.h>
#include <stdarg.h>
#include <input.h>

//SPIClassRP2040 disp_spi(spi0, PIN_DISP_MISO, PIN_DISP_CS, PIN_DISP_SCK, PIN_DISP_MOSI);
//Adafruit_SSD1305 display(DISP_RES_W, DISP_RES_H, &disp_spi, PIN_DISP_DC, PIN_DISP_RES, PIN_DISP_CS, 2000000UL);
Adafruit_SSD1305 display(DISP_RES_W, DISP_RES_H, PIN_DISP_MOSI, PIN_DISP_SCK, PIN_DISP_DC, PIN_DISP_RES, PIN_DISP_CS);
void rend_init()
{
  pinMode(DISP_EN_12V, OUTPUT);
  digitalWrite(DISP_EN_12V, HIGH);
  display.begin();
  display.display();
  delay(1000);
  display.clearDisplay();
}

void rend_test()
{
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  display.print("!!!TEST!!!");
  display.display();

}

void rend_printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);

  char buffer[0xFF];
  vsnprintf(buffer, sizeof(buffer), format, args);
  display.print(buffer);
  display.display();
  va_end(args);
}

void rend_clear(bool update)
{
  display.clearDisplay();
  display.setCursor(0,0);
  if(update)
    display.display();
}

void _rend_draw_menu(const char** list, size_t size, int offset, int selector)
{
  rend_clear(false);
  for(size_t i=offset;i<size;i++)
  {
    if(i==selector)
    {
      display.drawLine(0, (i-offset+1)*REND_LINE_SIZE-1, DISP_RES_W, (i-offset+1)*REND_LINE_SIZE-1, WHITE);
    }
    display.printf("%d: %s\n", i, list[i]);
  }

  display.display();
}

size_t rend_menu(const char** list, size_t size)
{
  int cursor=0;
  int offset=0;
  _rend_draw_menu(list, size, 0, 0);

  input_directions inputs;
  for(;;)
  {
    bool change = input_get(&inputs);
    if(!change) continue;
    delay(100);
    
    /* Enter */
    if(inputs.pressed) break;

    /* Navigate */
    if(inputs.down) cursor++;
    if(inputs.up) cursor--;
    if(cursor<0) cursor = 0;
    if(cursor>=size) cursor=size-1;
    /* Page offset */
    offset=(cursor/REND_MAX_LINES)*REND_MAX_LINES;

    _rend_draw_menu(list, size, offset, cursor);
    
  }
  rend_clear(true);
  return cursor;

}