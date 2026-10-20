#include "map/map_scroll.h"
#include "gfx/gs.h"
#include "map/map.h"
#include "gfx/packet.h"
#include "enemy/enemy.h"
#include "gfx/gs_draw.h"

static MapSlotPair map_scroll_copy_pairs[2][16]; /* 0x471F60 */
static MapSlotPair map_scroll_draw_pairs[2][16];  /* 0x471FE0 */

u32 *MapScroll_AppendBitblt(u32 *packet, int end_of_packet, int format, int buffer_width, int width, int height,
                               int source_base, int source_x, int source_y, int dest_base, int dest_x, int dest_y);

/* Clears the copy and draw scroll offsets for one map slot. */
void MapScroll_ClearPairs(int map_slot) {
    int i;
    s16 *p; // match: retail local name

    p = (s16 *)(map_slot * 64 + (char *)map_scroll_copy_pairs);
    i = 0xF;
    do {
        p[0] = 0;
        i--;
        p[1] = 0;
        p += 2;
    } while (i >= 0);
    p = (s16 *)(map_slot * 64 + (char *)map_scroll_draw_pairs);
    i = 0xF;
    do {
        p[0] = 0;
        i--;
        p[1] = 0;
        p += 2;
    } while (i >= 0);
}

void MapScroll_PrepareCopy(void) {
}

/* Scrolls each of a map's moving textures of this mode in place: moves the pair that goes with it along,
   then copies the texture to the spare buffer at 0x3C40 and back in two pieces, shifted by the pair. */
void MapScroll_UpdateCopy(int map, int mode) {
    MapScrollCopy *texture;
    MapSlotPair *pair = map_scroll_copy_pairs[map];
    MapSlot *slot = &map_slots[map];
    int base;
    int format;
    int width;
    int height;
    int buffer_width;
    int shift_x;
    int shift_y;
    int count;
    u32 *packet_start;
    int x_is_last;
    int i;
    int spare_base;
    u32 *packet;

    texture = (MapScrollCopy *)slot->scroll_copy_textures;
    if (slot->loaded != 0) {
        for (i = 15; i >= 0; i--, texture++, pair++) {
            if (texture->base != 0 && texture->mode == mode &&
                (texture->period == 0 || frame_counter % texture->period == 0)) {
                width = texture->width;
                base = texture->base;
                format = texture->format;
                height = texture->height;
                buffer_width = width / 64;
                pair->scroll_x += (s8)texture->step_x;
                pair->scroll_y += (s8)texture->step_y;
                if (pair->scroll_x < 0) {
                    pair->scroll_x += width;
                }
                if (pair->scroll_x >= width) {
                    pair->scroll_x -= width;
                }
                if (pair->scroll_y < 0) {
                    pair->scroll_y += height;
                }
                if (pair->scroll_y >= height) {
                    pair->scroll_y -= height;
                }
                shift_x = pair->scroll_x;
                shift_y = pair->scroll_y;
                if (shift_x != 0 || shift_y != 0) {
                    count = 0;
                    if (shift_x != 0) {
                        count = 0x12;
                    }
                    if (shift_y != 0) {
                        count += 0x12;
                    }
                    spare_base = 0x3C40;
                    x_is_last = shift_y == 0;
                    packet_start = packet = Packet_Alloc(count + 3);
                    *packet++ = count | 0x10000001;
                    *packet++ = 0;
                    *packet++ = 0;
                    *packet++ = 0;
                    *packet++ = 0;
                    *packet++ = 0;
                    *packet++ = 0;
                    *packet++ = count | 0x51000000;
                    if (shift_x > 0) {
                        packet = MapScroll_AppendBitblt(packet, FALSE, format, buffer_width, width, height, base, 0, 0,
                                                          spare_base, 0, 0);
                        packet = MapScroll_AppendBitblt(packet, FALSE, format, buffer_width, width - shift_x, height,
                                                          spare_base, 0, 0, base, shift_x, 0);
                        packet = MapScroll_AppendBitblt(packet, x_is_last, format, buffer_width, shift_x, height,
                                                          spare_base, width - shift_x, 0, base, 0, 0);
                    } else if (shift_x < 0) {
                        packet = MapScroll_AppendBitblt(packet, FALSE, format, buffer_width, width, height, base, 0, 0,
                                                          spare_base, 0, 0);
                        packet = MapScroll_AppendBitblt(packet, FALSE, format, buffer_width, width + shift_x, height,
                                                          spare_base, -shift_x, 0, base, 0, 0);
                        packet = MapScroll_AppendBitblt(packet, x_is_last, format, buffer_width, -shift_x, height,
                                                          spare_base, 0, 0, base, width + shift_x, 0);
                    }
                    if (shift_y > 0) {
                        packet = MapScroll_AppendBitblt(packet, FALSE, format, buffer_width, width, height, base, 0, 0,
                                                          spare_base, 0, 0);
                        packet = MapScroll_AppendBitblt(packet, FALSE, format, buffer_width, width, height - shift_y,
                                                          spare_base, 0, 0, base, 0, shift_y);
                        packet = MapScroll_AppendBitblt(packet, TRUE, format, buffer_width, width, shift_y, spare_base, 0,
                                                          height - shift_y, base, 0, 0);
                    } else if (shift_y < 0) {
                        packet = MapScroll_AppendBitblt(packet, FALSE, format, buffer_width, width, height, base, 0, 0,
                                                          spare_base, 0, 0);
                        packet = MapScroll_AppendBitblt(packet, FALSE, format, buffer_width, width, height + shift_y,
                                                          spare_base, 0, -shift_y, base, 0, 0);
                        packet = MapScroll_AppendBitblt(packet, TRUE, format, buffer_width, width, -shift_y, spare_base,
                                                          0, 0, base, 0, height + shift_y);
                    }
                    Packet_AppendToChain(packet_start, packet);
                }
            }
        }
    }
}

