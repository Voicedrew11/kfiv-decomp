/* DarkOne: the behaviour of entity type 75 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: the Dark One, the last boss,
   chr.prm record 99 (ENTITY_TYPE_LAST_BOSS in func_001B2F70), whose one spawn is in 040.map, the end of
   the Dark Reality. The game's own text calls him the Dark One. func_001B2F70 gives him no damage:
   each weapon hit counts D_00446928 down and throws the player back, magic does nothing, and the hit
   that takes the count to 0 kills him; Prima's official guide (2004) describes the same fight. */
#include "kf_entity.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void EntityAi_SetUpdate(char *, s32);
int EntityAi_StartAction(Entity *, void (*)(Entity *), int);
void Motion_ClearPartSlots(EntityModel *);
int Motion_StepModel(EntityModel *, s32, s32); // match: an int result keeps $2 busy after the call
void Vector_Copy(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
void EntityAi_OnHit(char *);
void EntityAi_Move(Entity *);
void EntityAi_Dormant(Entity *);
extern s32 D_003E6E40[][2];
extern s32 D_00446928;
extern u32 D_00446ED4;
void EntityEvent_StartEffectAtPoint(char **, int, u32, int);
void EntityEvent_StartCue(Entity *, s32, s32, s32);
int Sound_PlayMapAtRange(u8, u8, float *, float);
void EntityAi_SensePlayer(void *, int);
void EntityAi_StepState(void *);
int EntityAi_ThinkIdle(void *, int);
void EntityAi_ThinkCombat(Entity *, s32);
extern u8 D_00413FBC[];
int Motion_Start(void *, int, int, u32, int);
void EntityEvent_OnDeath(Entity *);

static void DarkOne_Think(Entity *entity, s32 unknown);
static void DarkOne_Death(Entity *entity);
static void DarkOne_DeathByOtherWeapon(Entity *entity);
static void DarkOne_DeathByMoonlightSword(Entity *entity);

/* Set-up of a type 75 entity: clears its work area and installs the type's handlers. */
void DarkOne_Init(Entity *entity) {
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
    unkE0->think = DarkOne_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))EntityAi_OnHit;
    unkE0->move = (void (*)(void))EntityAi_Move;
    unkE0->dormant = (void (*)(void))EntityAi_Dormant;
    unkE0->death = (void (*)(void))DarkOne_Death;
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
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit16 = TRUE;
    unkE0->unk7A = D_00446928;
}

/* The think handler: as EntityAi_Think without the walk back to the spawn point, with
   EntityAi_ThinkCombat for states 1 and 2. Every third frame it calls EntityEvent_StartEffectAtPoint with
   EntityParams.unk430, unk432 or unk434, by D_00446928 (3, 2, other). In a frame with flag bit 43
   set it calls EntityEvent_StartEffectAtPoint with 0x13D and clears the bit; then, with D_00446928 at 0 or less, it
   sets Entity.unkA3 to 6 and starts DarkOne_Death, and else, when D_00446928 has changed (work
   byte unk7A keeps it), calls EntityEvent_StartCue and Sound_PlayMapAtRange. */
static void DarkOne_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (D_00446ED4 % 3U == 0) {
        switch (D_00446928) {
        case 3:
            EntityEvent_StartEffectAtPoint((char **)entity, entity->params->unk430, 0xFF, (int)entity->model->unkD0);
            break;
        case 2:
            EntityEvent_StartEffectAtPoint((char **)entity, entity->params->unk432, 0xFF, (int)entity->model->unkD0);
            break;
        default:
            EntityEvent_StartEffectAtPoint((char **)entity, (s16)entity->params->unk434, 0xFF, (int)entity->model->unkD0);
            break;
        }
    }
    if (ENTITY_FLAG_BITS(entity)->bit43) {
        EntityEvent_StartEffectAtPoint((char **)entity, 0x13D, 0xFF, (int)entity->model->unkD0);
        ENTITY_FLAG_BITS(entity)->bit43 = FALSE;
        if (D_00446928 <= 0) {
            entity->unkA3 = 6;
            EntityAi_StartAction(entity, DarkOne_Death, unknown);
            return;
        }
        if (unkE0->unk7A != D_00446928) {
            EntityEvent_StartCue(entity, entity->params->unk436, entity->params->unk438, 0);
            // match: an int result keeps $2 busy after the call
            ((int (*)(s32, s32, f32 *, f32))Sound_PlayMapAtRange)((u8)entity->params->unk43A, (u8)entity->params->unk43C,
                                                           entity->model->position, entity->params->unk42E);
            unkE0->unk7A = D_00446928;
        }
    }
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        EntityAi_SensePlayer(entity, 1);
    }
    if (entity->unkC6 == 0) {
        EntityAi_StepState(entity);
        switch (entity->state) {
        case 1:
        case 2:
            EntityAi_ThinkCombat(entity, unknown);
            break;
        case 0:
        default:
            EntityAi_ThinkIdle(entity, unknown);
            break;
        }
    }
}

/* The death action in the slot: it only starts one of the two others, DarkOne_DeathByMoonlightSword with
   flag bit 29 set when the weapon in hand (PlayerState.weapon, D_00413FBC[0]) is 0x2E, the Moonlight Sword
   (item 196), else DarkOne_DeathByOtherWeapon with flag bit 29 clear.
   DarkOne_Think starts it too. */
static void DarkOne_Death(Entity *entity) {
    if (D_00413FBC[0] == 0x2E) {
        ENTITY_FLAG_BITS(entity)->bit29 = TRUE;
        EntityAi_StartAction(entity, DarkOne_DeathByMoonlightSword, 1);
    } else {
        ENTITY_FLAG_BITS(entity)->bit29 = FALSE;
        EntityAi_StartAction(entity, DarkOne_DeathByOtherWeapon, 1);
    }
}

/* A death action (busy bit 0x40), started by DarkOne_Death: plays animation 16 as
   EntityAi_Death does, calling EntityEvent_OnDeath at the start, and when the animation has ended sets
   the flags of the fade and stops there: no fade and no removal. */
static void DarkOne_DeathByOtherWeapon(Entity *entity) {
    entity->unkC6 = 0x40;
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
        if (!ENTITY_PARAMS_BITS(entity)->bit25 && ENTITY_PARAMS_BITS(entity)->bit26) {
            entity->unk30 = 20.0f;
        }
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        EntityEvent_OnDeath(entity);
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
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
            entity->action_step++;
        }
        break;
    }
}

/* A death action (busy bit 0x40), started by DarkOne_Death: as DarkOne_DeathByOtherWeapon with
   animation 0 in place of 16. */
static void DarkOne_DeathByMoonlightSword(Entity *entity) {
    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        Motion_Start(entity, 0, 0, 5, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        EntityEvent_OnDeath(entity);
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
            ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
            ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
            entity->action_step++;
        }
        break;
    }
}
