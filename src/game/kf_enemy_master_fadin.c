/* MasterFadin: the behaviour of entity type 79 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Master Fadin. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "kf_entity_world.h"

void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
extern s32 D_003E6E40[][2];
void EntityEvent_Respawn(char *);
int Motion_Start(void *, int, int, u32, int);
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
void EntityEvent_OnDeath(Entity *);

static void MasterFadin_Think(Entity *entity, s32 unknown);
static void func_001435B0(Entity *entity, s32 unknown);
static void MasterFadin_Dormant(Entity *entity);
static void MasterFadin_Death(Entity *entity);
static void MasterFadin_AlertPartner(Entity *entity);

/* Set-up of a type 79 entity: clears its work area and installs the type's handlers. */
void MasterFadin_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = MasterFadin_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->dormant = (void (*)(void))MasterFadin_Dormant;
    unkE0->death = (void (*)(void))MasterFadin_Death;
    unkE0->on_hit = NULL;
    unkE0->move = NULL;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 1;
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

/* The think handler: it sets Entity.state itself and does not call EntityAi_StepState. In mode 2
   (EntitySpawnInfo.unk40) it takes the state of the entity whose spawn id is
   EntitySpawnInfo.variant, and starts the death action once that one's flag bits 2-3 are 2 or more.
   In mode 1 it calls EntityEvent_Respawn for every other entity with the same params whose flag bit 1 is
   clear or whose bits 2-3 are 3. While the entity is not busy it calls func_001435B0 in states 1
   and 2, and in state 0 starts what the move slot holds (the set-up function leaves it NULL). Keeps
   animation 0; once work word unk68 has run out it puts the state back to 0. */
static void MasterFadin_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    Entity *leader;
    Entity *other;

    if (entity->spawn_info->unk40 == 2) {
        leader = Entity_GetBySpawnId(entity->spawn_info->variant);
        if (leader != NULL) {
            if (ENTITY_FLAG_BITS(leader)->bits2 >= 2) {
                // match: called as returning int; only then are the flags kept in $3 afterwards
                ((int (*)(char *, void (*)(void), int))EntityAi_StartAction)((char *)entity, unkE0->death, 0);
                ENTITY_FLAG_BITS(entity)->bit7 = FALSE;
                return;
            }
            entity->state = leader->state;
        }
    }
    if (entity->spawn_info->unk40 == 1) {
        for (other = D_004467E0; other != NULL; other = other->next) {
            if (other->params == entity->params && other != entity && ((s32)other->flags & 1)) {
                if (!ENTITY_FLAG_BITS(other)->bit1 || ENTITY_FLAG_BITS(other)->bits2 >= 3) {
                    EntityEvent_Respawn((char *)other);
                }
            }
        }
    }
    if (entity->unkC6 == 0) {
        switch (entity->state) {
        case 1:
        case 2:
            func_001435B0(entity, unknown);
            break;
        case 0:
        default:
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
            break;
        }
    }
    Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    if (unkE0->unk68 > 0) {
        unkE0->unk68--;
    } else {
        entity->state = 0;
    }
}

static void func_001435B0(Entity *entity, s32 unknown) {
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

    if (entity->unkA0 == 0) {
        from = (f32 *)func_0010CAB8((char *)entity->model, entity->params->unk432);
        tip = (f32 *)func_0010CAB8((char *)entity->model, entity->params->unk432 + 1);
        if (from != NULL && tip != NULL) {
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
            EntityEvent_StartCue(entity, 0, 0, 0);
            entity->unkA0 = entity->params->unk434;
        }
    }
    Motion_Start(entity, 0, 0, entity->params->unkC4, 0);
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The dormant action (busy bit 0x20): as EntityAi_Dormant with flag bits 11 to 13 at 0; flag bit 43
   does not end the wait, it does not go to state 2, and it leaves flag bit 28 set. */
static void MasterFadin_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 1, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->action_step++;
        break;
    case 1:
        if (entity->distance_to_player < entity->spawn_info->unk54) {
            Motion_Start(entity, 2, 0, entity->params->unkC4, 0);
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
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
            ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
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

/* The death action (busy bit 0x40): animation 16, the fade over 20 frames and the removal, as
   EntityAi_Death with other flag settings; in mode 2 it calls MasterFadin_AlertPartner where that
   one calls EntityEvent_OnDeath. */
static void MasterFadin_Death(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0x10, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags |= 0x100000;
        entity->flags = (entity->flags & ~0xC) | 4;
        entity->flags |= 0x8000000;
        entity->flags |= 0x4000000;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            entity->unkA0 = 20;
            if (entity->spawn_info->unk40 != 2) {
                EntityEvent_OnDeath(entity);
            } else {
                MasterFadin_AlertPartner(entity);
            }
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            entity->flags |= 0xC;
            entity->flags &= ~2;
        }
        break;
    }
}

/* Helper of MasterFadin_Death: when the dying entity's partner (spawn id EntitySpawnInfo.variant)
   has flag bit 1 set and bits 2-3 at 0, puts it in state 2 and reloads its work word unk68 from
   EntityParams.unk43A. */
static void MasterFadin_AlertPartner(Entity *entity) {
    Entity *partner;

    partner = Entity_GetBySpawnId(entity->spawn_info->variant);
    if (partner != NULL) {
        if ((partner->flags & 0xE) == 2) {
            partner->state = 2;
            partner->unkE0.unk68 = partner->params->unk43A;
        }
    }
}