/* Writes a GS packet that copies a rectangle from one buffer to another (BITBLTBUF, TRXPOS, TRXREG, TRXDIR,
   TEXFLUSH) and returns where the next packet goes. */
u32 *MapScroll_AppendBitblt(u32 *packet, int end_of_packet, int format, int buffer_width, int width, int height,
                               int source_base, int source_x, int source_y, int dest_base, int dest_x, int dest_y) {
    *((u64 *)packet)++ = ((s64)end_of_packet << 15) | 0x1000000000000005L;
    *((u64 *)packet)++ = 0xE;
    *((u64 *)packet)++ = (s64)source_base | ((s64)buffer_width << 16) | ((s64)format << 24) |
                         ((s64)dest_base << 32) | ((s64)buffer_width << 48) | ((s64)format << 56);
    *packet++ = 0x50;
    *packet++ = 0;
    *((u64 *)packet)++ = (s64)source_x | ((s64)source_y << 16) | ((s64)dest_x << 32) | ((s64)dest_y << 48);
    *packet++ = 0x51;
    *packet++ = 0;
    *((u64 *)packet)++ = (s64)width | ((s64)height << 32);
    *packet++ = 0x52;
    *packet++ = 0;
    *((u64 *)packet)++ = 2;
    *packet++ = 0x53;
    *packet++ = 0;
    *((u64 *)packet)++ = 0xFFFFFFFF;
    *packet++ = 0x3F;
    *packet++ = 0;
    return packet;
}

void MapScroll_PrepareDraw(void) {
}

/* Draws each of a map's moving textures of this mode into its target texture, shifted by the pair that
   goes with it, and moves the pair along. */
