/* LavaFace: the behaviour of entity type 32 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Lava Face. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Dormant(Entity *);
void EntityAi_Death(Entity *);
extern s32 D_003E6E40[][2];
extern float D_00413F80[];
float sqrtf(float);
float atan2f(float, float);
double atan2(double, double);
char *func_0010CAB8(char *, u32);
float EntityMath_WrapAngle(float);
float EntityMath_DistanceXZ(void *, void *);
float EntityMath_AngleXZ(float *, char *);
f32 EntityMath_Approach(f32, f32, f32);
void Vector_Copy(void *, void *);
int EntityAi_LaunchShot(void *, int, void *, void *);
void EntityEvent_StartCue(Entity *, s32, s32, s32);
int Motion_Start(void *, int, int, u32, int);
int rand(void);

static void LavaFace_Think(Entity *entity, s32 unknown);

/* Set-up of a type 32 entity: clears its work area and installs the type's handlers. */
void LavaFace_Init(Entity *entity) {
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
    unkE0->think = LavaFace_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))EntityAi_Death;
    unkE0->move = NULL;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
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

/* The think handler: each time the timer Entity.unkA0 runs out (and EntityParams.unk434 is not 0)
   launches shot EntityParams.unk430 from model point unk432, aimed at the player, and sets the
   timer to unk434 plus 0 to 2 frames; keeps animation 0. */
static void LavaFace_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angles[4];
    f32 target[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 pitch;
    f32 yaw;

    if (entity->unkA0 == 0 && (s16)entity->params->unk434 != 0) {
        from = (f32 *)func_0010CAB8((char *)entity->model, entity->params->unk432);
        tip = (f32 *)func_0010CAB8((char *)entity->model, entity->params->unk432 + 1);
        if (from == NULL || tip == NULL) {
            return;
        }
        if (entity->params->unk432 != 0xFF) {
            delta_x = tip[0] - from[0];
            delta_y = tip[1] - from[1];
            delta_z = tip[2] - from[2];
            angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
            angles[1] = EntityMath_WrapAngle(-atan2f(delta_z, delta_x) + 1.5707964f);
            angles[2] = angles[3] = 0.0f;
        } else {
            Vector_Copy(angles, entity->model->unkD0);
            angles[1] = EntityMath_WrapAngle(angles[1] + 3.1415927f);
        }
        Vector_Copy(target, D_00413F80);
        target[1] -= 190.0f;
        pitch = EntityMath_WrapAngle(-atan2(target[1] - from[1], EntityMath_DistanceXZ(from, target)));
        yaw = EntityMath_AngleXZ(from, (char *)target);
        angles[0] = EntityMath_Approach(angles[0], pitch, entity->params->unk436);
        angles[1] = EntityMath_Approach(angles[1], yaw, entity->params->unk438);
        EntityAi_LaunchShot(entity, entity->params->unk430, from, angles);
        EntityEvent_StartCue(entity, entity->params->unk43A, entity->params->unk43C, entity->params->unk43E);
        entity->unkA0 = entity->params->unk434 + rand() % 3;
    }
    Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}
