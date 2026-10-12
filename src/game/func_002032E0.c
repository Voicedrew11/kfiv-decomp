#include "kf_game.h"
#include "kf_load.h"
#include "kf_texture.h"

extern PoolSlot D_0042A8F0[32];
// match: incomplete arrays, so these tables of eight bytes or less are addressed with lui/addiu, not $gp
extern const PoolSlotList7 D_00446D88[];
extern const PoolSlotList4 D_00446D90[];
extern const PoolSlotList2 D_00446D98[];
extern const PoolSlotList6 D_00446DA0[];
extern const PoolSlotList2 D_00446DA8[];
extern const PoolSlotCounts10 D_00440DF8;
extern const PoolSlotValues11 D_00440E20;
extern const PoolSlotValues11 D_00440E50;

void func_00204180(TextureHeader *, LoadedBlockRef *);

/* Uploads a group's texture to its place in GS memory and gives the group's slots the file's parts. */
void func_002032E0(PoolPartSource *source, u32 group) {
    PoolSlotList7 group0 = D_00446D88[0];
    PoolSlotList4 group1 = D_00446D90[0];
    PoolSlotList2 group2 = D_00446D98[0];
    PoolSlotList6 group3 = D_00446DA0[0];
    PoolSlotList2 group4 = D_00446DA8[0];
    u8 slots20[1] = { 20 };
    u8 slots21[1] = { 21 };
    u8 slots22[1] = { 22 };
    u8 slots23[1] = { 23 };
    u8 *groups[9] = {
        group0.slots, group1.slots, group2.slots, group3.slots, group4.slots, slots20, slots21, slots22, slots23
    };
    PoolSlotCounts10 group_sizes = D_00440DF8;
    PoolPartFile *file = source->file;
    TextureHeader *texture = source->texture;
    PoolSlotValues11 texture_pages = D_00440E20;
    PoolSlotValues11 clut_pages = D_00440E50;
    PoolPart *part;
    PoolPartRecord *record;
    u8 *slots;
    u32 i;
    u32 remaining;

    TEXTURE_HEADER_BITS(texture)->tex0.f.tbp0 = texture_pages.values[group];
    TEXTURE_HEADER_BITS(texture)->tex0.f.cbp = clut_pages.values[group];
    func_00204180(texture, NULL);
    if (group < 6) {
        if (group == 4) {
            group_sizes.counts[4] = file->part_count;
        }
        part = file->parts;
        for (i = 0; i < group_sizes.counts[group]; i++) {
            slots = groups[group];
            remaining = group_sizes.counts[group] - i;
            D_0042A8F0[slots[i]].part = part;
            // match: tex0 and unk18 go across as one 16-byte struct
            POOL_SLOT_REGISTERS(&D_0042A8F0[slots[i]]) = *(PoolSlotRegisters *)&texture->tex0;
            record = (PoolPartRecord *)((char *)file + (s32)file->records_end) - remaining;
            if (record->unk00 > 0) {
                D_0042A8F0[slots[i]].unk48 = record;
                D_0042A8F0[slots[i]].unk40 = &D_0042A8F0[slots[record->unk08]];
            } else {
                D_0042A8F0[slots[i]].unk48 = NULL;
                D_0042A8F0[slots[i]].unk40 = NULL;
            }
            part++;
        }
    } else {
        part = file->parts;
        D_0042A8F0[groups[group][0]].part = part;
        POOL_SLOT_REGISTERS(&D_0042A8F0[groups[group][0]]) = *(PoolSlotRegisters *)&texture->tex0; // match: as above
        D_0042A8F0[groups[group][0]].unk48 = NULL;
        D_0042A8F0[groups[group][0]].unk40 = NULL;
    }
}
