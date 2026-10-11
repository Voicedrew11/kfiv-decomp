#include "kf_entity.h"

int func_0019A538(void *, void *, int);
s32 func_00191600(Entity *, EntityUnkE0SlotBits *, s32, s32);
void func_0018D670(Entity *, s32, s32, s32);
void func_0018D8B8(Entity *, s32, s32, s32);
int func_001960C0(void *, int, int, int, int, float);
void func_00196120(float, void *, void *, int, void *);

void func_0017F998(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    f32 limits[4];
    f32 unused_vector[4]; // match: the frame has 16 unused bytes
    s32 previous;
    f32 rate;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (!ENTITY_FLAG_BITS(entity)->bit14) {
        return;
    }
    if (slots->unk80.unk01 != 0) {
        if (entity->unkC6 & 0x12) {
            rate = unkE0->unk40;
        } else {
            rate = 1.0f / slots->unk80.unk01;
        }
        // match: the caller passes a float the callee does not take
        ENTITY_FLAG_BITS(entity)->bit34 = ((int (*)(void *, void *, int, float))func_0019A538)(entity, &slots->unk80, 0, rate);
    } else {
        previous = slots->unk80.unk04;
        ENTITY_FLAG_BITS(entity)->bit34 = func_00191600(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, ENTITY_FLAG_BITS(entity)->bits11);
        if (slots->unk80.index != 3) {
            func_0018D670(entity, slots->unk80.index, previous, slots->unk80.unk04);
            func_0018D8B8(entity, slots->unk80.index, previous, slots->unk80.unk04);
        }
    }
    if (slots->unk88.unk01 != 0) {
        if (entity->unkC6 & 0x12) {
            rate = unkE0->unk40;
        } else {
            rate = 1.0f / slots->unk88.unk01;
        }
        // match: the caller passes a float the callee does not take
        ((int (*)(void *, void *, int, float))func_0019A538)(entity, &slots->unk88, 1, rate);
    } else {
        previous = slots->unk88.unk04;
        func_00191600(entity, &slots->unk88, 1, ENTITY_FLAG_BITS(entity)->bits11);
        if (slots->unk88.index == 3) {
            func_0018D670(entity, slots->unk88.index, previous, slots->unk88.unk04);
            func_0018D8B8(entity, slots->unk88.index, previous, slots->unk88.unk04);
        }
    }
    limits[1] = entity->params->unk430 * 0.017453292f;
    limits[0] = -entity->params->unk430 * 0.017453292f;
    limits[3] = entity->params->unk432 * 0.017453292f;
    limits[2] = -entity->params->unk432 * 0.017453292f;
    if (ENTITY_FLAG_BITS(entity)->bit9 && entity->unk5C != 0) {
        func_001960C0(entity, (int)unkE0->unk20, ENTITY_FLAG_BITS(entity)->bit9, entity->unk5C, (int)limits, 12.0f);
    } else {
        func_00196120(8.0f, entity, unkE0->unk20, ENTITY_FLAG_BITS(entity)->bit9, NULL);
    }
}
