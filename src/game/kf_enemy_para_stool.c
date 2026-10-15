/* ParaStool: the behaviour of entity type 43 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Para Stool. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
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
int EntityAi_TurnFromPush(void *, void *);
s32 EntityAi_MoveCommand(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
int Motion_Start(void *, int, int, u32, int);
s32 EntityAi_TurnToHeading(Entity *, f32 *, f32);
void EntityAi_SetAttackDelay(Entity *, s32);
double atan(double);
void *memset(void *, int, u32);
float EntityMath_AngleXZ(float *, char *);
float EntityMath_AngleDifference(float, float);
float EntityMath_WrapAngle(float);
void func_001B2C48(void *);
s32 func_001B1470(Unk1B1470Request *, Unk1B1470Extra *);

static void ParaStool_Think(Entity *entity, s32 unknown);
static void ParaStool_ThinkCombat(Entity *entity, s32 unknown);
static void ParaStool_MeleeAttack0(Entity *entity);
static s32 ParaStool_CheckPartHitInPlayerView(Entity *entity, EntityParamsUnk240 *record, s32 index, s32 frame, s32 current_frame, f32 angle);

/* Set-up of a type 43 entity: clears its work area and installs the type's handlers. */
void ParaStool_Init(Entity *entity) {
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
    unkE0->think = ParaStool_Think;
    unkE0->animate = EntityAi_Animate;
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
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
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
    Motion_StepModel(entity->model, 0, 0);
}

/* The think handler: as EntityAi_Think, with ParaStool_ThinkCombat for state 2. Counts down work
   word unk68, the pause before the next ParaStool_MeleeAttack0. */
static void ParaStool_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (unkE0->unk68 > 0) {
        unkE0->unk68--;
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
        ParaStool_ThinkCombat(entity, unknown);
        return;
    }
    EntityAi_ThinkIdle(entity, unknown);
}

/* The think function of state 2: as EntityAi_ThinkCombat, but its one attack is
   ParaStool_MeleeAttack0, started only when the pause in work word unk68 is over (which it then
   sets to EntityParams.unk430); closing in never runs. */
static void ParaStool_ThinkCombat(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 range;
    f32 angle;
    s32 next_action;
    s32 side;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
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
            if (unkE0->unk68 == 0) {
                unkE0->unk68 = (u16)entity->params->unk430;
                EntityAi_StartAction(entity, ParaStool_MeleeAttack0, unknown);
                return;
            }
            unkE0->unk78 = -1;
            break;
        case 1:
            entity->target = D_00413F80;
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
    if (entity->unkCD == 1) {
        side = EntityAi_TurnFromPush(entity, direction);
        if (unkE0->unk78 == 0) {
            if (side > 0) {
                entity->unkC8 = 0x201;
            } else if (side < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = EntityAi_MoveCommand(entity, 0, entity->radius + 100.0f + 30.0f);
            }
        }
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The melee attack with record 0 (busy bit 1): animation 20, as EntityAi_MeleeAttack, but only part
   0 is checked, by ParaStool_CheckPartHitInPlayerView. */
static void ParaStool_MeleeAttack0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[0];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x14, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            s32 kind = 2;

            if (!(ENTITY_PARAMS_BITS(entity)->bit25 && !ENTITY_PARAMS_BITS(entity)->bit26)) {
                kind = 1;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
        ENTITY_FLAG_BITS(entity)->bit54 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit55 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit56 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
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
            EntityAi_TurnToHeading(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!ENTITY_FLAG_BITS(entity)->bit54) {
            ENTITY_FLAG_BITS(entity)->bit54 = ParaStool_CheckPartHitInPlayerView(entity, unk240, 0, elapsed, entity->model->unkFC->unk82, 0.0f);
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            EntityAi_SetAttackDelay(entity, unk240->unk02);
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
}

/* Helper of ParaStool_MeleeAttack0: the hit check of EntityAi_CheckMeleeHitAt and EntityAi_SendMeleeHit for one
   part of the record on the part's frames, written out, with conditions of its own: the entity must
   be within EntityParams.unk432 degrees of the player's facing and unk434 degrees of the player's
   pitch, and Entity.unk68 within the angle of part 0 of its heading. It then calls func_001B2C48
   before the check (func_001B1470). */
static s32 ParaStool_CheckPartHitInPlayerView(Entity *entity, EntityParamsUnk240 *record, s32 index, s32 frame, s32 current_frame, f32 angle) {
    Unk1B1470Request request;
    Unk1B1470Extra extra;
    f32 yaw;
    f32 pitch;
    f32 spread;

    if (record->unk10[index].unk24 != 0 && !(frame < record->unk10[index].unk24 && current_frame < record->unk10[index].unk24) &&
        !(record->unk10[index].unk26 < frame && record->unk10[index].unk26 < current_frame)) {
        yaw = EntityMath_AngleDifference(D_00413F80[5], EntityMath_AngleXZ(D_00413F80, (char *)entity->model->position));
        pitch = (f32)atan((entity->model->position[1] - D_00413F80[1] + 220.0f) / entity->distance_to_player) + D_00413F80[4];
        if (ABS(yaw) < entity->params->unk432 * 0.017453292f && ABS(pitch) < (s16)entity->params->unk434 * 0.017453292f) {
            spread = EntityMath_WrapAngle(record->unk10[0].unk22 * 0.017453292f);
            if (ABS(EntityMath_WrapAngle(entity->unk68 - entity->model->unkD0[1])) < spread) {
                func_001B2C48(&entity->params->unk240[0].unk10[0].unk12);
                memset(&request, 0, sizeof(request));
                memset(&extra, 0, sizeof(extra));
                Vector_Copy(request.position, entity->model->position);
                request.unk34 = entity->height + 10.0f;
                request.unk30 = entity->radius + record->unk10[index].unk20;
                request.unk38 = EntityMath_WrapAngle(angle + 3.1415927f);
                request.unk3C = EntityMath_WrapAngle(record->unk10[index].unk22 * 0.017453292f);
                request.owner = entity;
                request.unk4C_0 = 3;
                request.unk4D = 1;
                request.unk50 = record->unk10[index].unk00;
                request.unk62 = record->unk10[index].unk12;
                request.unk4E = record->unk0C;
                extra.unk00 = 0;
                extra.unk02 = 0;
                extra.unk20 = 0;
                extra.unk22 = 0;
                request.unk4C_3 = record->unk10[index].unk1E & 1;
                return func_001B1470(&request, &extra);
            }
        }
    }
    return 0;
}
