#include <apps/imageviewer.h>
#include <bmp.h>
#include <ff.h>
#include <renderer.h>
#include <badgeio.h>

#define APP_IV_MAX_DIR_ENTRIES 128

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
}

void _app_IV_view(String path)
{
}

void _app_IV_find_and_view()
{
  rend_printf("Select file");
  delay(1000);
  rend_clear(false);

  String file_found;
  bool found = _app_IV_find_file(file_found);
  if (!found)
  {
    rend_clear(false);
    rend_printf("FS error");
    return;
  }
  _app_IV_view(file_found);
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
    break;
  }
}