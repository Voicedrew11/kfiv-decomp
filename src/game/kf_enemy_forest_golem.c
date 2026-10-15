/* Forest Golem: the behaviour of entity type 77 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. The Forest Golem: its one spawn (013.map,
   Holy Forest) says "Unable to move" when examined and takes the Forest Dew, the Golems' food. */
#include "kf_entity.h"
#include "kf_map.h"
#include "kf_entity_world.h"
#include "kf_map_object.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *entity, void (*handler)(Entity *), int run_now);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32); // match: an int result keeps $2 busy after the call
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
extern s32 D_003E6E40[][2];
extern char D_00349690[0x2000];
extern MapSlot D_0038CD20[];
extern int D_00446770;
extern EntityFrameTable D_0043EAD8;
int Motion_Start(void *, int, int, u32, int);
int Script_Start(int, int);
void func_001B57A0(u8, u8, u8);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
void EntityAi_CheckFarFromSpawn(char *);
void EntityAi_StepState(void *);
int EntityAi_ThinkReturn(void *, int);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkPursue(Entity *, s32);
extern float D_00413F80[];
int rand(void);
s32 EntityAi_ShouldGuard(Entity *, f32, f32);
s32 EntityAi_ShouldDodge(Entity *, f32, f32);
s32 EntityAi_PickCombatChoice(Entity *, s32);
void EntityAi_LowerChoiceWeight(Entity *, s32);
int EntityAi_PickAttack(char *);
int EntityAi_TurnFromPush(void *, void *);
float EntityMath_AngleDifference(float, float);
int EntityAi_TurnCommand(Entity *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
void EntityAi_MeleeAttack(Entity *);
void EntityAi_RangedAttack(Entity *);
void EntityEvent_OnDeath(Entity *);
void EntityEvent_Respawn(char *);
s32 EntityAi_CanAttack(Entity *);
float EntityMath_DistanceXZ(void *, void *);

static void ForestGolem_ThinkStayInPlace(Entity *entity, int invoke_now);
static void ForestGolem_Think(Entity *entity, s32 unknown);
static void ForestGolem_StartScriptAndRumble(Entity *entity);
static void ForestGolem_Move(Entity *entity);
static void func_0017A540(Entity *entity);
static void ForestGolem_ThinkByState(Entity *entity, s32 unknown);
static void func_0017A880(Entity *entity, s32 unknown);
static void ForestGolem_Death(Entity *entity);
static void ForestGolem_SignalMapObject(Entity *entity);
static void ForestGolem_ThinkUseMapObjectsPosX(Entity *entity, s32 unknown);
static void ForestGolem_WalkToMapObjectPosX(Entity *entity, s32 unknown);
static void ForestGolem_ThinkUseMapObjectsNegX(Entity *entity, s32 unknown);
static void ForestGolem_WalkToMapObjectNegX(Entity *entity, s32 unknown);

/* Set-up of a type 77 entity: clears its work area and installs the type's handlers. */
void ForestGolem_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind1740A8 *work = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    s32 i;
    s32 index;

    // match: in the three loops the pointer has to be formed inside the loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *dest = (u8 *)&entity->unkE0;

        dest[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->animate = EntityAi_Animate;
    unkE0->move = (void (*)(void))ForestGolem_Move;
    unkE0->on_hit = NULL;
    unkE0->dormant = NULL;
    unkE0->death = NULL;
    index = entity->params->unk432;
    index += entity->params->unk430 * 64;
    if (((u8 *)D_00349690)[index] != 0) {
        unkE0->unk7A = 1;
        unkE0->think = ForestGolem_ThinkStayInPlace;
    } else {
        unkE0->unk7A = 0;
        unkE0->think = ForestGolem_Think;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
    ENTITY_FLAG_BITS(entity)->bit16 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    work->unk48 = 1600.0f;
    work->unk4C = 0.0f;
    work->unk50 = -1430.0f;
}

/* The think handler the set-up function installs when the byte of D_00349690 that
   EntityParams.unk430 and unk432 index is set, and ForestGolem_StartScriptAndRumble when it ends: copies
   the position in work floats unk48 to unk50 to the model, relative to the map origin
   EntityParams.unk43A names when the entity is on another map than D_00446770, sets the heading to
   0 and starts the move action. */
static void ForestGolem_ThinkStayInPlace(Entity *entity, int invoke_now) {
    EntityUnkE0 *sub;
    ModelInstance *model;
    MapSlot *slot;
    Vector4 *origin;
    float x;
    float y;
    float z;

    sub = &entity->unkE0;
    model = ENTITY_MODEL(entity);
    x = sub->unk48;
    model->position.x = x;
    y = sub->unk4C;
    model->position.y = y;
    z = sub->unk50;
    model->position.z = z;
    if (entity->group_id != D_00446770) {
        slot = &D_0038CD20[D_00446770];
        origin = &slot->origins[ENTITY_CONTROLLER(entity)->unk43A];
        model->position.x = x - origin->x;
        model->position.y = y - origin->y;
        model->position.z = z - origin->z;
    }
    model->rotation.y = 0.0f;
    EntityAi_StartAction(entity, (void (*)(Entity *))sub->move, invoke_now);
}

/* The think handler (the set-up function installs it while that byte of D_00349690 is clear): while
   the entity is not busy it starts ForestGolem_StartScriptAndRumble once the byte is set, and the move
   action until then. */
static void ForestGolem_Think(Entity *entity, s32 unknown) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    if (entity->unkC6 == 0) {
        s32 index = entity->params->unk432;

        index += entity->params->unk430 * 64;
        if (((u8 *)D_00349690)[index] != 0) {
            EntityAi_StartAction(entity, ForestGolem_StartScriptAndRumble, unknown);
            return;
        }
        EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
    }
}

/* An action (busy bit 0x2000) that ForestGolem_Think starts: plays animation 17 to its end. Past
   frame EntityParams.unk434, on the map D_00446770, it starts script EntityParams.unk436
   (Script_Start); after that, on the four frames of D_0043EAD8 and on frame 440, it asks for a
   controller rumble (func_001B57A0) with a strength that falls off with the player's distance up to
   3000. At the end it keeps the model's position in work floats unk48 to unk50, puts
   ForestGolem_ThinkStayInPlace in the think slot and calls it. */
static void ForestGolem_StartScriptAndRumble(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    EntityFrameTable shake_frames = D_0043EAD8;
    u32 i;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x11, 0, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s16)entity->params->unk434 < entity->model->unkFC->unk82) { // match: read signed here
            if (entity->group_id == D_00446770) {
                // match: an int result keeps $2 busy after the call
                ((int (*)(s32, s32))Script_Start)(entity->params->unk436, 0);
                entity->action_step++;
            }
        }
        break;
    case 3:
        for (i = 0; i < 4; i++) {
            if (entity->model->unkFC->unk82 == shake_frames.frames[i]) {
                if (entity->distance_to_player < 3000.0f) {
                    func_001B57A0((3000.0f - entity->distance_to_player) * 64.0f / 3000.0f, 5, 0);
                }
            }
        }
        if (entity->model->unkFC->unk82 == 0x1B8) {
            if (entity->distance_to_player < 3000.0f) {
                func_001B57A0((3000.0f - entity->distance_to_player) * 32.0f / 3000.0f, 0x3C, 0);
            }
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk48 = entity->model->position[0];
            unkE0->unk4C = entity->model->position[1];
            unkE0->unk50 = entity->model->position[2];
            unkE0->think = ForestGolem_ThinkStayInPlace;
            unkE0->unk7A = 1;
            unkE0->think(entity, 1);
        }
        break;
    }
}

