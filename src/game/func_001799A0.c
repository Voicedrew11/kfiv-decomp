#include "kf_entity.h"

extern u32 D_00446ED4;
extern s32 D_00446928;

void func_0018D600(char **, int, u32, int);
void func_0018DA38(Entity *, s32, s32, s32);
void func_0019D490(s32, s32, f32 *, f32);
void func_00196870(void *, int);
void func_0018E9C0(void *);
int func_00182FD8(void *, int);
void func_00183240(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00179BB0(Entity *);

void func_001799A0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 3U == 0) {
        switch (D_00446928) {
        case 3:
            func_0018D600((char **)entity, entity->params->unk430, 0xFF, (int)entity->model->unkD0);
            break;
        case 2:
            func_0018D600((char **)entity, entity->params->unk432, 0xFF, (int)entity->model->unkD0);
            break;
        default:
            func_0018D600((char **)entity, (s16)entity->params->unk434, 0xFF, (int)entity->model->unkD0);
            break;
        }
    }
    if (ENTITY_FLAG_BITS(entity)->bit43) {
        func_0018D600((char **)entity, 0x13D, 0xFF, (int)entity->model->unkD0);
        ENTITY_FLAG_BITS(entity)->bit43 = FALSE;
        if (D_00446928 <= 0) {
            entity->unkA3 = 6;
            func_001958B8((char *)entity, (void (*)(void))func_00179BB0, unknown);
            return;
        }
        if (unkE0->unk7A != D_00446928) {
            func_0018DA38(entity, entity->params->unk436, entity->params->unk438, 0);
            // match: an int result keeps $2 busy after the call
            ((int (*)(s32, s32, f32 *, f32))func_0019D490)((u8)entity->params->unk43A, (u8)entity->params->unk43C,
                                                           entity->model->position, entity->params->unk42E);
            unkE0->unk7A = D_00446928;
        }
    }
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
}
