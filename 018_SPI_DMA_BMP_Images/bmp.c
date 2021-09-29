#include "bmp.h"
#include "stm32f10x.h"
#include "xprintf.h"
#include "uart.h"
#include "ff9/src/ff.h"
#include "ST7735.h"

#define BLOCK_SIZE 128 // possible values: 1, 16, 32, 64, 128 pixels
#define WIDTH 128
#define HEIGHT 160
#define BITSPP 24 // 24bit color

#define RED_OFFSET 0 // red offset in a 16 bit depth pixel (for shifting left <<)
#define GREEN_OFFSET 5
#define BLUE_OFFSET 11

#define RED_MASK 0b11111000 // red takes 5 **lower** bits in a 8 bit R part of a 24 bit depth pixel
#define GREEN_MASK 0b11111100
#define BLUE_MASK 0b11111000

struct bmpfile_magic {
    unsigned char magic[2];
};

struct bmpfile_header {
    uint32_t filesz;
    uint16_t creator1;
    uint16_t creator2;
    uint32_t bmp_offset;
};

typedef struct {
    uint32_t header_sz;
    int32_t width;
    int32_t height;
    uint16_t nplanes;
    uint16_t bitspp;
    uint32_t compress_type;
    uint32_t bmp_bytesz;
    int32_t hres;
    int32_t vres;
    uint32_t ncolors;
    uint32_t nimpcolors;
} BITMAPINFOHEADER;

struct bmppixel { // little endian byte order
    uint8_t b;
    uint8_t g;
    uint8_t r;
};

struct bmpfile_magic magic;
struct bmpfile_header header;
BITMAPINFOHEADER info;

static int is_valid_bmp(FIL Fil, char *filename)
{
    UINT br;
    FRESULT rc;

    // Check magic number
    rc = f_read(&Fil, (void *) &magic, 2, &br);
    xprintf("Magic %c%c\n", magic.magic[0], magic.magic[1]);
    if (rc || !br || !(magic.magic[0] == 'B' && magic.magic[1] == 'M')) return 0;

    // Reading the rest of the header
    rc = f_read(&Fil, (void *) &header, sizeof(header), &br);
    xprintf("file size %d offset %d\n", header.filesz, header.bmp_offset);
    if (rc || !br) return 0;

    // Check bitspp and height/width
    rc = f_read(&Fil, (void *) &info, sizeof(info), &br);
    xprintf("Width %d Height %d, bitspp %d\n", info.width, info.height, info.bitspp);
    if (rc || !br || !(info.width == WIDTH && info.height == HEIGHT) || info.bitspp != BITSPP) return 0;

    return 1; // success validating
}

/**
 * Converts 24 bit depth pixels from <Buff> to 16 bit depth pixels in <converted>
 */
void convert_color_24_to_16(uint8_t *Buff, uint16_t *converted) {
    uint16_t new_pixel;
    for (int i = 0; i < (BLOCK_SIZE * 3); i += 3) { // one pixel takes 3 bytes
        new_pixel = 0;

        // Blue
        new_pixel |= (((Buff[i] & BLUE_MASK) >> 3) << (BLUE_OFFSET));
        // Green
        new_pixel |= (((Buff[i+1] & GREEN_MASK) >> 2) << (GREEN_OFFSET));
        // Red
        new_pixel |= (((Buff[i+2] & RED_MASK) >> 3) << (RED_OFFSET));

        converted[i / 3] = new_pixel;
    }
}

int bmp_display(char *filename)
{
    FRESULT rc;
    FIL Fil;
    UINT bw, br; // bytes written, bytes read
    uint8_t Buff[BLOCK_SIZE * 3]; // initial buffer, one pixel takes 3 bytes
    uint16_t converted[BLOCK_SIZE]; // one pixel takes one uint16_t cell

    xprintf("\nOpen an existing file (%s).\n", filename);
    rc = f_open(&Fil, filename, FA_READ);
    
    if (!rc) {
        if (!is_valid_bmp(Fil, filename)) {
            xprintf("Rejecting file...\n");
            return -1;
        }
        f_lseek(&Fil, header.bmp_offset);
        ST7735_setAddrWindow(0, 0, WIDTH, HEIGHT, MADCTLBMP);
        for (int i = 0; i < HEIGHT * (WIDTH / BLOCK_SIZE); i++) { // how many pixels the loop will visit
            f_read(&Fil, (void *) &Buff, (3 * BLOCK_SIZE), &br); // one pixel takes 3 bytes
            convert_color_24_to_16(Buff, converted);
            ST7735_pushColor(converted, BLOCK_SIZE);
        }
    
        xprintf("\nClose the file.\n\n");
        rc = f_close(&Fil);
        if (rc) return(rc);
    }

    f_close(&Fil);
    return 0;
}