/* The move handler: this type stays where it is; keeps animation 0, and the motion never moves the
   entity. */
static void ForestGolem_Move(Entity *entity) {
    Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    ENTITY_FLAG_BITS(entity)->bits11 = 0;
}

static void func_0017A540(Entity *entity) {
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
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->death = (void (*)(void))ForestGolem_Death;
    unkE0->dormant = NULL;
    if (entity->spawn_info->variant == 1) {
        unkE0->think = ForestGolem_ThinkUseMapObjectsPosX;
    } else if (entity->spawn_info->variant == 2) {
        unkE0->think = ForestGolem_ThinkUseMapObjectsNegX;
    } else {
        unkE0->think = ForestGolem_ThinkByState;
    }
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 0);
    if (unk138 != NULL) {
        Vector_Copy(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* A think handler only func_0017A540 installs (for an EntitySpawnInfo.variant other than 1 and 2),
   a set-up function nothing refers to: as EntityAi_Think, with func_0017A880 for state 2. The same
   as ExpeditionSoldier_Think. */
static void ForestGolem_ThinkByState(Entity *entity, s32 unknown) {
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
        func_0017A880(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

static void func_0017A880(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 range;
    f32 angle;
    f32 turn;
    s32 next_action;
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    range = 300.0f;
    angle = 0.5235988f;
    if (EntityAi_ShouldGuard(entity, range, angle)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (EntityAi_ShouldDodge(entity, range, angle)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    next_action = EntityAi_PickCombatChoice(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->unkA0 == 0 || next_action == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = next_action;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        // match: called with one argument here
        ((void (*)(Entity *))EntityAi_LowerChoiceWeight)(entity);
        switch (entity->unkCD) {
        case 0:
            if (EntityAi_PickAttack((char *)entity)) {
                EntityAi_StartAction(entity, EntityAi_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, EntityAi_MeleeAttack, unknown);
            }
            return;
        case 1:
            if ((s16)entity->params->unk434 < entity->distance_to_player) { // match: read signed here
                turn = EntityMath_AngleDifference(entity->unk68, entity->model->unkD0[1]);
                turn = ABS(turn);
                if (turn > 0.7853982f) {
                    entity->target = D_00413F80;
                    entity->unkA0 = rand() % 30 + 60;
                    unkE0->unk78 = 0;
                } else {
                    unkE0->unk78 = -1;
                }
            } else {
                entity->target = D_00413F80;
                entity->unkA0 = rand() % 60 + 120;
                unkE0->unk78 = 1;
            }
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (entity->unkCD == 1) {
        side = EntityAi_TurnFromPush(entity, direction);
        switch (unkE0->unk78) {
        case 0:
            if (side > 0) {
                entity->unkC8 = 0x201;
            } else if (side < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = EntityAi_TurnCommand(entity);
            }
            break;
        case 1:
            if (side > 0) {
                entity->unkC8 = 0x201;
            } else if (side < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
            break;
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* A death action (busy bit 0x40) only func_0017A540 puts in the death slot, a set-up function
   nothing refers to; ForestGolem_Init leaves the slot empty. As EntityAi_Death; when the fade
   starts and EntitySpawnInfo.unk40 is set, it calls EntityEvent_Respawn for the entity with that spawn id
   (if its flag bit 0 is set) and puts it at this entity's position. */
static void ForestGolem_Death(Entity *entity) {
    Entity *other;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) == 0x2000000 ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
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
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
            entity->unkA0 = 20;
            EntityEvent_OnDeath(entity);
            if (entity->spawn_info->unk40 != 0) {
                other = Entity_GetBySpawnId(entity->spawn_info->unk40);
                if (other != NULL && ENTITY_FLAG_BITS(other)->bit0) {
                    EntityEvent_Respawn((char *)other);
                    Vector_Copy(other->model->position, entity->model->position);
                }
            }
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

/* An action (busy bit 0x1000): plays animation 17 to its end; on frame 20 it calls MapObject_Signal
   with 1 for the map object work byte unk7B names. ForestGolem_WalkToMapObjectPosX and
   ForestGolem_WalkToMapObjectNegX start it. The same as ExpeditionSoldier_SignalMapObject. */
static void ForestGolem_SignalMapObject(Entity *entity) {
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
        if (entity->model->unkFC->unk82 == 0x14) {
            map_object = MapObject_GetById(entity->group_id, unkE0->unk7B);
            if (map_object != NULL) {
                MapObject_Signal(map_object, 1);
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

/* A think handler only func_0017A540 installs (for EntitySpawnInfo.variant 1), a set-up function
   nothing refers to: as ForestGolem_ThinkByState (with func_0017A880 for state 1 too), but it
   first reads bit 17 of the two map objects EntitySpawnInfo.unk42 and unk43 name into work bytes
   unk79 and unk7A, and while either is clear it calls ForestGolem_WalkToMapObjectPosX instead of
   stepping the states. The same as ExpeditionSoldier_ThinkUseMapObjectsPosX. */
static void ForestGolem_ThinkUseMapObjectsPosX(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk42);
    if (map_object != NULL) {
        unkE0->unk79 = (map_object->unk10 >> 17) & 1;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk43);
    if (map_object != NULL) {
        unkE0->unk7A = (map_object->unk10 >> 17) & 1;
    }
    if (!unkE0->unk79 || !unkE0->unk7A) {
        ForestGolem_WalkToMapObjectPosX(entity, unknown);
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
        break;
    case 1:
    case 2:
        func_0017A880(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* Helper of ForestGolem_ThinkUseMapObjectsPosX, which calls it in place of the state functions: attacks when
   EntityAi_CanAttack says so; else walks to a fixed point (x 950) and from there to a point next to the
   map object whose bit is clear, turns to face it and starts ForestGolem_SignalMapObject for it;
   until then it starts the move action. */
static void ForestGolem_WalkToMapObjectPosX(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 near_point[4];
    f32 far_point[4];
    f32 range;

    // match: takes the entity; the shared prototype has no parameters
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    range = 100.0f;
    if (entity->model->position[0] > 1200.0f || entity->model->position[0] < 800.0f) {
        unkE0->unk30[0] = 950.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[2] = -1400.0f;
        unkE0->unk30[3] = 0.0f;
        entity->target = entity->unkE0.unk30;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else {
        if (unkE0->unk79 == 0) {
            near_point[0] = 900.0f;
            near_point[1] = entity->spawn_info->spawn_y;
            near_point[2] = -1800.0f;
            near_point[3] = 0.0f;
            far_point[0] = 800.0f;
            far_point[1] = entity->spawn_info->spawn_y;
            far_point[2] = -1800.0f;
            far_point[3] = 0.0f;
            unkE0->unk7B = entity->spawn_info->unk42;
        } else if (unkE0->unk7A == 0) {
            near_point[0] = 900.0f;
            near_point[1] = entity->spawn_info->spawn_y;
            near_point[2] = -1000.0f;
            near_point[3] = 0.0f;
            far_point[0] = 800.0f;
            far_point[1] = entity->spawn_info->spawn_y;
            far_point[2] = -1000.0f;
            far_point[3] = 0.0f;
            unkE0->unk7B = entity->spawn_info->unk43;
        }
        if (EntityMath_DistanceXZ(entity->model->position, near_point) < range) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            Vector_Copy(unkE0->unk30, far_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_TurnCommand(entity);
            if (entity->unkC8 == 0) {
                EntityAi_StartAction(entity, ForestGolem_SignalMapObject, unknown);
                return;
            }
        } else {
            Vector_Copy(unkE0->unk30, near_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, range * 0.5f);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* A think handler only func_0017A540 installs (for EntitySpawnInfo.variant 2), a set-up function
   nothing refers to: as ForestGolem_ThinkUseMapObjectsPosX, with ForestGolem_WalkToMapObjectNegX
   (the points at x -950 and -900). */
static void ForestGolem_ThinkUseMapObjectsNegX(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 != 0) {
        return;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk42);
    if (map_object != NULL) {
        unkE0->unk79 = (map_object->unk10 >> 17) & 1;
    }
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk43);
    if (map_object != NULL) {
        unkE0->unk7A = (map_object->unk10 >> 17) & 1;
    }
    if (!unkE0->unk79 || !unkE0->unk7A) {
        ForestGolem_WalkToMapObjectNegX(entity, unknown);
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
        break;
    case 1:
    case 2:
        func_0017A880(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* Helper of ForestGolem_ThinkUseMapObjectsNegX: as ForestGolem_WalkToMapObjectPosX with its own points (x -950).
   */
static void ForestGolem_WalkToMapObjectNegX(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 near_point[4];
    f32 far_point[4];
    f32 range;

    // match: takes the entity; the shared prototype has no parameters
    if (((int (*)(Entity *))EntityAi_CanAttack)(entity)) {
        if (EntityAi_PickAttack((char *)entity)) {
            EntityAi_StartAction(entity, EntityAi_RangedAttack, 1);
        } else {
            EntityAi_StartAction(entity, EntityAi_MeleeAttack, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    range = 100.0f;
    if (entity->model->position[0] > -800.0f || entity->model->position[0] < -1200.0f) {
        unkE0->unk30[0] = -950.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[2] = -1400.0f;
        unkE0->unk30[3] = 0.0f;
        entity->target = entity->unkE0.unk30;
        entity->unkC8 = EntityAi_MoveCommand(entity, 0, 50.0f);
    } else {
        if (unkE0->unk79 == 0) {
            near_point[0] = -900.0f;
            near_point[1] = entity->spawn_info->spawn_y;
            near_point[2] = -1000.0f;
            near_point[3] = 0.0f;
            far_point[0] = -800.0f;
            far_point[1] = entity->spawn_info->spawn_y;
            far_point[2] = -1000.0f;
            far_point[3] = 0.0f;
            unkE0->unk7B = entity->spawn_info->unk42;
        } else if (unkE0->unk7A == 0) {
            near_point[0] = -900.0f;
            near_point[1] = entity->spawn_info->spawn_y;
            near_point[2] = -1800.0f;
            near_point[3] = 0.0f;
            far_point[0] = -800.0f;
            far_point[1] = entity->spawn_info->spawn_y;
            far_point[2] = -1800.0f;
            far_point[3] = 0.0f;
            unkE0->unk7B = entity->spawn_info->unk43;
        }
        if (EntityMath_DistanceXZ(entity->model->position, near_point) < range) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            Vector_Copy(unkE0->unk30, far_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_TurnCommand(entity);
            if (entity->unkC8 == 0) {
                EntityAi_StartAction(entity, ForestGolem_SignalMapObject, unknown);
                return;
            }
        } else {
            Vector_Copy(unkE0->unk30, near_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = EntityAi_MoveCommand(entity, 0, range * 0.5f);
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}
