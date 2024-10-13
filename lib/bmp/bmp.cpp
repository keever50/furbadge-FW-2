#include <bmp.h>

FRESULT Bmp_loader::file_err_check(FRESULT fr)
{
    if (fr != FR_OK)
    {
        err = FRESULT_str(fr);
    }

    return fr;
}

bmp_err_e Bmp_loader::load(std::string path)
{

    FRESULT fr = f_open(&file, path.c_str(), FA_READ | FA_OPEN_ALWAYS);
    if (fr != FR_OK)
    {
        err = FRESULT_str(fr);
        return BMP_ANIM_FILE_NOT_OPENED;
    }
    opened = true;

    uint br;
    // Get header info
    fr = f_read(&file, &header.signature, sizeof(header.signature), &br);
    fr = f_read(&file, &header.filesize, sizeof(header.filesize), &br);
    fr = f_read(&file, &header.reserved, sizeof(header.reserved), &br);
    fr = f_read(&file, &header.data_offset, sizeof(header.data_offset), &br);

    fr = f_read(&file, &header.size, sizeof(header.size), &br);
    fr = f_read(&file, &header.width, sizeof(header.width), &br);
    fr = f_read(&file, &header.height, sizeof(header.height), &br);
    fr = f_read(&file, &header.planes, sizeof(header.planes), &br);

    fr = f_read(&file, &header.bits, sizeof(header.bits), &br);
    fr = f_read(&file, &header.compression, sizeof(header.compression), &br);
    fr = f_read(&file, &header.image_size, sizeof(header.image_size), &br);
    fr = f_read(&file, &header.xpixels_per_m, sizeof(header.xpixels_per_m), &br);

    fr = f_read(&file, &header.ypixels_per_m, sizeof(header.ypixels_per_m), &br);
    fr = f_read(&file, &header.colors_used, sizeof(header.colors_used), &br);
    fr = f_read(&file, &header.important_colors, sizeof(header.important_colors), &br);

    // Check info //
    // Check signature
    if (header.signature != BMP_ANIM_BM_SIGN)
    {
        err = "Wrong signature. Expected BM";
        return BMP_ANIM_WRONG_FORMAT;
    }

    // Check file size
    if (header.filesize < bmp_header_tOTAL_SIZE)
    {
        err = "File size too small";
        return BMP_ANIM_WRONG_FORMAT;
    }

    // Check data offset
    if (header.data_offset < bmp_header_tOTAL_SIZE)
    {
        err = "Incorrect data offset";
        return BMP_ANIM_WRONG_FORMAT;
    }

    // Check header size
    if (header.size != BMP_ANIM_INFO_HEADER_SIZE)
    {
        err = "Incorrect info header size";
        return BMP_ANIM_WRONG_FORMAT;
    }

    // CHANGE THIS WHEN ADDING SUPPORT FOR MORE BITS
    if (header.bits != 1)
    {
        err = "Unsupported format. BMP is not monochrome.";
        return BMP_ANIM_WRONG_FORMAT;
    }

    // CHANGE THIS WHEN ADDING SUPPORT FOR MORE COMPRESSION
    if (header.compression != BMP_ANIM_COMPRESSION_RGB)
    {
        err = "Unsupported compression. Compression is not supported.";
        return BMP_ANIM_WRONG_FORMAT;
    }

    // Generate color table here (if supported)

    // Reserve line buffer
    if (header.bits == 1)
    {
        linebufferv = (uint8_t *)malloc(header.width / 8);
        allocated = true;
    }

    return BMP_ANIM_OK;
}

bmp_err_e Bmp_loader::close()
{
    if (opened)
    {
        f_close(&file);
        opened = false;
    }
    if (allocated)
    {
        free(linebufferv);
        allocated = false;
    }
    return BMP_ANIM_OK;
}

void Bmp_loader::get_pixel_line_mono(uint x, uint y, uint amount, bmp_pixel_s *destination)
{
    uint w = header.width, h = header.height;
    // Boundry check
    if (x + amount > w || y >= h)
        return;

    // Get address
    y = h - y - 1;
    uint addr = x + y * w;
    uint byteaddr = addr / 8;
    uint byteamount = amount / 8;

    // Get pixels
    uint br;
    f_lseek(&file, header.data_offset + byteaddr);
    f_read(&file, linebufferv, byteamount, &br);

    // Convert bytes to bits
    for (uint i = 0; i < amount; i++)
    {
        uint8_t vbyteaddr = i / 8;
        uint8_t vbitshift = i % 8;
        uint8_t pixel = (linebufferv[vbyteaddr] << vbitshift) & 0b10000000;
        bmp_pixel_s bmppixel;
        if (!pixel)
        {
            bmppixel = {255, 255, 255};
        }
        else
        {
            bmppixel = {0, 0, 0};
        }

        destination[i] = bmppixel;
    }
}

void Bmp_loader::get_pixel_line(uint x, uint y, uint amount, bmp_pixel_s *destination)
{
    if (header.bits == 1)
    {
        get_pixel_line_mono(x, y, amount, destination);
    }
}

bmp_pixel_s Bmp_loader::get_pixel_mono(uint x, uint y)
{
    uint w = header.width, h = header.height;
    // Boundry check
    if (x >= w || y >= h)
        return {0, 0, 0};

    // Get address
    y = h - y - 1;
    uint addr = x + y * w;
    uint bitaddr = addr / 8;
    uint8_t bitshift = addr % 8;

    // Get pixel
    uint8_t bits;
    uint br;
    f_lseek(&file, header.data_offset + bitaddr);
    f_read(&file, &bits, 1, &br);

    uint8_t bit = (bits << bitshift) & 0b10000000;

    if (!bit)
    {
        return {255, 255, 255};
    }
    else
    {
        return {0, 0, 0};
    };
}

bmp_pixel_s Bmp_loader::get_pixel(uint x, uint y)
{
    if (header.bits == 1)
    {
        return get_pixel_mono(x, y);
    }

    return {0, 0, 0};
}
