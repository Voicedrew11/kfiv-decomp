#include "kf_entity.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00128DD8(Entity *, s32);
void func_00128F40(Entity *, s32);
void func_00129CA0(Entity *);

/* Per-frame update of this kind: wakes up when the player comes near, else runs the handler of its state. */
void func_00128CA0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    if (unkE0->unk7A != 0 && entity->distance_to_player < (float)entity->params->unk432) {
        unkE0->unk7A = 0;
        func_001958B8((char *)entity, (void (*)(void))func_00129CA0, 1);
        return;
    }
    func_00196CC0((char *)entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00182DE0(entity, unknown);
    } else if (entity->state == 2) {
        func_00128F40(entity, unknown);
    } else {
        func_00128DD8(entity, unknown);
    }
}
