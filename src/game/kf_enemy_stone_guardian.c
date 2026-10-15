/* StoneGuardian: the behaviour of entity type 23 (Entity.params->unk02, the index into D_003E6E40): its
   set-up function, which the table names, and the handlers it installs. Used by: Stone Guardian. The game
   names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm
   records by hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_map_object.h"
#include "kf_entity_world.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void EntityAi_Think(Entity *, s32);
void EntityAi_Move(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
void EntityAi_LowerWeightsAfterHit(char *);
void EntityAi_SetState(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionHeavy(Entity *);
int Motion_Start(void *, int, int, u32, int);
float EntityMath_AngleDifference(float, float);
int func_0019A3F0(void *, int);
float EntityMath_EaseAngleToward(float *, float, float);
extern u32 D_00446ED4;
extern PlayerData D_00413F80;
void EntityAi_SensePlayer(void *, int);
s32 EntityAi_CanAttack(Entity *);
int EntityAi_PickAttack(char *);
float EntityMath_DistanceXZ(void *, void *);
void Vector_Copy(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
float EntityMath_HeadingTo(float *, float *);
extern EntityIdTable D_0043E890;

static void StoneGuardian_Animate(Entity *entity);
static void StoneGuardian_OnHit(Entity *entity);
static void StoneGuardian_Move(Entity *entity);
static void StoneGuardian_TurnToSpawnHeading(Entity *entity);
static void StoneGuardian_HoldAnim1(Entity *entity);
static void StoneGuardian_ThinkHoldAnim17(Entity *entity, s32 unknown);
static void StoneGuardian_ThinkMode2StayInBounds(Entity *entity, s32 unknown);
static void StoneGuardian_Mode2WalkToPost(Entity *entity);
static void StoneGuardian_Mode2ChaseInBounds(Entity *entity);
static void StoneGuardian_Mode2ReturnToSpawn(Entity *entity);
static void StoneGuardian_ThinkMode3StayInBounds(Entity *entity, s32 unknown);
static void StoneGuardian_Mode3WalkToPost(Entity *entity);
static void StoneGuardian_Mode3ChaseInBounds(Entity *entity);
static void StoneGuardian_Mode3ReturnToSpawn(Entity *entity);
static void StoneGuardian_ThinkMode4StayInBounds(Entity *entity, s32 unknown);
static void StoneGuardian_Mode4WalkToPost(Entity *entity);
static void StoneGuardian_Mode4ChaseInBounds(Entity *entity);
static void StoneGuardian_Mode4ReturnToSpawn(Entity *entity);
static void StoneGuardian_ThinkMode5StayInBounds(Entity *entity, s32 unknown);
static void StoneGuardian_Mode5WalkToPost(Entity *entity);
static void StoneGuardian_Mode5ChaseInBounds(Entity *entity);
static void StoneGuardian_Mode5ReturnToSpawn(Entity *entity);
static void StoneGuardian_ThinkMode6HideSequence(Entity *entity, s32 unknown);
static void StoneGuardian_Mode6StayHidden(Entity *entity, s32 unknown);
static void StoneGuardian_Mode6WaitTurnHidden(Entity *entity, s32 unknown);
static void StoneGuardian_Mode6LeaveHiding(Entity *entity, s32 unknown);
static void StoneGuardian_PlayAnim9(Entity *entity);
static void StoneGuardian_Mode6WaitTurnShown(Entity *entity, s32 unknown);
static void StoneGuardian_Mode6ReturnToHiding(Entity *entity, s32 unknown);
static void StoneGuardian_PlayAnim8(Entity *entity);
static void StoneGuardian_Mode6WalkToHidePoint(Entity *entity, s32 unknown);
static s32 StoneGuardian_Mode6IsStepFree(Entity *entity, s32 value);
static s32 StoneGuardian_Mode6IsNearestWaiting(Entity *entity, f32 limit);

/* Set-up of a type 23 entity: clears its work area and installs the type's handlers. */
void StoneGuardian_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    s32 i;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->animate = (void (*)(void))StoneGuardian_Animate;
    unkE0->on_hit = (void (*)(void))StoneGuardian_OnHit;
    unkE0->move = (void (*)(void))StoneGuardian_Move;
    unkE0->death = (void (*)(void))EntityAi_Death;
    unkE0->dormant = NULL;
    if (entity->spawn_info->unk40 == 1) {
        unkE0->move = NULL;
        unkE0->think = StoneGuardian_ThinkHoldAnim17;
    } else if (entity->spawn_info->unk40 == 2) {
        unkE0->think = StoneGuardian_ThinkMode2StayInBounds;
    } else if (entity->spawn_info->unk40 == 3) {
        unkE0->think = StoneGuardian_ThinkMode3StayInBounds;
    } else if (entity->spawn_info->unk40 == 4) {
        unkE0->think = StoneGuardian_ThinkMode4StayInBounds;
    } else if (entity->spawn_info->unk40 == 5) {
        unkE0->think = StoneGuardian_ThinkMode5StayInBounds;
    } else if (entity->spawn_info->unk40 == 6) {
        unkE0->think = StoneGuardian_ThinkMode6HideSequence;
        unkE0->move = (void (*)(void))EntityAi_Move;
        entity->model->position[0] = -16200.0f;
        entity->model->position[1] = 3030.0f;
        entity->model->position[2] = 4900.0f;
        entity->unkD6 = 0;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        unkE0->unk7A = 1;
    } else {
        unkE0->think = EntityAi_Think;
        unkE0->move = (void (*)(void))EntityAi_Move;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    unkE0->unk40 = 0.5f;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
    unkE0->unk48 = entity->spawn_info->spawn_x;
    unkE0->unk4C = entity->spawn_info->spawn_y;
    unkE0->unk50 = entity->spawn_info->spawn_z;
}

