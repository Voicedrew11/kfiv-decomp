#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;
extern ModelInstanceBlock D_003E7140[2];

void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
void func_001319B8(Entity *, s32);
void func_001319F0(Entity *, s32);

void func_001317D0(Entity *entity, s32 unknown) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;
    ModelInstanceBlock *block;
    ModelInstanceBlock *previous;

    block = &D_003E7140[1];
    previous = block - 1;
    *block = *previous;
    D_003E7140[0] = *(ModelInstanceBlock *)entity->model;
    D_003E7140[0].model.alpha *= 0.5f;
    D_003E7140[1].model.alpha *= 0.5f;
    unkE0->unk7E = 0;
    if (unkE0->unk6A > 0) {
        unkE0->unk6A--;
    }
    if (D_00413F80.unk259 != 0) {
        unkE0->unk6A = entity->params->unk434;
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 1:
            case 2:
                func_001319F0(entity, unknown);
                break;
            case 0:
            default:
                func_001319B8(entity, unknown);
                break;
            }
        }
    }
    if (unkE0->unk7D > 0) {
        unkE0->unk7D--;
    }
}
