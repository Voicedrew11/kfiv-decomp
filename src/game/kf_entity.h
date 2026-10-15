/* Entities: the live objects kept on singly linked lists. */
#ifndef KF_ENTITY_H
#define KF_ENTITY_H

#include "common.h"
#include "kf_model.h"

#define ENTITY_DESPAWN_MARGIN           300.0f
#define ENTITY_VERTICAL_DESPAWN_MARGIN  150.0f

/* what Entity.spawn_info points at */
typedef struct {
    f32 spawn_x;                    /* 0x00 the block starts with the spawn position */
    f32 spawn_y;                    /* 0x04 */
    f32 spawn_z;                    /* 0x08 */
    char pad0C[0x04];
    f32 unk10;                      /* 0x10 start of the rotation it spawns with (unk14 is its y) */
    f32 unk14;                      /* 0x14 an angle (the facing it spawns with?) */
    f32 unk18;                      /* 0x18 third rotation component (ScriptCmd_GetEntitySpawnRotation) */
    char pad1C[0x04];
    f32 unk20[4];                   /* 0x20 the scale it spawns with */
    u16 type_id;                    /* 0x30 which pool block holds its data */
    char pad32[0x02];
    s8 pool_size;                   /* 0x34 0-4: one pool block, else two */
    u8 unk35;                       /* 0x35 strips of CollisionMesh.unk24 rated below it are skipped (EntityCollision_SphereAgainstMesh) */
    u16 unk36;                      /* 0x36 what Script_Start gets when the entity dies */
    u64 unk38;                      /* 0x38 */
    u8 unk40;                       /* 0x40 the mode of this spawn: picks among the behaviours of its type, most often the think handler the set-up function installs */
    u8 variant;                     /* 0x41 picks the animation set */
    u8 unk42;                       /* 0x42 ids of two map objects (see MapObject_GetById) */
    u8 unk43;                       /* 0x43 */
    u8 unk44;                       /* 0x44 an animation frame number */
    char pad45[0x03];
    u16 vertical_despawn_range;     /* 0x48 0: no vertical limit */
    char pad4A[0x02];
    u16 unk4C;                      /* 0x4C */
    u16 unk4E;                      /* 0x4E asked of Script_IsRunning before unk58 is used, 0: nothing */
    u16 unk50;                      /* 0x50 0: Entity_InitFromMapSpawns makes it 1800 */
    u8 unk52;                       /* 0x52 0: Entity_InitFromMapSpawns makes it 100 */
    char pad53;
    u16 unk54;                      /* 0x54 how near the player has to come for a dormant entity to get up (EntityAi_Dormant) */
    u16 unk56;                      /* 0x56 */
    u16 unk58;                      /* 0x58 passed to Script_Start when an item is used on the entity, 0: nothing */
    u16 despawn_range;              /* 0x5A */
} EntitySpawnInfo;

/* EntitySpawnInfo.unk38 read through a union. The game's field is a union member:
   only then does the compiler keep earlier stores of other types ahead of the read. */
typedef union {
    u64 word;
    u32 halves[2];
    u8 bytes[8];                    /* [4]: spawn id for entity_by_spawn_id; [6]: 0 becomes 80 (Entity_InitFromMapSpawns) */
} EntitySpawnFlags;

#define ENTITY_SPAWN_FLAGS(entity) (((EntitySpawnFlags *)&(entity)->spawn_info->unk38)->word)

struct Entity;

/* what EntityModel.unkFC points at */
typedef struct {
    char pad00[0x81];
    u8 unk81;                       /* 0x81 */
    s16 unk82;                      /* 0x82 */
    s32 unk84;                      /* 0x84 */
    s32 unk88;                      /* 0x88 */
    u16 unk8C;                      /* 0x8C set from EntityParams.unk80[] */
} EntityModelUnkFC;

/* one 0x100-byte element of what EntityModel.unkF8 points at */
typedef struct {
    char pad00[0x50];
    f32 unk50[4];                   /* 0x50 */
    char pad60[0x08];
    u64 unk68;                      /* 0x68 flags */
    char pad70[0x90];
} EntityModelUnkF8;

/* what EntityModel.unkF8 points at, seen as 0x80-byte elements: MutantWall_TurnPart2ToPlayer indexes
   it that way (element 2 is at 0x100) */
typedef struct {
    char pad00[0x50];
    f32 unk50[4];                   /* 0x50 */
    char pad60[0x08];
    u64 unk68;                      /* 0x68 flags */
    char pad70[0x10];
} EntityModelPart80;

/* EntityModelPart80 with the byte at 0x6C, which lies inside unk68 (reached by cast) */
typedef struct {
    char pad00[0x6C];
    s8 unk6C;                       /* 0x6C */
    char pad6D[0x13];
} EntityModelPart80Byte;

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
    char pad104[0x30];
    s32 unk134;                     /* 0x134 */
    void *unk138;                   /* 0x138 */
    f32 unk13C;                     /* 0x13C the larger of Entity.unk30 and unk34 */
    char pad140[0x1C];
    f32 unk15C;                     /* 0x15C */
} EntityModel;

/* two blocks of an EntityParamsUnk240Part that ParaStool_CheckPartHitInPlayerView copies whole into its request */
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
    u8 unk1F;                       /* 0x1F */
    u16 unk20;                      /* 0x20 */
    u16 unk22;                      /* 0x22 an angle in degrees */
    u16 unk24;                      /* 0x24 first animation frame it is active on, 0: unused */
    u16 unk26;                      /* 0x26 last animation frame it is active on */
} EntityParamsUnk240Part;