/* The animate handler: as EntityAi_Animate without the head turn. */
static void StoneGuardian_Animate(Entity *entity) {
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

/* The on_hit handler: as EntityAi_OnHit, but only a heavy hit starts a reaction
   (EntityAi_ReactionHeavy). */
static void StoneGuardian_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    ENTITY_UNK5C(entity)->as_s32 = 0;
    if (((entity->flags >> 36) & 3) == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    EntityAi_SetState((char *)entity, 2);
}

/* The move handler: turns only while the command also walks or runs; picks animation 3 or 4
   walking, 11 or 12 for a turn on the spot, else 0; then clears the busy bits and the command. */
static void StoneGuardian_Move(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn = 0.0f;

    if (entity->unkC8 & 0x41) {
        if (entity->unkC8 & 0x100) {
            unkE0->unk1C -= entity->params->unk58;
            if (unkE0->unk1C < -entity->params->turn_limit) {
                unkE0->unk1C = -entity->params->turn_limit;
            }
        } else if (entity->unkC8 & 0x200) {
            unkE0->unk1C += entity->params->unk58;
            if (unkE0->unk1C > entity->params->turn_limit) {
                unkE0->unk1C = entity->params->turn_limit;
            }
        } else {
            unkE0->unk1C = turn;
        }
    } else {
        unkE0->unk1C = turn;
    }
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
    if (entity->unkC8 & 1) {
        if (entity->unkD1 != 3 && entity->unkD1 != 4) {
            if (entity->unkC8 & 0x40) {
                Motion_Start(entity, 4, 0, entity->params->unkC4 >> 1, 1);
            } else {
                Motion_Start(entity, 3, 0, entity->params->unkC4, 1);
            }
        } else if (entity->unkC8 & 0x40) {
            entity->unkD1 = 4;
            entity->model->unkFC->unk8C = entity->params->unk80[entity->unkD1];
        } else {
            entity->unkD1 = 3;
            entity->model->unkFC->unk8C = entity->params->unk80[entity->unkD1];
        }
    } else if (entity->unkC8 & 0x100) {
        Motion_Start(entity, 0xB, 0, entity->params->unkC4, 0);
    } else if (entity->unkC8 & 0x200) {
        Motion_Start(entity, 0xC, 0, entity->params->unkC4, 0);
    } else {
        Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    }
    entity->unkC6 = 0;
    entity->unkC8 = 0;
    entity->unkA0 = 0;
}

/* An action (busy bit 0x1000): turns on the spot (animation 12 or 11) to the heading the entity
   spawned with, EntitySpawnInfo.unk14, then goes to state 0 and thinks. */
static void StoneGuardian_TurnToSpawnHeading(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 frames;

    entity->unkC6 = 0x1000;
    switch (entity->action_step) {
    case 0:
        if (EntityMath_AngleDifference(entity->model->unkD0[1], entity->spawn_info->unk14) >= 0.0f) {
            Motion_Start(entity, 0xC, 0, entity->params->unkC4, 1);
        } else {
            Motion_Start(entity, 0xB, 0, entity->params->unkC4, 1);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            frames = func_0019A3F0(entity, entity->unkD1);
            entity->unkA0 = frames;
            if (entity->unk28 != 0) {
                entity->unkA0 = frames * 2;
            }
            entity->action_step++;
        }
        break;
    case 2:
        EntityMath_EaseAngleToward(&entity->model->unkD0[1], entity->spawn_info->unk14, entity->unkA0);
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk48 = entity->model->position[0];
            unkE0->unk4C = entity->model->position[1];
            unkE0->unk50 = entity->model->position[2];
            entity->prev_state = entity->state;
            entity->state = 0;
            unkE0->think(entity, 1);
        }
        break;
    }
}

/* The action of state 0 in modes 2 to 5: stands in animation 1 with flag bit 18 set. */
static void StoneGuardian_HoldAnim1(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    Motion_Start(entity, 1, 0, entity->params->unkC4, 0);
    unkE0->unk1C = 0.0f;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
}

/* The think handler of mode 1 (EntitySpawnInfo.unk40 is 1): holds animation 17 with flag bits 16,
   18, 21 to 24 and 28 set and starts no action. */
static void StoneGuardian_ThinkHoldAnim17(Entity *entity, s32 unknown) {
    s64 flags;

    Motion_Start(entity, 0x11, 0, 0, unknown);
    flags = entity->flags & ~0x3800;
    flags |= 0x400000;
    flags |= 0x800000;
    flags |= 0x01000000;
    flags |= 0x40000;
    flags |= 0x200000;
    flags |= 0x10000000;
    flags |= 0x10000;
    entity->flags = flags;
}

