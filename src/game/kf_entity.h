/* Entities: the live objects kept on singly linked lists. */
#ifndef KF_ENTITY_H
#define KF_ENTITY_H

#include "common.h"

#define ENTITY_DESPAWN_MARGIN           300.0f
#define ENTITY_VERTICAL_DESPAWN_MARGIN  150.0f

/* what Entity.spawn_info points at */
typedef struct {
    char pad00[0x04];
    f32 spawn_y;                    /* 0x04 */
    char pad08[0x40];
    u16 vertical_despawn_range;     /* 0x48 0: no vertical limit */
    char pad4A[0x10];
    u16 despawn_range;              /* 0x5A */
} EntitySpawnInfo;

struct Entity;

/* what EntityModel.unkFC points at */
typedef struct {
    char pad00[0x82];
    s16 unk82;                      /* 0x82 */
    char pad84[0x04];
    s32 unk88;                      /* 0x88 */
} EntityModelUnkFC;

/* what Entity.model points at */
typedef struct {
    char pad00[0xC0];
    f32 position[4];                /* 0xC0 */
    f32 unkD0[4];                   /* 0xD0 */
    char padE0[0x1C];
    EntityModelUnkFC *unkFC;        /* 0xFC */
} EntityModel;

/* what Entity.params points at: per-kind tuning values */
typedef struct {
    char pad00[0xC4];
    u8 unkC4;                       /* 0xC4 */
    char padC5[0x36F];
    u16 unk434;                     /* 0x434 */
    s16 unk436;                     /* 0x436 */
    s16 unk438;                     /* 0x438 */
    s16 unk43A;                     /* 0x43A */
    s16 unk43C;                     /* 0x43C */
    s16 unk43E;                     /* 0x43E */
} EntityParams;

/* sub-struct at Entity+0xE0 */
typedef struct {
    void (*unk00)(struct Entity *entity, s32 unknown); /* 0x00 */
    char pad04[0x14];
    s32 unk18;                      /* 0x18 */
    char pad1C[0x4C];
    s16 unk68;                      /* 0x68 */
    s16 unk6A;                      /* 0x6A one bit per entity 10..15 */
    char pad6C[0x0E];
    char unk7A;                     /* 0x7A */
    char pad7B[0x0F];
    s16 unk8A;                      /* 0x8A */
} EntityUnkE0;

typedef struct Entity {
    EntityModel *model;             /* 0x00 */
    char pad04[0x08];
    EntityParams *params;           /* 0x0C */
    EntitySpawnInfo *spawn_info;    /* 0x10 */
    char pad14[0x08];
    struct Entity *next;            /* 0x1C */
    char pad20[0x1C];
    f32 height;                     /* 0x3C */
    char pad40[0x24];
    f32 distance_to_player;         /* 0x64 */
    char pad68[0x38];
    u16 unkA0;                      /* 0xA0 */
    char padA2[0x24];
    u16 unkC6;                      /* 0xC6 */
    u16 unkC8;                      /* 0xC8 */
    s8 state;                       /* 0xCA step of the current action */
    char padCB[0x08];
    char unkD3;                     /* 0xD3 */
    char padD4;
    char group_id;                  /* 0xD5 */
    char padD6[0x02];
    u64 flags;                      /* 0xD8 */
    EntityUnkE0 unkE0;              /* 0xE0 */
} Entity;

extern Entity *D_004467E0;          /* entity list head */
extern Entity *D_004470A8;          /* other lists of the same layout */
extern Entity *D_004470B0;

float func_00124DE0(Entity *entity);    /* distance to the player */
void func_00123AB8(Entity *entity);
void func_00124BC8(Entity *entity);
Entity *func_00124C08(Entity *entity);  /* free; returns the next one */
Entity *func_00124CA0(Entity *entity);  /* free; returns the next one */
Entity *func_00124D48(Entity *entity);  /* free; returns the next one */

void KF_Entity_ResetAllLists(void);
void KF_Entity_FreeAllInGroup(int group_id);
void KF_Entity_DespawnOutOfRange(void);

#endif
