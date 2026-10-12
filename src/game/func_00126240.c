#include "kf_entity.h"
#include "kf_map.h"
#include "kf_collision.h"

extern MapSlot D_0038CD20[];
extern int D_00446770;
extern u8 D_004141D0[];

/* Whether the map record the entity belongs to (Entity.unk14) is switched on: FALSE when the record
   is flagged off or its map flag is clear. The record's map flag is a bit of the current map's
   flag block; for a record of the other map it is looked up among the 16 flags the block keeps
   for other maps. TRUE for an entity without a record and for types 0x10, 0x2F, 0x53 and 0x9E. */
int func_00126240(Entity *entity) {
    MapRecordBounds *record;
    MapFlagBlock *block;
    MapSlot *slot;
    u8 *bits;
    int found;
    int index; // match: the map slot of the record, then reused as the counter of the last loop
    int record_index;
    int result;
    u8 tag;

    if (entity->unk14 == NULL) {
        return TRUE;
    }
    if (entity->spawn_info->type_id == 0x10 || entity->spawn_info->type_id == 0x2F ||
        entity->spawn_info->type_id == 0x53 || entity->spawn_info->type_id == 0x9E) {
        return TRUE;
    }
    found = FALSE;
    for (index = 0; index < 2; index++) {
        record = (MapRecordBounds *)D_0038CD20[index].records;
        for (record_index = 0; record_index < D_0038CD20[index].header->record_count; record_index++, record++) {
            if (record == entity->unk14) {
                found = TRUE;
                break;
            }
        }
        if (found) {
            break;
        }
    }
    if (!found) {
        return TRUE;
    }
    if (record->unk3C & 0x40000000) {
        return FALSE;
    }
    if (index == D_00446770) {
        slot = &D_0038CD20[index];
        bits = slot->flag_blocks[D_004141D0[0]].bits;
        if (slot->unk00 == 0) {
            return FALSE;
        }
        return (bits[record->unk38 >> 3] >> (record->unk38 & 7)) & 1;
    }
    slot = &D_0038CD20[D_00446770];
    block = &slot->flag_blocks[D_004141D0[0]];
    slot = &D_0038CD20[D_00446770 ^ 1];
    if (slot->unk00 == 0) {
        return FALSE;
    }
    tag = slot->tag;
    result = FALSE;
    for (index = 0; index < 0x10; index++) {
        if (tag == block->tags[index] && record->unk38 == block->values[index]) {
            result = TRUE;
        }
    }
    return result;
}
