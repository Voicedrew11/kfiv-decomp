#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
void func_00195588(f32 *, f32);
int func_0018F030(char *);
void func_001958B8(char *, void (*)(void), int);
void func_00165A70(Entity *);

/* one frame of the jump: forwards at unk44 along unk30, upwards at unk48; both wear off */
static inline void move_along_arc(Entity *entity, EntityUnkE0 *unkE0) {
    entity->unk84 = unkE0->unk44 * unkE0->unk30[0];
    entity->unk88 = -unkE0->unk48;
    entity->unk8C = unkE0->unk44 * unkE0->unk30[2];
    unkE0->unk48 -= entity->params->unk43E * 0.1f;
    unkE0->unk44 -= 0.5f;
    if (unkE0->unk44 < 0.0f) {
        unkE0->unk44 = 0.0f;
    }
}

/* the jump at the player: crouch, leap along the arc func_00166BF8 tried out, land; being hit
   in the air ends it (unk7B: what to go back to afterwards) */
void func_00166158(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModel *model;
    f32 unused[4]; // match: the frame has room for it, and no call here is a tail call

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 8, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit17 = FALSE;
        entity->action_step++;
    case 1:
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        model = entity->model;
        if (model->unkFC->unk88 == 0) {
            unkE0->unk48 = entity->params->unk43A;
            unkE0->unk44 = entity->params->unk43C;
            entity->action_step++;
            func_00195588(unkE0->unk30, model->unkD0[1]);
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= 10) {
            move_along_arc(entity, unkE0);
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00192C98(entity, 0x11, 0, 1, 1);
            entity->action_step += 2;
        }
        break;
    case 4:
        move_along_arc(entity, unkE0);
        if (unkE0->unk48 < 10.0f) {
            if (ABS(entity->unk68 - entity->model->unkD0[1]) > 1.5707964f) {
                ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
                entity->action_step++;
            } else {
                func_00192C98(entity, 9, 0, 5, 1);
                entity->action_step += 2;
            }
        }
        break;
    case 5:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1 || ENTITY_FLAG_BITS(entity)->bit50 == 1) {
            func_00192C98(entity, 9, 0, 1, 1);
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step = 8;
            entity->unk88 = 0.0f;
            entity->unk8C = 0.0f;
            entity->unk84 = 0.0f;
            break;
        }
        move_along_arc(entity, unkE0);
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00192C98(entity, 9, 0, 1, 1);
            entity->action_step = 7;
        }
        break;
    case 6:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1 || ENTITY_FLAG_BITS(entity)->bit50 == 1) {
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step = 8;
            entity->unk88 = 0.0f;
            entity->unk8C = 0.0f;
            entity->unk84 = 0.0f;
            break;
        }
    case 3:
        move_along_arc(entity, unkE0);
        if (entity->model->unkFC->unk88 == 0) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            entity->action_step++;
        }
        break;
    case 7:
        if (ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit49 == 1
            || ENTITY_FLAG_BITS(entity)->bit48 == 1 || ENTITY_FLAG_BITS(entity)->bit50 == 1) {
            ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            entity->action_step++;
            entity->unk88 = 0.0f;
            entity->unk8C = 0.0f;
            entity->unk84 = 0.0f;
            break;
        }
        move_along_arc(entity, unkE0);
        break;
    case 8:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            ENTITY_FLAG_BITS(entity)->bit17 = TRUE;
            if (unkE0->unk7B == 0) {
                if (ENTITY_FLAG_BITS(entity)->bit5) {
                    unkE0->unk00(entity, 1);
                } else {
                    func_001958B8((char *)entity, unkE0->unk18, 1);
                }
            } else {
                func_0018F030((char *)entity);
                func_001958B8((char *)entity, (void (*)(void))func_00165A70, 1);
            }
        }
        break;
    }
}
