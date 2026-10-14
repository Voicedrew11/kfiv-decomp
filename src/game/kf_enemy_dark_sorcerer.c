/* DarkSorcerer: the behaviour of entity type 53 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers it installs. Used by: Dark Sorcerer. The game names no
   monsters; these names are from https://www.cleanhit.wiki/KF4:Creatures, matched to the chr.prm records by
   hit points and experience, with resistances and drops where two monsters share those. */
#include "kf_entity.h"
#include "common.h"
#include "kf_map.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int EntityAi_StartAction(Entity *entity, void (*handler)(Entity *), int run_now);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to EntityAi_Animate.c
void EntityAi_Animate(void);
extern s32 D_003E6E40[][2];
void func_00190A30(char *);
void func_00195898(char *, u8);
int EntityAi_SetReaction(char *, void (*)(void), int);
void EntityAi_ReactionRewind(Entity *);
void EntityAi_ReactionLight(Entity *);
void EntityAi_ReactionHeavy(Entity *);
extern u32 D_00446ED4;
// match: the size is unknown; a complete small type would be read through $gp
extern f32 D_00446830[];
void func_00196870(void *, int);
void func_00196CC0(void *);
void func_0018E9C0(void *);
int EntityAi_ThinkReturn(void *, int);
f32 func_0018A9F0(void *, void *);
extern PlayerData D_00413F80;
int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
float func_00194FB8(float);
float func_00195478(float *, char *);
float func_001957B8(float, float);
s32 func_0018EC78(Entity *, f32);
int func_00190BE0(Entity *entity);
int func_0018EAC0(void *, void *);
s32 func_00190F90(Entity *, s32, f32);
void EntityAi_Dodge(Entity *);
void EntityAi_Guard(Entity *);
int func_00192C98(void *, int, int, u32, int);
extern f32 D_00431AF0[16];
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void func_00216BC0(void *, void *, void *);
void Vector_Scale(float *, float *, float);
// match: the size is unknown; a complete small type would be read through $gp
extern u8 D_0034B21C[];
int func_00195AC0(Entity *, void *, s32, s32, s32);
void func_0019A290(Entity *, s32);
int func_0018CF30(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
int func_0019A3F0(void *, int);
void func_0018DCD8(Entity *);
extern EntityActionWeights3 D_0043EA88;
int func_00195E48(void *, int, void *, void *);
int func_0019D490(u8, u8, float *, float);
extern s32 D_00446770;
extern MapSlot D_0038CD20[];
void *func_00116160(int, int);
int func_002109C8(int, int);

static void DarkSorcerer_OnHit(Entity *entity);
static void DarkSorcerer_Think(Entity *entity, s32 unknown);
static void DarkSorcerer_ThinkDecide(Entity *entity, s32 unknown);
static void DarkSorcerer_Move(Entity *entity);
static void DarkSorcerer_ApplyMoveCommand(Entity *entity);
static void DarkSorcerer_Dormant(Entity *entity);
static void DarkSorcerer_MeleeAttack(Entity *entity);
static void DarkSorcerer_RangedAttack(Entity *entity);
static void DarkSorcerer_Death(Entity *entity);
static s32 DarkSorcerer_PickAction(Entity *entity);
static void DarkSorcerer_PlayAnim17AndShoot(Entity *entity);
static void DarkSorcerer_LaunchShotsAtFixedPoints(Entity *entity);
static void DarkSorcerer_PlayAnim18AndShoot(Entity *entity);
static void DarkSorcerer_PlayAnim19StartScripts(Entity *entity);
static void DarkSorcerer_StartMapScripts(Entity *entity);

/* Set-up of a type 53 entity: clears its work area and installs the type's handlers. */
void DarkSorcerer_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u8 *unkBC = entity->unkBC;
    u8 *unkC0 = entity->unkC0;
    char *unk138 = entity->model->unk138;
    s32 i;

    // match: the three fills name their target inside the loop; unkBC and unkC0 above are
    // otherwise unused but retail computes them up front and copies them before each loop
    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        u8 *work_area = (u8 *)&entity->unkE0;

        work_area[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->think = DarkSorcerer_Think;
    unkE0->animate = EntityAi_Animate;
    unkE0->on_hit = (void (*)(void))DarkSorcerer_OnHit;
    unkE0->move = (void (*)(void))DarkSorcerer_Move;
    unkE0->dormant = (void (*)(void))DarkSorcerer_Dormant;
    unkE0->death = (void (*)(void))DarkSorcerer_Death;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    entity->target = NULL;
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->dormant, 1);
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

/* The on_hit handler: as EntityAi_OnHit, but it leaves the target and flag bit 10 as they are. */
static void DarkSorcerer_OnHit(Entity *entity) {
    u64 flags;
    u64 hit_strength;

    func_00190A30((char *)entity);
    entity->flags &= ~0x200;
    flags = entity->flags;
    hit_strength = (flags >> 36) & 3;
    if (hit_strength == 1) {
        if ((s32)(flags >> 19) & 1) {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionRewind, 0);
        } else {
            EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionLight, 0);
        }
    } else if (hit_strength == 2) {
        EntityAi_SetReaction((char *)entity, (void (*)(void))EntityAi_ReactionHeavy, 0);
    }
    func_00195898((char *)entity, 2);
}

