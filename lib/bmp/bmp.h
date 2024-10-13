#pragma once
#include <stdint.h>
#include <string>
#include <ff.h>
#include <vector>
#include <f_util.h>
#include <memory>

#define BMP_ANIM_BM_SIGN 0x4D42
#define bmp_header_tOTAL_SIZE 54
#define BMP_ANIM_INFO_HEADER_SIZE 40
#define BMP_ANIM_COMPRESSION_RGB 0
#define BMP_ANIM_COMPRESSION_RLE8 1
#define BMP_ANIM_COMPRESSION_RLE4 2

enum bmp_err_e
{
    BMP_ANIM_OK,
    BMP_ANIM_FILE_NOT_OPENED,
    BMP_ANIM_FILE_INCOMPLETE,
    BMP_ANIM_WRONG_FORMAT
};

typedef struct
{
    // Header

    uint16_t signature;
    uint32_t filesize;
    uint32_t reserved;
    uint32_t data_offset;

    // Info header

    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t image_size;
    uint32_t xpixels_per_m;
    uint32_t ypixels_per_m;
    uint32_t colors_used;
    uint32_t important_colors;
} bmp_header_t;

struct bmp_pixel_s
{
    uint8_t R, G, B;
};

struct bmp_color_s
{
    uint8_t R, G, B, r;
};

typedef std::vector<bmp_color_s> bmp_color_table_t;
typedef bmp_pixel_s *bmp_anim_frame_t;

class Bmp_loader
{
public:
    // Loads a .bmp file using std::string path as input. Call close() after last use.
    bmp_err_e load(std::string path);
    // Files have to be closed before loading a new one. Otherwise memory leaks will be caused.
    bmp_err_e close();
    // Gets an individual pixel. load( path ) has to be called first
    bmp_pixel_s get_pixel(uint x, uint y);
    // Gets multiple pixels at once in a line (On X axis). This can be faster than fetching individual pixels. load( path ) has to be called first
    void get_pixel_line(uint x, uint y, uint amount, bmp_pixel_s *destination);
    // Gets the last error in a std::string.
    const std::string &get_err_str() { return err; };
    // Gets the .bmp file header. This contains all information a .bmp contains, except data and color table(When used).
    const bmp_header_t &get_header() { return header; };

    Bmp_loader() {};
    ~Bmp_loader() { close(); };

private:
    FIL file;
    std::string err;

    bmp_header_t header;
    bmp_color_table_t color_table;

    // uint8_t linebufferv[640]; // FIX THIS LIMIT //std::vector<uint8_t> linebufferv;
    uint8_t *linebufferv;

    bmp_pixel_s get_pixel_mono(uint x, uint y);
    void get_pixel_line_mono(uint x, uint y, uint amount, bmp_pixel_s *destination);
    FRESULT file_err_check(FRESULT fr);
    bool allocated = false;
    bool opened = false;
};
