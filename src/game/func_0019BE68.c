#include "kf_packet.h"

#define SCREEN_WIDTH 0x280
#define SCREEN_HEIGHT 0x1C0
#define SCREEN_BYTES (SCREEN_WIDTH * SCREEN_HEIGHT * 3)

void *memset(void *, int, u32);
int FlushCache(int);
int sceClose(int);
int sceOpen(const char *, int);
int sceWrite(int, const void *, int);
void sceGsSetDefStoreImage(void *, s16, s16, s16, s16, s16, s16, s16);
void sceGsExecStoreImage(void *, void *);
int sprintf(char *, const char *, ...);
extern int D_00446F78;
extern char D_0043EBF0[];

/* Stores the screen into a 16-byte aligned buffer, turns it into the rows of a .bmp behind it (bottom row
   first, blue first) and writes that to the first unused numbered screenshot file. */
void func_0019BE68(u8 *buffer) {
    BmpFileHeader file_header;
    BmpInfoHeader info_header;
    char store_image[0x70];
    char name[0x100];
    int file;
    int i;
    int in_range;
    s16 base;
    u32 misalign;
    u8 *aligned;
    u8 *source;
    u8 *dest;
    int row;
    int column;
    u8 red;
    u8 green;
    u8 blue;
    u32 file_size;
    u32 pixel_offset;

    base = D_00446F78 != 0 ? 0x1180 : 0; // the other way round from func_0019C0F0 (the .raw screenshot)
    memset(&file_header, 0, sizeof(BmpFileHeader));
    file_header.type[0] = 'B';
    file_header.type[1] = 'M';
    // match: the sizes go through variables, written as literals the halves become plain constants
    file_size = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader) + SCREEN_BYTES;
    file_header.file_size_low = file_size;
    file_header.file_size_high = file_size >> 16;
    pixel_offset = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader);
    file_header.pixel_offset_low = pixel_offset;
    file_header.pixel_offset_high = pixel_offset >> 16;
    memset(&info_header, 0, sizeof(BmpInfoHeader));
    info_header.header_size = sizeof(BmpInfoHeader);
    info_header.width = SCREEN_WIDTH;
    info_header.height = SCREEN_HEIGHT;
    info_header.planes = 1;
    info_header.bit_count = 24;
    info_header.compression = 0;
    info_header.image_size = SCREEN_BYTES;
    info_header.pixels_per_metre_x = 0;
    info_header.pixels_per_metre_y = 0;
    info_header.colours_used = 0;
    info_header.colours_important = 0;
    i = 0;
    // match: the test stays at the top only in a do loop; while/for get rotated
    do {
        in_range = i < 0x400;
        if (!in_range) {
            break;
        }
        sprintf(name, D_0043EBF0, i);
        file = sceOpen(name, 2);
        if (file < 0) {
            break;
        }
        sceClose(file);
        i++;
    } while (TRUE);
    if (in_range) {
        aligned = buffer;
        misalign = (u32)aligned & 0xF;
        if (misalign != 0) {
            aligned -= misalign - 0x10;
        }
        sceGsSetDefStoreImage(store_image, base, 0xA, 1, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        FlushCache(0);
        sceGsExecStoreImage(store_image, aligned);
        sceGsSyncPath(0, 0);
        FlushCache(0);
        source = aligned;
        for (row = 0; row < SCREEN_HEIGHT; row++) {
            dest = aligned + SCREEN_BYTES; // match: two statements, one sum gets added up in another order
            dest += (SCREEN_HEIGHT - 1 - row) * (SCREEN_WIDTH * 3);
            for (column = 0; column < SCREEN_WIDTH; column++) {
                red = *source++;
                green = *source++;
                blue = *source++;
                *dest++ = blue;
                *dest++ = green;
                *dest++ = red;
            }
        }
        FlushCache(0);
        file = sceOpen(name, 0x202);
        if (file >= 0) {
            sceWrite(file, &file_header, sizeof(BmpFileHeader));
            sceWrite(file, &info_header, sizeof(BmpInfoHeader));
            sceWrite(file, aligned + SCREEN_BYTES, SCREEN_BYTES);
            sceClose(file);
        }
    }
}
