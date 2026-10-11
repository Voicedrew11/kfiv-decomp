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
    char pad35;
    u16 unk36;                      /* 0x36 what func_002109C8 gets when the entity dies */
    u64 unk38;                      /* 0x38 */
    u8 unk40;                       /* 0x40 */
    u8 variant;                     /* 0x41 picks the animation set */
    u8 unk42;                       /* 0x42 ids of two map objects (see func_0011E1F0) */
    u8 unk43;                       /* 0x43 */
    char pad44[0x04];
    u16 vertical_despawn_range;     /* 0x48 0: no vertical limit */
    char pad4A[0x02];
    u16 unk4C;                      /* 0x4C */
    char pad4E[0x06];
    u16 unk54;                      /* 0x54 */
    char pad56[0x04];
    u16 despawn_range;              /* 0x5A */
} EntitySpawnInfo;

/* EntitySpawnInfo.unk38 read through a union. The game's field is a union member:
   only then does the compiler keep earlier stores of other types ahead of the read. */
typedef union {
    u64 word;
    u32 halves[2];
} EntitySpawnFlags;

#define ENTITY_SPAWN_FLAGS(entity) (((EntitySpawnFlags *)&(entity)->spawn_info->unk38)->word)

struct Entity;

/* what EntityModel.unkFC points at */
typedef struct {
    char pad00[0x82];
    s16 unk82;                      /* 0x82 */
    s32 unk84;                      /* 0x84 */
    s32 unk88;                      /* 0x88 */
} EntityModelUnkFC;

/* one 0x100-byte element of what EntityModel.unkF8 points at */
typedef struct {
    char pad00[0x50];
    f32 unk50[4];                   /* 0x50 */
    char pad60[0x08];
    u64 unk68;                      /* 0x68 flags */
    char pad70[0x90];
} EntityModelUnkF8;

/* what EntityModel.unkF8 points at, seen as 0x80-byte elements: func_0017CB38 indexes
   it that way (element 2 is at 0x100) */
typedef struct {
    char pad00[0x50];
    f32 unk50[4];                   /* 0x50 */
    char pad60[0x08];
    u64 unk68;                      /* 0x68 flags */
    char pad70[0x10];
} EntityModelPart80;

/* EntityModelPart80.unk68 as the bit-fields the game sets (see ENTITY_FLAG_BITS) */
typedef struct {
    u64 pad0 : 40;
    u64 bit40 : 1;
    u64 bit41 : 1;
    u64 pad42 : 22;
} EntityModelPartFlagBits;

#define ENTITY_MODEL_PART_FLAG_BITS(part) ((EntityModelPartFlagBits *)&(part)->unk68)

/* what Entity.model points at. ModelInstance (kf_model.h) is a second view of the same block,
   see ENTITY_MODEL; the two are to be unified. */
typedef struct {
    char pad00[0xC0];
    f32 position[4];                /* 0xC0 */
    f32 unkD0[4];                   /* 0xD0 */
    f32 unkE0[4];                   /* 0xE0 */
    char padF0[0x08];
    EntityModelUnkF8 *unkF8;        /* 0xF8 */
    EntityModelUnkFC *unkFC;        /* 0xFC */
    char pad100[0x02];
    u16 unk102;                     /* 0x102 */
    char pad104[0x34];
    void *unk138;                   /* 0x138 */
    char pad13C[0x20];
    f32 unk15C;                     /* 0x15C */
} EntityModel;

/* two blocks of an EntityParamsUnk240Part that func_00175110 copies whole into its request */
typedef struct {
    s16 unk00[9];                   /* 0x00 */
} EntityParamsUnk240PartUnk00;

typedef struct {
    s16 unk00[6];                   /* 0x00 */
} EntityParamsUnk240PartUnk12;

/* one of the three 0x28-byte parts of an EntityParamsUnk240 */
typedef struct {
    EntityParamsUnk240PartUnk00 unk00; /* 0x00 */
    EntityParamsUnk240PartUnk12 unk12; /* 0x12 */
    u8 unk1E;                       /* 0x1E */
    char pad1F;
    u16 unk20;                      /* 0x20 */
    u16 unk22;                      /* 0x22 an angle in degrees */
    u16 unk24;                      /* 0x24 first animation frame it is active on, 0: unused */
    u16 unk26;                      /* 0x26 last animation frame it is active on */
} EntityParamsUnk240Part;

