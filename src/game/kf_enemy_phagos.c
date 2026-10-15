/* Phagos: the behaviour of entity type 14 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Phagos. The game names no monsters;
   these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by hit points
   and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *entity, void (*handler)(Entity *), int run_now);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int Motion_BlendToPose(EntityModel *, f32);
void EntityAi_LowerWeightsAfterHit(char *);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_SetState(char *, u8);
void EntityAi_ReactionLight(Entity *);
extern u32 D_00446ED4;
void EntityAi_SensePlayer(void *, int);
int rand(void);
int Motion_Start(void *, int, int, u32, int);
float EntityMath_WrapAngle(float);
float EntityMath_EaseAngleToward(float *, float, float);
void EntityMath_FacingVector(Vector4 *, float);
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
s32 EntityCollision_RayHitsPropWall(f32 *, f32 *, f32, f32 *);
void Vector_Copy(void *, void *);
float EntityMath_DistanceXZ(void *, void *);

static void Phagos_Animate(Entity *entity);
static void Phagos_OnHit(Entity *entity);
static void Phagos_Think(Entity *entity, s32 unknown);
static void Phagos_RunAwayAndRemove(Entity *entity);

/* Set-up of a type 14 entity: clears its work area and installs the type's handlers. */
void Phagos_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = Phagos_Think;
    unkE0->animate = (void (*)(void))Phagos_Animate;
    unkE0->on_hit = (void (*)(void))Phagos_OnHit;
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
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    Motion_ClearPartSlots(entity->model);
    Motion_StepModel(entity->model, 0, 0);
}

/* The animate handler: as EntityAi_Animate without the test of the EntityParams kind bits and
   without the head turn. */
static void Phagos_Animate(Entity *entity) {
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

/* The on_hit handler: every hit starts EntityAi_ReactionLight, whatever its strength, and puts the
   entity in state 3; the target and flag bits 9 and 10 are left as they are. */
static void Phagos_OnHit(Entity *entity) {
    EntityAi_LowerWeightsAfterHit((char *)entity);
    EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionLight, 0);
    EntityAi_SetState((char *)entity, 3);
}

/* The think handler: no states. While the entity is not busy it starts Phagos_RunAwayAndRemove; it
   tests for state 3, but both branches start the same action. */
static void Phagos_Think(Entity *entity, s32 unknown) {
    if (D_00446ED4 % 10 == entity->unkD7 % 10) {
        EntityAi_SensePlayer(entity, 2);
    }
    if (entity->unkC6 == 0) {
        if (entity->state == 3) {
            EntityAi_StartAction(entity, Phagos_RunAwayAndRemove, unknown);
        } else {
            EntityAi_StartAction(entity, Phagos_RunAwayAndRemove, unknown);
        }
    }
}

/* An action (busy bit 0x1000), the only one Phagos_Think starts: plays animation 4 and turns
   the entity to a heading drawn between 90 and 269 degrees from Entity.unk68 (work float unk48).
   When flag bit 52 is set it starts effect EntityParams.unk430 at model point 0xFF, fades the model
   out over 20 frames and removes the entity. When EntityCollision_RayHitsPropWall finds a wall ahead within the
   radius it starts the same effect, sets flag bit 28, and removes the entity once it is more than
   four radii from that place. */
static void Phagos_RunAwayAndRemove(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 bounds[4];
    f32 direction[4];
    f32 heading;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 4, 0, entity->params->unkC4, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = 1;
        }
        unkE0->unk48 = entity->unk68 + 1.5707964f + (f32)(rand() % 180) * 0.017453292f;
        unkE0->unk48 = EntityMath_WrapAngle(unkE0->unk48);
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
        entity->action_step++;
        break;
    case 1:
        heading = entity->model->unkD0[1];
        EntityMath_EaseAngleToward(&heading, unkE0->unk48, 10.0f);
        entity->unk94 += heading - entity->model->unkD0[1];
        bounds[0] = entity->model->position[0] - entity->radius;
        bounds[1] = entity->model->position[0] + entity->radius;
        bounds[2] = entity->model->position[2] - entity->radius;
        bounds[3] = entity->model->position[2] + entity->radius;
        EntityMath_FacingVector((Vector4 *)direction, entity->model->unkD0[1]);
        if ((s32)(entity->flags >> 52) & 1) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
            entity->unkA0 = 20;
            EntityEvent_StartEffectAtPoint((char **)entity, entity->params->unk430, 0xFF, (int)entity->model->unkD0);
            entity->action_step = 3;
        } else if (EntityCollision_RayHitsPropWall(entity->model->position, direction, entity->radius, bounds)) {
            Vector_Copy(unkE0->unk30, entity->model->position);
            ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
            EntityEvent_StartEffectAtPoint((char **)entity, entity->params->unk430, 0xFF, (int)entity->model->unkD0);
            entity->action_step++;
        }
        break;
    case 2:
        if (EntityMath_DistanceXZ(entity->model->position, unkE0->unk30) > entity->radius * 4.0f) {
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}
