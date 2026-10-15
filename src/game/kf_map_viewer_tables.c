/* Map viewer entries, screen rectangles, lights and blend colours. */
#include "kf_game.h"
#include "kf_map.h"
#include "kf_packet.h"

/* the viewers and the functions that find where the marker goes (kf_map_viewer.c) */
void func_0011AA08();
int func_0011B1C0(int *screen_x, int *screen_y);
int func_0011B4F0(int *screen_x, int *screen_y);
int func_0011BAB8(int *map_x, int *map_y);
int func_0011BBC0(int *screen_x, int *screen_y);
void func_0011BCD0();

/* 0x38D6B0 one entry per viewer, indexed by picked_viewer */
ViewerEntry D_0038D6B0[5] = {
    { func_0011AA08, (int)func_0011B1C0, 2 },
    { func_0011AA08, (int)func_0011B4F0, 3 },
    { func_0011AA08, (int)func_0011BAB8, 1 },
    { func_0011AA08, (int)func_0011BBC0, 1 },
    { func_0011BCD0, 0, 0 },
};

/* 0x38D6F0 two unused screen rectangles */
static ScreenQuad viewer_screen_rects[2] __attribute__((aligned(16))) = {
            { .tag = 0x0000000000000000, .unk08 = 0x0000000000000000, .unk10 = 0x46,
          .red = 255, .green = 0, .blue = 0, .alpha = 128,
          .first_x = 0x6C00, .first_y = 0x7200, .first_z = 0xFFFFFFFF,
          .first_u = 0, .first_v = 0,
          .second_x = 0x9400, .second_y = 0x8E00, .second_z = 0xFFFFFFFF,
          .second_u = 0x1E00, .second_v = 0x1500 },
            { .tag = 0x4007FC0599306BC0, .unk08 = 0x0000000000000060, .unk10 = 0x156,
          .red = 128, .green = 128, .blue = 128, .alpha = 128,
          .first_x = 0x6C00, .first_y = 0x7200, .first_z = 0xFFFFFFFF,
          .first_u = 0, .first_v = 0,
          .second_x = 0x7400, .second_y = 0x7A00, .second_z = 0xFFFFFFFF,
          .second_u = 0x400, .second_v = 0x400 },
};

ScreenQuad D_0038D770 = { .tag = 0x4007FC0665322BC0, .unk08 = 0x0000000000000060, .unk10 = 0x156,
  .red = 128, .green = 128, .blue = 128, .alpha = 128,
  .first_x = 0x6C00, .first_y = 0x7200, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0x1500,
  .second_x = 0x9400, .second_y = 0x8E00, .second_z = 0xFFFFFFFF,
  .second_u = 0x800, .second_v = 0x1D00 }; /* 0x38D770 the pulsing square the player marker is drawn as */

ScreenQuad D_0038D7B0 = { .tag = 0x4007FC0665322BC0, .unk08 = 0x0000000000000060, .unk10 = 0x156,
  .red = 64, .green = 64, .blue = 64, .alpha = 128,
  .first_x = 0x6C00, .first_y = 0x7200, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0,
  .second_x = 0x9400, .second_y = 0x8E00, .second_z = 0xFFFFFFFF,
  .second_u = 0x1E00, .second_v = 0x1500 }; /* 0x38D7B0 the picture the viewer picked */

ScreenQuad D_0038D7F0 = { .tag = 0x4007FC059D30ABC0, .unk08 = 0x0000000000000060, .unk10 = 0x156,
  .red = 64, .green = 64, .blue = 64, .alpha = 128,
  .first_x = 0x6D00, .first_y = 0x8900, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0,
  .second_x = 0x7500, .second_y = 0x8D00, .second_z = 0xFFFFFFFF,
  .second_u = 0x800, .second_v = 0x400 }; /* 0x38D7F0 one of the picture's arrow rectangles */

ScreenQuad D_0038D830 = { .tag = 0x4007FC059D30ABC0, .unk08 = 0x0000000000000060, .unk10 = 0x156,
  .red = 64, .green = 64, .blue = 64, .alpha = 128,
  .first_x = 0x7000, .first_y = 0x8900, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0,
  .second_x = 0x7800, .second_y = 0x8D00, .second_z = 0xFFFFFFFF,
  .second_u = 0x800, .second_v = 0x400 }; /* 0x38D830 */

ScreenQuad D_0038D870 = { .tag = 0x4007FC055D40ABC0, .unk08 = 0x0000000000000060, .unk10 = 0x156,
  .red = 64, .green = 64, .blue = 64, .alpha = 128,
  .first_x = 0x8B00, .first_y = 0x8B00, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0,
  .second_x = 0x9300, .second_y = 0x8D00, .second_z = 0xFFFFFFFF,
  .second_u = 0x800, .second_v = 0x200 }; /* 0x38D870 */

ScreenQuad D_0038D8B0 = { .tag = 0x4007FC0665322BC0, .unk08 = 0x0000000000000060, .unk10 = 0x156,
  .red = 64, .green = 64, .blue = 64, .alpha = 128,
  .first_x = 0x6C00, .first_y = 0x7200, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0,
  .second_x = 0x9400, .second_y = 0x8E00, .second_z = 0xFFFFFFFF,
  .second_u = 0x2000, .second_v = 0x1800 }; /* 0x38D8B0 one of the picture list's rectangles */

ScreenQuad D_0038D8F0 = { .tag = 0x4007FC05E1312BC0, .unk08 = 0x0000000000000060, .unk10 = 0x156,
  .red = 64, .green = 64, .blue = 64, .alpha = 128,
  .first_x = 0x6D00, .first_y = 0x8700, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0,
  .second_x = 0x7B00, .second_y = 0x8D00, .second_z = 0xFFFFFFFF,
  .second_u = 0x1000, .second_v = 0x800 }; /* 0x38D8F0 */

/* 0x38D930 the light block the viewer's models are lit by: one grey light */
MapSlotUnk10Lights D_0038D930 = {
    { { 0 } },
    { { 0 }, { 0 }, { 0 }, { 0.5f, 0.5f, 0.5f, 0.0f } },
};

/* 0x38D9B0 the 0x40 bytes retail keeps after that block, which nothing reads */
static MapSlotUnk10Half viewer_light_unused = { 0 };

Vector4 D_0038D9F0[4] = { /* 0x38D9F0 the colours a map object's lights are blended towards */
    { 0 }, { 0 }, { 0 }, { 0.5f, 1.0f, 0.5f, 1.0f },
};
