/* WiddaArcher: the behaviour of entity type 20 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Widda Archer Large, Widda Archer
   Small. The game names no monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to
   the chr.prm records by hit points and experience, with resistances and drops where two monsters share
   those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_map_object.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
int EntityAi_LookAtPlayer(Entity *, void *, s32, void *, f32);
double atan2(double, double);
float EntityMath_WrapAngle(float);
float EntityMath_EaseAngleToward(float *, float, float);
float atan2f(float, float);
float EntityMath_HeadingTo(float *, float *);
float EntityMath_AngleDifference(float, float);
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
extern PlayerData D_00413F80;
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
float EntityMath_AngleXZ(float *, char *);
int EntityAi_TurnCommand(Entity *);
int EntityAi_TurnFromPush(void *, void *);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
int EntityShot_LaunchAimed(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void EntityAi_SetAttackDelay(Entity *, s32);
extern u8 D_00349690[];
float EntityMath_DistanceXZ(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
int Script_Start(int, int);
extern int D_00446770;

static void WiddaArcher_Animate(Entity *entity);
static void WiddaArcher_PitchPartToPlayer(Entity *entity);
static void WiddaArcher_AimPartsAtTarget(Entity *entity);
static void WiddaArcher_Think(Entity *entity, s32 unknown);
static void func_00149610(Entity *entity, s32 unknown);
static void WiddaArcher_RangedAttack(Entity *entity);
static void WiddaArcher_ThinkUseMapObject(Entity *entity, s32 unknown);
static void WiddaArcher_WalkToMapObject(Entity *entity, s32 unknown);
static void WiddaArcher_StartMapObjectScript(Entity *entity);

/* Set-up of a type 20 entity: clears its work area and installs the type's handlers. */
void WiddaArcher_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    if (entity->spawn_info->unk40 != 0) {
        unkE0->think = WiddaArcher_ThinkUseMapObject;
    } else {
        unkE0->think = WiddaArcher_Think;
    }
    unkE0->animate = (void (*)(void))WiddaArcher_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
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
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate; during animations 23 and 24, once blended in,
   WiddaArcher_PitchPartToPlayer pitches a model part (else work float unk58 is cleared), and
   WiddaArcher_AimPartsAtTarget turns two more. */
static void WiddaArcher_Animate(Entity *entity) {
    f32 limits[4];
    u32 blend_frames; // match: converted to float as unsigned
    EntityUnkE0 *unkE0;

    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_BlendToPose(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->bits11);
        }
        if ((u32)((u8)entity->unkD1 - 0x17) < 2 && entity->model->unkFC->unk88 == 0) {
            WiddaArcher_PitchPartToPlayer(entity);
        } else {
            unkE0->unk58 = 0.0f;
        }
        WiddaArcher_AimPartsAtTarget(entity);
        limits[1] = entity->params->unk430 * 0.017453292f;
        limits[0] = -entity->params->unk430 * 0.017453292f;
        limits[3] = entity->params->unk432 * 0.017453292f;
        limits[2] = -entity->params->unk432 * 0.017453292f;
        if (ENTITY_FLAG_BITS(entity)->bit9) {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, ENTITY_FLAG_BITS(entity)->bit9, limits, 12.0f);
        } else {
            EntityAi_LookAtPlayer(entity, unkE0->unk20, 0, NULL, 8.0f);
        }
    }
}

/* Helper of WiddaArcher_Animate: pitches the model part EntityParams.unk434 names towards the
   player, at most 50 degrees either way (work float unk58, rate 5). */
static void WiddaArcher_PitchPartToPlayer(Entity *entity) {
    EntityController *controller;
    EntityModelPart80 *part;
    EntityUnkE0 *unkE0;
    f32 pitch;

    controller = ENTITY_CONTROLLER(entity);
    part = &((EntityModelPart80 *)entity->model->unkF8)[controller->unk434];
    unkE0 = &entity->unkE0;
    if (part != NULL) {
        pitch = EntityMath_WrapAngle(atan2(D_00413F80.position[1] - entity->model->position[1], entity->distance_to_player));
        if (0.87266463f < pitch) {
            pitch = 0.87266463f;
        } else if (pitch < -0.87266463f) {
            pitch = -0.87266463f;
        }
        EntityMath_EaseAngleToward(&unkE0->unk58, pitch, 5.0f);
        part->unk50[0] = unkE0->unk58;
    }
}

