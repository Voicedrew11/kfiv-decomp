/* The behaviour of entity type 26 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"
#include "kf_effect.h"

#define ENTITY_WORK_AREA_SIZE 0xA0
void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00183570(Entity *);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
void func_00190A30(char *);
void func_00195898(char *, u8);
int func_0019A310(char *, void (*)(void), int);
void func_00181378(Entity *);
void func_001814B0(Entity *);
void func_001816A0(Entity *);
extern u32 D_00446ED4;
void func_00196870(void *, int);
void func_00196CC0(char *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
void func_00183240(Entity *, s32);
char *func_0010CAB8(char *, u32);
int func_00192C98(void *, int, int, u32, int);
float func_00194F80(void *, void *);

static void func_00140650(Entity *entity);
static void func_00140738(Entity *entity, s32 unknown);
static void func_00140878(Entity *entity, s32 unknown);
static void func_001408B0(Entity *entity);
static void func_001409D0(Entity *entity);

/* Set-up of a type 26 entity: clears its work area and installs the type's handlers. */
void EntityType26_Init(Entity *entity) {
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
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_00140738;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00140650;
    unkE0->unk18 = (void (*)(void))func_00183570;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_001818B8;
    for (i = 4 - 1; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 6 - 1; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_UNK5C(entity)->as_s32 = 0;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    if (entity->spawn_info->unk38 & (0x8000L << 43)) {
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk10, 1);
    } else {
        func_001958B8(entity, func_001408B0, 1);
    }
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

static void func_00140650(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    u64 flags;
    u64 hit_strength;

    func_00190A30((char *)entity);
    ENTITY_UNK5C(entity)->as_s32 = 0;
    flags = entity->flags & ~0x200;
    entity->flags = flags;
    hit_strength = (flags >> 36) & 3;
    if (hit_strength == 1) {
        if ((s32)(flags >> 19) & 1) {
            func_0019A310((char *)entity, (void (*)(void))func_00181378, 0);
        } else {
            func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
        }
    } else if (hit_strength == 2) {
        func_0019A310((char *)entity, (void (*)(void))func_001816A0, 0);
    }
    func_00195898((char *)entity, 2);
    unkE0->unk7A = 1;
    ENTITY_FLAG_BITS(entity)->bit32 = 1;
    ENTITY_FLAG_BITS(entity)->bit33 = 1;
    func_001409D0(entity);
}

static void func_00140738(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *part;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (entity->unkC6 == 0) {
        func_00196CC0((char *)entity);
        func_0018E9C0(entity);
        if (ENTITY_FLAG_BITS(entity)->bit53) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->state) {
            case 1:
            case 2:
                func_00183240(entity, unknown);
                unkE0->unk7A = 0;
                break;
            case 0:
            default:
                func_001958B8(entity, func_001408B0, unknown);
                break;
            }
        }
    }
    part = func_0010CAB8((char *)entity->model, 0x1E);
    Effect_StartSpawner(entity->params->unk43A, part, entity->model->unkD0, entity);
    part = func_0010CAB8((char *)entity->model, 0x1F);
    Effect_StartSpawner(entity->params->unk43A, part, entity->model->unkD0, entity);
}

static void func_00140878(Entity *entity, s32 unknown) {
    entity->unkC8 = 0;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    func_001958B8(entity, (void (*)(Entity *))entity->unkE0.unk18, unknown);
}

static void func_001408B0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x1000;
    func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->distance_to_player < entity->params->unk436 || unkE0->unk7A != 0) {
        ENTITY_FLAG_BITS(entity)->bit32 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit33 = TRUE;
        unkE0->unk7A = 1;
        func_001409D0(entity);
        entity->unkC6 = 0;
        entity->unkC8 = 0;
        entity->unkA0 = 0;
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            unkE0->unk00(entity, 0);
        } else {
            func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 0);
        }
    }
}

static void func_001409D0(Entity *entity) {
    Entity *other_entity = D_004467E0;
    while (other_entity != NULL) {
        if (entity->params == other_entity->params) {
            if (*(int *)&other_entity->flags & 1) {
                u64 other_flags = other_entity->flags;
                if (((int)(other_flags >> 1) & 1) && !((int)(other_flags >> 2) & 3) &&
                    other_entity != entity) {
                    EntityUnkE0 *other_unkE0 = &other_entity->unkE0;
                    if (other_unkE0->unk7A == 0 &&
                        func_00194F80(entity->model->position, other_entity->model->position) <
                            (float)entity->params->unk438) {
                        other_unkE0->unk7A = 1;
                    }
                }
            }
        }
        other_entity = other_entity->next;
    }
}