/* EntityParamsUnk240Part.unk1E as EntityAi_SendMeleeHit reads it: bit 1 of the halfword at 0x1E
   says unk1F is copied (reached by cast: a union changes gcc's scheduling) */
typedef struct {
    u16 flags;                      /* 0x1E bit 1: unk1F is copied */
} EntityParamsUnk240PartFlags;

#define ENTITY_PART_FLAGS(part) ((EntityParamsUnk240PartFlags *)&(part)->unk1E)

/* one of the 0x88-byte records at EntityParams+0x240 (EntityParams.unk350 is the third) */
typedef struct {
    u8 unk00;                       /* 0x00 */
    u8 unk01;                       /* 0x01 */
    u8 unk02;                       /* 0x02 */
    u8 unk03;                       /* 0x03 */
    u16 unk04;                      /* 0x04 nearest the player may be for it, 0: no limit */
    u16 unk06;                      /* 0x06 farthest the player may be for it */
    f32 unk08;                      /* 0x08 widest angle to the player for it */
    u8 unk0C;                       /* 0x0C */
    char pad0D;
    s16 unk0E;                      /* 0x0E degrees its direction is off the entity's heading */
    EntityParamsUnk240Part unk10[3]; /* 0x10 */
} EntityParamsUnk240;

/* the two blocks ParaStool_CheckPartHitInPlayerView fills for func_001B1470 */
typedef struct {
    f32 position[4];                /* 0x00 */
    f32 unk10[4];                   /* 0x10 an earlier position: func_001B2F70 takes the hit direction from it */
    f32 unk20[4];                   /* 0x20 shape 2: a second point (func_001B4F90) */
    f32 unk30;                      /* 0x30 */
    f32 unk34;                      /* 0x34 */
    f32 unk38;                      /* 0x38 */
    f32 unk3C;                      /* 0x3C */
    f32 unk40;                      /* 0x40 unk40, unk44, unk78 and unk7C: more of the shape of the player's swept requests (func_001B1B90) */
    f32 unk44;                      /* 0x44 */
    struct Entity *owner;           /* 0x48 */
    u64 unk4C_0 : 2;                /* 0x4C */
    u64 unk4C_2 : 1;
    u64 unk4C_3 : 1;
    u64 unk4C_4 : 4;
    u8 unk4D;                       /* 0x4D */
    u8 unk4E;                       /* 0x4E */
    u8 unk4F;                       /* 0x4F copied to Entity.unk82 by a hit of kind 1 */
    EntityParamsUnk240PartUnk00 unk50; /* 0x50 */
    EntityParamsUnk240PartUnk12 unk62; /* 0x62 */
    u8 unk6E;                       /* 0x6E a weapon hit's damage to a map object is multiplied by it when the object asks (func_001B3AC0) */
    u8 unk6F;                       /* 0x6F chance in percent that the hit counts double */
    u8 unk70;                       /* 0x70 chance in percent that the hit kills outright */
    u8 unk71;                       /* 0x71 taken off PlayerState.mp_now when the hit lands on the player (func_001B29C8) */
    u16 unk72;                      /* 0x72 not 0: the hit does nothing to the player */
    u8 unk74;                       /* 0x74 set once the hit has landed on the player */
    char pad75[0x03];
    f32 unk78;                      /* 0x78 */
    f32 unk7C;                      /* 0x7C */
} Unk1B1470Request;

typedef struct {
    u8 unk00;                       /* 0x00 */
    char pad01;
    s16 unk02;                      /* 0x02 */
    s32 unk04;                      /* 0x04 */
    char pad08[0x06];
    s16 unk0E[6];                   /* 0x0E */
    char pad1A[0x02];
    s32 unk1C;                      /* 0x1C */
    s16 unk20;                      /* 0x20 */
    s16 unk22;                      /* 0x22 */
    s16 unk24;                      /* 0x24 */
    s16 unk26;                      /* 0x26 */
    char pad28[0x08];
} Unk1B1470Extra;

/* Unk1B1470Request with the bits at 0x4C as byte-sized fields: func_001B2F70 reads unk4C_3 as a u8 */
typedef struct {
    char pad00[0x4C];
    u8 unk4C_0 : 3;                 /* 0x4C */
    u8 unk4C_3 : 1;
    u8 unk4C_4 : 4;
    char pad4D[0x33];
} __attribute__((aligned(8))) Unk1B1470RequestBits;

/* Unk1B1470Request with the four flag bytes from 0x4C as one 32-bit field, the way
   func_001B1470 reads the shape in the low two bits */
typedef struct {
    char pad00[0x48];
    struct Entity *owner;           /* 0x48 */
    u64 flags : 32;                 /* 0x4C */
} Unk1B1470RequestWord;

/* Unk1B1470Request with the owner and the flag bytes after it as the one 64-bit word
   func_001B1510 and func_001B29C8 test bits of (the flag bytes are its upper half) */
typedef struct {
    char pad00[0x48];
    u64 owner_and_flags;            /* 0x48 */
} Unk1B1470RequestUnit;