/* one of the 0x88-byte records at EntityParams+0x240 (EntityParams.unk350 is the third) */
typedef struct {
    u8 unk00;                       /* 0x00 */
    u8 unk01;                       /* 0x01 */
    u8 unk02;                       /* 0x02 */
    u8 unk03;                       /* 0x03 */
    char pad04[0x08];
    u8 unk0C;                       /* 0x0C */
    char pad0D[0x03];
    EntityParamsUnk240Part unk10[3]; /* 0x10 */
} EntityParamsUnk240;

/* the two blocks func_00175110 fills for func_001B1470 */
typedef struct {
    f32 position[4];                /* 0x00 */
    char pad10[0x20];
    f32 unk30;                      /* 0x30 */
    f32 unk34;                      /* 0x34 */
    f32 unk38;                      /* 0x38 */
    f32 unk3C;                      /* 0x3C */
    char pad40[0x08];
    struct Entity *owner;           /* 0x48 */
    u64 unk4C_0 : 2;                /* 0x4C */
    u64 unk4C_2 : 1;
    u64 unk4C_3 : 1;
    u64 unk4C_4 : 4;
    u8 unk4D;                       /* 0x4D */
    u8 unk4E;                       /* 0x4E */
    char pad4F;
    EntityParamsUnk240PartUnk00 unk50; /* 0x50 */
    EntityParamsUnk240PartUnk12 unk62; /* 0x62 */
    char pad6E[0x12];
} Unk1B1470Request;

typedef struct {
    u8 unk00;                       /* 0x00 */
    char pad01;
    s16 unk02;                      /* 0x02 */
    char pad04[0x1C];
    s16 unk20;                      /* 0x20 */
    s16 unk22;                      /* 0x22 */
    char pad24[0x0C];
} Unk1B1470Extra;

/* eight bytes the game copies with unaligned loads and stores */
typedef struct {
    s64 value;
} __attribute__((packed)) EntityUnaligned64;

/* what func_001B1390 returns: something an entity launches */
typedef struct {
    char unk00;                     /* 0x00 */
    char pad01[0x1F];
    u16 unk20;                      /* 0x20 */
    s16 unk22;                      /* 0x22 */
    char pad24[0x04];
    EntityUnaligned64 unk28;        /* 0x28 copy of EntityParams.unk238 */
} EntityUnk1B1390;

/* one launch of a record at EntityParams+0x3D8 */
typedef struct {
    u8 unk00;                       /* 0x00 passed to func_001B4F30 */
    u8 unk01;                       /* 0x01 passed to func_0010CAB8; 0xFF: use the model's direction */
    u16 unk02;                      /* 0x02 animation frame it happens on, 0: unused */
} EntityParamsUnk3D8Launch;

/* a whole 0x1C-byte record at EntityParams+0x3D8 (EntityParamsUnk3D8 is its first word) */
typedef struct {
    u8 unk00;                       /* 0x00 */
    u8 unk01;                       /* 0x01 */
    u8 unk02;                       /* 0x02 */
    u8 unk03;                       /* 0x03 */
    char pad04[0x08];
    u8 unk0C;                       /* 0x0C */
    char pad0D[0x03];
    EntityParamsUnk3D8Launch unk10[3]; /* 0x10 */
} EntityParamsUnk3D8Full;

/* sub-struct at EntityParams+0x3D8 */
typedef struct {
    u8 unk00;                       /* 0x00 */
    u8 unk01;                       /* 0x01 */
    u8 unk02;                       /* 0x02 */
    u8 unk03;                       /* 0x03 */
} EntityParamsUnk3D8;

/* what Entity.params points at: per-kind tuning values. EntityController below is a second
   view of the same block; the two are to be unified. */