/* Helper of WiddaArcher_Animate: turns model parts 20 and 21 towards the target, pitch from the
   player's height (at most 30 degrees) and yaw from the bearing (at most 40), each part by half of
   it; the angles are work floats unk48 to unk54. Without a target or while the entity is busy it
   reads the angles of parts 30 and 31 into them instead. */
static void WiddaArcher_AimPartsAtTarget(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelPart80 *parts = (EntityModelPart80 *)entity->model->unkF8;
    f32 pitch;
    f32 pitch_limit;
    f32 half_pitch;
    f32 yaw;
    f32 yaw_limit;
    f32 half_yaw;
    f32 yaw_to_target;

    if (entity->target == NULL || entity->unkC6 != 0) {
        unkE0->unk48 = parts[30].unk50[0];
        unkE0->unk4C = parts[30].unk50[1];
        unkE0->unk50 = parts[31].unk50[0];
        unkE0->unk54 = parts[31].unk50[1];
    } else {
        pitch = atan2f(D_00413F80.position[1] - entity->model->position[1], entity->distance_to_player);
        pitch_limit = 0.5235988f;
        if (pitch_limit < pitch) {
            pitch = pitch_limit;
        } else if (pitch < -pitch_limit) {
            pitch = -pitch_limit;
        }
        half_pitch = pitch * 0.5f;
        EntityMath_EaseAngleToward(&unkE0->unk48, half_pitch, 10.0f);
        EntityMath_EaseAngleToward(&unkE0->unk50, half_pitch, 10.0f);
        parts[20].unk50[0] = unkE0->unk48;
        parts[21].unk50[0] = unkE0->unk50;
        yaw_to_target = EntityMath_HeadingTo(entity->model->position, entity->target);
        yaw = EntityMath_AngleDifference(entity->model->unkD0[1], yaw_to_target);
        yaw_limit = 0.6981317f;
        if (yaw_limit < yaw) {
            yaw = yaw_limit;
        } else if (yaw < -yaw_limit) {
            yaw = -yaw_limit;
        }
        half_yaw = yaw * 0.5f;
        EntityMath_EaseAngleToward(&unkE0->unk4C, half_yaw, 10.0f);
        EntityMath_EaseAngleToward(&unkE0->unk54, half_yaw, 10.0f);
        parts[20].unk50[1] = unkE0->unk4C;
        parts[21].unk50[1] = unkE0->unk54;
    }
}

/* The think handler: as EntityAi_Think, with func_00149610 for state 2. Before that it sets bit 40
   of model part 29's unk68 during frames 7 to 35 of animation 23 and frames 25 to 49 of animation
   24, and clears it otherwise. */