/* The think handler: no states. While the entity is not busy it calls EntityAi_ThinkReturn when
   flag bit 53 is set, else DarkSorcerer_ThinkDecide. After that it keeps flag bit 28 set except in
   the death action; while the bit is set it adds command bit 0x20 or 0x10 by the height
   func_0018A9F0 reads (work byte unk7A: set above EntityParams.unk43C or when there is no reading,
   cleared below unk43E); and it calls DarkSorcerer_ApplyMoveCommand while the entity is not busy or is in a hit
   reaction. Counts down work word unk6A. */
static void DarkSorcerer_Think(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 height;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        func_00196CC0(entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            EntityAi_ThinkReturn(entity, unknown);
        } else {
            DarkSorcerer_ThinkDecide(entity, unknown);
        }
    }
    if (entity->unkC6 & 0x40) {
        entity->flags &= ~0x10000000;
    } else {
        entity->flags |= 0x10000000;
    }
    if ((s32)(entity->flags >> 28) & 1) {
        height = func_0018A9F0(entity->unk14, entity->model->position);
        if (height != D_00446830[0]) {
            if (!((f32)entity->params->unk43C < height)) {
                if (height < (f32)entity->params->unk43E) {
                    unkE0->unk7A = 0;
                }
            } else {
                unkE0->unk7A = 1;
            }
        } else {
            unkE0->unk7A = 1;
        }
        if (unkE0->unk7A != 0) {
            entity->unkC8 |= 0x20;
        } else {
            entity->unkC8 |= 0x10;
        }
    }
    if (entity->unkC6 == 0 || (entity->unkC6 & 6)) {
         DarkSorcerer_ApplyMoveCommand(entity);
    }
    if (unkE0->unk6A > 0) {
        unkE0->unk6A--;
    }
}

/* Decides for DarkSorcerer_Think: guards or dodges the player's swing and picks among attacking,
   closing in and waiting as EntityAi_ThinkCombat does, with the type's own attacks
   (DarkSorcerer_RangedAttack, DarkSorcerer_MeleeAttack). Then, unless
   DarkSorcerer_PickAction starts one of its three actions, it gives the command for the way
   of closing in kept in work byte unk78 (0 forward, 1 backwards, 2 and 3 sideways with command bits
   4 and 8, 4 turning only) and starts the move action. */