typedef struct {
    u32 unk00_0 : 16;               /* 0x00 */
    u32 unk02 : 8;                  /* 0x02 index into D_003E6E40 */
    u32 unk03_0 : 1;                /* 0x03 */
    u32 unk03_1 : 2;
    u32 unk03_3 : 5;
    char pad04[0x06];
    u16 unk0A;                      /* 0x0A what Entity.unk2C is measured against */
    char pad0C[0x04];
    u16 flags;                      /* 0x10 */
    char pad12[0x02];
    u8 unk14[4];                    /* 0x14 weights of the four actions, scaled by Entity.unkBC */
    char pad18[0x38];
    f32 unk50;                      /* 0x50 */
    f32 unk54;                      /* 0x54 */
    f32 unk58;                      /* 0x58 */
    f32 turn_limit;                 /* 0x5C */
    char pad60[0x20];
    u16 unk80[0x22];                /* 0x80 indexed by Entity.unkD1 */
    u8 unkC4;                       /* 0xC4 */
    char padC5;
    u16 unkC6;                      /* 0xC6 */
    u8 unkC8;                       /* 0xC8 */
    char padC9;
    u8 unkCA;                       /* 0xCA */
    u8 unkCB;                       /* 0xCB */
    char padCC[0x02];
    u16 unkCE;                      /* 0xCE */
    char padD0[0x168];
    EntityUnaligned64 unk238;       /* 0x238 */
    EntityParamsUnk240 unk240[2];   /* 0x240 indexed by flag bits 58-59 */
    char unk350[0x88];              /* 0x350 */
    EntityParamsUnk3D8 unk3D8;      /* 0x3D8 */
    char pad3DC[0x34];
    u8 unk410;                      /* 0x410 */
    char pad411[0x1D];
    u16 unk42E;                     /* 0x42E */
    s16 unk430;                     /* 0x430 */
    s16 unk432;                     /* 0x432 */
    u16 unk434;                     /* 0x434 */
    s16 unk436;                     /* 0x436 */
    s16 unk438;                     /* 0x438 */
    s16 unk43A;                     /* 0x43A */
    s16 unk43C;                     /* 0x43C */
    s16 unk43E;                     /* 0x43E */
} EntityParams;

/* EntityParams.unk3D8 is the first of several 0x1C-byte records */
#define ENTITY_PARAMS_UNK3D8(params, index) (&((EntityParamsUnk3D8Array *)&(params)->unk3D8)[index].record)

typedef struct {
    EntityParamsUnk3D8 record;      /* 0x00 */
    char pad04[0x08];
    u8 unk0C;                       /* 0x0C */
    char pad0D[0x03];
    u8 unk10;                       /* 0x10 */
    u8 unk11;                       /* 0x11 */
    char pad12[0x0A];
} EntityParamsUnk3D8Array;

/* the first word of EntityParams as plain bytes: unlike a bit-field read, a
   read through this does not depend on every store before it */
typedef struct {
    u8 unk00;                       /* 0x00 */
    u8 unk01;                       /* 0x01 */
    u8 unk02;                       /* 0x02 index into D_003E6E40 */
    u8 unk03;                       /* 0x03 */
} EntityParamsBytes;

/* the first word of EntityParams read whole (unk00_0, unk02, unk03_x together) */
typedef struct {
    u32 unk00;                      /* 0x00 bits 25-26: unk03_1 */
} EntityParamsWord;

/* sub-struct at Entity+0xE0 */
typedef struct {
    void (*unk00)(struct Entity *entity, s32 unknown); /* 0x00 */
    void (*unk04)(void);            /* 0x04 */
    void (*unk08)(void);            /* 0x08 */
    char pad0C[0x04];
    void (*unk10)(void);            /* 0x10 */
    void (*unk14)(void);            /* 0x14 */
    void (*unk18)(void);            /* 0x18 */
    f32 unk1C;                      /* 0x1C */
    f32 unk20[4];                   /* 0x20 */
    f32 unk30[4];                   /* 0x30 (Entity+0x110) a point to walk to (Entity.target points here) */
    f32 unk40;                      /* 0x40 */
    f32 unk44;                      /* 0x44 */
    f32 unk48;                      /* 0x48 */
    f32 unk4C;                      /* 0x4C */
    f32 unk50;                      /* 0x50 */
    f32 unk54;                      /* 0x54 */
    f32 unk58;                      /* 0x58 */
    f32 unk5C;                      /* 0x5C */
    f32 unk60;                      /* 0x60 */
    char pad64[0x04];
    s16 unk68;                      /* 0x68 */
    s16 unk6A;                      /* 0x6A one bit per entity 10..15 */
    s16 unk6C;                      /* 0x6C */
    s16 unk6E;                      /* 0x6E */
    char pad70[0x08];
    s8 unk78;                       /* 0x78 */
    s8 unk79;                       /* 0x79 */
    char unk7A;                     /* 0x7A */
    char unk7B;                     /* 0x7B */
    char unk7C;                     /* 0x7C */
    char pad7D[0x03];
    char unk80[0x08];               /* 0x80 (Entity+0x160) passed by address; an EntityUnkE0Slot */
    char unk88[0x08];               /* 0x88 (Entity+0x168) passed by address; an EntityUnkE0Slot */
} EntityUnkE0;