#define UNK1B1470_REQUEST_UNIT(request) (((Unk1B1470RequestUnit *)(request))->owner_and_flags)
/* unk4C_3 as a bit of the 64-bit word */
#define UNK1B1470_UNIT_UNK4C_3 (0x8000L << 20)

/* eight bytes the game copies with unaligned loads and stores */
typedef struct {
    s64 value;
} __attribute__((packed)) EntityUnaligned64;

/* what Shot_Alloc returns: something an entity launches */
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

typedef struct {
    EntityParamsUnk3D8 record;      /* 0x00 */
    u16 reach_min;                  /* 0x04 nearest distance (past the radius) the attack is used at, 0: none */
    u16 reach_max;                  /* 0x06 farthest distance it is used at */
    f32 unk08;                      /* 0x08 widest angle to the player for it */
    u8 unk0C;                       /* 0x0C */
    char pad0D;
    s16 unk0E;                      /* 0x0E degrees its direction is off the entity's heading */
    u8 unk10;                       /* 0x10 */
    u8 unk11;                       /* 0x11 */
    char pad12[0x0A];
} EntityParamsUnk3D8Array;

/* one of the four bodies of an entity kind the player collides with piece by piece (EntityParams.unk20) */
/* the same slot as the motion code reads it: every flag a byte-wide bit-field (Motion_StepSlot) */
typedef struct {
    u8 index : 5;                   /* 0x00 which EntityParams.unk60 entry (motion) it plays */
    u8 moves_entity : 1;            /* the root part's movement in the motion is handed to the entity */
    u8 bit6 : 1;
    u8 backwards : 1;               /* the motion plays from its last frame to its first */
    u8 unk01;                       /* 0x01 */
    s16 frame;                      /* 0x02 */
    s32 frame_fixed;                /* 0x04 the frame times 256 */
} EntityMotionSlot;

#define ENTITY_BODY_USED    0x100
#define ENTITY_BODY_ROUND   0x200   /* as tall as it is wide, the point being its top */

typedef struct {
    union {
        u32 flags;                  /* 0x00 ENTITY_BODY_ bits */
        u8 point;                   /* 0x00 the model point it sits at */
    } unk00;
    u16 unk04;                      /* 0x04 its radius, before the model's x scale */
    u16 unk06;                      /* 0x06 its height, before the model's y scale */
    f32 unk08;                      /* 0x08 a hit on this body counts this many times over (func_001B1510) */
} EntityBodyPoint;

/* what Entity.params points at: per-kind tuning values. EntityController below is a second
   view of the same block; the two are to be unified. */
typedef struct {
    u32 unk00_0 : 16;               /* 0x00 */
    u32 unk02 : 8;                  /* 0x02 index into D_003E6E40 */
    u32 unk03_0 : 1;                /* 0x03 */
    u32 unk03_1 : 2;
    u32 unk03_3 : 5;
    u16 unk04;                      /* 0x04 Entity.unk44 before scaling */
    u16 unk06;                      /* 0x06 Entity.unk30 before scaling */
    u16 unk08;                      /* 0x08 Entity.unk34 before scaling */
    u16 unk0A;                      /* 0x0A what Entity.unk2C is measured against */
    u16 unk0C;                      /* 0x0C 0: Entity_InitFromMapSpawns makes it 70 */
    u16 unk0E;                      /* 0x0E passed to Player_AddExperience when the entity dies */
    u16 flags;                      /* 0x10 */
    char pad12[0x02];
    u8 unk14[4];                    /* 0x14 weights of the four actions, scaled by Entity.unkBC */
    u16 unk18;                      /* 0x18 a distance to the player (see Npc_Think) */
    u16 unk1A;                      /* 0x1A a height: ranged attacks need the player level with it (EntityAi_PickCombatChoice) */
    char pad1C[0x04];
    EntityBodyPoint unk20[4];       /* 0x20 what the player collides with for the kinds made of several bodies (func_001BECA8) */
    f32 unk50;                      /* 0x50 */
    f32 unk54;                      /* 0x54 */
    f32 unk58;                      /* 0x58 */
    f32 turn_limit;                 /* 0x5C */
    u8 unk60[0x20];                 /* 0x60 byte table */
    u16 unk80[0x22];                /* 0x80 indexed by Entity.unkD1 */
    u8 unkC4;                       /* 0xC4 */
    u8 unkC5;                       /* 0xC5 bone index for func_0010CAB8 (EntityEvent_Respawn) */
    u16 unkC6;                      /* 0xC6 */
    u8 unkC8;                       /* 0xC8 */
    u8 unkC9;                       /* 0xC9 */
    u8 unkCA;                       /* 0xCA */
    u8 unkCB;                       /* 0xCB */
    u16 unkCC;                      /* 0xCC Entity.radius before scaling */
    u16 unkCE;                      /* 0xCE */
    u8 unkD0;                       /* 0xD0 */
    u8 unkD1;                       /* 0xD1 */
    u8 unkD2;                       /* 0xD2 the model's scale in percent */
    char padD3;
    s16 unkD4;                      /* 0xD4 unkD4 to unkE4: nine resistances, 100 and up: immune (func_001B2F70) */
    s16 unkD6;                      /* 0xD6 */
    s16 unkD8;                      /* 0xD8 */
    s16 unkDA;                      /* 0xDA */
    s16 unkDC;                      /* 0xDC */
    s16 unkDE;                      /* 0xDE */
    s16 unkE0;                      /* 0xE0 */
    s16 unkE2;                      /* 0xE2 */
    s16 unkE4;                      /* 0xE4 */
    u16 unkE6;                      /* 0xE6 */
    u16 unkE8;                      /* 0xE8 */
    u8 unkEA;                       /* 0xEA unkEA to unkED: percent taken off a hit's chance to start each of the four ailments (func_001B3908) */
    u8 unkEB;                       /* 0xEB */
    u8 unkEC;                       /* 0xEC */
    u8 unkED;                       /* 0xED */
    u16 unkEE;                      /* 0xEE Entity.height before scaling */
    u8 unkF0[0x20];                 /* 0xF0 how many records unk130[n] leads to */
    u8 unk110[0x20];                /* 0x110 how many records unk1B0[n] leads to */
    s32 unk130[0x20];               /* 0x130 offsets from D_004467C0 to EntityParamsUnk130 records, by animation */
    s32 unk1B0[0x20];               /* 0x1B0 offsets from D_004467C0 to EntityParamsUnk1B0 records, by animation */
    u16 gold;                       /* 0x230 how much gold it drops (Gold_DropFromEntity): half of it for sure, the rest by chance */
    u16 drop_item;                  /* 0x232 the item it may drop (func_001C8CB0), 0xFFFF: none */
    u16 second_drop_item;           /* 0x234 the item it may drop when the first one's roll fails, 0xFFFF: none */
    s8 drop_chance;                 /* 0x236 percent */
    s8 second_drop_chance;          /* 0x237 percent */
    EntityUnaligned64 unk238;       /* 0x238 */
    EntityParamsUnk240 unk240[2];   /* 0x240 indexed by flag bits 58-59 */
    char unk350[0x88];              /* 0x350 */
    EntityParamsUnk3D8Array unk3D8[3]; /* 0x3D8 */
    u8 unk42C;                      /* 0x42C how far below its position a floor may be to count (EntityCollision_WalkAgainstMesh) */
    u8 unk42D;                      /* 0x42D what it does at a ledge (EntityCollision_WalkAgainstMap): 0 walks off, 1 steps down at most Entity.unk34, else stops */
    u16 unk42E;                     /* 0x42E */
    s16 unk430;                     /* 0x430 */
    s16 unk432;                     /* 0x432 */
    u16 unk434;                     /* 0x434 */
    s16 unk436;                     /* 0x436 */
    s16 unk438;                     /* 0x438 */
    s16 unk43A;                     /* 0x43A */
    s16 unk43C;                     /* 0x43C */
    s16 unk43E;                     /* 0x43E */
    u16 unk440[8];                  /* 0x440 indexed by flag bits 60-62; passed to Effect_StartSpawner */
} EntityParams;

