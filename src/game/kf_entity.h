/* Entities: the live objects kept on singly linked lists. */
#ifndef KF_ENTITY_H
#define KF_ENTITY_H

#include "common.h"
#include "kf_model.h"

#define ENTITY_DESPAWN_MARGIN           300.0f
#define ENTITY_VERTICAL_DESPAWN_MARGIN  150.0f

/* what Entity.spawn_info points at */
typedef struct {
    char pad00[0x04];
    f32 spawn_y;                    /* 0x04 */
    char pad08[0x28];
    u16 type_id;                    /* 0x30 which pool block holds its data */
    char pad32[0x02];
    s8 pool_size;                   /* 0x34 0-4: one pool block, else two */
    char pad35[0x03];
    u16 unk38;                      /* 0x38 */
    char pad3A[0x07];
    u8 variant;                     /* 0x41 picks the animation set */
    char pad42[0x06];
    u16 vertical_despawn_range;     /* 0x48 0: no vertical limit */
    char pad4A[0x10];
    u16 despawn_range;              /* 0x5A */
} EntitySpawnInfo;

/* sub-struct at Entity+0xE0 */
typedef struct {
    char pad00[0x18];
    void (*unk18)(void);            /* 0x18 */
    char pad1C[0x14];
    char unk30[0x18];               /* 0x30 (Entity+0x110) passed by address */
    f32 unk48;                      /* 0x48 */
    f32 unk4C;                      /* 0x4C */
    f32 unk50;                      /* 0x50 */
    char pad54[0x14];
    s16 unk68;                      /* 0x68 */
    char pad6A[0x10];
    char unk7A;                     /* 0x7A */
    char pad7B;
    char unk7C;                     /* 0x7C */
    char pad7D[0x03];
    char unk80[0x08];               /* 0x80 (Entity+0x160) passed by address */
    char unk88[0x08];               /* 0x88 (Entity+0x168) passed by address */
} EntityUnkE0;

/* what Entity.unk0C points at (the entity's controller / type data) */
typedef struct {
    char pad00[0x10];
    u16 flags;                      /* 0x10 */
    u16 unk12;                      /* 0x12 reload value for Entity.timer */
    char pad14[0x48];
    f32 turn_limit;                 /* 0x5C */
    char pad60[0x64];
    u8 unkC4;                       /* 0xC4 */
    char padC5;
    u16 unkC6;                      /* 0xC6 */
    char padC8[0x368];
    s16 unk430;                     /* 0x430 */
    char pad432[0x02];
    s16 unk434;                     /* 0x434 */
    char pad436[0x02];
    s16 unk438;                     /* 0x438 */
    s16 unk43A;                     /* 0x43A */
} EntityController;

/* EntityController.flags: the type needs two pool blocks (0x100000 bytes) */
#define CONTROLLER_FLAG_TWO_BLOCKS  0x8000

/* bit view of Entity.flags, for the files that set single bits */
typedef struct {
    u64 bit0 : 1;                   /* on list D_004467E0 */
    u64 pad1 : 8;
    u64 bit9 : 1;
    u64 bit10 : 1;
    u64 bits11 : 3;
    u64 pad14 : 4;
    u64 bit18 : 1;
    u64 bit19 : 1;
    u64 bit20 : 1;
    u64 bit21 : 1;
    u64 pad22 : 42;
} EntityFlagBits;