/* The think handler of mode 2: sets Entity.state itself and starts the action of the state
   (StoneGuardian_HoldAnim1, StoneGuardian_Mode2WalkToPost, StoneGuardian_Mode2ChaseInBounds,
   StoneGuardian_Mode2ReturnToSpawn). While bit 17 of the map object EntitySpawnInfo.variant names is
   clear the state is 2 with the player inside a fixed rectangle, else 1; once the bit is set it is
   3 until the entity is back at its place, then 0. Every frame it clamps the entity's position to
   fixed bounds. */
static void StoneGuardian_ThinkMode2StayInBounds(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 0);
    }
    if (entity->unkC6 == 0) {
        is_open = 0;
        map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->unk10 >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (D_00413F80.position[0] > -9700.0f && D_00413F80.position[0] < -700.0f &&
            D_00413F80.position[2] > -4800.0f && D_00413F80.position[2] < -3600.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == 0) {
                entity->state = 0;
            } else {
                entity->state = 3;
            }
        } else if (player_in_zone) {
            entity->state = 2;
        } else {
            entity->state = 1;
        }
        switch (entity->state) {
        case 0:
            EntityAi_StartAction(entity, StoneGuardian_HoldAnim1, unknown);
            unkE0->unk68 = entity->params->unk430;
            break;
        case 1:
            if (unkE0->unk68 == 0) {
                EntityAi_StartAction(entity, StoneGuardian_Mode2WalkToPost, unknown);
                unkE0->unk6A = entity->params->unk432;
            }
            break;
        case 2:
            if (unkE0->unk68 == 0) {
                EntityAi_StartAction(entity, StoneGuardian_Mode2ChaseInBounds, unknown);
                unkE0->unk6A = entity->params->unk432;
            }
            break;
        case 3:
            if (unkE0->unk68 == 0) {
                EntityAi_StartAction(entity, StoneGuardian_Mode2ReturnToSpawn, unknown);
            }
            break;
        default:
            entity->state = 0;
            break;
        }
        if (unkE0->unk68 > 0) {
            unkE0->unk68--;
        }
        if (unkE0->unk6A > 0) {
            unkE0->unk6A--;
        }
    }
    if (entity->model->position[0] < -8400.0f) {
        entity->model->position[0] = -8400.0f;
    }
    if (entity->model->position[0] > -1950.0f) {
        entity->model->position[0] = -1950.0f;
    }
    if (entity->model->position[2] > -4000.0f - entity->unk30) {
        entity->model->position[2] = -4000.0f - entity->unk30;
    }
    if (entity->model->position[0] < -3150.0f) {
        if (entity->model->position[2] < entity->unk30 + -4800.0f) {
            entity->model->position[2] = entity->unk30 + -4800.0f;
        }
    }
}

/* The action of state 1 in mode 2: attacks when EntityAi_CanAttack says so; else goes to a post picked
   by where the player is and turns to a fixed heading there. */
