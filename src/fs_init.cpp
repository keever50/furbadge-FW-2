#include <hardware/spi.h>
#include <ff.h>
#include "sd_card.h"
#include <f_util.h>
#include <hardware.h>

// Hardware Configuration of SPI "objects"
// Note: multiple SD cards can be driven by one SPI if they use different slave
// selects.
static spi_t spi = {
    .hw_inst = SD_SPI, // RP2040 SPI component
    .miso_gpio = SD_MISO,
    .mosi_gpio = SD_MOSI,
    .sck_gpio = SD_SCK,   // GPIO number (not Pico pin number)
    .baud_rate = SD_SPEED // Actual frequency: 10416666.
};

static sd_spi_if_t spi_if = {
    .spi = &spi,             // Pointer to the SPI driving this card
    .ss_gpio = SD_CS // The SPI slave select GPIO for this SD card
};

// Hardware Configuration of the SD Card "objects"
static sd_card_t sd_card = {
    /* "pcName" is the FatFs "logical drive" identifier.
    (See http://elm-chan.org/fsw/ff/doc/filename.html#vol) */
    .pcName = "0:",
    .type = SD_IF_SPI,
    .spi_if_p = &spi_if, // Pointer to the SPI interface driving this card
    .use_card_detect = false,
    .card_detected_true = 0, // What the GPIO read returns when a card is present.
    .card_detect_use_pull = false,
    .card_detect_pull_hi = false

};
extern "C" size_t sd_get_num()
{
    return 1;
}
extern "C" sd_card_t *sd_get_by_num(size_t num)
{
    if (0 == num)
    {
        return &sd_card;
    }
    else
    {
        return NULL;
    }
}

bool fs_init()
{
  sd_card_t *pSD = sd_get_by_num(0);
  FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
  if(fr!=FR_OK) return false;
  return true;
}