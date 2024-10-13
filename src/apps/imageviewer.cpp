#include <apps/imageviewer.h>
#include <bmp.h>
#include <ff.h>
#include <renderer.h>
#include <badgeio.h>
#include <hardware.h>
#include <input.h>

#define APP_IV_MAX_DIR_ENTRIES 128
#define APP_IV_WAIT_CONFIRM_S 3
bool _app_IV_dirmenu(String path, String &selected_path, bool &is_file)
{
  String list[APP_IV_MAX_DIR_ENTRIES];
  bool list_isfile[APP_IV_MAX_DIR_ENTRIES];
  String list_path[APP_IV_MAX_DIR_ENTRIES];
  size_t list_index = 0;
  FRESULT res;
  DIR dir;
  FILINFO fno;
  int nfile, ndir;

  b_printf("Opening dir\n");
  res = f_opendir(&dir, path.c_str()); /* Open the directory */
  if (res == FR_OK)
  {
    nfile = ndir = 0;
    for (;;)
    {
      b_printf("read dir\n");
      res = f_readdir(&dir, &fno); /* Read a directory item */
      b_printf("done\n");
      if (res != FR_OK || fno.fname[0] == 0)
      {
        b_printf("end of dir\n");
        break;
      }
      if (fno.fattrib & AM_DIR)
      { /* Directory */
        b_printf("   <DIR>   ");
        b_printf(fno.fname);
        b_printf("\n");

        list_isfile[list_index] = false;
        list_path[list_index] = String(fno.fname);
        list[list_index++] = "<DIR> " + String(fno.fname);
        ndir++;
      }
      else
      { /* File */
        b_printf(fno.fname);
        b_printf("\n");
        list_isfile[list_index] = true;
        list_path[list_index] = String(fno.fname);
        list[list_index++] = String(fno.fname);
        nfile++;
      }
    }
    f_closedir(&dir);
    b_printf("%d dirs, %d files.\n", ndir, nfile);
  }
  else
  {
    b_printf("Failed to open \"%s\". (%u)\n", path, res);
    return false;
  }
  size_t selected = rend_menu(list, list_index);
  selected_path = list_path[selected];
  is_file = list_isfile[selected];
  return true;
}

bool _app_IV_find_dir(String &dir_found)
{
  String selected_path = "";
  String start_path = "";
  for (;;)
  {
    bool is_file;
    bool opened = _app_IV_dirmenu(start_path, selected_path, is_file);
    b_printf(selected_path.c_str());
    b_printf("\n");
    if (!opened)
      return false;
    start_path = start_path + "/" + selected_path;
    b_printf(start_path.c_str());
    b_printf("\n");

    /* is dir */
    if (!is_file)
    {
      /* Wait X to confirm */
      rend_clear(false);
      rend_printf("Wait %ds to confirm\nPress to enter directory\n", APP_IV_WAIT_CONFIRM_S);
      rend_printf(start_path.c_str());

      /* Wait loop */
      bool confirmed = true;
      unsigned long start = millis();
      while (millis() - start < APP_IV_WAIT_CONFIRM_S * 1000)
      {
        /* On button press, cancel wait and dont confirm */
        input_directions inputs;
        bool changed = input_get(&inputs);
        if (changed && inputs.pressed)
        {
          confirmed = false;
          break;
        }
      }

      /*When confirmed, leave with result*/
      if (confirmed)
      {
        dir_found = start_path;
        return true;
      }
    }
  }
}

/* Returns true when file found */
bool _app_IV_find_file(String &file_found)
{
  String selected_path = "";
  String start_path = "";
  for (;;)
  {
    bool is_file;
    bool opened = _app_IV_dirmenu(start_path, selected_path, is_file);
    b_printf(selected_path.c_str());
    b_printf("\n");
    if (!opened)
      return false;
    start_path = start_path + "/" + selected_path;
    b_printf(start_path.c_str());
    b_printf("\n");

    if (is_file)
    {
      file_found = start_path;
      return true;
    }
  }

  return false;
}

bool _app_IV_view(String path, bool dontwait)
{

  Bmp_loader bmp;
#if BADGE_DEBUG == 1
  b_printf("Loading bmp\n");
  b_printf(path.c_str());
  b_printf("\n");
#endif

  bmp_err_e err = bmp.load(path.c_str());
  if (err != BMP_ANIM_OK)
  {
    rend_clear(false);
    b_printf(bmp.get_err_str().c_str());
    b_printf("\n");
    delay(2000);
    bmp.close();
    return false;
  }

  rend_clear(false);
  for (uint8_t y = 0; y < DISP_RES_H; y++)
  {
    bmp_pixel_s pixelline[DISP_RES_W];
    bmp.get_pixel_line(0, y, DISP_RES_W, pixelline);

    for (uint8_t x = 0; x < DISP_RES_W; x++)
    {
      uint8_t bit = pixelline[x].B > 100;
      rend_draw_monopixel(x, y, bit);
    }
  }

  rend_display();

  bmp.close();
  input_directions inputs;
  if (!dontwait)
  {
    for (;;)
    {
      input_get(&inputs);
      if (inputs.pressed)
        break;
    }
  }

  return true;
}

void _app_IV_find_and_view()
{
  rend_printf("Select file");
  delay(1000);

  String file_found;
  bool found = _app_IV_find_file(file_found);
  if (!found)
  {
    rend_clear(false);
    rend_printf("FS error");
    return;
  }
  rend_clear(false);
  _app_IV_view(file_found, false);
  rend_printf("Exit file find\n");
}

void _app_IV_find_and_rotate_view()
{
  String dir_found;
  bool found = _app_IV_find_dir(dir_found);

  for (;;)
  {
    /* Open dir */
    DIR dir;
    FRESULT res = f_opendir(&dir, dir_found.c_str());
    if (res != FR_OK)
      return;

    /* Start rotation */
    for (;;)
    {
      FILINFO fno;
      res = f_readdir(&dir, &fno);
      /* On end of line, restart */
      if (res != FR_OK || fno.fname[0] == 0)
      {
        f_closedir(&dir);
        break;
      }

      /* Ignore dirs */
      if (fno.fattrib & AM_DIR)
      {
        continue;
      }

      /* View file if .bmp, otherwise ignore */
      char fname[128];
      strcpy(fname, fno.fname);
      char *name = strtok(fname, ".");
      char *extension = strtok(NULL, ".");
#if BADGE_DEBUG == 1
      b_printf(extension);
      b_printf("\n");
#endif
      if (strcmp(extension, "bmp") != 0)
        continue;

      /* Load and render */
      String path = dir_found + "/" + String(fno.fname);
      bool opened = _app_IV_view(path, true);

      delay(APP_IV_SLIDE_DELAY_MS);

      /* Exit? */
      input_directions inputs = {0};
      input_get(&inputs);
      if (inputs.pressed)
      {
        f_closedir(&dir);
        return;
      }
    }
  }
}

void app_imageviewer()
{
  String options[] =
      {
          "single image",
          "rotate directory"};
  size_t option_selected = rend_menu(options, 2);

  switch (option_selected)
  {
  case 0:
    _app_IV_find_and_view();
    break;
  case 1:
    _app_IV_find_and_rotate_view();
    break;
  }
  rend_printf("Exit viewer\n");
}