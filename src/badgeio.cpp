#include <badgeio.h>

void b_printf(const char *format, ...)
{
  va_list args;
  va_start(args, format);

  char buffer[0xFF];
  vsnprintf(buffer, sizeof(buffer), format, args);

  rend_printf(buffer);
  Serial.print(buffer);

  va_end(args);
}