static void DarkSorcerer_ThinkDecide(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Guard, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        EntityAi_StartAction(entity, EntityAi_Dodge, 1);
        return;
    }
    side = func_0018FD50(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    if (entity->unkA0 == 0 || side == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = side;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, side);
        switch (entity->unkCD) {
        case 0:
            if (func_0018F030((char *)entity)) {
                EntityAi_StartAction(entity, DarkSorcerer_RangedAttack, unknown);
            } else {
                EntityAi_StartAction(entity, DarkSorcerer_MeleeAttack, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            if ((f32)entity->params->unk438 < entity->distance_to_player) {
                unkE0->unk78 = 0;
            } else if (entity->distance_to_player < (f32)entity->params->unk436) {
                unkE0->unk78 = 1;
            } else if (func_001957B8(D_00413F80.unk14,
                                     func_00195478(D_00413F80.position, (char *)entity->model->position)) > 0.0f) {
                unkE0->unk78 = 2;
            } else {
                unkE0->unk78 = 3;
            }
            entity->unkA0 = entity->params->unk43A;
            break;
        case 2:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = 4;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (DarkSorcerer_PickAction(entity)) {
        return;
    }
    switch (unkE0->unk78) {
    case 0:
        turn = func_0018EAC0(entity, direction);
        if (turn > 0) {
            entity->unkC8 |= 0x201;
        } else if (turn < 0) {
            entity->unkC8 |= 0x101;
        } else {
            entity->unkC8 |= func_00190F90(entity, 0, entity->radius + 100.0f + 30.0f);
        }
        break;
    case 1:
        if (func_0018EC78(entity, func_00194FB8(entity->model->unkD0[1] + 3.1415927f))) {
            unkE0->unk78 = rand() % 3 + 2;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
            entity->unkC8 |= 2;
        }
        break;
    case 2:
        entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
        entity->unkC8 |= 4;
        if ((f32)entity->params->unk438 < entity->distance_to_player) {
            entity->unkC8 |= 1;
        } else if (entity->distance_to_player < (f32)entity->params->unk436) {
            entity->unkC8 |= 2;
        }
        break;
    case 3:
        entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
        entity->unkC8 |= 8;
        if ((f32)entity->params->unk438 < entity->distance_to_player) {
            entity->unkC8 |= 1;
        } else if (entity->distance_to_player < (f32)entity->params->unk436) {
            entity->unkC8 |= 2;
        }
        break;
    case 4:
        entity->unkC8 |= func_00190BE0(entity);
        break;
    }
    EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, unknown);
}

/* The move handler: only picks the animation, 6 or 7 for command bits 4 and 8, 3 forward, 5
   backwards, else 0. DarkSorcerer_ApplyMoveCommand does the turning and the moving. */
static void DarkSorcerer_Move(Entity *entity) {
    u16 command = entity->unkC8;

    if (command & 4) {
        func_00192C98(entity, 6, 0, entity->params->unkC4, 0);
        return;
    }
    if (command & 8) {
        func_00192C98(entity, 7, 0, entity->params->unkC4, 0);
        return;
    }
    if (command & 1) {
        func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
        return;
    }
    if (command & 2) {
        func_00192C98(entity, 5, 0, entity->params->unkC4, 0);
        return;
    }
    func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
}

/* Carries out the movement command in Entity.unkC8; DarkSorcerer_Think, the attacks and the three
   PlayAnim actions call it. It turns the entity and speeds up or slows down along its three axes by
   EntityParams.unk50 a frame up to unk54 (work floats unk44, unk48 and unk4C; in y half the step
   and four fifths of the limit), twice as fast for command bit 0x40. */
static void DarkSorcerer_ApplyMoveCommand(Entity *entity) {
    EntityUnkE0Kind16BF98 *unkE0 = (EntityUnkE0Kind16BF98 *)&entity->unkE0;
    f32 turn = 0.0f;
    f32 step;
    f32 limit;

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
    } else if (entity->params->unk58 < unkE0->unk1C) {
        unkE0->unk1C -= entity->params->unk58;
    } else if (unkE0->unk1C < -entity->params->unk58) {
        unkE0->unk1C += entity->params->unk58;
    } else {
        unkE0->unk1C = 0.0f;
    }
    turn = unkE0->unk1C;
    if (entity->unkC8 & 0x400) {
        turn += turn;
    }
    entity->unk94 += turn;
    ENTITY_SCRATCH_VECTOR[0] = ENTITY_SCRATCH_VECTOR[1] = ENTITY_SCRATCH_VECTOR[2] = ENTITY_SCRATCH_VECTOR[3] = 0.0f;
    step = entity->params->unk50 * 0.5f;
    limit = entity->params->unk54 * 0.8f;
    if (entity->unkC8 & 1) {
        unkE0->unk4C -= entity->params->unk50;
        if (unkE0->unk4C < -entity->params->unk54) {
            unkE0->unk4C = -entity->params->unk54;
        }
    } else if (entity->unkC8 & 2) {
        unkE0->unk4C += entity->params->unk50;
        if (entity->params->unk54 < unkE0->unk4C) {
            unkE0->unk4C = entity->params->unk54;
        }
    } else if (entity->params->unk50 < unkE0->unk4C) {
        unkE0->unk4C -= entity->params->unk50;
    } else if (unkE0->unk4C < -entity->params->unk50) {
        unkE0->unk4C += entity->params->unk50;
    } else {
        unkE0->unk4C = 0.0f;
    }
    if (entity->unkC8 & 4) {
        unkE0->unk44 += entity->params->unk50;
        if (entity->params->unk54 < unkE0->unk44) {
            unkE0->unk44 = entity->params->unk54;
        }
    } else if (entity->unkC8 & 8) {
        unkE0->unk44 -= entity->params->unk50;
        if (unkE0->unk44 < -entity->params->unk54) {
            unkE0->unk44 = -entity->params->unk54;
        }
    } else if (entity->params->unk50 < unkE0->unk44) {
        unkE0->unk44 -= entity->params->unk50;
    } else if (unkE0->unk44 < -entity->params->unk50) {
        unkE0->unk44 += entity->params->unk50;
    } else {
        unkE0->unk44 = 0.0f;
    }
    if (entity->unkC8 & 0x10) {
        unkE0->unk48 -= step;
        if (unkE0->unk48 < -limit) {
            unkE0->unk48 = -limit;
        }
    } else if (entity->unkC8 & 0x20) {
        unkE0->unk48 += step;
        if (limit < unkE0->unk48) {
            unkE0->unk48 = limit;
        }
    } else if (step < unkE0->unk48) {
        unkE0->unk48 -= step;
    } else if (unkE0->unk48 < -step) {
        unkE0->unk48 += step;
    } else {
        unkE0->unk48 = 0.0f;
    }
    ENTITY_SCRATCH_VECTOR[0] = unkE0->unk44;
    ENTITY_SCRATCH_VECTOR[1] = unkE0->unk48;
    ENTITY_SCRATCH_VECTOR[2] = unkE0->unk4C;
    sceVu0RotMatrixZ(ENTITY_SCRATCH_MATRIX, D_00431AF0, entity->model->unkD0[2]);
    sceVu0RotMatrixX(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[0]);
    sceVu0RotMatrixY(ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_MATRIX, entity->model->unkD0[1]);
    func_00216BC0(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_MATRIX, ENTITY_SCRATCH_VECTOR);
    if (entity->unkC8 & 0x40) {
        Vector_Scale(ENTITY_SCRATCH_VECTOR, ENTITY_SCRATCH_VECTOR, 2.0f);
    }
    entity->unk84 += ENTITY_SCRATCH_VECTOR[0];
    entity->unk88 += ENTITY_SCRATCH_VECTOR[1];
    entity->unk8C += ENTITY_SCRATCH_VECTOR[2];
}

/* The dormant action (busy bit 0x20), which the set-up function always starts: animation 0 with
   flag bits 18, 20 and 21 set and Entity.unkD6 at 0x20 until D_0034B21C[0] is not 0; then it brings
   unkD6 up to 0x80 over 20 frames (counted in work byte unk7B), clears the three bits and thinks.
   */
static void DarkSorcerer_Dormant(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x20;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0, 0, 0, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x200000;
        entity->flags |= 0x100000;
        entity->flags |= 0x40000;
        entity->unkD6 = 0x20;
        entity->flags |= 0x10000000;
        unkE0->unk7B = 0;
        entity->action_step++;
    case 1:
        if (D_0034B21C[0] != 0) {
            entity->action_step++;
        }
        break;
    case 2:
        entity->unkD6 = unkE0->unk7B * 0x60 / 20 + 0x20;
        unkE0->unk7B++;
        if (unkE0->unk7B > 20) {
            entity->flags &= ~0x200000;
            entity->flags &= ~0x100000;
            entity->flags &= ~0x40000;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
        }
        break;
    }
    entity->model->unk15C = entity->unkD6;
}

