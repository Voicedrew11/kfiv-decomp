/* Effect actors: short-lived effects that move a model and end in another effect. */
#ifndef KF_EFFECT_H
#define KF_EFFECT_H

#include "common.h"
#include "kf_model.h"

/* the effect's definition, EffectActor.def */
typedef struct {
    char pad00[0xD8];
    f32 speed;                      /* 0xD8 */
    f32 unkDC;                      /* 0xDC */
    char padE0[0x0C];
    u16 radius_half;                /* 0xEC */
    u16 interval;                   /* 0xEE frames between collision checks */
    u16 unkF0;                      /* 0xF0 effect ids passed to func_001A2838 */
    u16 unkF2;                      /* 0xF2 */
    char padF4[0x0E];
    u16 restart_frame;              /* 0x102 frame to resume from after the last spawn */
    f32 gravity;                    /* 0x104 */
    u16 max_frames;                 /* 0x108 0: no limit */
    char pad10A[0x0E];
    u16 end_effect;                 /* 0x118 spawned when the effect ends */
} EffectDef;

/* EffectActor+0x10, which the update functions keep a pointer to. Each kind of
   effect uses the bytes after the velocity its own way; the *Motion types below
   are those views, reached by a cast (a union changes gcc's scheduling). */
typedef struct {
    Vector4 velocity;               /* 0x00 (EffectActor+0x10) */
    char unk10[0x20];               /* 0x10 per-kind state */
} EffectMotion;

/* EffectMotion as func_001A4CE8 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    u16 end_frame;                  /* 0x10 */
    char pad12[0x1E];
} ProjectileMotion;

/* EffectMotion as func_001A96B0 (falls under gravity) uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    int mode;                       /* 0x10 3 or 5: spawn effect 0x15 on landing */
    u16 end_frame;                  /* 0x14 */
    char pad16;
    char unk17[0x19];               /* 0x17 passed by address */
} FallingMotion;

/* EffectMotion as func_001A98D0 (checks for hits every interval frames) uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x04];
    int countdown;                  /* 0x14 frames to the next check */
    char unk18[0x18];               /* 0x18 passed by address */
} EmitterMotion;

/* EffectMotion as func_001AC1F8 (scales its model) uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x10];
    f32 scale_per_mille;            /* 0x20 */
    char pad24[0x0C];
} ScalerMotion;

/* EffectMotion as func_001AA330 and func_001A5D20 (turn toward a point) use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    s16 unk20;                      /* 0x20 */
    s16 unk22;                      /* 0x22 */
    u8 unk24;                       /* 0x24 */
    u8 unk25;                       /* 0x25 */
    char pad26[0x0A];
} TurningMotion;

/* EffectMotion as func_001AA230 uses it: the definition's collision radius and
   interval copied over the start of the velocity */
typedef struct {
    u16 radius_half;                /* 0x00 */
    u16 interval;                   /* 0x02 */
    char pad04[0x2C];
} PulseMotion;

/* EffectMotion as func_001A4570 (spawns an effect every interval) uses it */
typedef struct {
    u16 count;                      /* 0x00 effects spawned so far */
    u16 effect_id;                  /* 0x02 */
    u16 repeat_limit;               /* 0x04 */
    u16 interval_base;              /* 0x06 */
    u16 next_frame;                 /* 0x08 */
    u16 interval_range;             /* 0x0A */
    char pad0C[0x24];
} SpawnerMotion;

/* EffectMotion as func_001ACE80 (stays attached to another model) uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    ModelInstance **anchor;         /* 0x10 the model it is attached to */
    char unk14[0x1C];               /* 0x14 passed by address */
} AttachedMotion;

/* EffectMotion as func_001AAF58 (falls once its frame passes fall_frame) uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x08];
    s32 fall_frame;                 /* 0x18 gravity applies once the frame passes this */
    char unk1C[0x14];               /* 0x1C passed by address */
} GravityMotion;

/* EffectMotion as func_001A4128 (sits on another entity) uses it */
typedef struct {
    s32 radius_half;                /* 0x00 */
    s8 unk04;                       /* 0x04 */
    char pad05[0x1B];
} SnappedMotion;

/* EffectMotion as func_001A6748 (rests on the ground) uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 range;                      /* 0x10 */
    s32 unk14;                      /* 0x14 */
    u8 landed;                      /* 0x18 */
    char pad19[0x17];
} LandingMotion;

/* EffectMotion as func_001AD278 (follows EffectActor.target) uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 speed;                      /* 0x10 the target's velocity.x */
    s32 unk14;                      /* 0x14 */
    f32 unk18;                      /* 0x18 */
    char pad1C[0x04];
    s8 unk20;                       /* 0x20 */
    char pad21[0x0F];
} FollowerMotion;

/* EffectMotion as func_001ABBE0 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x0C];
    char unk1C;                     /* 0x1C */
    char pad1D[0x13];
} BurstMotion;

typedef struct EffectActor {
    char pad00[0x10];
    EffectMotion motion;            /* 0x10 */
    s64 flags;                      /* 0x40 */
    void *owner;                    /* 0x48 */
    u16 frame;                      /* 0x4C */
    u16 effect_param;               /* 0x4E */
    ModelInstance *model;           /* 0x50 */
    EffectDef *def;                 /* 0x54 */
    char pad58[0x04];
    int effect_arg;                 /* 0x5C */
    struct EffectActor *target;     /* 0x60 */
    void *start_position;           /* 0x64 */
    void *start_rotation;           /* 0x68 */
    int unk6C;                      /* 0x6C */
} EffectActor;

/* a request to spawn an effect at a position (func_00214660) */
typedef struct {
    s32 pending;                    /* 0x00 cleared once the effect is spawned */
    char pad04[0x04];
    char kind;                      /* 0x08 */
    char pad09[0x07];
    u8 unk10;                       /* 0x10 */
    char pad11[0x07];
    u8 unk18;                       /* 0x18 */
    char pad19[0x07];
    f32 unk20;                      /* 0x20 */
    char pad24[0x04];
    f32 pos_x;                      /* 0x28 */
    char pad2C[0x04];
    f32 pos_y;                      /* 0x30 */
    char pad34[0x04];
    f32 pos_z;                      /* 0x38 */
} SpawnRequest;

/* a task that feeds the cosine of an angle to a command (func_00214DC0) */
typedef struct {
    s32 active;                     /* 0x00 cleared after one step */
    char pad04[0x04];
    f32 angle;                      /* 0x08 radians */
    char command[4];                /* 0x0C the command func_00211D90 fills in */
} AngleTask;

#endif