/* one of the two 8-byte slots func_0019A448 fills */
typedef struct {
    u8 unk00;                       /* 0x00 */
    u8 unk01;                       /* 0x01 */
    s16 unk02;                      /* 0x02 */
    s32 unk04;                      /* 0x04 */
} EntityUnkE0Slot;

/* Entity+0xE0 as the kinds handled at 0x1740A8..0x180000 use it */
typedef struct {
    void (*unk00)(struct Entity *entity, s32 unknown); /* 0x00 */
    char pad04[0x14];
    void (*unk18)(void);            /* 0x18 */
    s32 unk1C;                      /* 0x1C */
    char pad20[0x10];
    f32 unk30[4];                   /* 0x30 a point to move towards */
    char pad40[0x08];
    f32 unk48;                      /* 0x48 */
    f32 unk4C;                      /* 0x4C */
    f32 unk50;                      /* 0x50 */
    char pad54[0x14];
    s16 unk68;                      /* 0x68 */
    char pad6A[0x0E];
    s8 unk78;                       /* 0x78 */
    s8 unk79;                       /* 0x79 */
    s8 unk7A;                       /* 0x7A */
    char pad7B[0x05];
    EntityUnkE0Slot unk80;          /* 0x80 */
    EntityUnkE0Slot unk88;          /* 0x88 */
} EntityUnkE0Kind1740A8;

/* Entity+0xE0 as the flying kind set up by func_001719F8 lays it out */
typedef struct {
    void (*unk00)(struct Entity *entity, s32 unknown); /* 0x00 */
    char pad04[0x14];
    void (*unk18)(void);            /* 0x18 */
    f32 unk1C;                      /* 0x1C turn speed */
    char pad20[0x24];
    f32 unk44;                      /* 0x44 speed sideways */
    f32 unk48;                      /* 0x48 speed forwards */
    f32 unk4C;                      /* 0x4C pitch speed */
    char pad50[0x28];
    s8 unk78;                       /* 0x78 */
    s8 unk79;                       /* 0x79 climb (above 0), dive (below 0) or level out (0) */
    char pad7A[0x06];
    EntityUnkE0Slot unk80;          /* 0x80 */
    EntityUnkE0Slot unk88;          /* 0x88 */
    char pad90[0x10];
} EntityUnkE0Kind1719F8;

/* Entity+0xE0 as the kind set up by func_00178358 lays it out */
typedef struct {
    void (*unk00)(struct Entity *entity, s32 unknown); /* 0x00 */
    char pad04[0x14];
    void (*unk18)(void);            /* 0x18 */
    s32 unk1C;                      /* 0x1C */
    f32 unk20[4];                   /* 0x20 */
    f32 unk30[4];                   /* 0x30 where the entity walks to (Entity.target points here) */
    char pad40[0x3A];
    s8 unk7A;                       /* 0x7A state of map object spawn_info->unk42 */
    s8 unk7B;                       /* 0x7B state of map object spawn_info->unk43 */
    s8 unk7C;                       /* 0x7C id of the map object to operate */
    char pad7D[0x23];
} EntityUnkE0Kind178358;

/* Entity+0xE0 as the kind set up by func_00150588 lays it out */
typedef struct {
    char pad00[0x1C];
    s32 unk1C;                      /* 0x1C */
} EntityUnkE0Kind150588;

