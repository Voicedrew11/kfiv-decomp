#include "kf_packet.h"

extern u32 D_00446DF8;
extern s64 D_00446DB0;
extern s64 D_00446DB8;
extern s64 *D_00447344;
extern SpritePacket D_005DDA60;

/* Splits a colour word (red in bits 24-31, green 16-23, blue 8-15) into three bytes. */
static inline void UnpackColour(u8 *rgb, u32 *colour) {
    rgb[0] = *colour >> 24;
    rgb[1] = *colour >> 16;
    rgb[2] = *colour >> 8;
}

/* Copies the packet in the colour D_00446DF8 holds (none when it is 0), picking the data for two special colours. */
SpritePacket *func_00206018(SpritePacket *packet) {
    u32 colour = D_00446DF8;
    u8 unused[16]; // match: retail's frame has 16 unused bytes below the colour bytes
    u8 rgb[3];
    s32 green;
    s32 blue;

    if (colour == 0x80FF00) {
        D_00447344 = &D_00446DB0;
    } else if (colour == 0xFF800000) {
        D_00447344 = &D_00446DB8;
    } else if (colour == 0) {
        return NULL;
    }
    D_005DDA60 = *packet;
    UnpackColour(rgb, &colour); // match: taking the colour's address keeps the three shifts after the copy
    D_005DDA60.red = rgb[0];
    green = rgb[1]; // match: green and blue widened to int (andi 0xFF), red stored as a byte
    blue = rgb[2];
    D_005DDA60.green = green;
    D_005DDA60.blue = blue;
    return &D_005DDA60;
}
