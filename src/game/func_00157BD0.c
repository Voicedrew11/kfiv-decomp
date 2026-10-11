#include "kf_entity.h"

extern u32 D_00446ED4;
extern f32 D_00446814[];

void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_00157E70(Entity *, s32);
f32 func_0018A9F0(void *, void *);
void func_001583D8(Entity *);

void func_00157BD0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 height;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (unkE0->unk7B != 0 || unkE0->unk68 == 1) {
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
    } else {
        ENTITY_FLAG_BITS(entity)->bit20 = 0;
        ENTITY_FLAG_BITS(entity)->bit18 = 0;
        ENTITY_FLAG_BITS(entity)->bit21 = 0;
        ENTITY_FLAG_BITS(entity)->bit26 = 0;
        ENTITY_FLAG_BITS(entity)->bit27 = 0;
    }
    if (unkE0->unk6A > 0) {
        unkE0->unk6A--;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 1;
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->unkCC) {
            case 0:
                func_00182FD8(entity, unknown);
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                func_00157E70(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
    if (ENTITY_FLAG_BITS(entity)->bit28) {
        height = func_0018A9F0(entity->unk14, entity->model->position);
        if (height != D_00446814[0]) {
            if (entity->params->unk43C < height) {
                unkE0->unk7A = 1;
            } else if (height < entity->params->unk43E) {
                unkE0->unk7A = 0;
            }
            if (unkE0->unk7A != 0) {
                entity->unkC8 |= 0x20;
            } else {
                entity->unkC8 |= 0x10;
            }
        } else {
            unkE0->unk7A = 0;
            entity->unkC8 |= 0x10;
        }
    }
    if (entity->unkC6 == 0 || (entity->unkC6 & 0x3006)) {
        func_001583D8(entity);
    }
}
