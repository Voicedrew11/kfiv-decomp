/* Bell: the behaviour of entity type 38 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: chr.prm record 60, model c0060
   (a bell), 65,535 hit points, one spawn in 007.map. Struck, it calls one of the D_0043EE30 handlers
   (Sound_PlayFalloff), then waits EntityParams.unk434 frames before it answers another hit. Prima's
   official guide (2004): shoot the bell with a bow to ring it and destroy the skeletons. */
#include "kf_entity.h"
#include "common.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
void Vector_Copy(void *, void *);
extern s32 D_003E6E40[][2];
int Motion_Step(void *, int);
extern PlayerData D_00413F80;
float EntityMath_AngleXZ(float *, char *);
float EntityMath_WrapAngle(float);
// match: the definition names only the three integer arguments; the floats go on to the handler it calls in their registers
void Sound_PlayFalloff(s32, s32, s32, f32, f32, f32);

static void Bell_Animate(Entity *entity);
static void Bell_Think(Entity *entity);

/* Set-up of a type 38 entity: clears its work area and installs the type's handlers. */
void Bell_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = (void (*)(Entity *, s32))Bell_Think;
    unkE0->animate = (void (*)(void))Bell_Animate;
    unkE0->on_hit = NULL;
    unkE0->move = NULL;
    unkE0->dormant = NULL;
    unkE0->death = NULL;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 1;
    ENTITY_FLAG_BITS(entity)->bit18 = 1;
    ENTITY_FLAG_BITS(entity)->bit21 = 1;
    ENTITY_FLAG_BITS(entity)->bit16 = 1;
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

/* The animate handler: plays the motion a step on (Motion_Step) and sets flag bit 34 when it has
   ended; no blend and no head turn. */
static void Bell_Animate(Entity *entity) {
    ENTITY_FLAG_BITS(entity)->bit34 = Motion_Step(entity, ENTITY_FLAG_BITS(entity)->bits11);
}

/* The think handler (it takes the entity only): no states and no actions. In a frame with flag bit
   43 set and the timer Entity.unkA0 at 0 it sets flag bit 29, calls Sound_PlayFalloff with
   EntityParams.unk432, unk430 and unk42E, Entity.unkD2, Entity.unk60 and the angle of the entity
   off the player's facing, and sets the timer to EntityParams.unk434; otherwise flag bit 29 is
   clear. It always clears flag bit 43. */
static void Bell_Think(Entity *entity) {
    u64 flags;
    f32 angle;

    flags = entity->flags & ~0x20000000ULL;
    entity->flags = flags;
    if ((s32)(flags >> 43) & 1) {
        if (entity->unkA0 == 0) {
            entity->flags = flags | 0x20000000ULL;
            angle = EntityMath_AngleXZ(D_00413F80.position, (char *)entity->model->position);
            angle = EntityMath_WrapAngle(angle - D_00413F80.unk14);
            Sound_PlayFalloff((u8)entity->params->unk432, (u8)entity->params->unk430, entity->unkD2,
                          (f32)entity->params->unk42E, entity->unk60, angle);
            entity->unkA0 = entity->params->unk434;
        }
    }
    entity->flags &= ~(1ULL << 43);
}