/* The melee attack (busy bit 1): as EntityAi_MeleeAttack, but it turns by adding the command of
   func_00190BE0, and calls DarkSorcerer_ApplyMoveCommand every frame. */
static void DarkSorcerer_MeleeAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk240 *unk240 = &entity->params->unk240[kind];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, kind | 0x14, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
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
            entity->unkC8 |= func_00190BE0(entity);
        }
        break;
    case 2:
        if (unk240->unk01 >= entity->model->unkFC->unk82) {
            entity->unkC8 |= func_00190BE0(entity);
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        if (!((s32)(entity->flags >> 54) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit54 = func_00195AC0(entity, unk240, 0, elapsed, entity->model->unkFC->unk82);
        }
        if (!((s32)(entity->flags >> 55) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit55 = func_00195AC0(entity, unk240, 1, elapsed, entity->model->unkFC->unk82);
        }
        if (!((s32)(entity->flags >> 56) & 1)) {
            ENTITY_FLAG_BITS(entity)->bit56 = func_00195AC0(entity, unk240, 2, elapsed, entity->model->unkFC->unk82);
        }
        if (entity->model->unkFC->unk82 > 0 && unk240->unk03 >= entity->model->unkFC->unk82) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk240->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
                return;
            }
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            return;
        }
        break;
    }
     DarkSorcerer_ApplyMoveCommand(entity);
}

