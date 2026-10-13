#include "kf_entity.h"
#include "kf_entity_world.h"

int func_00192C98(void *, int, int, u32, int);
void func_001958B8(char *, void (*)(void), int);
char *func_0010CAB8(char *, u32);
void func_0016F930(Entity *);
void func_0016FE10(Entity *);

void func_0016F7B8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    Entity *other;
    f32 *point;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, 0, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x200000;
        entity->flags |= 0x100000;
        entity->flags |= 0x40000;
        entity->flags |= 0x10000000;
        entity->flags |= 0x400000;
        entity->flags |= 0x800000;
        entity->flags |= 0x1000000;
        entity->flags |= 0x8000000;
        entity->action_step++;
    case 1:
        if (unkE0->unk7A > 0) {
            func_001958B8((char *)entity, (void (*)(void))func_0016F930, 1);
            return;
        }
        if (unkE0->unk7A < 0) {
            func_001958B8((char *)entity, (void (*)(void))func_0016FE10, 1);
            return;
        }
        other = Entity_GetBySpawnId(entity->spawn_info->variant);
        if (other != NULL && (other->flags & 3) == 3) {
            point = (f32 *)func_0010CAB8((char *)other->model, 8);
            if (point != NULL) {
                entity->model->position[0] = point[0];
                entity->model->position[1] = point[1];
                entity->model->position[2] = point[2];
            }
        }
        break;
    }
}