/* record index of the 0x88-byte records at +0x240, reached by stepping the whole params pointer
   (the match keeps the index scaling apart from the +0x240) */
#define ENTITY_PARAMS_UNK240_RECORD(params, index) (((EntityParams *)((char *)(params) + (index) * sizeof(EntityParamsUnk240)))->unk240[0])

/* what EntityParams.unk130[n] leads to: something Effect_StartSpawner starts at a bone on an animation frame */
typedef struct {
    u16 unk00;                      /* 0x00 passed to Effect_StartSpawner */
    u16 unk02;                      /* 0x02 the animation frame */
    u8 unk04;                       /* 0x04 passed to func_0010CAB8 (and the one after it) */
    s8 unk05;                       /* 0x05 not 0: EntityParams.unk440 picks what to start */
} EntityParamsUnk130;

/* what EntityParams.unk1B0[n] leads to: something Sound_PlayFalloff starts on an animation frame */
typedef struct {
    u8 unk00;                       /* 0x00 passed to Sound_PlayFalloff */
    u8 unk01;                       /* 0x01 passed to Sound_PlayFalloff */
    u16 unk02;                      /* 0x02 the animation frame */
    u8 unk04;                       /* 0x04 not 0: left out while Entity.unk2A is set */
    char pad05[0x03];
} EntityParamsUnk1B0;

/* the first word of an EntityParams.unk3D8 record, as the AI functions take it */
#define ENTITY_PARAMS_UNK3D8(params, index) (&(params)->unk3D8[index].record)
/* the three ranged attacks */
#define ENTITY_PARAMS_RANGED(params) ((params)->unk3D8)
#define ENTITY_PARAMS_RECORD(params, index) (&(params)->unk3D8[index])

/* EntityParams with the three ranged-attack records as a real array member: indexed this way the
   compiler adds the scaled index to the params pointer, base first (func_00164C88 needs that) */
typedef struct {
    char pad00[0x3D8];
    EntityParamsUnk3D8Array ranged[3]; /* 0x3D8 */
} EntityParamsRangedView;

#define ENTITY_PARAMS_RANGED_VIEW(params) ((EntityParamsRangedView *)(params))

/* EntityParams with the float at 0xC0, which lies inside unk80 (Motion_StepSlot) */
typedef struct {
    char pad00[0xC0];
    f32 motion_scale;               /* 0xC0 how far the root part's movement in a motion carries the entity */
} EntityParamsMotionScale;

#define ENTITY_PARAMS_MOTION_SCALE(params) (((EntityParamsMotionScale *)(params))->motion_scale)

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

/* sub-struct at Entity+0xE0: the work area of an entity's type. It starts with the handlers the
   type's set-up function installs; EntityAi_Update calls them. All of them take the entity. */
