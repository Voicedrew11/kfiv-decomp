#include "kf_entity.h"

extern u32 D_00446ED4;
// match: const, so that the store to unk48 can follow the load of it
extern const f32 D_00446808[];

void func_00196870(void *, int);
void func_00196C28(char *);
void func_0018E9C0(void *);
f32 func_0018B518(f32 *);
int func_00182DE0(void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00145D68(Entity *, s32);
void func_00145EA8(Entity *, s32);
void func_00146580(Entity *);

void func_00145BE8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        func_00196C28((char *)entity);
        func_0018E9C0(entity);
        unkE0->unk48 = func_0018B518(entity->model->position);
        if (unkE0->unk48 != D_00446808[0] && -entity->unk34 < unkE0->unk48) {
            entity->model->position[1] += 5.0f;
            if (unkE0->unk48 > 0.0f) {
                func_001958B8((char *)entity, (void (*)(void))func_00146580, 1);
                return;
            }
        }
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 1:
            case 2:
                func_00145EA8(entity, unknown);
                break;
            case 0:
            default:
                func_00145D68(entity, unknown);
                break;
            }
        }
    }
}
