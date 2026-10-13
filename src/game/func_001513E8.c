#include "kf_entity.h"
#include "kf_entity_world.h"

extern u32 D_00446ED4;

void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_00183240(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00151B60(Entity *);

void func_001513E8(Entity *entity, s32 unknown) {
    Entity *other;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->spawn_info->unk40 != 0 && entity->spawn_info->variant != 0) {
        other = Entity_GetBySpawnId(entity->spawn_info->variant);
        if (other != NULL && D_00446ED4 % entity->unkD7 == 0) {
            if (ENTITY_FLAG_BITS(other)->bits2 >= 2 || !ENTITY_FLAG_BITS(other)->bit1) {
                func_001958B8((char *)entity, (void (*)(void))func_00151B60, 1);
                return;
            }
        }
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
                func_00183240(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}