void MapScroll_UpdateDraw(int map, int mode) {
    ScreenQuad quad;
    MapSlot *slot = &map_slots[map];
    MapSlotPair *pair = map_scroll_draw_pairs[map];
    MapScrollTexture *texture;
    int i;
    u32 width;
    u32 height;
    u32 size;
    u32 width_log;
    u32 height_log;
    int half;

    texture = (MapScrollTexture *)slot->scroll_draw_textures;
    if (slot->loaded != 0) {
        for (i = 0; i < 16; i++, texture++, pair++) {
            if (texture->target_base != 0 && texture->mode == mode) {
                width = texture->width << 4;
                height = texture->height << 4;
                if (texture->period == 0 || frame_counter % texture->period == 0) {
                    if (texture->step_x != 0) {
                        pair->scroll_x += texture->step_x;
                        if (pair->scroll_x < 0) {
                            pair->scroll_x += width;
                        }
                        if (pair->scroll_x >= width) {
                            pair->scroll_x -= width;
                        }
                    }
                    if (texture->step_y != 0) {
                        pair->scroll_y += texture->step_y;
                        if (pair->scroll_y < 0) {
                            pair->scroll_y += height;
                        }
                        if (pair->scroll_y >= height) {
                            pair->scroll_y -= height;
                        }
                    }
                }
                for (width_log = 0, size = texture->width; size >= 2; size >>= 1) {
                    width_log++;
                }
                for (height_log = 0, size = texture->height; size >= 2; size >>= 1) {
                    height_log++;
                }
                GsDraw_SetDrawTarget(((s64)texture->target_base >> 5) | (((s64)texture->target_width / 64) << 16) |
                                  ((s64)texture->target_format << 24),
                              ((s64)(texture->target_width - 1) << 16) | ((s64)(texture->target_height - 1) << 48),
                              ((s64)(0x800 - (texture->target_width >> 1)) << 4) |
                                  ((s64)(0x800 - (texture->target_height >> 1)) << 36));
                Packet_BeginGsRegisters();
                // match: A is read from the byte, the others from the word
                Packet_AddGsRegister(GS_REG_ALPHA_1,
                              (s64)(texture->blend & MAP_SCROLL_BLEND_A_MASK) | ((s64)MAP_SCROLL_BLEND_BITS(texture)->b << 2) |
                                  ((s64)MAP_SCROLL_BLEND_BITS(texture)->c << 4) |
                                  ((s64)MAP_SCROLL_BLEND_BITS(texture)->d << 6) | ((s64)texture->blend_fix << 32));
                Packet_AddGsRegister(GS_REG_TEST_1, 0x51001);
                Packet_EndGsRegisters();
                GsDraw_QueueFlush();
                quad.tag = (s64)texture->base | ((s64)texture->width >> 6 << 14) | ((s64)texture->format << 20) |
                           ((s64)width_log << 26) | ((s64)height_log << 30) | (1L << 34) |
                           ((s64)texture->clut_base << 37) | ((s64)texture->clut_format << 51) | (4L << 61);
                quad.gif_register_list = 0x60;
                quad.prim = 0x156;
                half = texture->target_width >> 1;
                quad.red = texture->red;
                quad.green = texture->green;
                quad.blue = texture->blue;
                quad.alpha = texture->alpha;
                quad.first_y = quad.first_x = (0x800 - half) << 4;
                quad.first_z = 0xFFFFFFFF;
                quad.first_u = 0;
                quad.first_v = 0;
                quad.second_y = quad.second_x = (half << 4) + 0x7FF8;
                quad.second_z = 0xFFFFFFFF;
                quad.second_u = width * texture->scale_x;
                quad.second_v = height * texture->scale_y;
                quad.first_u += pair->scroll_x;
                quad.first_v += pair->scroll_y;
                quad.second_u += pair->scroll_x;
                quad.second_v += pair->scroll_y;
                GsDraw_QueueTexturedSprite(&quad);
                GsDraw_QueueFlush();
                GsDraw_QueueTexFlush();
            }
        }
        GsDraw_QueueTexFlush();
        GsDraw_ResetDrawTarget();
    }
}

/* Carries pairs over from one map slot to the other for the maps that share them. */
void MapScroll_CarryPairs(int from_slot, int to_slot) {
    int from_tag = map_slots[from_slot].tag;
    int to_tag = map_slots[to_slot].tag;

    if ((from_tag == 4 && to_tag == 0x63) || (from_tag == 0x63 && to_tag == 4) ||
        (from_tag == 0x62 && to_tag == 0x63) || (from_tag == 0x63 && to_tag == 0x62)) {
        map_scroll_draw_pairs[to_slot][0] = map_scroll_draw_pairs[from_slot][0];
        map_scroll_draw_pairs[to_slot][1] = map_scroll_draw_pairs[from_slot][1];
        map_scroll_draw_pairs[to_slot][2] = map_scroll_draw_pairs[from_slot][2];
    // match: the same copy again in its own branch; one test for all six pairs orders the code differently
    } else if ((from_tag == 0x15 && to_tag == 0x1B) || (from_tag == 0x1B && to_tag == 0x15)) {
        map_scroll_draw_pairs[to_slot][0] = map_scroll_draw_pairs[from_slot][0];
        map_scroll_draw_pairs[to_slot][1] = map_scroll_draw_pairs[from_slot][1];
        map_scroll_draw_pairs[to_slot][2] = map_scroll_draw_pairs[from_slot][2];
    } else if (from_tag == 0x1A && to_tag == 0x1B) {
        map_scroll_draw_pairs[to_slot][4] = map_scroll_draw_pairs[from_slot][5];
        map_scroll_draw_pairs[to_slot][5] = map_scroll_draw_pairs[from_slot][6];
        map_scroll_draw_pairs[to_slot][6] = map_scroll_draw_pairs[from_slot][7];
        map_scroll_draw_pairs[to_slot][7] = map_scroll_draw_pairs[from_slot][8];
    } else if (from_tag == 0x1B && to_tag == 0x1A) {
        map_scroll_draw_pairs[to_slot][5] = map_scroll_draw_pairs[from_slot][4];
        map_scroll_draw_pairs[to_slot][6] = map_scroll_draw_pairs[from_slot][5];
        map_scroll_draw_pairs[to_slot][7] = map_scroll_draw_pairs[from_slot][6];
        map_scroll_draw_pairs[to_slot][8] = map_scroll_draw_pairs[from_slot][7];
    }
}
