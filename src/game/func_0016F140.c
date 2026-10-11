#include "kf_entity.h"

extern u32 D_00446ED4;
extern MapSlot D_0038CD20[];

void func_00196870(void *, int);
void func_0018E9C0(void *);
int func_00182FD8(void *, int);
void func_00183240(Entity *, s32);
void func_0018D600(char **, int, u32, int);
void func_001958B8(char *, void (*)(void), int);
void func_0016F5E8(Entity *);

void func_0016F140(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapSlotUnk10 *record;
    f32 scale;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        func_0018E9C0(entity);
        switch (entity->unkCC) {
        case 1:
        case 2:
            func_00183240(entity, unknown);
            break;
        case 0:
        default:
            func_00182FD8(entity, unknown);
            break;
        }
    }
    if (D_00446ED4 % 20U == 0) {
        func_0018D600((char **)entity, entity->params->unk43A, 0xFF, (int)entity->model->unkD0);
    }
    if (unkE0->unk6A != 0) {
        unkE0->unk6A--;
        scale = (f32)unkE0->unk6A / 1200.0f + 0.5f;
        entity->model->unkE0[0] = unkE0->unk54 * scale;
        entity->model->unkE0[1] = unkE0->unk58 * scale;
        entity->model->unkE0[2] = unkE0->unk5C * scale;
        scale = (f32)unkE0->unk6A / 600.0f;
        record = &D_0038CD20[entity->group_id].unk10[(s8)((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3]];
        record->unk70 = scale * 0.75f + 0.25f;
        record->unk74 = scale * 0.375f + 0.125f;
        if (unkE0->unk6A <= 0) {
            func_001958B8((char *)entity, (void (*)(void))func_0016F5E8, 0);
        }
    }
}
