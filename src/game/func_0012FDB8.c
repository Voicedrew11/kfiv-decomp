#include "kf_entity.h"

extern u8 D_00349690[];

int func_00192C98(void *, int, int, u32, int);
f32 func_0018A9F0(void *, void *);
void func_00195588(Vector4 *, float);
void Vector_Scale(float *, float *, float);
void func_00195898(char *, u8);
int func_001958B8(Entity *, void (*)(Entity *), int);

void func_0012FDB8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 distance;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 8, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit25 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->spawn_info->variant != 0 &&
            D_00349690[entity->spawn_info->unk43 + entity->spawn_info->unk42 * 64] != 0) {
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            break;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
        if (entity->distance_to_player < entity->spawn_info->unk54) {
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 > 30) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            entity->action_step++;
        }
        break;
    case 3:
        distance = func_0018A9F0(entity->unk14, entity->model->position);
        if (ENTITY_FLAG_BITS(entity)->bit44 || distance < 100.0f) {
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            entity->action_step++;
            break;
        }
        func_00195588(direction, entity->model->unkD0[1]);
        Vector_Scale(direction, direction, entity->params->unk436);
        direction[1] += entity->params->unk438;
        entity->unk84 += direction[0];
        entity->unk88 += direction[1];
        entity->unk8C += direction[2];
        break;
    case 4:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00195898((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit25 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
