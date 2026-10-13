#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_effect.h"

extern float D_00413F80[];

int func_00192C98(void *, int, int, u32, int);
char *func_0010CAB8(char *, u32);
void func_0018DCD8(Entity *);

void func_00135798(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    Entity *other;
    char *part;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x4000000) {
            entity->unk30 = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            part = func_0010CAB8((char *)entity->model, 0x14);
            if (part != NULL) {
                Effect_StartSpawner(0x3BC, part, entity->model->unkD0, D_00413F80);
            }
            part = func_0010CAB8((char *)entity->model, 0x15);
            if (part != NULL) {
                Effect_StartSpawner(0x3BC, part, entity->model->unkD0, D_00413F80);
            }
            part = func_0010CAB8((char *)entity->model, 0x16);
            if (part != NULL) {
                Effect_StartSpawner(0x3BC, part, entity->model->unkD0, D_00413F80);
            }
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            unkE0->unk7A = 1;
            entity->action_step++;
        }
        break;
    case 3:
        other = Entity_GetBySpawnIdForMap(entity->group_id, entity->spawn_info->unk40);
        if (other != NULL && ((s32)other->flags & 1) && other->unkE0.unk7A != 0) {
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
            entity->unkA0 = 20;
            func_0018DCD8(entity);
            entity->action_step++;
        }
        break;
    case 4:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}