/* The ranged attack (busy bit 1): as EntityAi_RangedAttack, but the shots are turned towards a
   point 190 below the player (func_0018CF30); it turns and moves as DarkSorcerer_MeleeAttack does.
   */
static void DarkSorcerer_RangedAttack(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 target[4];
    s32 elapsed;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
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
            entity->unkC8 |= func_00190BE0(entity);
        }
        break;
    case 2:
        if (unk3D8->unk01 >= entity->model->unkFC->unk82) {
            entity->unkC8 |= func_00190BE0(entity);
        }
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        func_00216778(target, &D_00413F80);
        target[1] -= 190.0f;
        func_0018CF30(entity, unk3D8, elapsed, entity->model->unkFC->unk82, target, 0.7853982f, 0.0f);
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
            } else {
                EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            }
            return;
        }
        break;
    }
     DarkSorcerer_ApplyMoveCommand(entity);
}

/* The death action (busy bit 0x40): animation 16; once it is past frame EntityParams.unk434 it
   calls func_0018DCD8 and fades the model out over the rest of the motion (the frames left, kept in
   work word unk68), then removes the entity when the motion has ended. */
static void DarkSorcerer_Death(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x10, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags |= 0x100000;
        entity->flags = (entity->flags & ~0xC) | 4;
        entity->flags |= 0x8000000;
        entity->flags |= 0x4000000;
        entity->flags &= ~0x10000000;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s16)entity->params->unk434 < entity->model->unkFC->unk82) {
            entity->unkA0 = unkE0->unk68 = func_0019A3F0(entity, entity->unkD1) - entity->model->unkFC->unk82;
            ENTITY_FLAG_BITS(entity)->bits2 = 2;
            ENTITY_FLAG_BITS(entity)->bit4 = 1;
            func_0018DCD8(entity);
            entity->action_step++;
        }
        break;
    case 3:
        entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / (f32)unkE0->unk68;
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            ENTITY_FLAG_BITS(entity)->bit14 = 0;
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = 0;
        }
        break;
    }
}

/* Helper of DarkSorcerer_ThinkDecide: once the pause in work word unk6A is over, draws one of three
   actions by the weights in D_0043EA88 (the third weight is 90 while the player is outside a fixed
   rectangle) and starts it: DarkSorcerer_PlayAnim19StartScripts, DarkSorcerer_PlayAnim18AndShoot or
   DarkSorcerer_PlayAnim17AndShoot. Returns whether it started one. */