/* Entity+0xE0 as the kind set up by func_00168160 lays it out. The area is
   8-byte aligned: tests of neighbouring bytes are merged into one 64-bit load. */
typedef struct {
    void (*unk00)(struct Entity *entity, s32 unknown); /* 0x00 */
    void (*unk04)(void);            /* 0x04 */
    void (*unk08)(void);            /* 0x08 */
    char pad0C[0x04];
    void (*unk10)(void);            /* 0x10 */
    void (*unk14)(void);            /* 0x14 */
    void (*unk18)(void);            /* 0x18 */
    f32 unk1C;                      /* 0x1C */
    f32 unk20[4];                   /* 0x20 */
    char pad30[0x10];
    f32 unk40;                      /* 0x40 */
    f32 unk44;                      /* 0x44 */
    char pad48[0x22];
    s16 unk6A;                      /* 0x6A */
    s16 unk6C;                      /* 0x6C */
    s16 unk6E;                      /* 0x6E */
    char pad70[0x08];
    s8 unk78;                       /* 0x78 */
    s8 unk79;                       /* 0x79 */
    s8 unk7A;                       /* 0x7A */
    s8 unk7B;                       /* 0x7B */
    s8 unk7C;                       /* 0x7C */
    char pad7D[0x23];
} __attribute__((aligned(8))) EntityUnkE0Kind168160;

/* an 8-byte slot of the work area as the bit-fields the init functions set */
typedef struct {
    u8 index : 5;                   /* what func_0019A448 looks up */
    u64 bit5 : 1;
    u64 bit6 : 1;
    u64 pad7 : 1;
    u8 unk01;                       /* 0x01 */
    s16 unk02;                      /* 0x02 */
    s32 unk04;                      /* 0x04 */
} EntityUnkE0SlotBits;

/* Entity+0xE0 as the kinds set up by func_00151D30 and its neighbours use it */
typedef struct {
    char pad00[0x80];
    EntityUnkE0SlotBits unk80;      /* 0x80 */
    EntityUnkE0SlotBits unk88;      /* 0x88 */
} EntityUnkE0Kind151D30;

/* Entity+0xE0 as the kind set up by func_0016BF98 lays it out */
typedef struct {
    char pad00[0x1C];
    f32 unk1C;                      /* 0x1C turn speed */
    char pad20[0x24];
    f32 unk44;                      /* 0x44 speed along x, y and z */
    f32 unk48;                      /* 0x48 */
    f32 unk4C;                      /* 0x4C */
    char pad50[0x50];
} EntityUnkE0Kind16BF98;

/* Entity+0xE0 as the kind set up by func_0015F8C0 lays it out. The area is
   8-byte aligned: only then is "unk68 == 0 && unk6A == 0" one lwu. */
typedef struct {
    char pad00[0x68];
    s16 unk68;                      /* 0x68 counts down */
    s16 unk6A;                      /* 0x6A counts down */
    s16 unk6C;                      /* 0x6C counts down */
    char pad6E[0x0A];
    s8 unk78;                       /* 0x78 */
    s8 unk79;                       /* 0x79 */
    s8 unk7A;                       /* 0x7A */
    s8 unk7B;                       /* 0x7B */
    s8 unk7C;                       /* 0x7C */
    s8 unk7D;                       /* 0x7D */
    s8 unk7E;                       /* 0x7E */
    char pad7F[0x21];
} __attribute__((aligned(8))) EntityUnkE0Kind15F8C0;

/* Entity+0xE0 as the kind set up by func_00162BC8 lays it out */
typedef struct {
    char pad00[0x30];
    f32 unk30[4];                   /* 0x30 pitch and yaw passed to func_0018D600 */
    char pad40[0x2E];
    s16 unk6E;                      /* 0x6E */
    char pad70[0x10];
    f32 unk80[4];                   /* 0x80 pitch and yaw passed to func_0018D600 */
    f32 unk90[4];                   /* 0x90 pitch and yaw passed to func_0018D600 */
} EntityUnkE0Kind162BC8;

/* what Entity.params points at (the entity's controller / type data), see ENTITY_CONTROLLER.
   EntityParams above is a second view of the same block; the two are to be unified. */
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

