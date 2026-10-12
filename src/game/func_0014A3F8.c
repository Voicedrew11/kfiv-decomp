#include "kf_entity.h"

extern u32 D_00446ED4;
extern float D_00413F84[];

int rand(void);
void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_0014A6D0(Entity *, s32);
void func_0014ACD0(Entity *);

void func_0014A3F8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 & 0x40) {
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
    } else {
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
    }
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 0:
                func_00182FD8(entity, unknown);
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                func_0014A6D0(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
    if (unkE0->unk6A > 0) {
        unkE0->unk6A--;
    }
    if (ENTITY_FLAG_BITS(entity)->bit28 && !(entity->unkC6 & 1)) {
        if (unkE0->unk6A <= 0) {
            if (entity->model->position[1] < D_00413F84[0] - 300.0f) {
                unkE0->unk7A = 1;
            } else if (D_00413F84[0] - 100.0f < entity->model->position[1]) {
                unkE0->unk7A = 0;
            } else {
                unkE0->unk7A = unkE0->unk7A == 0;
            }
            unkE0->unk6A = rand() % 15 + 20;
        }
        if (unkE0->unk7A != 0) {
            entity->unkC8 |= 0x20;
        } else {
            entity->unkC8 |= 0x10;
        }
    }
    func_0014ACD0(entity);
    if (unkE0->unk68 < 0) {
        unkE0->unk7C = (rand() % 10 + 10) * 2;
        if (rand() % 10 == 1) {
            unkE0->unk7B = 0;
        } else {
            unkE0->unk7B = rand() % entity->params->unk438;
        }
        unkE0->unk68 = unkE0->unk7C * 2;
    }
}