static s32 DarkSorcerer_PickAction(Entity *entity) {
    EntityActionWeights3 chances = D_0043EA88;
    s32 choice;
    s32 total;
    s32 i;
    s32 roll;
    s32 sum;

    if (entity->unkE0.unk6A > 0) {
        return FALSE;
    }
    if (D_00413F80.position[0] < -16900.0f || D_00413F80.position[0] > -16500.0f || D_00413F80.position[2] < 2500.0f
        || D_00413F80.position[2] > 3600.0f) {
        chances.weights[2] = 90;
    }
    total = chances.weights[0] + chances.weights[1] + chances.weights[2];
    roll = rand() % total;
    choice = 0;
    sum = 0;
    for (i = 0; i < 3; i++) {
        sum += chances.weights[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    if (choice == 0) {
        EntityAi_StartAction(entity, DarkSorcerer_PlayAnim19StartScripts, 1);
    } else if (choice == 1) {
        EntityAi_StartAction(entity, DarkSorcerer_PlayAnim18AndShoot, 1);
    } else {
        EntityAi_StartAction(entity, DarkSorcerer_PlayAnim17AndShoot, 1);
    }
    return TRUE;
}

/* An action (busy bit 0x1000) that DarkSorcerer_PickAction starts: plays animation 17 to its
   end; at frame 40 it calls DarkSorcerer_LaunchShotsAtFixedPoints and sets the pause, work word unk6A, to
   90. Calls DarkSorcerer_ApplyMoveCommand every frame. */
static void DarkSorcerer_PlayAnim17AndShoot(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4 >> 1, 1);
        entity->flags &= ~0x3800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= 0x28) {
            DarkSorcerer_LaunchShotsAtFixedPoints(entity);
            unkE0->unk6A = 0x5A;
            entity->action_step++;
        }
        break;
    case 3:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
                return;
            }
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            return;
        }
        break;
    }
     DarkSorcerer_ApplyMoveCommand(entity);
}

/* Helper of DarkSorcerer_PlayAnim17AndShoot: launches shot 0xCA from 18 fixed points along the four sides
   of a rectangle (func_00195E48) and calls func_0019D490 with sound 0x42 at five fixed points. */
static void DarkSorcerer_LaunchShotsAtFixedPoints(Entity *entity) {
    f32 position[4];
    f32 direction[4];
    s32 i;

    position[0] = -17300.0f;
    position[1] = 2930.0f;
    position[2] = 1800.0f;
    position[3] = 0.0f;
    direction[0] = 0.0f;
    direction[1] = 0.0f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    for (i = -17300; i <= -16100; i += 400) {
        position[0] = i;
        func_00195E48(entity, 0xCA, position, direction);
    }
    position[0] = -17300.0f;
    position[1] = 2930.0f;
    position[2] = 4200.0f;
    position[3] = 0.0f;
    direction[0] = 0.0f;
    direction[1] = 3.1415927f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    for (i = -17300; i <= -16100; i += 400) {
        position[0] = i;
        func_00195E48(entity, 0xCA, position, direction);
    }
    position[0] = -17700.0f;
    position[1] = 2830.0f;
    position[2] = 2000.0f;
    position[3] = 0.0f;
    direction[0] = 0.0f;
    direction[1] = 1.5707964f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    for (i = 2000; i <= 4000; i += 400) {
        position[2] = i;
        func_00195E48(entity, 0xCA, position, direction);
    }
    position[0] = -15700.0f;
    position[1] = 2830.0f;
    position[2] = 3200.0f;
    position[3] = 0.0f;
    direction[0] = 0.0f;
    direction[1] = -1.5707964f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    for (i = 3200; i <= 4000; i += 400) {
        position[2] = i;
        func_00195E48(entity, 0xCA, position, direction);
    }
    position[0] = -15700.0f;
    position[1] = 2830.0f;
    position[2] = 2000.0f;
    position[3] = 0.0f;
    func_00195E48(entity, 0xCA, position, direction);
    position[0] = -15700.0f;
    position[1] = 2830.0f;
    position[2] = 3600.0f;
    position[3] = 0.0f;
    func_0019D490(0x42, 0, position, 2000.0f);
    position[0] = -16900.0f;
    position[1] = 2930.0f;
    position[2] = 4200.0f;
    position[3] = 0.0f;
    func_0019D490(0x42, 0, position, 2000.0f);
    position[0] = -17700.0f;
    position[1] = 2830.0f;
    position[2] = 3200.0f;
    position[3] = 0.0f;
    func_0019D490(0x42, 0, position, 2000.0f);
    position[0] = -17300.0f;
    position[1] = 2930.0f;
    position[2] = 2200.0f;
    position[3] = 0.0f;
    func_0019D490(0x42, 0, position, 2000.0f);
    position[0] = -15700.0f;
    position[1] = 2830.0f;
    position[2] = 2000.0f;
    position[3] = 0.0f;
    func_0019D490(0x42, 0, position, 2000.0f);
}

