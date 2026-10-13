#include "kf_entity.h"
#include "kf_effect.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
void func_00183240(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);
char *func_0010CAB8(char *, u32);
void func_001408B0(Entity *);

void func_00140738(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *part;

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
                func_00183240(entity, unknown);
                unkE0->unk7A = 0;
                break;
            case 0:
            default:
                func_001958B8((char *)entity, (void (*)(void))func_001408B0, unknown);
                break;
            }
        }
    }
    part = func_0010CAB8((char *)entity->model, 0x1E);
    Effect_StartSpawner(entity->params->unk43A, part, entity->model->unkD0, entity);
    part = func_0010CAB8((char *)entity->model, 0x1F);
    Effect_StartSpawner(entity->params->unk43A, part, entity->model->unkD0, entity);
}