typedef struct Entity {
    EntityModel *model;             /* 0x00 position at +0xC0; ENTITY_MODEL() is the ModelInstance view */
    char pad04[0x08];
    EntityParams *params;           /* 0x0C ENTITY_CONTROLLER() is the EntityController view */
    EntitySpawnInfo *spawn_info;    /* 0x10 */
    void *unk14;                    /* 0x14 */
    struct Entity *prev;            /* 0x18 */
    struct Entity *next;            /* 0x1C */
    char pad20[0x0A];
    u16 unk2A;                      /* 0x2A */
    u16 unk2C;                      /* 0x2C */
    char pad2E[0x02];
    f32 unk30;                      /* 0x30 */
    f32 unk34;                      /* 0x34 */
    f32 radius;                     /* 0x38 */
    f32 height;                     /* 0x3C */
    char pad40[0x1C];
    void *target;                   /* 0x5C */
    f32 unk60;                      /* 0x60 */
    f32 distance_to_player;         /* 0x64 */
    f32 unk68;                      /* 0x68 */
    char pad6C[0x16];
    u16 unk82;                      /* 0x82 */
    f32 unk84;                      /* 0x84 */
    f32 unk88;                      /* 0x88 */
    f32 unk8C;                      /* 0x8C */
    f32 unk90;                      /* 0x90 */
    f32 unk94;                      /* 0x94 */
    char pad98[0x04];
    u16 timer;                      /* 0x9C */
    u16 unk9E;                      /* 0x9E */
    u16 unkA0;                      /* 0xA0 */
    u8 unkA2;                       /* 0xA2 */
    u8 unkA3;                       /* 0xA3 */
    f32 unkA4[3];                   /* 0xA4 a push applied while flag bit 45 or 46 is set */
    f32 unkB0[3];                   /* 0xB0 a push applied while flag bit 48 is set */
    u8 unkBC[4];                    /* 0xBC */
    u8 unkC0[6];                    /* 0xC0 */
    u16 unkC6;                      /* 0xC6 */
    u16 unkC8;                      /* 0xC8 */
    s8 action_step;                 /* 0xCA step of the current action */
    s8 unkCB;                       /* 0xCB step counter used by some kinds instead of action_step */
    s8 state;                       /* 0xCC */
    s8 unkCD;                       /* 0xCD */
    char prev_state;                /* 0xCE */
    s8 unkCF;                       /* 0xCF */
    char padD0;
    s8 unkD1;                       /* 0xD1 */
    char unkD2;                     /* 0xD2 copied from its pool block */
    char unkD3;                     /* 0xD3 pool block index, -1: none */
    char unkD4;                     /* 0xD4 work area index, -1: none */
    char group_id;                  /* 0xD5 */
    u8 unkD6;                       /* 0xD6 */
    u8 unkD7;                       /* 0xD7 */
    u64 flags;                      /* 0xD8 */
    EntityUnkE0 unkE0;              /* 0xE0 */
} Entity;

/* Entity with the work area at 0xE0 as plain bytes: the init functions clear it
   one byte at a time, and only an array member gives their code */
#define ENTITY_WORK_AREA_SIZE 0xA0

typedef struct {
    char pad00[0xE0];
    u8 work_bytes[ENTITY_WORK_AREA_SIZE];   /* 0xE0 */
} EntityWorkBytes;

#define ENTITY_WORK_BYTES(entity) (((EntityWorkBytes *)(entity))->work_bytes)

/* Entity.flags as bit-fields. The game sets the flags through bit-fields:
   several changed in a row only match when written this way. */
