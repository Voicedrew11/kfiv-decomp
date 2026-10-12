#include "kf_movie.h"
#include "kf_gs.h"

int func_0021EE78(int, s32 *, s32 *);
int func_0021EFE8(int, u32, u32, u32, u32, u32, u32);
int func_0021F040(int, s64, s32, s32, s32, s32, s32, s32);
int func_0021F0C8(int);
int func_0021F0E8(s64, u32, u32, u32, u32, u32, u32, u64);
int func_0021F150(s64, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
int func_0021F560(int, u32, u32, u32);
int func_0021F590(int, u32, u32, u32);
int func_0021F5C0(int, u32, u32);
int func_0021F5F0(int, u32);

#define ROUND_UP(value, unit) ((((value) + (unit) - 1) / (unit)) * (unit))

/* Builds the GIF chain that shows a decoded picture on one field: for the first field it
   uploads the picture as a texture, one 16x16 macroblock at a time, and for both it draws
   the texture over the field's frame buffer. */
void func_0021E960(MovieDisplay *display, u32 packet_start, u32 picture, s32 field) {
    s32 screen_rect[4];
    s32 texture_rect[4];
    s32 picture_width = display->picture_width;
    s32 picture_height = display->picture_height;
    s32 width = display->width;
    s32 field_height = display->height / 2;
    s32 rows = picture_height >> 4;
    s32 columns = picture_width >> 4;
    s32 packet = (packet_start & 0x0FFFFFFF) | 0x20000000;
    s32 field_pages = (field_height + 31) / 32;      /* a field's frame buffer, in rows of GS pages */
    s32 buffer_width = ROUND_UP(width, 64) / 64;
    s32 buffer_pixels = buffer_width * 64;
    s32 texture_base = field_pages * buffer_pixels;  /* the texture comes after the two frame buffers */
    s32 column;
    s32 row;

    if (field == 0) {
        screen_rect[0] = display->screen_x << 4;
        screen_rect[1] = display->screen_y << 4;
        screen_rect[2] = width << 4;
        screen_rect[3] = field_height << 4;
        texture_rect[0] = 8;
        texture_rect[1] = 0x18;
        texture_rect[2] = picture_width << 4;
        texture_rect[3] = picture_height << 4;
    } else {
        screen_rect[0] = display->screen_x << 4;
        screen_rect[1] = (display->screen_y + field_pages * 32) << 4;
        screen_rect[2] = width << 4;
        screen_rect[3] = field_height << 4;
        texture_rect[1] = texture_rect[0] = 8;
        texture_rect[2] = picture_width << 4;
        texture_rect[3] = picture_height << 4;
    }
    if (field == 0) {
        packet = func_0021EFE8(packet, 0, 0, 0, 1, 0, 3);
        packet = func_0021F040(packet, 0xE, 1, 0, 0, 0, 0, 2);
        packet = func_0021F560(packet, texture_base, buffer_width, 0);
        packet = func_0021F5C0(packet, 16, 16);
        for (column = 0; column < columns; column++) {
            for (row = 0; row < rows; row++) {
                packet = func_0021EFE8(packet, 0, 0, 0, 1, 0, 4);
                packet = func_0021F040(packet, 0xE, 1, 0, 0, 0, 0, 2);
                packet = func_0021F590(packet, 0, column << 4, row << 4);
                packet = func_0021F5F0(packet, 0);
                packet = func_0021F040(packet, 0, 0, 2, 0, 0, 0, 0x40);
                packet = func_0021EFE8(packet, 0, picture & 0x0FFFFFFF, 0, 3, 0, 0x40);
                picture += 0x400;
            }
        }
    }
    packet = func_0021EFE8(packet, 0, 0, 0, 7, 0, 9);
    packet = func_0021F040(packet, 0xE, 1, 0, 0, 0, 1, 8);
    packet = func_0021F0C8(packet);
    packet = func_0021F150(packet, texture_base, buffer_width, 0, 10, 10, 0, 1, 0, 0, 0, 0, 0);
    packet = func_0021F0E8(packet, 0, 0, 1, 1, 0, 0, 0);
    func_0021EE78(packet, screen_rect, texture_rect);
}
