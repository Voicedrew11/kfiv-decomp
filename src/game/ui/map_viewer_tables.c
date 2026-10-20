/* Map viewer entries, screen rectangles, lights and blend colours. */
#include "core/game.h"
#include "map/map.h"
#include "gfx/packet.h"
#include "ui/map_viewer.h"

/* 0x38D6B0 one entry per viewer, indexed by picked_viewer */
ViewerEntry viewer_entries[5] = {
    { MapViewer_RunPictureMap, MapViewer_LocateOnCaretakersMap, 2 },
    { MapViewer_RunPictureMap, MapViewer_LocateOnZastarisMap, 3 },
    { MapViewer_RunPictureMap, MapViewer_LocateOnEarthFolksMap, 1 },
    { MapViewer_RunPictureMap, MapViewer_LocateOnWiddasMap, 1 },
    { MapViewer_RunKingsMap, 0, 0 },
};

/* 0x38D6F0 two unused screen rectangles */
static ScreenQuad viewer_screen_rects[2] __attribute__((aligned(16))) = {
            { .tag = 0x0000000000000000, .gif_register_list = 0x0000000000000000, .prim = 0x46,
          .red = 255, .green = 0, .blue = 0, .alpha = 128,
          .first_x = 0x6C00, .first_y = 0x7200, .first_z = 0xFFFFFFFF,
          .first_u = 0, .first_v = 0,
          .second_x = 0x9400, .second_y = 0x8E00, .second_z = 0xFFFFFFFF,
          .second_u = 0x1E00, .second_v = 0x1500 },
            { .tag = 0x4007FC0599306BC0, .gif_register_list = 0x0000000000000060, .prim = 0x156,
          .red = 128, .green = 128, .blue = 128, .alpha = 128,
          .first_x = 0x6C00, .first_y = 0x7200, .first_z = 0xFFFFFFFF,
          .first_u = 0, .first_v = 0,
          .second_x = 0x7400, .second_y = 0x7A00, .second_z = 0xFFFFFFFF,
          .second_u = 0x400, .second_v = 0x400 },
};

ScreenQuad viewer_marker_rect = { .tag = 0x4007FC0665322BC0, .gif_register_list = 0x0000000000000060, .prim = 0x156,
  .red = 128, .green = 128, .blue = 128, .alpha = 128,
  .first_x = 0x6C00, .first_y = 0x7200, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0x1500,
  .second_x = 0x9400, .second_y = 0x8E00, .second_z = 0xFFFFFFFF,
  .second_u = 0x800, .second_v = 0x1D00 }; /* 0x38D770 the pulsing square the player marker is drawn as */

ScreenQuad viewer_picture_rect = { .tag = 0x4007FC0665322BC0, .gif_register_list = 0x0000000000000060, .prim = 0x156,
  .red = 64, .green = 64, .blue = 64, .alpha = 128,
  .first_x = 0x6C00, .first_y = 0x7200, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0,
  .second_x = 0x9400, .second_y = 0x8E00, .second_z = 0xFFFFFFFF,
  .second_u = 0x1E00, .second_v = 0x1500 }; /* 0x38D7B0 the picture the viewer picked */

ScreenQuad viewer_arrow_rect_1 = { .tag = 0x4007FC059D30ABC0, .gif_register_list = 0x0000000000000060, .prim = 0x156,
  .red = 64, .green = 64, .blue = 64, .alpha = 128,
  .first_x = 0x6D00, .first_y = 0x8900, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0,
  .second_x = 0x7500, .second_y = 0x8D00, .second_z = 0xFFFFFFFF,
  .second_u = 0x800, .second_v = 0x400 }; /* 0x38D7F0 one of the picture's arrow rectangles */

ScreenQuad viewer_arrow_rect_2 = { .tag = 0x4007FC059D30ABC0, .gif_register_list = 0x0000000000000060, .prim = 0x156,
  .red = 64, .green = 64, .blue = 64, .alpha = 128,
  .first_x = 0x7000, .first_y = 0x8900, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0,
  .second_x = 0x7800, .second_y = 0x8D00, .second_z = 0xFFFFFFFF,
  .second_u = 0x800, .second_v = 0x400 }; /* 0x38D830 */

ScreenQuad viewer_picture_label_rect = { .tag = 0x4007FC055D40ABC0, .gif_register_list = 0x0000000000000060, .prim = 0x156,
  .red = 64, .green = 64, .blue = 64, .alpha = 128,
  .first_x = 0x8B00, .first_y = 0x8B00, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0,
  .second_x = 0x9300, .second_y = 0x8D00, .second_z = 0xFFFFFFFF,
  .second_u = 0x800, .second_v = 0x200 }; /* 0x38D870 the small picture of the page shown (texture pages + 1 + page) */

ScreenQuad kings_map_overlay_rect = { .tag = 0x4007FC0665322BC0, .gif_register_list = 0x0000000000000060, .prim = 0x156,
  .red = 64, .green = 64, .blue = 64, .alpha = 128,
  .first_x = 0x6C00, .first_y = 0x7200, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0,
  .second_x = 0x9400, .second_y = 0x8E00, .second_z = 0xFFFFFFFF,
  .second_u = 0x2000, .second_v = 0x1800 }; /* 0x38D8B0 the King's Map's full-screen overlay (textures 0 and 1) */

ScreenQuad kings_map_corner_rect = { .tag = 0x4007FC05E1312BC0, .gif_register_list = 0x0000000000000060, .prim = 0x156,
  .red = 64, .green = 64, .blue = 64, .alpha = 128,
  .first_x = 0x6D00, .first_y = 0x8700, .first_z = 0xFFFFFFFF,
  .first_u = 0, .first_v = 0,
  .second_x = 0x7B00, .second_y = 0x8D00, .second_z = 0xFFFFFFFF,
  .second_u = 0x1000, .second_v = 0x800 }; /* 0x38D8F0 the King's Map's bottom-left panel (textures 2 and 3) */

/* 0x38D930 the light block the viewer's models are lit by: one grey light */
MapLightBlockLights viewer_default_lights = {
    { { 0 } },
    { { 0 }, { 0 }, { 0 }, { 0.5f, 0.5f, 0.5f, 0.0f } },
};

/* 0x38D9B0 the 0x40 bytes retail keeps after that block, which nothing reads */
static MapLightBlockHalf viewer_light_unused = { 0 };

Vector4 map_object_glow_colors[4] = { /* 0x38D9F0 the colours a map object's lights are blended towards */
    { 0 }, { 0 }, { 0 }, { 0.5f, 1.0f, 0.5f, 1.0f },
};
