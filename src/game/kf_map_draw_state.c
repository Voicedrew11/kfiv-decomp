/* The haze layers the maps are drawn with, and the map draw state. */
#include "kf_map.h"

/* 0x38A158 the distances of the haze layers, nearest last */
u32 haze_layer_distances[8] = { 0x800, 0x600, 0x400, 0, 0, 0, 0, 0 };

/* 0x38A178 the screen depth of each haze layer, written as they are projected */
int haze_layer_depths[8] = { 0 };

/* 0x38A198 six words the game's code never refers to */
int D_0038A198[6] = { 0x20, 0x18, 0x10, 0x8, 0x4, 0 };

/* 0x38A1B0 the map draw state (MapDrawState): this unit's objects from here on are its fields */
MapDrawStateMirror map_draw_state = { 0 };

/* 0x38A218 MapDrawState.hud_shown on: the HUD is drawn (kf_hud.c) */
s32 D_0038A218[0xE] = { 0 };

/* 0x38A250 MapDrawState.unkA0: the current map's first block (kf_held_weapon.c) */
MapSlotUnk0C D_0038A250 = { { 0 } };

/* 0x38A270 MapDrawState.unkC0: the block as the eight vectors it holds, in two halves */
MapSlotUnk10Half D_0038A270 = { { 0 } };
MapSlotUnk10Half D_0038A2B0 = { { 0 } };

/* 0x38A2F0 MapDrawState.pad140 */
u8 D_0038A2F0[0x140] = { 0 };

/* 0x38A430 MapDrawState.unk280: the current map's fog colour */
MapFogColor D_0038A430 = { 0 };

/* 0x38A440 MapDrawState.fog_blocks: each map slot's 0x20 blocks (a model's unk14C points at one) */
MapSlotUnk0C map_draw_slot_blocks[2][0x20] = { 0 };

/* 0x38AC40 MapDrawState.light_blocks: each map slot's 0x20 0x80-byte blocks (a model's unk148) */
MapSlotUnk10Words map_draw_slot_draw_blocks[2][0x20] = { 0 };

/* 0x38CC40 MapDrawState.screen_tint_amount to unk2A98: the screen tint, 0 to 0.25, and its padding */
f32 map_draw_screen_tint[4] = { 0 };