static void StoneGuardian_Mode2WalkToPost(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 post[4];
    f32 facing;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (!(s32)(entity->flags >> 63) && ((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    if (D_00413F80.position[0] < -6000.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4400.0f;
        post[3] = 0.0f;
        facing = 1.5707964f;
    } else if (D_00413F80.position[0] > -4350.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4400.0f;
        post[3] = 0.0f;
        facing = -1.5707964f;
    } else if (D_00413F80.position[2] < -4000.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = entity->unk30 + -4800.0f;
        post[3] = 0.0f;
        facing = 0.0f;
    } else {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4000.0f - entity->unk30;
        post[3] = 0.0f;
        facing = 3.1415927f;
    }
    if (entity->model->position[2] < -4800.0f) {
        unkE0->unk30[0] = entity->spawn_info->spawn_x;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4200.0f;
        entity->target = unkE0->unk30;
        entity->unkC8 = EntityAi_MoveCommand(entity, 1, 50.0f);
    } else if (EntityMath_DistanceXZ(entity->model->position, post) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->unkD0[1], facing);
        if (ABS(angle) > 0.5235988f) {
            if (angle >= 0.0f) {
                entity->unkC8 = 0x200;
            } else {
                entity->unkC8 = 0x100;
            }
        }
    } else {
        Vector_Copy(unkE0->unk30, post);
        entity->target = unkE0->unk30;
        entity->unkC8 = EntityAi_MoveCommand(entity, 1, 50.0f) | 0x400;
    }
    ((void (*)(Entity *))unkE0->move)(entity);
}

/* The action of state 2 in mode 2: attacks when EntityAi_CanAttack says so; else goes for the player,
   stopping at the edge of its bounds and turning to the player there. */
static void StoneGuardian_Mode2ChaseInBounds(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (!(s32)(entity->flags >> 63) && ((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    range = 100.0f;
    if (D_00413F80.position[0] < -8400.0f) {
        unkE0->unk30[0] = entity->unk30 + -8400.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = EntityMath_HeadingTo(entity->model->position, D_00413F80.position);
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else if (D_00413F80.position[0] > -1950.0f) {
        unkE0->unk30[0] = -1950.0f - entity->unk30;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = EntityMath_HeadingTo(entity->model->position, D_00413F80.position);
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else if (D_00413F80.position[2] > -4000.0f) {
        unkE0->unk30[0] = -5200.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[2] = -4000.0f - entity->unk30;
        unkE0->unk30[3] = 0.0f;
        if (EntityMath_DistanceXZ(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = EntityMath_HeadingTo(entity->model->position, D_00413F80.position);
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else if (D_00413F80.position[2] < -4800.0f && D_00413F80.position[0] > -5600.0f &&
               D_00413F80.position[0] < -4800.0f) {
        unkE0->unk30[0] = -5200.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[2] = entity->unk30 + -4800.0f;
        unkE0->unk30[3] = 0.0f;
        if (EntityMath_DistanceXZ(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = EntityMath_HeadingTo(entity->model->position, D_00413F80.position);
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else {
        entity->target = &D_00413F80;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
    }
    ((void (*)(Entity *))unkE0->move)(entity);
}

/* The action of state 3 in mode 2: after the pause in work word unk6A goes back to its spawn point,
   turns to its spawn heading (StoneGuardian_TurnToSpawnHeading) and goes to state 0; still attacks when
   EntityAi_CanAttack says so. */
static void StoneGuardian_Mode2ReturnToSpawn(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    if (unkE0->unk6A > 0) {
        unkE0->move();
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (!(s32)(entity->flags >> 63) && ((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    if (entity->model->position[0] < -3150.0f) {
        unkE0->unk30[0] = entity->spawn_info->spawn_x;
        entity->target = unkE0->unk30;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4200.0f;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else if (EntityMath_DistanceXZ(entity->model->position, entity->spawn_info) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->spawn_info->unk14);
        if (ABS(angle) > 0.17453292f) {
            EntityAi_StartAction(entity, StoneGuardian_TurnToSpawnHeading, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = 0;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    ((void (*)(Entity *))unkE0->move)(entity);
}

/* The think handler of mode 3: as StoneGuardian_ThinkMode2StayInBounds with its own rectangle,
   bounds and actions (StoneGuardian_Mode3WalkToPost, StoneGuardian_Mode3ChaseInBounds,
   StoneGuardian_Mode3ReturnToSpawn). */
static void StoneGuardian_ThinkMode3StayInBounds(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (entity->unkC6 == 0) {
        is_open = 0;
        map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->unk10 >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (D_00413F80.position[2] > -6000.0f && D_00413F80.position[2] < -2800.0f &&
            entity->model->position[2] >= -4800.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == 0) {
                entity->state = 0;
            } else {
                entity->state = 3;
            }
        } else if (player_in_zone) {
            entity->state = 2;
        } else {
            entity->state = 1;
        }
        switch (entity->state) {
        case 0:
            EntityAi_StartAction(entity, StoneGuardian_HoldAnim1, unknown);
            break;
        case 1:
            EntityAi_StartAction(entity, StoneGuardian_Mode3WalkToPost, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 2:
            EntityAi_StartAction(entity, StoneGuardian_Mode3ChaseInBounds, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 3:
            EntityAi_StartAction(entity, StoneGuardian_Mode3ReturnToSpawn, unknown);
            break;
        default:
            entity->state = 0;
            break;
        }
        if (unkE0->unk6A > 0) {
            unkE0->unk6A--;
        }
    }
    if (entity->model->position[0] < 4800.0f) {
        entity->model->position[0] = 4800.0f;
    }
    if (entity->model->position[0] > 6650.0f - entity->unk30) {
        entity->model->position[0] = 6650.0f - entity->unk30;
    }
    if (entity->model->position[2] > -4000.0f - entity->unk30) {
        entity->model->position[2] = -4000.0f - entity->unk30;
    }
}

/* The action of state 1 in mode 3: attacks when EntityAi_CanAttack says so; else goes to a fixed post
   and turns to a fixed heading there. */
static void StoneGuardian_Mode3WalkToPost(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 home[4];
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    home[0] = entity->spawn_info->spawn_x;
    home[1] = entity->spawn_info->spawn_y;
    home[2] = -4400.0f;
    home[3] = 0.0f;
    range = 100.0f;
    if (EntityMath_DistanceXZ(entity->model->position, home) < range) {
        angle = EntityMath_AngleDifference(entity->model->unkD0[1], 3.1415927f);
        if (ABS(angle) > 1.5707964f) {
            if (angle >= 0.0f) {
                entity->unkC8 = 0x200;
            } else {
                entity->unkC8 = 0x100;
            }
        }
    } else {
        Vector_Copy(unkE0->unk30, home);
        entity->target = unkE0->unk30;
        entity->unkC8 = EntityAi_MoveCommand(entity, 1, 50.0f) | 0x400;
    }
    ((void (*)(Entity *))unkE0->move)(entity);
}

/* The action of state 2 in mode 3: attacks when EntityAi_CanAttack says so; else goes for the player,
   stopping at the edge of its bounds and turning to the player there. */
static void StoneGuardian_Mode3ChaseInBounds(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    range = 100.0f;
    if (D_00413F80.position[0] < 4800.0f) {
        unkE0->unk30[0] = 4800.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = EntityMath_HeadingTo(entity->model->position, D_00413F80.position);
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else if (D_00413F80.position[0] > 6650.0f) {
        unkE0->unk30[0] = 6650.0f - entity->unk30;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = EntityMath_HeadingTo(entity->model->position, D_00413F80.position);
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else {
        entity->target = &D_00413F80;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + range + 30.0f);
    }
    ((void (*)(Entity *))unkE0->move)(entity);
}

/* The action of state 3 in mode 3: after the pause in work word unk6A goes back to its spawn point,
   turns to its spawn heading (StoneGuardian_TurnToSpawnHeading) and goes to state 0; still attacks when
   EntityAi_CanAttack says so. */
static void StoneGuardian_Mode3ReturnToSpawn(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    if (unkE0->unk6A > 0) {
        unkE0->move();
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    if (entity->model->position[0] < 5450.0f || entity->model->position[0] > 6150.0f) {
        unkE0->unk30[0] = entity->spawn_info->spawn_x;
        entity->target = unkE0->unk30;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4200.0f;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else if (EntityMath_DistanceXZ(entity->model->position, entity->spawn_info) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->spawn_info->unk14);
        if (ABS(angle) > 0.17453292f) {
            EntityAi_StartAction(entity, StoneGuardian_TurnToSpawnHeading, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = 0;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    ((void (*)(Entity *))unkE0->move)(entity);
}

/* The think handler of mode 4: as StoneGuardian_ThinkMode3StayInBounds with its own rectangle,
   bounds and actions (StoneGuardian_Mode4WalkToPost, StoneGuardian_Mode4ChaseInBounds,
   StoneGuardian_Mode4ReturnToSpawn). */
static void StoneGuardian_ThinkMode4StayInBounds(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (entity->unkC6 == 0) {
        is_open = 0;
        map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->unk10 >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (D_00413F80.position[2] > -6000.0f && D_00413F80.position[2] < -2800.0f &&
            entity->model->position[2] <= -4000.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == 0) {
                entity->state = 0;
            } else {
                entity->state = 3;
            }
        } else if (player_in_zone) {
            entity->state = 2;
        } else {
            entity->state = 1;
        }
        switch (entity->state) {
        case 0:
            EntityAi_StartAction(entity, StoneGuardian_HoldAnim1, unknown);
            break;
        case 1:
            EntityAi_StartAction(entity, StoneGuardian_Mode4WalkToPost, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 2:
            EntityAi_StartAction(entity, StoneGuardian_Mode4ChaseInBounds, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 3:
            EntityAi_StartAction(entity, StoneGuardian_Mode4ReturnToSpawn, unknown);
            break;
        default:
            entity->state = 0;
            break;
        }
        if (unkE0->unk6A > 0) {
            unkE0->unk6A--;
        }
    }
    if (entity->model->position[0] < entity->unk30 + 6150.0f) {
        entity->model->position[0] = entity->unk30 + 6150.0f;
    }
    if (entity->model->position[0] > 8000.0f) {
        entity->model->position[0] = 8000.0f;
    }
    if (entity->model->position[2] < entity->unk30 + -4800.0f) {
        entity->model->position[2] = entity->unk30 + -4800.0f;
    }
}

/* The action of state 1 in mode 4: the same as StoneGuardian_Mode3WalkToPost. */
static void StoneGuardian_Mode4WalkToPost(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 home[4];
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    home[0] = entity->spawn_info->spawn_x;
    home[1] = entity->spawn_info->spawn_y;
    home[2] = -4400.0f;
    home[3] = 0.0f;
    range = 100.0f;
    if (EntityMath_DistanceXZ(entity->model->position, home) < range) {
        angle = EntityMath_AngleDifference(entity->model->unkD0[1], 3.1415927f);
        if (ABS(angle) > 1.5707964f) {
            if (angle >= 0.0f) {
                entity->unkC8 = 0x200;
            } else {
                entity->unkC8 = 0x100;
            }
        }
    } else {
        Vector_Copy(unkE0->unk30, home);
        entity->target = unkE0->unk30;
        entity->unkC8 = EntityAi_MoveCommand(entity, 1, 50.0f) | 0x400;
    }
    ((void (*)(Entity *))unkE0->move)(entity);
}

/* The action of state 2 in mode 4: as StoneGuardian_Mode3ChaseInBounds with its own bounds. */
static void StoneGuardian_Mode4ChaseInBounds(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    range = 100.0f;
    if (D_00413F80.position[0] < 6150.0f) {
        unkE0->unk30[0] = entity->unk30 + 6150.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = EntityMath_HeadingTo(entity->model->position, D_00413F80.position);
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else if (D_00413F80.position[0] > 8000.0f) {
        unkE0->unk30[0] = 8000.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (EntityMath_DistanceXZ(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = EntityMath_HeadingTo(entity->model->position, D_00413F80.position);
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else {
        entity->target = &D_00413F80;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + range + 30.0f);
    }
    ((void (*)(Entity *))unkE0->move)(entity);
}

/* The action of state 3 in mode 4: as StoneGuardian_Mode3ReturnToSpawn with its own bounds. */
static void StoneGuardian_Mode4ReturnToSpawn(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    if (unkE0->unk6A > 0) {
        unkE0->move();
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    if (entity->model->position[0] < 6650.0f || entity->model->position[0] > 7350.0f) {
        unkE0->unk30[0] = entity->spawn_info->spawn_x;
        entity->target = unkE0->unk30;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4200.0f;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else if (EntityMath_DistanceXZ(entity->model->position, entity->spawn_info) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->spawn_info->unk14);
        if (ABS(angle) > 0.17453292f) {
            EntityAi_StartAction(entity, StoneGuardian_TurnToSpawnHeading, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = 0;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    ((void (*)(Entity *))unkE0->move)(entity);
}

/* The think handler of mode 5: as StoneGuardian_ThinkMode3StayInBounds with its own rectangle,
   bounds and actions (StoneGuardian_Mode5WalkToPost, StoneGuardian_Mode5ChaseInBounds,
   StoneGuardian_Mode5ReturnToSpawn). */
static void StoneGuardian_ThinkMode5StayInBounds(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (entity->unkC6 == 0) {
        is_open = 0;
        map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->unk10 >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (D_00413F80.position[0] > -20300.0f && D_00413F80.position[0] < -13350.0f &&
            D_00413F80.position[2] > -250.0f && D_00413F80.position[2] < 1800.0f &&
            entity->model->position[2] <= 950.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == 0) {
                entity->state = 0;
            } else {
                entity->state = 3;
            }
        } else if (player_in_zone) {
            entity->state = 2;
        } else {
            entity->state = 1;
        }
        switch (entity->state) {
        case 0:
            EntityAi_StartAction(entity, StoneGuardian_HoldAnim1, unknown);
            break;
        case 1:
            EntityAi_StartAction(entity, StoneGuardian_Mode5WalkToPost, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 2:
            EntityAi_StartAction(entity, StoneGuardian_Mode5ChaseInBounds, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 3:
            EntityAi_StartAction(entity, StoneGuardian_Mode5ReturnToSpawn, unknown);
            break;
        default:
            entity->state = 0;
            break;
        }
        if (unkE0->unk6A > 0) {
            unkE0->unk6A--;
        }
    }
    if (entity->model->position[0] < -18300.0f) {
        entity->model->position[0] = -18300.0f;
    }
    if (entity->model->position[0] > -16050.0f) {
        entity->model->position[0] = -16050.0f;
    }
    if (entity->model->position[2] < 600.0f) {
        entity->model->position[2] = 600.0f;
    }
}

/* The action of state 1 in mode 5: as StoneGuardian_Mode3WalkToPost with its own post and heading. */
static void StoneGuardian_Mode5WalkToPost(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 home[4];
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    home[0] = entity->spawn_info->spawn_x;
    home[1] = entity->model->position[1];
    home[2] = 600.0f;
    home[3] = 0.0f;
    range = 100.0f;
    if (EntityMath_DistanceXZ(entity->model->position, home) < range) {
        angle = EntityMath_AngleDifference(entity->model->unkD0[1], 0.0f);
        if (ABS(angle) > 1.5707964f) {
            if (angle >= 0.0f) {
                entity->unkC8 = 0x200;
            } else {
                entity->unkC8 = 0x100;
            }
        }
    } else {
        Vector_Copy(unkE0->unk30, home);
        entity->target = unkE0->unk30;
        entity->unkC8 = EntityAi_MoveCommand(entity, 1, 50.0f) | 0x400;
    }
    ((void (*)(Entity *))unkE0->move)(entity);
}

/* The action of state 2 in mode 5: as StoneGuardian_Mode3ChaseInBounds with its own bounds. */
static void StoneGuardian_Mode5ChaseInBounds(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction;
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    range = 100.0f;
    if (D_00413F80.position[0] < -18300.0f) {
        unkE0->unk30[0] = -18300.0f;
        unkE0->unk30[1] = entity->model->position[1];
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = 600.0f;
        if (EntityMath_DistanceXZ(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = EntityMath_HeadingTo(entity->model->position, D_00413F80.position);
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else if (D_00413F80.position[0] > -16050.0f) {
        unkE0->unk30[0] = -16050.0f;
        unkE0->unk30[1] = entity->model->position[1];
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = 600.0f;
        if (EntityMath_DistanceXZ(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
        } else {
            entity->target = &D_00413F80;
            direction = EntityMath_HeadingTo(entity->model->position, D_00413F80.position);
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else {
        entity->target = &D_00413F80;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + range + 30.0f);
    }
    ((void (*)(Entity *))unkE0->move)(entity);
}

/* The action of state 3 in mode 5: as StoneGuardian_Mode3ReturnToSpawn with its own bounds. */
static void StoneGuardian_Mode5ReturnToSpawn(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    if (unkE0->unk6A > 0) {
        unkE0->move();
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: EntityAi_CanAttack takes the entity
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    if (entity->model->position[0] < -17900.0f || entity->model->position[0] > -17200.0f) {
        unkE0->unk30[0] = entity->spawn_info->spawn_x;
        entity->target = unkE0->unk30;
        unkE0->unk30[1] = entity->model->position[1];
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = 600.0f;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else if (EntityMath_DistanceXZ(entity->model->position, entity->spawn_info) < 100.0f) {
        angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->spawn_info->unk14);
        if (ABS(angle) > 0.17453292f) {
            EntityAi_StartAction(entity, StoneGuardian_TurnToSpawnHeading, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = 0;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    }
    ((void (*)(Entity *))unkE0->move)(entity);
}

/* The think handler of mode 6: a sequence kept in work byte unk7A (1 to 7; at 0 it is
   EntityAi_Think), driven by whether the two map objects EntitySpawnInfo.variant and unk42 name
   both have bit 17 set. While both are set the entity is kept unseen at a fixed point (steps 1 and
   2); when one clears it comes out, one entity of the five in D_0043E890 at a time (step 3,
   StoneGuardian_PlayAnim9, then 0); when both are set again it goes back the same way (steps 4 to 7,
   StoneGuardian_PlayAnim8). The functions named Mode6... do the work of a step. */
static void StoneGuardian_ThinkMode6HideSequence(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 post[4];
    MapObject *map_object;
    s32 first_open;
    s32 second_open;
    s32 both_open;

    first_open = 0;
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
    if (map_object != NULL) {
        first_open = (map_object->unk10 >> 17) & 1;
    }
    second_open = 0;
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk42);
    if (map_object != NULL) {
        second_open = (map_object->unk10 >> 17) & 1;
    }
    both_open = first_open && second_open;
    if (entity->unkC6 != 0) {
        return;
    }
    if (!both_open && unkE0->unk7A != 0) {
        if (EntityMath_DistanceXZ(entity->spawn_info, &D_00413F80) > 5400.0f) {
            Vector_Copy(entity->model->position, entity->spawn_info);
            unkE0->unk7A = 0;
        }
    }
    switch (unkE0->unk7A) {
    case 1:
        if (!both_open) {
            unkE0->unk7A++;
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        } else {
            StoneGuardian_Mode6StayHidden(entity, unknown);
        }
        break;
    case 2:
        if (!both_open) {
            if (!StoneGuardian_Mode6IsStepFree(entity, 3) || !StoneGuardian_Mode6IsStepFree(entity, 6) || !StoneGuardian_Mode6IsStepFree(entity, 7)) {
                StoneGuardian_Mode6WaitTurnHidden(entity, unknown);
            } else {
                unkE0->unk7A++;
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
            }
        } else {
            unkE0->unk7A = 1;
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        }
        break;
    case 3:
        if (!both_open) {
            StoneGuardian_Mode6LeaveHiding(entity, unknown);
        } else {
            unkE0->unk7A = 7;
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        }
        break;
    case 4:
        post[0] = -18450.0f;
        post[1] = 3190.0f;
        post[2] = 4900.0f;
        unkE0->unk54 = EntityMath_DistanceXZ(entity->model->position, post);
        unkE0->unk7A++;
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        break;
    case 5:
        if (!both_open) {
            unkE0->unk7A = 0;
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        } else if (StoneGuardian_Mode6IsStepFree(entity, 3) && StoneGuardian_Mode6IsStepFree(entity, 6) && StoneGuardian_Mode6IsStepFree(entity, 7) &&
                   StoneGuardian_Mode6IsNearestWaiting(entity, unkE0->unk54)) {
            unkE0->unk7A++;
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        } else {
            StoneGuardian_Mode6WaitTurnShown(entity, unknown);
        }
        break;
    case 6:
        if (!both_open) {
            unkE0->unk7A = 0;
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        } else {
            StoneGuardian_Mode6ReturnToHiding(entity, unknown);
        }
        break;
    case 7:
        if (!both_open) {
            unkE0->unk7A = 3;
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        } else {
            StoneGuardian_Mode6WalkToHidePoint(entity, unknown);
        }
        break;
    default:
        ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        entity->unkD6 = 0x80;
        if (both_open) {
            unkE0->unk7A = 4;
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
        } else {
            EntityAi_Think(entity, unknown);
        }
        break;
    }
}

/* Step 1 of StoneGuardian_ThinkMode6HideSequence: keeps the entity unseen (Entity.unkD6 0; flag bits 18, 20, 21,
   26, 27) at a fixed point. */
static void StoneGuardian_Mode6StayHidden(Entity *entity, s32 unknown) {
    EntityModel *model;

    model = entity->model;
    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkD6 = 0;
    model->position[0] = -16200.0f;
    model->position[1] = 3030.0f;
    model->position[2] = 4900.0f;
    model->unkD0[1] = 1.5707964f;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    EntityAi_StartAction(entity, (void (*)(Entity *))entity->unkE0.move, unknown);
}

/* Step 2 of StoneGuardian_ThinkMode6HideSequence: the same as StoneGuardian_Mode6StayHidden. What
   differs is when the think handler calls it: one of the two map objects has its bit clear, and
   another of the five entities is still at step 3, 6 or 7 (StoneGuardian_Mode6IsStepFree). */
static void StoneGuardian_Mode6WaitTurnHidden(Entity *entity, s32 unknown) {
    EntityModel *model = entity->model;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkD6 = 0;
    model->position[0] = -16200.0f;
    model->position[1] = 3030.0f;
    model->position[2] = 4900.0f;
    model->unkD0[1] = 1.5707964f;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    EntityAi_StartAction(entity, (void (*)(Entity *))entity->unkE0.move, unknown);
}

/* Step 3 of StoneGuardian_ThinkMode6HideSequence: shows the entity, walks it to a second fixed point, turns it
   to heading 90 degrees and starts StoneGuardian_PlayAnim9. */
static void StoneGuardian_Mode6LeaveHiding(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *point;
    f32 angle;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    entity->unkD6 = 0x80;
    unkE0->unk30[0] = -18050.0f;
    unkE0->unk30[1] = 3030.0f;
    unkE0->unk30[2] = 4900.0f;
    if (entity->model->position[1] > 3031.0f) {
        unkE0->unk7A = 0;
        ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
    } else {
        point = unkE0->unk30;
        if (EntityMath_DistanceXZ(entity->model->position, point) < range) {
            ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], 1.5707964f);
            if (ABS(angle) > 0.17453292f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
                ENTITY_FLAG_BITS(entity)->bits11 = 0;
            } else {
                EntityAi_StartAction(entity, StoneGuardian_PlayAnim9, unknown);
                return;
            }
        } else {
            entity->target = point;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, range);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* An action (busy bit 0x1000) of mode 6, which StoneGuardian_Mode6LeaveHiding starts: plays
   animation 9 to its end with Entity.unkD6 at 0x80, then puts the sequence at 0 (from there the
   think handler is EntityAi_Think) and thinks. */
static void StoneGuardian_PlayAnim9(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    entity->unkD6 = 0x80;
    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 9, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            unkE0->unk7A = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit17 = TRUE;
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

/* Step 5 of StoneGuardian_ThinkMode6HideSequence: the entity is seen (Entity.unkD6 0x80, the flag
   bits of the other steps cleared) and stands, the move action with no command. The think handler
   calls it while both map objects have their bit set and it is not yet this entity's turn to go
   back: another one is at step 3, 6 or 7 (StoneGuardian_Mode6IsStepFree) or waits nearer
   (StoneGuardian_Mode6IsNearestWaiting). */
static void StoneGuardian_Mode6WaitTurnShown(Entity *entity, s32 unknown) {
    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkD6 = 0x80;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    EntityAi_StartAction(entity, (void (*)(Entity *))entity->unkE0.move, unknown);
}

/* Step 6 of StoneGuardian_ThinkMode6HideSequence: walks the entity to a third fixed point, turns it to heading
   -90 degrees and starts StoneGuardian_PlayAnim8. */
static void StoneGuardian_Mode6ReturnToHiding(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *point;
    f32 angle;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    entity->unkD6 = 0x80;
    unkE0->unk30[0] = -18450.0f;
    unkE0->unk30[1] = 3190.0f;
    unkE0->unk30[2] = 4900.0f;
    if (entity->model->position[1] <= 3030.0f) {
        unkE0->unk7A = 7;
    } else {
        point = unkE0->unk30;
        if (EntityMath_DistanceXZ(entity->model->position, point) < range) {
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], -1.5707964f);
            if (ABS(angle) > 0.17453292f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
                ENTITY_FLAG_BITS(entity)->bits11 = 0;
            } else {
                EntityAi_StartAction(entity, StoneGuardian_PlayAnim8, unknown);
                return;
            }
        } else {
            entity->target = point;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, range);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* An action (busy bit 0x1000) of mode 6, which StoneGuardian_Mode6ReturnToHiding starts: plays
   animation 8 to its end, then puts the sequence at 7 (StoneGuardian_Mode6WalkToHidePoint) and
   thinks. */
static void StoneGuardian_PlayAnim8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    entity->unkD6 = 0x80;
    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 8, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            unkE0->unk7A = 7;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit17 = TRUE;
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

/* Step 7 of StoneGuardian_ThinkMode6HideSequence: walks the entity back to the first fixed point, then puts the
   sequence at 1. */
static void StoneGuardian_Mode6WalkToHidePoint(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *point = unkE0->unk30;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkD6 = 0x80;
    unkE0->unk30[0] = -16200.0f;
    unkE0->unk30[1] = 3030.0f;
    unkE0->unk30[2] = 4900.0f;
    if (EntityMath_DistanceXZ(entity->model->position, point) < range) {
        unkE0->unk7A = 1;
    } else {
        entity->target = point;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, range);
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* Helper of StoneGuardian_ThinkMode6HideSequence: whether none of the other live entities among the five in
   D_0043E890 is at step `value` of the sequence. */
static s32 StoneGuardian_Mode6IsStepFree(Entity *entity, s32 value) {
    EntityIdTable table = D_0043E890;
    s32 i;
    Entity *other;

    for (i = 0; i < 5; i++) {
        other = Entity_GetBySpawnId(table.ids[i]);
        if (other != NULL && (other->flags & 0xD) == 1 && other != entity && other->unkE0.unk7A == value) {
            return FALSE;
        }
    }
    return TRUE;
}

/* Helper of StoneGuardian_ThinkMode6HideSequence: whether none of the other live entities among the
   five in D_0043E890 is at step 5 with its work float unk54 below `limit`. unk54 is the distance to
   the post of step 6 that step 4 stored, and the caller passes its own, so TRUE means this entity
   is the nearest of those waiting. */
static s32 StoneGuardian_Mode6IsNearestWaiting(Entity *entity, f32 limit) {
    EntityIdTable table = D_0043E890;
    s32 i;
    Entity *other;
    EntityUnkE0 *other_unkE0;

    for (i = 0; i < 5; i++) {
        other = Entity_GetBySpawnId(table.ids[i]);
        if (other != NULL && (other->flags & 0xD) == 1 && other != entity) {
            other_unkE0 = &other->unkE0;
            if (other_unkE0->unk7A == 5 && other_unkE0->unk54 < limit) {
                return FALSE;
            }
        }
    }
    return TRUE;
}
