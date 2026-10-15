/* Crow: the behaviour of entity type 12 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Crow. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
int rand(void);
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
float EntityMath_EaseAngleToward(float *, float, float);
float EntityMath_DistanceXZ(void *, void *);
int Motion_Start(void *, int, int, u32, int);
void EntityEvent_OnDeath(Entity *);

static void Crow_Animate(Entity *entity);
static void Crow_Think(Entity *entity, int frame);
static void Crow_PickAction(Entity *entity, s32 unknown);
static void Crow_MoveOffAndRemove(Entity *entity, s32 unknown);
static void Crow_Death(Entity *entity);
static void Crow_PlayAnim20(Entity *entity);
static void Crow_PlayAnim21(Entity *entity);

/* Set-up of a type 12 entity: clears its work area and installs the type's handlers. */
void Crow_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = Crow_Think;
    unkE0->animate = (void (*)(void))Crow_Animate;
    unkE0->on_hit = NULL;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))Crow_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate without the head turn. */
static void Crow_Animate(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, (s32)(flags >> 11) & 7);
        }
    }
}

/* The think handler: sets Entity.state itself. While the entity is not busy it puts the state to 3
   once the player is nearer than EntityParams.unk430; in state 3 it calls
   Crow_MoveOffAndRemove, before that Crow_PickAction. */
static void Crow_Think(Entity *entity, int frame) {
    if (entity->unkC6 != 0) {
        return;
    }
    if (entity->distance_to_player < (float)ENTITY_CONTROLLER(entity)->unk430) {
        // match: retail reads the u8 state signed (lb)
        if ((s8)entity->state != 3) {
            entity->state = 3;
        }
    }
    if ((s8)entity->state == 3) {
        Crow_MoveOffAndRemove(entity, frame);
        return;
    }
    Crow_PickAction(entity, frame);
}

/* Helper of Crow_Think, called until the state is 3: each time the timer runs out it draws
   among Crow_PlayAnim20, Crow_PlayAnim21 and a wait of 120 to 179 frames,
   weighted by EntityParams.unk432, unk434 and unk436; else it starts the move action. */
static void Crow_PickAction(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 roll;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (entity->unkA0 == 0) {
        roll = entity->params->unk432 + (s16)entity->params->unk434 + entity->params->unk436;
        if (roll != 0) {
            roll = rand() % roll;
        }
        if (roll < entity->params->unk432) {
            EntityAi_StartAction(entity, Crow_PlayAnim20, unknown);
            return;
        }
        if (roll < entity->params->unk432 + (s16)entity->params->unk434) {
            EntityAi_StartAction(entity, Crow_PlayAnim21, unknown);
            return;
        }
        if (roll < entity->params->unk432 + (s16)entity->params->unk434 + entity->params->unk436) {
            entity->unkA0 = rand() % 60 + 0x78;
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
    return;
}

/* Helper of Crow_Think, called in state 3: gives the forward command and starts the move
   action; the motion never moves the entity. It calls EntityEvent_StartEffectAtPoint with EntityParams.unk438 on the
   first unk43A frames, pitches the entity to -40 degrees while its y is above -2000 and back to 0
   after, and clears flag bit 1 once it is more than 10000 from its spawn point. */
static void Crow_MoveOffAndRemove(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    entity->unkC8 |= 1;
    if (unkE0->unk68 < entity->params->unk43A) {
        EntityEvent_StartEffectAtPoint((char **)entity, entity->params->unk438, 0xFF, (int)entity->model->unkD0);
        unkE0->unk68++;
    }
    // match: the whole step is written out in both arms (only the call is shared in retail)
    if (-2000.0f < entity->model->position[1]) {
        turn = entity->model->unkD0[0];
        EntityMath_EaseAngleToward(&turn, -0.6981317f, 20.0f);
        entity->unk90 += turn - entity->model->unkD0[0];
    } else {
        turn = entity->model->unkD0[0];
        EntityMath_EaseAngleToward(&turn, 0.0f, 20.0f);
        entity->unk90 += turn - entity->model->unkD0[0];
    }
    if (EntityMath_DistanceXZ(entity->model->position, entity->spawn_info) > 10000.0f) {
        ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The death action (busy bit 0x40): as EntityAi_Death; all the while it pitches the entity towards
   90 degrees until flag bit 44 is set, then back to 0. */
static void Crow_Death(Entity *entity) {
    f32 turn;

    entity->unkC6 = 0x40;
    // match: the whole step is written out in both arms (only the call is shared in retail)
    if (!ENTITY_FLAG_BITS(entity)->bit44) {
        turn = entity->model->unkD0[0];
        EntityMath_EaseAngleToward(&turn, 1.5707964f, 3.0f);
        entity->unk90 += turn - entity->model->unkD0[0];
    } else {
        turn = entity->model->unkD0[0];
        EntityMath_EaseAngleToward(&turn, 0.0f, 2.0f);
        entity->unk90 += turn - entity->model->unkD0[0];
    }
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
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
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            entity->unkA0 = 20;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}

/* An action (busy bit 0x1000): plays animation 20 to its end. Crow_PickAction starts it.
   */
static void Crow_PlayAnim20(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x14, 0, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x2000): plays animation 21 to its end. Crow_PickAction starts it.
   */
static void Crow_PlayAnim21(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x15, 0, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}
