#include "kf_entity.h"

extern u32 D_00446ED4;

int rand(void);
char *func_0010CAB8(char *, u32);
void func_00216778(void *, void *);
int func_00195E48(void *, int, void *, void *);
void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_0012B5F8(Entity *, s32);

void func_0012B338(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *part = func_0010CAB8((char *)entity->model, 0);
    f32 target[4];
    f32 direction[4];
    f32 offset;

    if (part != NULL && entity->unk2A == 0) {
        direction[0] = 0.0f;
        direction[1] = 0.0f;
        direction[2] = 0.0f;
        direction[3] = 0.0f;
        if (unkE0->unk68 == 0) {
            if (rand() % 100 < ENTITY_CONTROLLER(entity)->unk434) {
                func_00216778(target, part);
                offset = rand() % 10 - 5;
                target[0] += offset;
                target[1] += offset;
                target[2] += offset;
                func_00195E48(entity, entity->params->unk43C, target, direction);
                unkE0->unk68 = entity->params->unk436;
            }
        }
        if (unkE0->unk68 != 0) {
            unkE0->unk68--;
        }
        if (unkE0->unk6A == 0) {
            if (rand() % 100 < ENTITY_CONTROLLER(entity)->unk438) {
                func_00216778(target, part);
                offset = rand() % 10 - 5;
                target[0] += offset;
                target[1] += offset;
                target[2] += offset;
                func_00195E48(entity, entity->params->unk43C, target, direction);
                unkE0->unk6A = entity->params->unk43A;
            }
        }
        if (unkE0->unk6A != 0) {
            unkE0->unk6A--;
        }
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    func_00196CC0((char *)entity);
    func_0018E9C0(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        func_00182DE0(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        break;
    case 1:
    case 2:
        func_0012B5F8(entity, unknown);
        return;
    }
    ((void (*)(void *, int))func_00182FD8)(entity, unknown); // match: called as void, $2 is reused at once
    unkE0->unk7A = 1;
}