typedef struct {
    u32 bit0 : 1;
    u32 bit1 : 1;
    u32 bits2 : 2;
    u32 bit4 : 1;
    u32 bit5 : 1;
    u32 pad6 : 3;
    u32 bit9 : 1;
    u32 bit10 : 1;
    u32 bits11 : 3;
    u32 bit14 : 1;
    u32 pad15 : 1;
    u32 bit16 : 1;
    u32 bit17 : 1;
    u32 bit18 : 1;
    u32 bit19 : 1;
    u32 bit20 : 1;
    u32 bit21 : 1;
    u32 bit22 : 1;
    u32 bit23 : 1;
    u32 bit24 : 1;
    u32 bit25 : 1;
    u32 bit26 : 1;
    u32 bit27 : 1;
    u32 bit28 : 1;
    u32 bit29 : 1;
    u32 pad30 : 1;
    u32 bit31 : 1;
    u32 bit32 : 1;
    u32 pad33 : 1;
    u32 bit34 : 1;
    u32 bit35 : 1;
    u32 bits36 : 2;
    u32 bit38 : 1;
    u32 bit39 : 1;
    u32 pad40 : 3;
    u32 bit43 : 1;
    u32 bit44 : 1;
    u32 bit45 : 1;
    u32 bit46 : 1;
    u32 pad47 : 6;
    u32 bit53 : 1;
    u32 bit54 : 1;
    u32 bit55 : 1;
    u32 bit56 : 1;
    u64 bit57 : 1;
    u32 bits58 : 2;
    u64 pad60 : 4;
} EntityFlagBits;

#define ENTITY_FLAG_BITS(entity) ((EntityFlagBits *)&(entity)->flags)

/* Entity.target is a union member in the game: where a store to it has to stay
   ahead of a later load, only a store through a union matches. */
typedef union {
    s32 as_s32;
    f32 as_f32;
} EntityUnk5C;

#define ENTITY_UNK5C(entity) ((EntityUnk5C *)&(entity)->target)

/* the first word of EntityParams with bits 25 and 26 as separate fields */
typedef struct {
    u32 pad0 : 25;
    u32 bit25 : 1;
    u32 bit26 : 1;
    u32 pad27 : 5;
} EntityParamsBits;

#define ENTITY_PARAMS_BITS(entity) ((EntityParamsBits *)(entity)->params)

/* a 64-bit flag word read through a union: gcc then lets the read alias any
   earlier store, which changes how it orders loads and stores around it */
typedef union {
    u64 word;
    u8 bytes[8];
} EntityFlagWord;

#define ENTITY_FLAG_WORD(flags) (((EntityFlagWord *)&(flags))->word)

/* the block at D_00413F80: the player (position first). Files declare
   `extern PlayerData D_00413F80;` themselves: older files still use an array. */
typedef struct {
    f32 position[4];                /* 0x00 */
    char pad10[0x04];
    f32 unk14;                      /* 0x14 */
    char pad18[0x1EC];
    Entity *unk204;                 /* 0x204 */
    char pad208[0x4A];
    u16 unk252;                     /* 0x252 */
    char pad254[0x05];
    u8 unk259;                      /* 0x259 */
} PlayerData;

/* four animation frame numbers: a local table that a function copies whole
   from read-only data (in the game: `s32 frames[4] = { ... };`) */
typedef struct {
    s32 frames[4];                  /* 0x00 */
} EntityFrameTable;

/* a point or direction as the game stores it (16 bytes) */
typedef struct {
    f32 x;                          /* 0x00 */
    f32 y;                          /* 0x04 */
    f32 z;                          /* 0x08 */
    f32 w;                          /* 0x0C */
} KfVector;

/* one of the 0x80-byte records MapSlot.records (kf_map.h) points at */
typedef struct {
    char pad00[0x4E];
    u8 unk4E;                       /* 0x4E */
    char pad4F[0x31];
} MapRecord;

/* one of the 0x80-byte records MapSlot.unk10 (kf_map.h) points at, indexed by byte 3 of
   EntitySpawnInfo.unk38 */
typedef struct {
    char pad00[0x70];
    f32 unk70;                      /* 0x70 */
    f32 unk74;                      /* 0x74 */
    char pad78[0x08];
} MapSlotUnk10;

/* Entity.flags as retail reads it where a store through another pointer (the work area, say) sits
   between two uses: a union member can alias anything, so the compiler loads the flags again
   instead of reusing the value it has. Use ENTITY_FLAGS(entity) in place of entity->flags when
   the retail code has that second ld. */
typedef union {
    u64 all;
} EntityFlags;
#define ENTITY_FLAGS(entity) (((EntityFlags *)&(entity)->flags)->all)

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

/* upstream's typed views of the two blocks an Entity points at */
#define ENTITY_MODEL(entity) ((ModelInstance *)(entity)->model)
#define ENTITY_CONTROLLER(entity) ((EntityController *)(entity)->params)

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
