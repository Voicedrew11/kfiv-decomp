#include "kf_map.h"

extern MapSlot D_0038CD20[];
extern u32 D_00446ED4;
extern MapSlotPair D_00471F60[2][16];

void func_00109650(u32 *, u32 *);
u32 *func_00109990(int);
u32 *func_001174D0(u32 *, int, int, int, int, int, int, int, int, int, int, int);

/* Scrolls each of a map's moving textures of this mode in place: moves the pair that goes with it along,
   then copies the texture to the spare buffer at 0x3C40 and back in two pieces, shifted by the pair. */
void func_00116F98(int map, int mode) {
    MapScrollCopy *texture;
    MapSlotPair *pair = D_00471F60[map];
    MapSlot *slot = &D_0038CD20[map];
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

    texture = (MapScrollCopy *)slot->unk38;
    if (slot->unk00 != 0) {
        for (i = 15; i >= 0; i--, texture++, pair++) {
            if (texture->base != 0 && texture->mode == mode &&
                (texture->period == 0 || D_00446ED4 % texture->period == 0)) {
                width = texture->width;
                base = texture->base;
                format = texture->format;
                height = texture->height;
                buffer_width = width / 64;
                pair->unk00 += (s8)texture->step_x;
                pair->unk02 += (s8)texture->step_y;
                if (pair->unk00 < 0) {
                    pair->unk00 += width;
                }
                if (pair->unk00 >= width) {
                    pair->unk00 -= width;
                }
                if (pair->unk02 < 0) {
                    pair->unk02 += height;
                }
                if (pair->unk02 >= height) {
                    pair->unk02 -= height;
                }
                shift_x = pair->unk00;
                shift_y = pair->unk02;
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
                    packet_start = packet = func_00109990(count + 3);
                    *packet++ = count | 0x10000001;
                    *packet++ = 0;
                    *packet++ = 0;
                    *packet++ = 0;
                    *packet++ = 0;
                    *packet++ = 0;
                    *packet++ = 0;
                    *packet++ = count | 0x51000000;
                    if (shift_x > 0) {
                        packet = func_001174D0(packet, FALSE, format, buffer_width, width, height, base, 0, 0, spare_base,
                                               0, 0);
                        packet = func_001174D0(packet, FALSE, format, buffer_width, width - shift_x, height, spare_base,
                                               0, 0, base, shift_x, 0);
                        packet = func_001174D0(packet, x_is_last, format, buffer_width, shift_x, height, spare_base,
                                               width - shift_x, 0, base, 0, 0);
                    } else if (shift_x < 0) {
                        packet = func_001174D0(packet, FALSE, format, buffer_width, width, height, base, 0, 0, spare_base,
                                               0, 0);
                        packet = func_001174D0(packet, FALSE, format, buffer_width, width + shift_x, height, spare_base,
                                               -shift_x, 0, base, 0, 0);
                        packet = func_001174D0(packet, x_is_last, format, buffer_width, -shift_x, height, spare_base,
                                               0, 0, base, width + shift_x, 0);
                    }
                    if (shift_y > 0) {
                        packet = func_001174D0(packet, FALSE, format, buffer_width, width, height, base, 0, 0, spare_base,
                                               0, 0);
                        packet = func_001174D0(packet, FALSE, format, buffer_width, width, height - shift_y, spare_base,
                                               0, 0, base, 0, shift_y);
                        packet = func_001174D0(packet, TRUE, format, buffer_width, width, shift_y, spare_base, 0,
                                               height - shift_y, base, 0, 0);
                    } else if (shift_y < 0) {
                        packet = func_001174D0(packet, FALSE, format, buffer_width, width, height, base, 0, 0, spare_base,
                                               0, 0);
                        packet = func_001174D0(packet, FALSE, format, buffer_width, width, height + shift_y, spare_base,
                                               0, -shift_y, base, 0, 0);
                        packet = func_001174D0(packet, TRUE, format, buffer_width, width, -shift_y, spare_base, 0, 0, base,
                                               0, height + shift_y);
                    }
                    func_00109650(packet_start, packet);
                }
            }
        }
    }
}