typedef struct {
    /* 0x00 decides what the entity does next and starts that action. Called every frame with 0
       while flag bit 5 is set (not in a frame that calls on_hit or starts death), and with 1 by an
       action that has ended: the action it starts then runs at once. */
    void (*think)(struct Entity *entity, s32 run_now);
    /* 0x04 called every frame after the action: plays the motion on, sets flag bit 34 when it ends */
    void (*animate)(void);
    /* 0x08 called in a frame the entity was hit (flag bits 36-37 say how hard) unless flag bit 35
       is set: starts the reaction */
    void (*on_hit)(void);
    /* 0x0C while set, called every frame in place of Entity.handler (see EntityAi_SetReaction);
       Entity.unkCB is its step. It clears itself when it is done. */
    void (*reaction)(void);
    /* 0x10 the action the set-up function starts when the spawn has flag bit 58: wait for the
       player to come near, then get up */
    void (*dormant)(void);
    /* 0x14 the action EntityAi_Update starts when Entity.unk2C is 0 */
    void (*death)(void);
    /* 0x18 the action between the others: carries out the movement command in Entity.unkC8.
       think ends by starting it; an action that has ended starts it when flag bit 5 is clear. */
    void (*move)(void);
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

/* one of the two 8-byte slots EntityAi_StartSlotMotion fills */
typedef struct {
    u8 unk00;                       /* 0x00 */
    u8 unk01;                       /* 0x01 */
    s16 unk02;                      /* 0x02 */
    s32 unk04;                      /* 0x04 */
} EntityUnkE0Slot;

/* Entity+0xE0 as the kinds handled at 0x1740A8..0x180000 use it */
typedef struct {
    void (*think)(struct Entity *entity, s32 run_now); /* 0x00 */
    char pad04[0x14];
    void (*move)(void);             /* 0x18 */
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

/* Entity+0xE0 as the flying kind set up by DarkBird_Init lays it out */
typedef struct {
    void (*think)(struct Entity *entity, s32 run_now); /* 0x00 */
    char pad04[0x14];
    void (*move)(void);             /* 0x18 */
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

/* Entity+0xE0 as the kind set up by ExpeditionSoldier_Init lays it out */
typedef struct {
    void (*think)(struct Entity *entity, s32 run_now); /* 0x00 */
    char pad04[0x14];
    void (*move)(void);             /* 0x18 */
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

/* Entity+0xE0 as the kind set up by GreaterDemon_Init lays it out. The area is
   8-byte aligned: tests of neighbouring bytes are merged into one 64-bit load. */
typedef struct {
    void (*think)(struct Entity *entity, s32 run_now); /* 0x00 */
    void (*animate)(void);          /* 0x04 */
    void (*on_hit)(void);           /* 0x08 */
    char pad0C[0x04];
    void (*dormant)(void);          /* 0x10 */
    void (*death)(void);            /* 0x14 */
    void (*move)(void);             /* 0x18 */
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
    u8 index : 5;                   /* what EntityAi_StartSlotMotion looks up */
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

/* Entity+0xE0 as the kind set up by DarkSorcerer_Init lays it out */
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
    f32 unk30[4];                   /* 0x30 pitch and yaw passed to EntityEvent_StartEffectAtPoint */
    char pad40[0x2E];
    s16 unk6E;                      /* 0x6E */
    char pad70[0x10];
    f32 unk80[4];                   /* 0x80 pitch and yaw passed to EntityEvent_StartEffectAtPoint */
    f32 unk90[4];                   /* 0x90 pitch and yaw passed to EntityEvent_StartEffectAtPoint */
} EntityUnkE0Kind162BC8;

/* Entity+0xE0 as the kind set up by Onyx_Init lays it out */
typedef struct {
    char pad00[0x68];
    s16 unk68[3];                   /* 0x68 weights of the first three actions (see Onyx_PickAttack) */
    char pad6E[0x32];
} EntityUnkE0Kind17FC60;

/* Entity+0xE0 as the kind set up by func_00131390 lays it out */
typedef struct {
    void (*think)(struct Entity *entity, s32 run_now); /* 0x00 */
    char pad04[0x14];
    void (*move)(void);             /* 0x18 */
    f32 unk1C;                      /* 0x1C */
    char pad20[0x4A];
    s16 unk6A;                      /* 0x6A counts down; reloaded from EntityParams.unk434 */
    char pad6C[0x0E];
    s8 unk7A;                       /* 0x7A */
    s8 unk7B;                       /* 0x7B */
    s8 unk7C;                       /* 0x7C distance band to the player, 0 (far) to 4 (near) */
    s8 unk7D;                       /* 0x7D frames until unk7C is picked again */
    s8 unk7E;                       /* 0x7E */
    char pad7F[0x21];
} EntityUnkE0Kind131390;

/* Entity+0xE0 as the kind set up by func_0012ACA0 lays it out */
typedef struct {
    char pad00[0x1C];
    f32 unk1C;                      /* 0x1C turn speed */
    char pad20[0x20];
    f32 unk40[7];                   /* 0x40 [2] to [6]: how far each of the five neck joints (model parts 30 to 34) is bent */
    char pad5C[0x44];
} EntityUnkE0Kind12ACA0;

/* what Entity.params points at (the entity's controller / type data), see ENTITY_CONTROLLER.
   EntityParams above is a second view of the same block; the two are to be unified. */
typedef struct {
    char pad00[0x10];
    u16 flags;                      /* 0x10 */
    u16 unk12;                      /* 0x12 reload value for Entity.timer */
    char pad14[0x04];
    u16 unk18;                      /* 0x18 */
    u16 unk1A;                      /* 0x1A */
    f32 unk1C;                      /* 0x1C */
    char pad20[0x3C];
    f32 turn_limit;                 /* 0x5C */
    char pad60[0x64];
    u8 unkC4;                       /* 0xC4 */
    u8 unkC5;                       /* 0xC5 */
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
    u16 unk20;                      /* 0x20 a timer, set to 1800 by EntityCollision_ApplySurfaceEffect */
    char pad22[0x02];
    u16 unk24;                      /* 0x24 not 0: attacks reach half as far (EntityAi_PickMeleeAttack) */
    u16 unk26;                      /* 0x26 1-4: which of the timers unk20, unk24, unk28, unk2A tints the model (Entity_DrawAll) */
    u16 unk28;                      /* 0x28 */
    u16 unk2A;                      /* 0x2A */
    u16 unk2C;                      /* 0x2C */
    u16 unk2E;                      /* 0x2E capped at EntityParams.unk0A; Entity.unk4C is set from it (Entity_UpdateAll) */
    f32 unk30;                      /* 0x30 */
    f32 unk34;                      /* 0x34 */
    f32 radius;                     /* 0x38 */
    f32 height;                     /* 0x3C */
    f32 unk40;                      /* 0x40 EntityParams.unk0C scaled three ways (a volume or weight?) */
    f32 unk44;                      /* 0x44 EntityParams.unk04 scaled */
    f32 unk48;                      /* 0x48 how far above the floor it is kept (see EntityCollision_SphereAgainstMap) */
    f32 unk4C;                      /* 0x4C */
    s32 unk50;                      /* 0x50 */
    void (*handler)(struct Entity *entity);      /* 0x54 the current action; EntityAi_Update calls it every frame while flag bit 6 is set and no reaction is */
    void (*prev_handler)(struct Entity *entity); /* 0x58 the action before the last EntityAi_StartAction */
    void *target;                   /* 0x5C */
    f32 unk60;                      /* 0x60 */
    f32 distance_to_player;         /* 0x64 */
    f32 unk68;                      /* 0x68 */
    f32 unk6C;                      /* 0x6C the heading Entity.unk4C pushes it along (Entity_UpdateAll) */
    f32 unk70[4];                   /* 0x70 the model's position before this frame's move */
    u16 unk80;                      /* 0x80 */
    u16 unk82;                      /* 0x82 */
    f32 unk84;                      /* 0x84 */
    f32 unk88;                      /* 0x88 */
    f32 unk8C;                      /* 0x8C */
    f32 unk90;                      /* 0x90 */
    f32 unk94;                      /* 0x94 */
    f32 unk98;                      /* 0x98 */
    u16 timer;                      /* 0x9C frames left in state 1 (EntityAi_StepState) */
    u16 unk9E;                      /* 0x9E */
    u16 unkA0;                      /* 0xA0 a timer for the think functions and the actions; EntityAi_Update counts it down */
    u8 unkA2;                       /* 0xA2 frames before the next attack: set when an attack or a reaction ends (EntityAi_SetAttackDelay), counted down by EntityAi_Update */
    u8 unkA3;                       /* 0xA3 */
    f32 unkA4[3];                   /* 0xA4 a push applied while flag bit 45 or 46 is set */
    f32 unkB0[3];                   /* 0xB0 a push applied while flag bit 48 is set */
    u8 unkBC[4];                    /* 0xBC */
    u8 unkC0[6];                    /* 0xC0 */
    /* 0xC6 busy bits: what the current action or reaction is. 1 an attack, 2 the reaction to a light hit,
       4 to a heavy hit, 8 a dodge, 0x10 a guard, 0x20 dormant, 0x40 death, 0x1000 and up an action of
       the type's own. The think handlers decide nothing while it is not 0. */
    u16 unkC6;
    /* 0xC8 this frame's movement command, for the move action: 1 forward, 2 back, 4 and 8 sideways,
       0x10 and 0x20 up and down, 0x40 at the run, 0x100 and 0x200 turn, 0x400 turn at twice the speed.
       EntityAi_Update clears it. */
    u16 unkC8;
    s8 action_step;                 /* 0xCA step of the current action; EntityAi_StartAction puts it back to 0 */
    s8 unkCB;                       /* 0xCB step of the reaction (EntityUnkE0.reaction); EntityAi_SetReaction puts it back to 0 */
    /* 0xCC what the entity knows of the player (EntityAi_StepState): 0 nothing, 2 the player is noticed
       (flag bit 33), 1 it was and Entity.timer still runs. Scripts and some types set other values. */
    s8 state;
    s8 unkCD;                       /* 0xCD */
    char prev_state;                /* 0xCE state before the last change */
    s8 unkCF;                       /* 0xCF */
    u8 unkD0;                       /* 0xD0 the motion of the animation before (Motion_Start) */
    s8 unkD1;                       /* 0xD1 the current animation (Motion_Start) */
    char unkD2;                     /* 0xD2 copied from its pool block */
    char unkD3;                     /* 0xD3 pool block index, -1: none */
    char unkD4;                     /* 0xD4 work area index, -1: none */
    char group_id;                  /* 0xD5 */
    u8 unkD6;                       /* 0xD6 */
    u8 unkD7;                       /* 0xD7 */
    u64 flags;                      /* 0xD8 */
    EntityUnkE0 unkE0;              /* 0xE0 */
    char pad170[0x10];              /* 0x170 the record is 0x180 bytes */
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
    u32 bit6 : 1;
    u32 bit7 : 1;
    u32 pad8 : 1;
    u32 bit9 : 1;
    u32 bit10 : 1;
    u32 bits11 : 3;
    u32 bit14 : 1;
    u32 bit15 : 1;
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
    u32 bit30 : 1;
    u32 bit31 : 1;
    u32 bit32 : 1;
    u32 bit33 : 1;
    u32 bit34 : 1;
    u32 bit35 : 1;
    u32 bits36 : 2;
    u32 bit38 : 1;
    u32 bit39 : 1;
    u32 pad40 : 1;
    u32 bit41 : 1;
    u32 bit42 : 1;
    u32 bit43 : 1;
    u32 bit44 : 1;
    u32 bit45 : 1;
    u32 bit46 : 1;
    u32 bit47 : 1;
    u32 bit48 : 1;
    u32 bit49 : 1;
    u32 bit50 : 1;
    u32 bit51 : 1;
    u32 bit52 : 1;
    u32 bit53 : 1;
    u32 bit54 : 1;
    u32 bit55 : 1;
    u32 bit56 : 1;
    u64 bit57 : 1;
    u32 bits58 : 2;
    u32 bits60 : 3;
    u64 pad63 : 1;
} EntityFlagBits;

#define ENTITY_FLAG_BITS(entity) ((EntityFlagBits *)&(entity)->flags)

/* Entity.flags with bits 36-37 and 38-39 as two fields, the way func_001B2F70 sets them on a hit */
typedef struct {
    u64 pad0 : 36;
    u32 hit_strength : 2;           /* bits 36-37: 1 below the threshold D_00446214 (percent of EntityParams.unk0A), else 2 */
    u32 hit_source : 2;             /* bits 38-39: 1 the player's weapon, 2 the player's magic, 0 anything else */
    u64 pad40 : 24;
} EntityFlagHitBits;

#define ENTITY_FLAG_HIT_BITS(entity) ((EntityFlagHitBits *)&(entity)->flags)

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

/* ten weights, one per tenth of a range: a local table EntityPoint_PickNear copies whole
   from read-only data */
typedef struct {
    s32 weights[10];                /* 0x00 */
} EntityRangeWeights;

/* five animation frame numbers, copied whole from read-only data like
   EntityFrameTable (AncientKing_RangedAttack0, func_0014FCC0) */
typedef struct {
    s32 frames[5];                  /* 0x00 */
} EntityFrameTable5;

/* five entity ids: a local table that a function copies whole from read-only
   data (StoneGuardian_Mode6IsStepFree, StoneGuardian_Mode6IsNearestWaiting) */
typedef struct {
    s32 ids[5];                     /* 0x00 */
} EntityIdTable;

/* weights of three actions: a local table DarkSorcerer_PickAction copies whole from read-only
   data and then adjusts */
typedef struct {
    s32 weights[3];                 /* 0x00 */
} EntityActionWeights3;

/* a point or direction as the game stores it (16 bytes) */
typedef struct {
    f32 x;                          /* 0x00 */
    f32 y;                          /* 0x04 */
    f32 z;                          /* 0x08 */
    f32 w;                          /* 0x0C */
} KfVector;

/* one of the 0x80-byte records MapSlot.records (kf_map.h) points at */
typedef struct {
    char pad00[0x1C];
    f32 unk1C;                      /* 0x1C the heading its floor pushes the player along (Player_Move) */
    char pad20[0x0C];
    u16 unk2C;                      /* 0x2C the most speed the push gives, 0: no push */
    u16 unk2E;                      /* 0x2E how much speed the push adds per frame, 0: no push */
    char pad30[0x02];
    u16 unk32;                      /* 0x32 0xFFFF: unused */
    char pad34[0x02];
    u8 unk36;                       /* 0x36 which MapSlotUnk0C block the entities standing on it are drawn with */
    u8 unk37;                       /* 0x37 which MapSlotUnk10Words block they are drawn with */
    u64 unk38;                      /* 0x38 flags; 0x4000000: EntityEvent_StartFrameEffects does its work */
    char pad40[0x0E];
    u8 unk4E;                       /* 0x4E */
    char pad4F[0x11];
    f32 unk60[4];                   /* 0x60 lowest x, y, z it covers */
    f32 unk70[4];                   /* 0x70 highest x, y, z it covers */
} MapRecord;

/* one of the 0x80-byte records MapSlot.unk10 (kf_map.h) points at, indexed by byte 3 of
   EntitySpawnInfo.unk38 */
typedef struct {
    char pad00[0x04];
    f32 unk04;                      /* 0x04 */
    char pad08[0x0C];
    f32 unk14;                      /* 0x14 */
    char pad18[0x0C];
    f32 unk24;                      /* 0x24 */
    char pad28[0x28];
    f32 unk50;                      /* 0x50 */
    f32 unk54;                      /* 0x54 */
    f32 unk58;                      /* 0x58 */
    char pad5C[0x14];
    f32 unk70;                      /* 0x70 */
    f32 unk74;                      /* 0x74 */
    f32 unk78;                      /* 0x78 */
    char pad7C[0x04];
} MapSlotUnk10;

/* Entity.flags as retail reads it where a store through another pointer (the work area, say) sits
   between two uses: a union member can alias anything, so the compiler loads the flags again
   instead of reusing the value it has. Use ENTITY_FLAGS(entity) in place of entity->flags when
   the retail code has that second ld. */
typedef union {
    u64 all;
    u32 halves[2];                  /* Entity_ResetToSpawn clears bits 32-63 in one store */
} EntityFlags;
#define ENTITY_FLAGS(entity) (((EntityFlags *)&(entity)->flags)->all)

/* Entity.unkC8 (the movement command bits) read through a union: the value then does not survive a
   store of another type, and Scorpion_Move reads the field again after it (see ENTITY_FLAGS) */
typedef union {
    u16 all;
} EntityCommand;
#define ENTITY_COMMAND(entity) (((EntityCommand *)&(entity)->unkC8)->all)

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

/* one block of the entity pool, D_003D6DF0[5] (Entity_ResetAllLists.c's
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

/* the first 0x20 bytes of a loaded type file (D_00249680), kept in D_00474B70 while the file's
   sound part is handed to the sound loader */
typedef struct {
    char pad00[0x14];
    int unk14;                      /* 0x14 an offset into the file, made an address; 0: no sound part */
    int unk18;                      /* 0x18 an offset into the file, made an address; 0: no sound part */
    char pad1C[0x04];
} EntityTypeFileHeader;

/* the start of the data loaded into an entity pool block (EntityPoolSlot.base_address):
   offsets from the start of the block to its parts */
typedef struct {
    char pad00[0x04];
    char *model_offset;             /* 0x04 handed to func_0010B758 with the entity's model. The two are
                                       pointer typed: only then does func_00123918 load them after base_address */
    char *motion_offset;            /* 0x08 handed to func_00108940 */
    s32 unk0C;                      /* 0x0C becomes ModelInstance.unk194 */
    s32 pool_model_offset;          /* 0x10 the slot's own model (func_00124B00), 0: none */
} EntityPoolData;

/* a node of the D_003E7500 pool (0x200 of them, chained through prev/next): a point
   that eases toward a target over a duration or follows another vector. The two
   active lists and free list are owned by kf_linked_point.c. */
typedef struct LinkedPoint {
    Vector4 position;               /* 0x00 w is kept when following; w is 1 / duration while easing */
    Vector4 target;                 /* 0x10 w is 1.0 */
    Vector4 *follow;                /* 0x20 NULL: stays put */
    f32 progress_scale;             /* 0x24 1 / duration squared */
    f32 duration;                   /* 0x28 */
    u16 handle;                     /* 0x2C index in the pool */
    u8 active;                      /* 0x2E */
    u8 list_index;                  /* 0x2F which D_004470C8 list it is on */
    u8 kind_a;                      /* 0x30 */
    s8 kind_b;                      /* 0x31 */
    s16 unk32;                      /* 0x32 */
    char pad34[0x04];
    struct LinkedPoint *prev;       /* 0x38 */
    struct LinkedPoint *next;       /* 0x3C */
} LinkedPoint;

#define LINKED_POINT_COUNT 0x200

extern LinkedPoint D_003E7500[LINKED_POINT_COUNT];

/* the scratchpad as the movement handlers of entity types 65 and 71 use it: a velocity and the
   matrix that turns it to the model's heading */
#define ENTITY_SCRATCH_VECTOR ((f32 *)0x70000000)
#define ENTITY_SCRATCH_MATRIX ((f32 *)0x70000010)

/* upstream's typed views of the two blocks an Entity points at */
#define ENTITY_MODEL(entity) ((ModelInstance *)(entity)->model)
#define ENTITY_CONTROLLER(entity) ((EntityController *)(entity)->params)

/* a whole model instance (0x1C0 bytes) as func_00131390 copies it to D_003E7140[0] and [1]
   (afterimages drawn at half alpha): the game's struct is 8-byte aligned, the copy moves
   doublewords */
typedef struct {
    ModelInstance model;
} __attribute__((aligned(8))) ModelInstanceBlock;

extern Entity *D_004467E0;          /* entity list head */

float func_00124DE0(Entity *entity);    /* distance to the player */

/* what ladder_object refers to: a record with a kind id at 0x30 (Ladder_PlayStepSound) */
typedef struct {
    char pad00[0x30];
    u16 unk30;                      /* 0x30 */
    char pad32[0x1E];
    s16 unk50;                      /* 0x50 how many steps there are between the two end points (Ladder_Start) */
    s16 unk52;                      /* 0x52 frames one step takes */
} ReferencedObject;

/* ladder_object: the map object the player is climbing */
typedef struct {
    ModelInstance *model;           /* 0x00 its get_point gives the two end points (Ladder_Start) */
    char pad04[0x04];
    ReferencedObject *unk08;        /* 0x08 */
    char pad0C[0x04];
    s32 unk10;                      /* 0x10 flags; 0x10000 is tested by Ladder_WaitOnRung */
} ObjectReference;

void Entity_ResetAllLists(void);
void Entity_FreeAllInGroup(int group_id);
void Entity_DespawnOutOfRange(void);

#endif
