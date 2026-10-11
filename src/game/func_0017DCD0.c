#include "kf_entity.h"

extern u32 D_00446ED4;

int rand(void);
void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_0019A448(void *, void *, int, int, int, int, int);
int func_00182DE0(void *, int);
int func_0018EAC0(void *, void *);
void func_0017DF10(Entity *, s32);
void func_0017E220(Entity *, s32);
void func_0017E2E0(Entity *, s32);
void func_0017E5D0(Entity *);
s32 func_0017EA98(Entity *);

void func_0017DCD0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 side;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (D_00446ED4 % 60U == entity->unkD7 % 10U) {
        unkE0->unk54 = rand() % 10;
    }
    if (entity->unkC6 & 0x40) {
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    } else {
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    }
    if (!(entity->unkC6 & 0x44)) {
        func_0019A448(entity, &((EntityUnkE0Kind151D30 *)unkE0)->unk88, 1, 3, 5, entity->params->unkC4, 0);
    }
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->unkCC) {
            case 0:
                func_0017E2E0(entity, unknown);
                break;
            case 1:
                func_0017E220(entity, unknown);
                break;
            case 2:
                func_0017DF10(entity, unknown);
                break;
            default:
                func_0017E2E0(entity, unknown);
                break;
            }
        }
    }
    if (entity->unkC6 & 0x40) {
        unkE0->unk44 = 0.0f;
        unkE0->unk48[0] = 0.0f;
    } else {
        side = func_0018EAC0(entity, direction);
        if (side > 0) {
            entity->unkC8 = 0x201;
        } else if (side < 0) {
            entity->unkC8 = 0x101;
        } else {
            // match: called like func_00190F90, with a speed and a mode the callee ignores
            entity->unkC8 = ((s32 (*)(f32, Entity *, s32))func_0017EA98)(0.0f, entity, 0);
        }
        func_0017E5D0(entity);
    }
}
