#include "common.h"
#include "kf_effect.h"
#include "kf_map.h"

extern MapSlot D_0038CD20[2];
extern MapObject D_003A1A30[][0x200];
extern Entity D_003BEDF0[][0x80];
extern EffectSlot D_003F1750[0x60];
extern EffectActor D_003F3550[0x180];
extern int D_004468CC;
extern int D_004468D0;

/* Frees the spawner slots and effects owned by anything of map slot map (its entities, its
   map objects, the slot itself) and makes the other effects forget what they knew of it. */
void func_001A3040(int map) {
    MapSlot *map_slot = &D_0038CD20[map];
    char *first_record = map_slot->records;
    char *last_record = first_record + map_slot->header->record_count * 0x80;
    Entity *first_entity = D_003BEDF0[map];
    Entity *last_entity = &D_003BEDF0[map][0x7F];
    MapObject *first_object = D_003A1A30[map];
    MapObject *last_object = &D_003A1A30[map][0x1FF];
    EffectActor *effect = D_003F3550;
    EffectSlot *slot = D_003F1750;
    Entity *owner;
    int i;
    int effects_left;

    for (i = 0; i < 0x60 && D_004468CC > 0; i++, slot++) {
        if (slot->flags & EFFECT_SLOT_LIVE) {
            owner = slot->owner_entity;
            if ((owner >= first_entity && owner <= last_entity) ||
                ((MapObject *)owner >= first_object && (MapObject *)owner <= last_object) || owner == (Entity *)map_slot) {
                func_001A2AF8(slot);
            }
        }
    }
    effects_left = D_004468D0;
    for (i = 0; i < 0x180 && effects_left > 0; i++, effect++) {
        if (effect->flags & EFFECT_FLAG_LIVE) {
            owner = effect->owner_entity;
            if ((owner >= first_entity && owner <= last_entity) ||
                ((MapObject *)owner >= first_object && (MapObject *)owner <= last_object) || owner == (Entity *)map_slot) {
                func_001A2A18(effect);
                effects_left = D_004468D0;
            }
        }
    }
    effect = D_003F3550;
    for (i = 0; i < 0x180 && effects_left > 0; i++, effect++) {
        if (effect->flags & EFFECT_FLAG_LIVE) {
            if (effect->unk6C != 0 && ((Entity *)effect->unk6C)->group_id == map) {
                effect->unk6C = 0;
            }
            // match: last_record is one past the last record of the map, and the test still includes it
            if (effect->unk58 != NULL && effect->unk58 >= first_record && effect->unk58 <= last_record) {
                effect->unk58 = NULL;
            }
        }
    }
}