typedef struct Entity {
    int *model;                     /* 0x00 ModelInstance, position at +0xC0 */
    char pad04[0x08];
    char *unk0C;                    /* 0x0C EntityController */
    EntitySpawnInfo *spawn_info;    /* 0x10 */
    char pad14[0x04];
    struct Entity *prev;            /* 0x18 */
    struct Entity *next;            /* 0x1C */
    char pad20[0x0C];
    u16 unk2C;                      /* 0x2C */
    char pad2E[0x0A];
    f32 radius;                     /* 0x38 */
    f32 height;                     /* 0x3C */
    char pad40[0x1C];
    void *target;                   /* 0x5C */
    char pad60[0x04];
    f32 distance_to_player;         /* 0x64 */
    char pad68[0x1A];
    u16 unk82;                      /* 0x82 */
    char pad84[0x18];
    u16 timer;                      /* 0x9C */
    char pad9E[0x02];
    u16 unkA0;                      /* 0xA0 */
    char padA2[0x24];
    u16 unkC6;                      /* 0xC6 */
    s16 unkC8;                      /* 0xC8 */
    char padCA[0x02];
    u8 state;                       /* 0xCC */
    char padCD;
    char prev_state;                /* 0xCE */
    char padCF[0x02];
    char unkD1;                     /* 0xD1 */
    char unkD2;                     /* 0xD2 copied from its pool block */
    char unkD3;                     /* 0xD3 pool block index, -1: none */
    char unkD4;                     /* 0xD4 work area index, -1: none */
    char group_id;                  /* 0xD5 */
    char padD6[0x02];
    s64 flags;                      /* 0xD8 */
    EntityUnkE0 unkE0;              /* 0xE0 */
} Entity;

/* a turn request handed to func_00195AC0 with the entity */
typedef struct {
    char pad00[0x0E];
    s16 degrees;                    /* 0x0E */
} TurnCommand;

/* D_0034B6D0[EntityController.unk43A]: state shared by a group of entities */
typedef struct {
    char unk00;                     /* 0x00 */
    char unk01;                     /* 0x01 */
    char unk02;                     /* 0x02 set when a member dies */
    char pad03[0x3D];
} EntityGroupRecord;

#define ENTITY_POOL_SLOT_COUNT  5

/* EntityPoolSlot.head.fields.kind */
#define ENTITY_POOL_FREE        0
#define ENTITY_POOL_QUEUED      1   /* type assigned, not loaded */
#define ENTITY_POOL_READING     2
#define ENTITY_POOL_UNPACKING   3
#define ENTITY_POOL_READY       4
#define ENTITY_POOL_EXTENSION   5   /* second block of a two-block type */
#define ENTITY_POOL_6           6

/* EntityPoolSlot.head.word flag bits */
#define ENTITY_POOL_FLAG_100    0x100
#define ENTITY_POOL_FLAG_200    0x200

/* one block of the entity pool, D_003D6DF0[5] (KF_Entity_ResetAllLists.c's
   EntityPoolBlock, with the first word's bit view) */
typedef struct {
    union {
        u32 word;                   /* ENTITY_POOL_FLAG_* in bits 8, 9 */
        struct {
            u8 kind;                /* 0x00 0: free, 4/5: in use */
            u16 unk2;               /* 0x02 */
        } fields;
        struct {
            u32 kind : 8;
            u32 flag_100 : 1;       /* ENTITY_POOL_FLAG_100 */
            u32 flag_200 : 1;       /* ENTITY_POOL_FLAG_200 */
            u32 pad10 : 6;
            u32 unk2 : 16;
        } bits;
    } head;
    s16 unk4;                       /* 0x04 entities using it */
    u8 unk6;                        /* 0x06 */
    u8 unk7;                        /* 0x07 copied to Entity.unkD2 */
    int age;                        /* 0x08 D_00446ED4 when last released; the oldest is reused first */
    int base_address;               /* 0x0C */
} EntityPoolSlot;

extern EntityPoolSlot D_003D6DF0[ENTITY_POOL_SLOT_COUNT];

/* the two point lists at D_004470C8: points that follow another vector */
typedef struct LinkedPoint {
    Vector4 position;               /* 0x00 w is kept when following */
    char pad10[0x10];
    Vector4 *follow;                /* 0x20 NULL: stays put */
    char pad24[0x18];
    struct LinkedPoint *next;       /* 0x3C */
} LinkedPoint;

/* typed views of fields Entity keeps as plain storage */
#define ENTITY_MODEL(entity) ((ModelInstance *)(entity)->model)
#define ENTITY_CONTROLLER(entity) ((EntityController *)(entity)->unk0C)
#define ENTITY_FLAG_BITS(entity) ((EntityFlagBits *)&(entity)->flags)

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
