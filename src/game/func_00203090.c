#include "kf_game.h"

extern PoolSlot D_0042A8F0[32];
// match: incomplete arrays, so these tables of eight bytes or less are addressed with lui/addiu, not $gp
extern const PoolSlotList7 D_00446D88[];
extern const PoolSlotList4 D_00446D90[];
extern const PoolSlotList2 D_00446D98[];
extern const PoolSlotList6 D_00446DA0[];
extern const PoolSlotCounts6 D_00440DE0;

/* Gives the slots of one group the file's parts in order; group 5 loses its part instead. */
void func_00203090(PoolPartFile *file, s32 group) {
    PoolSlotList7 group0 = D_00446D88[0];
    PoolSlotList4 group1 = D_00446D90[0];
    PoolSlotList2 group2 = D_00446D98[0];
    PoolSlotList6 group3 = D_00446DA0[0];
    u8 slots15[1] = { 15 };
    u8 slots20[1] = { 20 };
    u8 *groups[6] = { group0.slots, group1.slots, group2.slots, group3.slots, slots15, slots20 };
    PoolSlotCounts6 group_sizes = D_00440DE0;
    PoolPart *part;
    u32 i;

    if (group != 5) {
        part = file->parts;
        for (i = 0; i < group_sizes.counts[group]; i++) {
            D_0042A8F0[groups[group][i]].part = part;
            part++;
        }
    } else {
        for (i = 0; i < group_sizes.counts[group]; i++) {
            D_0042A8F0[groups[group][i]].part = NULL;
        }
    }
}