/* An action (busy bit 0x2000) that DarkSorcerer_PickAction starts: plays animation 18 to its
   end; at frame 16 it launches shot 0xC9 from the entity's position along the model's angles
   (func_00195E48) and sets the pause, work word unk6A, to 90. Calls DarkSorcerer_ApplyMoveCommand every frame. */
static void DarkSorcerer_PlayAnim18AndShoot(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x12, 0, entity->params->unkC4 >> 1, 1);
        entity->flags &= ~0x3800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= 0x10) {
            func_00195E48(entity, 0xC9, entity->model->position, entity->model->unkD0);
            unkE0->unk6A = 0x5A;
            entity->action_step++;
        }
        break;
    case 3:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
                return;
            }
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            return;
        }
        break;
    }
     DarkSorcerer_ApplyMoveCommand(entity);
}

/* An action (busy bit 0x4000) that DarkSorcerer_PickAction starts: plays animation 19 to its
   end; at frame 24 it calls DarkSorcerer_StartMapScripts and sets the pause, work word unk6A, to
   90. Calls DarkSorcerer_ApplyMoveCommand every frame. */
static void DarkSorcerer_PlayAnim19StartScripts(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x4000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x13, 0, entity->params->unkC4 >> 1, 1);
        entity->flags &= ~0x3800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 >= 0x18) {
            DarkSorcerer_StartMapScripts(entity);
            unkE0->unk6A = 0x5A;
            entity->action_step++;
        }
        break;
    case 3:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->think(entity, 1);
                return;
            }
            EntityAi_StartAction(entity, (void (*)(Entity *))unkE0->move, 1);
            return;
        }
        break;
    }
     DarkSorcerer_ApplyMoveCommand(entity);
}

/* Helper of DarkSorcerer_PlayAnim19StartScripts: when the entity's map is the current one, starts script
   unk4E + 10000 of the map record D_00413F80.unk252 picks, and of up to two of the records
   func_00116160 gives for 0x6B to 0x88, drawn at random. */
static void DarkSorcerer_StartMapScripts(Entity *entity) {
    MapSlot *slot = &D_0038CD20[D_00446770];
    MapRecord *record = &((MapRecord *)slot->records)[D_00413F80.unk252];
    s32 remaining;
    s32 chance;
    s32 i;
    s32 slots_left;
    s32 roll_range;

    if (record->unk4E != 0 && entity->group_id == D_00446770) {
        func_002109C8(record->unk4E + 10000, 0);
    }
    slots_left = 0x1D;
    i = 0;
    remaining = 2;
    roll_range = 100;
    chance = 200;
    for (; i < 0x1E; i++) {
        if (rand() % roll_range < chance / slots_left) {
            record = func_00116160(entity->group_id, i + 0x6B);
            if (record != NULL && record->unk4E != 0 && entity->group_id == D_00446770) {
                chance -= 100;
                remaining--;
                func_002109C8(record->unk4E + 10000, 0);
            }
        }
        if (remaining <= 0) {
            break;
        }
        slots_left--;
        if (slots_left <= 0) {
            break;
        }
    }
}
