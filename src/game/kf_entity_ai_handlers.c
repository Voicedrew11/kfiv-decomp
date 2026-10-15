/* The shared entity AI: the per-frame update most entity types run (EntityAi_Update), the set-up of type 0,
   and the default handlers a type's set-up function installs in the work area (think, animate, on_hit,
   dormant, death, move), with the actions and reactions they start. See EntityUnkE0 in kf_entity.h. */
#include "kf_entity_ai_handlers.h"
#include "kf_entity_ai_choice.h"
#include "kf_entity_aim.h"
#include "kf_entity_events.h"
#include "kf_motion.h"
#include "kf_entity_math.h"

int EntityAi_StartAction(Entity *entity, void (*handler)(Entity *), int run_now);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(Entity *entity, void (*reaction)(void), int run_now);
void EntityAi_SetAttackDelay(Entity *, s32);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
s32 EntityAi_CheckMeleeHit(Entity *, void *, s32, s32, s32);
int EntityAi_LaunchShots(Entity *, EntityParamsUnk3D8 *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_RaiseAttackWeight(Entity *);
void EntityAi_RaiseRangedWeights(Entity *);
void EntityAi_UpdateCollision(Entity *);
void EntityAi_SetUpdate(char *, s32);
extern s32 D_003E6E40[][2];
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void Vector_Normalize(void *, void *);
void Vector_Cross(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
int rand(void);
extern PlayerData D_00413F80;
void func_00196D48(Entity *);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
float EntityAi_Accelerate(Entity *, float);

/* Motion_BlendToPose as if it returned a value.
   match: the callee is void, but retail keeps what it leaves in $2 */
#define MOTION_BLEND_TO_POSE ((int (*)(EntityModel *, f32))Motion_BlendToPose)

/* An action that starts no motion of its own: it waits until the current motion has blended in and played to
   its end, then ends as the other actions do (think, or the move action). Nothing refers to it. */
void EntityAi_FinishMotion(Entity *entity, s32 unused) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    switch (entity->action_step) {
    case 0:
        ((EntityUnkE0Kind150588 *)unkE0)->unk1C = 0;
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 1:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* An action (busy bit 0x200): plays animation 0 (standing) once to its end, then thinks. Beetle_Think starts
   it when the beetle falls back to state 0. */
void EntityAi_StandOnce(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x200;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The default dormant action (busy bit 0x20): holds animation 1 until the player is within
   EntitySpawnInfo.unk54 or flag bit 43 is set, plays animation 2, then goes to state 2 and thinks.
   */
void EntityAi_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit22 = 1;
        ENTITY_FLAG_BITS(entity)->bit23 = 1;
        ENTITY_FLAG_BITS(entity)->bit24 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 1;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->unk54 || ENTITY_FLAG_BITS(entity)->bit43) {
            Motion_Start(entity, 2, 0, entity->params->unkC4, 0);
            if (entity->params->flags & 0x100) {
                ENTITY_FLAG_BITS(entity)->bits11 = 0;
            } else {
                ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
            }
            ENTITY_FLAG_BITS(entity)->bits36 = 0;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 3:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            EntityAi_SetState((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit22 = 0;
            ENTITY_FLAG_BITS(entity)->bit23 = 0;
            ENTITY_FLAG_BITS(entity)->bit24 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            if (!(((EntityParamsWord *)entity->params)->unk00 & 0x6000000)) {
                ENTITY_FLAG_BITS(entity)->bit28 = 0;
            }
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The reaction to a light hit while flag bit 19 is set (an attack in the frames a hit does not
   break): takes the current animation three frames back, busy bit 2 while that blends in, and lets
   the action go on. */
void EntityAi_ReactionRewind(Entity *entity) {
    EntityParams *params;
    s32 frame;
    s32 kind;

    entity->unkC6 |= 2;
    switch (entity->unkCB) {
    case 0:
        frame = entity->model->unkFC->unk82 - 3;
        if (frame < 0) {
            frame = 0;
        }
        Motion_Start(entity, entity->unkD1, frame, 5, 1);
        params = entity->params;
        if (params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            kind = 1;
            if ((((EntityParamsWord *)params)->unk00 & 0x6000000) == 0x2000000) {
                kind = 2;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
        ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkC6 &= ~2;
            EntityAi_SetReaction(entity, NULL, 0);
        }
        break;
    }
}

/* The reaction to a light hit (busy bit 2): plays animation 14 until it has blended in, then ends
   whatever action was running and thinks. */
void EntityAi_ReactionLight(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 2;
    switch (entity->unkCB) {
    case 0:
        EntityEvent_StartFrameCues(entity, 0xE, 0, 0);
        Motion_Start(entity, 0xE, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        EntityAi_SetAttackDelay(entity, entity->params->unkC8);
        EntityAi_SetReaction(entity, NULL, 0);
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            unkE0->think(entity, 1);
        } else {
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
        }
        break;
    }
}

/* The reaction to a heavy hit (busy bit 4): plays animation 15 to its end, then ends whatever
   action was running and thinks. */
void EntityAi_ReactionHeavy(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        Motion_Start(entity, 0xF, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit19 = 0;
        ENTITY_FLAG_BITS(entity)->bit35 = 0;
        if (!(((EntityParamsWord *)entity->params)->unk00 & 0x06000000)) {
            ENTITY_FLAG_BITS(entity)->bit28 = 0;
        }
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            EntityAi_SetAttackDelay(entity, entity->params->unkC8);
            EntityAi_SetReaction(entity, NULL, 0);
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The default death action (busy bit 0x40): plays animation 16, fades the model out over 20 frames,
   then clears flag bit 1. */
void EntityAi_Death(Entity *entity) {
    EntityParams *params;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        params = entity->params;
        if (params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        if ((((EntityParamsWord *)params)->unk00 & 0x6000000) == 0x4000000) {
            entity->unk30 = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = 1;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkA0 = 20;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            EntityEvent_OnDeath(entity);
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = 0;
        }
        break;
    }
}

/* The default dodge action (busy bit 8): plays animation 10 once, its root motion moving the
   entity, then thinks. The name is a reading, kept because the code supports it this far: the
   action sets no guard flag and checks no hit; all it does is play a motion that moves the entity
   (flag bits 11 to 13 at 1 or 2); and nearly every place that starts it or a type's own dodge does
   so on EntityAi_ShouldDodge (the player is early in a swing, near and in front; chance EntityParams.unkCA
   / 5 in 100), the test that stands next to the guard's EntityAi_ShouldGuard, or on a type's test that the
   player faces the entity from 200 or more away (types 48 to 51, chance unkCA / 10). The types' own
   dodges play the sideways animations 6 and 7 instead (44, 60), draw among 10, 6 and 7 (50), or
   play 29 or 30 (54). */
void EntityAi_Dodge(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 8;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0xA, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The default guard action (busy bit 0x10): blends into animation 13 and holds it for
   EntityParams.unkCE frames with flag bit 35 set, which adds to the defence (func_001B2F70) and
   keeps a hit from starting a reaction. */
void EntityAi_Guard(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x10;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0xD, 0, 0xA, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkA0 = entity->params->unkCE;
            ENTITY_FLAG_BITS(entity)->bit35 = 1;
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->unkA0 == 0) {
            ENTITY_FLAG_BITS(entity)->bit35 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = 1;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The default melee attack (busy bit 1): plays animation 20 + n for attack record n (flag bits
   58-59, EntityParams.unk240), turns to the player during the record's first frames and checks its
   three parts for hits on their frames. */
void EntityAi_MeleeAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[ENTITY_FLAG_BITS(entity)->bits58];
    s32 frame;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, ENTITY_FLAG_BITS(entity)->bits58 | 0x14, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit54 = 0;
        ENTITY_FLAG_BITS(entity)->bit55 = 0;
        ENTITY_FLAG_BITS(entity)->bit56 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk240->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = EntityAi_CheckMeleeHit(entity, unk240, 0, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit55) {
            ENTITY_FLAG_BITS(entity)->bit55 = EntityAi_CheckMeleeHit(entity, unk240, 1, frame, entity->model->unkFC->unk82);
        }
        if (!ENTITY_FLAG_BITS(entity)->bit56) {
            ENTITY_FLAG_BITS(entity)->bit56 = EntityAi_CheckMeleeHit(entity, unk240, 2, frame, entity->model->unkFC->unk82);
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk240->unk02);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The default ranged attack (busy bit 1): plays animation 23 + n for ranged record n (flag bits
   58-59, EntityParams.unk3D8), turns to the player during the record's first frames and launches
   its shots on their frames. */
void EntityAi_RangedAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            EntityAi_TurnToHeading(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        EntityAi_LaunchShots(entity, unk3D8, (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8,
                      entity->model->unkFC->unk82);
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The per-frame update of 82 entity types (D_003E6E40): runs the death action, the on_hit or the think
   handler, then the reaction or the action, counts unkA0 and unkA2 down, animates, and applies this frame's
   move (three quarters of it while the unk28 timer runs). */
void EntityAi_Update(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    Vector_Copy(entity->unk70, entity->model->position);
    if (ENTITY_FLAG_BITS(entity)->bit41) {
        EntityAi_RaiseAttackWeight(entity);
    }
    if (ENTITY_FLAG_BITS(entity)->bit42) {
        EntityAi_RaiseRangedWeights(entity);
    }
    if (!(entity->unkC6 & 0x60)) {
        if (entity->unk2C == 0) {
            unkE0->reaction = NULL;
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->death, 0);
        } else if (ENTITY_FLAG_BITS(entity)->bits36 != 0 && !ENTITY_FLAG_BITS(entity)->bit35) {
            if (unkE0->on_hit != NULL) {
                ((void (*)(Entity *))unkE0->on_hit)(entity);
            }
        } else if (ENTITY_FLAG_BITS(entity)->bit5) {
            if (unkE0->think != NULL) {
                unkE0->think(entity, 0);
            }
        }
    }
    ENTITY_FLAG_BITS(entity)->bit41 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit42 = FALSE;
    if (ENTITY_FLAG_BITS(entity)->bit6) {
        if (unkE0->reaction != NULL) {
            ((void (*)(Entity *))unkE0->reaction)(entity);
        } else if (entity->handler != NULL) {
            entity->handler(entity);
        }
    }
    if (entity->unkA0 != 0) {
        entity->unkA0--;
    }
    if (entity->unkA2 != 0) {
        entity->unkA2--;
    }
    if (unkE0->animate != NULL) {
        ((void (*)(Entity *))unkE0->animate)(entity);
    }
    if (entity->unk28 != 0 && !(entity->unkC6 & 0x46)) {
        entity->model->position[0] += entity->unk84 * 0.75f;
        entity->model->position[1] += entity->unk88 * 0.75f;
        entity->model->position[2] += entity->unk8C * 0.75f;
        entity->model->unkD0[0] = EntityMath_WrapAngle(entity->model->unkD0[0] + entity->unk90 * 0.75f);
        entity->model->unkD0[1] = EntityMath_WrapAngle(entity->model->unkD0[1] + entity->unk94 * 0.75f);
        entity->model->unkD0[2] = EntityMath_WrapAngle(entity->model->unkD0[2] + entity->unk98 * 0.75f);
    } else {
        entity->model->position[0] += entity->unk84;
        entity->model->position[1] += entity->unk88;
        entity->model->position[2] += entity->unk8C;
        entity->model->unkD0[0] = EntityMath_WrapAngle(entity->model->unkD0[0] + entity->unk90);
        entity->model->unkD0[1] = EntityMath_WrapAngle(entity->model->unkD0[1] + entity->unk94);
        entity->model->unkD0[2] = EntityMath_WrapAngle(entity->model->unkD0[2] + entity->unk98);
    }
    ENTITY_FLAG_BITS(entity)->bits36 = 0;
    entity->unkC8 = 0;
    ENTITY_FLAG_BITS(entity)->bit43 = FALSE;
    EntityAi_UpdateCollision(entity);
}

/* The set-up function of entity type 0 (D_003E6E40[0]): clears the work area, installs the default handlers
   (EntityAi_Think, EntityAi_Animate, EntityAi_OnHit, EntityAi_Move, EntityAi_Dormant, EntityAi_Death), sets
   the choice weights unkBC and unkC0 to 0x80, and starts the dormant action (spawn flag bit 58) or the move
   action. */
void EntityAi_InitDefaults(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = EntityAi_Think;
    unkE0->animate = (void (*)(void))EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (ENTITY_PARAMS_BITS(entity)->bit25 || ENTITY_PARAMS_BITS(entity)->bit26) {
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    }
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    } else {
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    }
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(ENTITY_MODEL(entity), 0, 0);
}

/* The default animate handler: blends into the current motion or plays it a step on and sets flag
   bit 34 when it has ended, then turns the head (towards the target while flag bit 9 is set). */
void EntityAi_Animate(Entity *entity) {
    f32 limits[4];
    s32 look_at_target;
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        if ((((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x6000000) {
            entity->flags = flags & ~0x3800L;
        }
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = MOTION_BLEND_TO_POSE(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->bits11);
        }
        look_at_target = ENTITY_FLAG_BITS(entity)->bit9;
        limits[1] = entity->params->unk430 * 0.017453292f;
        limits[0] = -entity->params->unk430 * 0.017453292f;
        limits[3] = entity->params->unk432 * 0.017453292f;
        limits[2] = -entity->params->unk432 * 0.017453292f;
        if (look_at_target) {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, look_at_target, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, 0, NULL, 8.0f);
        }
    }
}

/* The default on_hit handler: forgets the target and starts the reaction the hit calls for. A light
   hit (flag bits 36-37 are 1) gives EntityAi_ReactionLight, or EntityAi_ReactionRewind while flag
   bit 19 is set; a heavy one (2) gives EntityAi_ReactionHeavy. The entity then goes to state 2. */
void EntityAi_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit(entity);
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (ENTITY_FLAG_BITS(entity)->bits36 == 1) {
        if (ENTITY_FLAG_BITS(entity)->bit19) {
            EntityAi_SetReaction(entity, (void (*)(void))EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction(entity, (void (*)(void))EntityAi_ReactionLight, 0);
        }
    } else if (ENTITY_FLAG_BITS(entity)->bits36 == 2) {
        EntityAi_SetReaction(entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The default think handler: every tenth frame updates what the entity knows of the player
   (EntityAi_SensePlayer); while the entity is not busy it steps Entity.state and calls the think function
   of that state. */
void EntityAi_Think(Entity *entity, s32 unknown) {
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    if (!(ENTITY_PARAMS_BITS(entity)->bit25 && ENTITY_PARAMS_BITS(entity)->bit26)) {
        EntityAi_CheckFarFromSpawn((char *)entity);
    }
    EntityAi_StepState(entity);
    if (ENTITY_FLAG_BITS(entity)->bit53) {
        EntityAi_ThinkReturn(entity, unknown);
        return;
    }
    switch (entity->state) {
    case 0:
        EntityAi_ThinkIdle(entity, unknown);
        return;
    case 1:
        EntityAi_ThinkPursue(entity, unknown);
        return;
    case 2:
        EntityAi_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* The think function while flag bit 53 is set (too far from the spawn point): walks back to the spawn point,
   turning aside while something pushes the entity from the front. */
int EntityAi_ThinkReturn(void *entity_ptr, int unknown) {
    Entity *entity = entity_ptr;
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 to_spawn[4];
    f32 push[4];
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    entity->unkA0 = 0;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn == 0) {
        entity->target = entity->spawn_info;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else {
        push[0] = push[1] = push[2] = push[3] = 0.0f;
        if (entity->flags & (0xC000L << 31)) {
            push[0] += entity->unkA4[0];
            push[2] += entity->unkA4[2];
        }
        if ((s32)(entity->flags >> 48) & 1) {
            push[0] += entity->unkB0[0];
            push[2] += entity->unkB0[2];
        }
        Vector_Normalize(push, push);
        Vector_Subtract(to_spawn, entity->spawn_info, entity->model->position);
        Vector_Normalize(to_spawn, to_spawn);
        Vector_Cross(to_spawn, push, to_spawn);
        if (to_spawn[1] <= 0.0f) {
            if (turn == -1) {
                turn = 1;
            }
        } else if (turn == 1) {
            turn = -1;
        }
        entity->target = entity->spawn_info;
        if (turn > 0) {
            entity->unkC8 = 0x201;
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
        } else {
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of state 0: every 120-180 frames picks a point near the spawn point to walk to
   (EntityPoint_PickNear), or stands for 60-90 frames (one time in five). */
int EntityAi_ThinkIdle(void *entity_ptr, int unknown) {
    Entity *entity = entity_ptr;
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (entity->unkA0 == 0) {
        if (rand() % 100 > 20) {
            EntityPoint_PickNear(entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
            entity->target = unkE0->unk30;
            entity->unkA0 = rand() % 60 + 120;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 30 + 60;
        }
    }
    turn = EntityAi_TurnFromPush(entity, direction);
    if (turn > 0) {
        entity->unkC8 = 0x201;
    } else if (turn < 0) {
        entity->unkC8 = 0x101;
    } else {
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The think function of state 1 (the player was noticed and is no longer): keeps walking to the
   player. */
void EntityAi_ThinkPursue(Entity *entity, int extra) {
    EntityUnkE0 *sub = &entity->unkE0;
    s64 buf;
    int result;

    if (ENTITY_CONTROLLER(entity)->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    ENTITY_FLAG_BITS(entity)->bit10 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = 0;
    if (entity->unkA0 == 0) {
        entity->target = &D_00413F80;
    }
    result = EntityAi_TurnFromPush(entity, &buf);
    if (result > 0) {
        entity->unkC8 = 0x201;
    } else if (result < 0) {
        entity->unkC8 = 0x101;
    } else {
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))sub->move, extra);
}

/* The think function of state 2 (the player is noticed): guards or dodges the player's swing, else
   picks among attacking (when an attack reaches), closing in and waiting, and gives the movement
   command for it. */
void EntityAi_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit10 = 0;
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    if (entity->unkA0 == 0 || side == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = side;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        EntityAi_LowerChoiceWeight(entity, side);
        switch (entity->unkCD) {
        case 0:
            if (EntityAi_PickAttack(entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 0x78;
            unkE0->unk78 = 0;
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (unkE0->unk78 == 0) {
        func_00196D48(entity);
        if (ENTITY_FLAG_BITS(entity)->bit31) {
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (unkE0->unk79 == 0) {
                unkE0->unk79 = ABS(angle) > 0.5235988f;
                if (unkE0->unk79 == 0) {
                    goto done;
                }
            }
            entity->unkC8 = EntityAi_TurnCommand(entity);
            if (entity->unkC8 == 0) {
                unkE0->unk79 = 0;
            }
        } else {
            turn = EntityAi_TurnFromPush(entity, direction);
            if (turn > 0) {
                entity->unkC8 = 0x201;
            } else if (turn < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
            if (ENTITY_CONTROLLER(entity)->unk434 != 0) {
                entity->unkC8 |= 0x40;
            }
        }
    }
done:
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The default move action: turns and moves as the command bits in Entity.unkC8 say, at the entity's speed
   (twice it with 0x40), and plays the animation that goes with the command (3 walk, 4 run, 5 back, 6 and 7
   sideways, 8 and 9 up and down, 0 standing). */
void EntityAi_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *vector;
    f32 turn;
    f32 amount;

    if (entity == D_00413F80.unk204 && (entity->params->flags & 0x800)) {
        entity->unkC8 = 0;
    }
    if (entity->unkC8 & 0x100) {
        unkE0->unk1C -= entity->params->unk58;
        if (unkE0->unk1C < -entity->params->turn_limit) {
            unkE0->unk1C = -entity->params->turn_limit;
        }
    } else if (entity->unkC8 & 0x200) {
        unkE0->unk1C += entity->params->unk58;
        if (entity->params->turn_limit < unkE0->unk1C) {
            unkE0->unk1C = entity->params->turn_limit;
        }
    } else {
        unkE0->unk1C = 0.0f;
    }
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
    if (ENTITY_FLAG_BITS(entity)->bits11 == 0) {
        vector = ENTITY_SCRATCH_VECTOR;
        vector[0] = vector[1] = vector[2] = vector[3] = 0.0f;
        if (entity->unkC8 & 3) {
            amount = 0.0f;
            if (entity->unkC8 & 1) {
                amount = -1.0f;
            } else if (entity->unkC8 & 2) {
                amount = 1.0f;
            }
            vector[2] += amount;
        }
        if (entity->unkC8 & 0xC) {
            amount = 0.0f;
            if (entity->unkC8 & 4) {
                amount = 1.0f;
            } else if (entity->unkC8 & 8) {
                amount = -1.0f;
            }
            vector[0] += amount;
        }
        if (entity->unkC8 & 0x30) {
            amount = 0.0f;
            if (entity->unkC8 & 0x10) {
                amount = -1.0f;
            } else if (entity->unkC8 & 0x20) {
                amount = 1.0f;
            }
            vector[1] += amount;
        }
        if (entity->unkC8 & 0x3F) {
            unkE0->unk44 = EntityAi_Accelerate(entity, unkE0->unk44);
            amount = unkE0->unk44;
            if (entity->unkC8 & 0x40) {
                amount += amount;
            }
            sceVu0RotMatrixZ(ENTITY_SCRATCH_MATRIX, D_00431AF0, entity->model->unkD0[2]);
            sceVu0RotMatrixX(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[0]);
            sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[1]);
            Vector_ApplyMatrix(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
            Vector_Normalize(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR);
            Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, amount);
            entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
            entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
            entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
        } else {
            unkE0->unk44 = 0.0f;
        }
    } else {
        unkE0->unk44 = 0.0f;
    }
    if (entity->unkC8 & 1) {
        if (entity->unkC8 & 0x40) {
            Motion_Start(entity, 4, 0, entity->params->unkC4 >> 1, 0);
        } else {
            Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
        }
    } else if (entity->unkC8 & 2) {
        Motion_Start(entity, 5, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x10) {
        Motion_Start(entity, 8, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x20) {
        Motion_Start(entity, 9, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 4) {
        Motion_Start(entity, 6, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 8) {
        Motion_Start(entity, 7, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x300) {
        Motion_Start(entity, 3, 0, entity->params->unkC4, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    }
}