static void WiddaArcher_Think(Entity *entity, s32 unknown) {
    if (entity->model->unkFC->unk88 == 0 &&
        ((entity->unkD1 == 0x17 && entity->model->unkFC->unk82 >= 7 && entity->model->unkFC->unk82 < 0x24) ||
         (entity->unkD1 == 0x18 && entity->model->unkFC->unk82 >= 0x19 && entity->model->unkFC->unk82 < 0x32))) {
        ((EntityModelPart80 *)entity->model->unkF8)[29].unk68 |= 0x8000L << 25;
    } else {
        ((EntityModelPart80 *)entity->model->unkF8)[29].unk68 &= ~(0x8000L << 25);
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
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
        func_00149610(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

static void func_00149610(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (EntityAi_ShouldGuard(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, 300.0f, 0.6981317f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    side = EntityAi_PickCombatChoice(entity, 0);
    if (entity->unkA0 == 0 || side == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = side;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        EntityAi_LowerChoiceWeight(entity, side);
        switch (entity->unkCD) {
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, WiddaArcher_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 120;
            unkE0->unk78 = 0;
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    angle = EntityMath_AngleDifference(D_00413F80.unk14, EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position));
    if (entity->unk60 - entity->radius - 100.0f < 200.0f && ABS(angle) < 0.6981317f) {
        if (D_00446ED4 % 10U == entity->unkD7 % 10U &&
            rand() % 100 < (entity->params->unk0A - entity->unk2C) * 100 / entity->params->unk0A) {
            angle = EntityMath_AngleDifference(entity->model->unkD0[1], entity->unk68);
            if (ABS(angle) > 0.6981317f && ABS(angle) < 1.5707964f) {
                EntityAi_StartAction(entity, EntityAi_Dodge, 1);
                return;
            }
        }
    }
    if (unkE0->unk78 == 0) {
        turn = EntityAi_TurnFromPush(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x201;
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
        } else {
            entity->unkC8 = EntityAi_TurnCommand(entity);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, but its shots are aimed at a point 190
   below the player's position, within 10 degrees (EntityShot_LaunchAimed). */
static void WiddaArcher_RangedAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 attack_index = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, attack_index);
    f32 target[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, attack_index + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAGS(entity) &= ~0x3800;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAGS(entity) &= ~0x200;
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
            EntityAi_TurnToHeading(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        Vector_Copy(target, D_00413F80.position);
        target[1] -= 190.0f;
        EntityShot_LaunchAimed(entity, unk3D8, elapsed, entity->model->unkFC->unk82, target, 0.17453294f, 0.17453294f);
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            ENTITY_FLAGS(entity) |= 0x80000;
        } else {
            ENTITY_FLAGS(entity) &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk3D8->unk02);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* The think handler of a spawn whose EntitySpawnInfo.unk40 is set: as WiddaArcher_Think, but while
   the byte of D_00349690 that EntitySpawnInfo.unk42 and variant index is not 0 it calls
   WiddaArcher_WalkToMapObject instead of stepping the states. */
static void WiddaArcher_ThinkUseMapObject(Entity *entity, s32 unknown) {
    if (entity->model->unkFC->unk88 == 0 &&
        ((entity->unkD1 == 0x17 && entity->model->unkFC->unk82 >= 7 && entity->model->unkFC->unk82 < 0x24) ||
         (entity->unkD1 == 0x18 && entity->model->unkFC->unk82 >= 0x19 && entity->model->unkFC->unk82 < 0x32))) {
        ((EntityModelPart80 *)entity->model->unkF8)[29].unk68 |= 0x8000L << 25;
    } else {
        ((EntityModelPart80 *)entity->model->unkF8)[29].unk68 &= ~(0x8000L << 25);
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    if (D_00349690[entity->spawn_info->unk42 + entity->spawn_info->variant * 64] != 0) {
        WiddaArcher_WalkToMapObject(entity, unknown);
        return;
    }
    EntityAi_CheckFarFromSpawn((char *)entity);
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
        func_00149610(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* Helper of WiddaArcher_ThinkUseMapObject, which calls it in place of the state functions: walks to a
   point 100 in z from the map object EntitySpawnInfo.unk43 names, turns to the object there, and
   then starts WiddaArcher_StartMapObjectScript; until then it starts the move action. */
static void WiddaArcher_WalkToMapObject(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 stand_point[4];
    f32 face_point[4];
    f32 reach;
    MapObject *map_object;

    entity->flags &= ~0x200;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk43);
    if (map_object != NULL) {
        stand_point[0] = map_object->model->position.x;
        stand_point[1] = map_object->model->position.y;
        stand_point[2] = map_object->model->position.z + 100.0f;
        stand_point[3] = 0.0f;
        face_point[0] = map_object->model->position.x;
        face_point[1] = map_object->model->position.y;
        face_point[2] = map_object->model->position.z;
        face_point[3] = 0.0f;
    }
    reach = 100.0f;
    if (EntityMath_DistanceXZ(entity->model->position, stand_point) < reach) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        Vector_Copy(unkE0->unk30, face_point);
        entity->target = unkE0->unk30;
        entity->unkC8 = EntityAi_TurnCommand(entity);
        if (entity->unkC8 == 0) {
            EntityAi_StartAction(entity, WiddaArcher_StartMapObjectScript, unknown);
            return;
        }
    } else {
        Vector_Copy(unkE0->unk30, stand_point);
        entity->target = unkE0->unk30;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, reach * 0.5f);
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* An action (busy bit 0x1000) that WiddaArcher_WalkToMapObject starts: plays animation 17 to its
   end; on frame EntitySpawnInfo.unk44 it starts the script (Script_Start) of the map object
   EntitySpawnInfo.unk43 names, when the object has one and the entity is on map D_00446770. */
static void WiddaArcher_StartMapObjectScript(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0.0f;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 == entity->spawn_info->unk44) {
            map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk43);
            if (map_object != NULL && map_object->unk08->unk40 != 0 && entity->group_id == D_00446770) {
                Script_Start(map_object->unk08->unk40, 0);
            }
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
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
