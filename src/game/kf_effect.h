/* Effect actors: short-lived effects that move a model and end in another effect. */
#ifndef KF_EFFECT_H
#define KF_EFFECT_H

#include "common.h"
#include "kf_model.h"

/* the effect's definition, EffectActor.def */
typedef struct {
    char pad00[0xD8];
    f32 speed;                      /* 0xD8 */
    char padDC[0x10];
    u16 radius_half;                /* 0xEC */
    u16 interval;                   /* 0xEE frames between collision checks */
    u16 unkF0;                      /* 0xF0 effect ids passed to func_001A2838 */
    u16 unkF2;                      /* 0xF2 */
    char padF4[0x10];
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
    char pad64[0x08];
    int unk6C;                      /* 0x6C */
} EffectActor;

#endif
