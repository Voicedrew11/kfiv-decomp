/* Effect actors: short-lived effects that move a model and end in another effect. */
#ifndef KF_EFFECT_H
#define KF_EFFECT_H

#include "common.h"
#include "kf_model.h"
#include "kf_entity.h"

/* the effect's definition, EffectActor.def: one 0x120-byte entry of the table at D_00502260.
   Three channels are keyframed over the effect's life: keys_a (func_001AEC48), keys_b
   (func_001AED58) and the colour (func_001AF0B0), each at frames 0, two own keyframes and
   keyframe_end. Most kinds read the fields from rotation_base on their own way. */
typedef struct {
    Vector4 keys_a[4];              /* 0x00 */
    Vector4 keys_b[4];              /* 0x40 */
    u8 colour_keys[4][4];           /* 0x80 */
    u16 keyframes_a[2];             /* 0x90 */
    u16 keyframes_b[2];             /* 0x94 */
    u16 colour_keyframes[2];        /* 0x98 */
    u8 modes_a[3];                  /* 0x9C easing between the keys_a stages */
    u8 modes_b[3];                  /* 0x9F */
    u8 colour_modes[3];             /* 0xA2 */
    u8 unkA5;                       /* 0xA5 how many effects the spawner makes each time it fires */
    u8 max_level;                   /* 0xA6 the spawner's level counts down from this */
    u8 unkA7;                       /* 0xA7 frames between two firings of the spawner, 0: every frame */
    f32 rotation_base[3];           /* 0xA8 */
    f32 rotation_spread[3];         /* 0xB4 random +- range around rotation_base */
    f32 offset_base[3];             /* 0xC0 */
    f32 offset_spread[3];           /* 0xCC random +- range around offset_base */
    f32 speed;                      /* 0xD8 */
    f32 unkDC;                      /* 0xDC per kind: spin, turn rate, spread */
    f32 unkE0;                      /* 0xE0 per kind: height, drift, count */
    f32 unkE4;                      /* 0xE4 */
    f32 unkE8;                      /* 0xE8 */
    u16 radius_half;                /* 0xEC */
    u16 interval;                   /* 0xEE frames between collision checks, or a range */
    u16 unkF0;                      /* 0xF0 effect ids passed to func_001A2838 */
    u16 unkF2;                      /* 0xF2 */
    u8 unkF4;                       /* 0xF4 with the placement bits, picks the model's draw routine from D_003F1710 */
    u8 unkF5;                       /* 0xF5 bit 0: drawn with the map's own light block; bit 1: not drawn in the second pass */
    u8 unkF6;                       /* 0xF6 how the texture frames run: 0 loop, 1 once, 2 once and hold the last */
    u8 unkF7;                       /* 0xF7 how many texture frames, 0: not animated */
    u16 unkF8;                      /* 0xF8 record ids between two texture frames */
    u16 unkFA;                      /* 0xFA nonzero: an id that func_001A1610 moves up by 0x1400; the first texture frame's record */
    u8 unkFC;                       /* 0xFC effect frames per texture frame, less one */
    u8 unkFD;                       /* 0xFD */
    u16 unkFE;                      /* 0xFE */
    u16 unk100;                     /* 0x100 index into the handler tables D_00402550 and D_00402DA8 */
    u16 keyframe_end;               /* 0x102 last keyframe; kinds that loop restart here */
    f32 gravity;                    /* 0x104 */
    u16 max_frames;                 /* 0x108 0: no limit; the low bits of EFFECT_DEF_FLAGS */
    char pad10A;
    u8 unk10B;                      /* 0x10B 0xFF: no hit check */
    u16 unk10C[3];                  /* 0x10C */
    u16 unk112[3];                  /* 0x112 */
    u16 end_effect;                 /* 0x118 spawned when the effect ends */
    u16 unk11A;                     /* 0x11A not 0: sets EffectActor flag bit 59; passed to func_0019C8D0 */
    f32 scale_per_frame;            /* 0x11C */
} EffectDef;

/* the 64-bit word at EffectDef+0xF0 as func_001A2168 tests it (bits 40 and 41 are unkF5's bits 0 and 1) */
#define EFFECT_DEF_WORD_F0(def) (*(s64 *)&(def)->unkF0)

/* the 64-bit flag word at EffectDef+0x108, whose low 16 bits are max_frames */
#define EFFECT_DEF_FLAGS(def) (*(s64 *)&(def)->max_frames)

/* the same word read through a union, which keeps the read behind any earlier store (func_001AF640) */
#define EFFECT_DEF_FLAG_WORD(def) (((EntityFlagWord *)&(def)->max_frames)->word)

/* ObjectSlot+0x20, whose low word func_001A2F20 reads as a pointer to the effect
   definition while the same 8 bytes are ObjectSlot.flags (reached by cast, not a union) */
typedef struct {
    char pad00[0x20];
    EffectDef *def;                 /* 0x20 */
} ObjectSlotDefView;

#define OBJECT_SLOT_DEF(slot) (((ObjectSlotDefView *)(slot))->def)

/* EffectActor+0x10, which the update functions keep a pointer to. Each kind of
   effect uses the bytes after the velocity its own way; the *Motion types below
   are those views, reached by a cast (a union changes gcc's scheduling). */
typedef struct {
    Vector4 velocity;               /* 0x00 (EffectActor+0x10) */
    Vector4 unk10;                  /* 0x10 per-kind state */
    char unk20[0x10];               /* 0x20 */
} EffectMotion;

/* EffectMotion as func_001A4CE8 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    u16 end_frame;                  /* 0x10 */
    char pad12[0x1E];
} ProjectileMotion;

/* EffectMotion as func_001A96B0 uses it: falls under gravity */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    int mode;                       /* 0x10 3 or 5: spawn effect 0x15 on landing */
    u16 end_frame;                  /* 0x14 */
    s8 spawned;                     /* 0x16 a follow-up effect has been spawned */
    char unk17[0x19];               /* 0x17 passed by address */
} FallingMotion;

/* EffectMotion as func_001A98D0 uses it: checks for hits every interval frames */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x04];
    int countdown;                  /* 0x14 frames to the next check */
    char unk18[0x18];               /* 0x18 passed by address */
} EmitterMotion;

/* EffectMotion as func_001A72F0, func_001A7650 and others use it: scales its model */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x10];
    f32 scale_per_mille;            /* 0x20 */
    char pad24[0x0C];
} ScalerMotion;

/* EffectMotion as func_001A52B8, func_001A5D20 and others use it: turn toward a point */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    s16 unk20;                      /* 0x20 */
    s16 unk22;                      /* 0x22 */
    u8 unk24;                       /* 0x24 */
    u8 unk25;                       /* 0x25 */
    u8 unk26;                       /* 0x26 */
    char pad27[0x09];
} TurningMotion;

/* EffectMotion as func_001AA230 uses it: the definition's collision radius and
   interval copied over the start of the velocity */
typedef struct {
    u16 radius_half;                /* 0x00 */
    u16 interval;                   /* 0x02 */
    char pad04[0x2C];
} PulseMotion;

/* EffectMotion as func_001ACE80 (stays attached to another model) uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    ModelInstance **anchor;         /* 0x10 the model it is attached to */
    char unk14[0x1C];               /* 0x14 passed by address */
} AttachedMotion;

/* EffectMotion as func_001A40E8 and func_001AD278 use it: follows EffectActor.target */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 speed;                      /* 0x10 the target's velocity.x */
    s32 unk14;                      /* 0x14 */
    f32 unk18;                      /* 0x18 */
    u8 unk1C;                       /* 0x1C */
    char pad1D[0x03];
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

/* EffectMotion as func_001A88A8 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x16];
    u16 cooldown;                   /* 0x26 frames before the effect can fire again */
    char pad28[0x08];
} CooldownMotion;

/* EffectMotion as func_001A3E30 and func_001AA040 use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    int unk10;                      /* 0x10 */
    f32 unk14;                      /* 0x14 */
    char pad18[0x04];
    char unk1C;                     /* 0x1C */
} TimedMotion;

/* EffectMotion as func_001A3F20 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 fall_speed;                 /* 0x10 */
    f32 scale;                      /* 0x14 */
    char pad18[0x04];
    char unk1C[0x14];               /* 0x1C passed by address */
} ScaledFallMotion;

/* EffectMotion as func_001A4128 and func_001A4248 use it */
typedef struct {
    int unk00;                      /* 0x00 */
    u8 unk04;                       /* 0x04 */
} TargetMotion;

/* EffectMotion as func_001A41F0, func_001A4380 and others use it */
typedef struct {
    int countdown;                  /* 0x00 (EffectActor+0x10) */
    s8 unk04;                       /* 0x04 (EffectActor+0x14) */
    char pad05[0x0B];
} CountdownMotion;

/* EffectMotion as func_001A44D8 and func_001A4570 use it */
typedef struct {
    u16 spawn_count;                /* 0x00 */
    u16 spawn_effect_id;            /* 0x02 copied from def->radius_half */
    u16 spawn_limit;                /* 0x04 copied from def->interval */
    u16 spawn_frames_base;          /* 0x06 copied from def->unkF0 */
    u16 next_spawn_frame;           /* 0x08 */
    u16 spawn_frames_span;          /* 0x0A copied from def->unkF2 */
} SpawnerMotion;

/* EffectMotion as func_001A4630, func_001A46F0, func_001A73B0 and others use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 unkDC;                      /* 0x10 copied from def->unkDC; func_001A46F0 grows it by def->unkE0 */
    int unk14;                      /* 0x14 func_001A46F0: frames until it may be hit again */
    int unk18;                      /* 0x18 func_001A46F0: 1 once hit (it wobbles) */
    f32 unk1C;                      /* 0x1C func_001A46F0: wobble offset x */
    f32 unk20;                      /* 0x20 func_001A46F0: wobble offset z */
    s8 unk24;                       /* 0x24 func_001A46F0: set once the end effect is spawned */
    char pad25[0x0B];
} HomingMotion;

/* EffectMotion as func_001A4A20 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 unkDC;                      /* 0x10 copied from def->unkDC */
} PlayerHomingMotion;

/* EffectMotion as func_001A4C50 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    u16 unk10;                      /* 0x10 */
} LaunchMotion;

/* EffectMotion as func_001A4DB8 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    int unk10;                      /* 0x10 */
    u8 unk14;                       /* 0x14 */
} DropMotion;

/* EffectMotion as func_001A4E38 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x04];
    s8 landed;                      /* 0x14 set once it has spawned its landing effects */
} LandedFlagMotion;

/* EffectMotion as func_001A4F78, func_001A50E8 and others use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    float unk10;                    /* 0x10 */
    u8 unk14;                       /* 0x14 */
    s8 state;                       /* 0x15 1: landed */
    char pad16[0x0A];
} LandingMotion;

/* EffectMotion as func_001A5598 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    float fraction;                 /* 0x10 */
    u8 near_target;                 /* 0x14 */
    char pad15[0x0B];
} NearTargetMotion;

/* EffectMotion as func_001A5880 sets it up and func_001A5988 uses it: runs along the ground */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 probe;                  /* 0x10 passed to func_001C1430; y is def->unkDC */
    f32 height;                     /* 0x20 def->unkDC: the ground is looked for from half of it down */
    f32 fall_speed;                 /* 0x24 */
    f32 hit_offset;                 /* 0x28 def->unkE0: how far below itself it checks for hits */
    s8 ended;                       /* 0x2C set when it starts with no ground under it; also passed by address */
    char pad2D[0x03];
} GroundRunMotion;

/* EffectMotion as func_001A6028 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 radius;                     /* 0x10 def->unkDC */
    f32 unk14;                      /* 0x14 */
    f32 height;                     /* 0x18 def->unkE0 */
    f32 unk1C;                      /* 0x1C def->unkE4 */
    f32 unk20;                      /* 0x20 */
    u8 on_ground;                   /* 0x24 */
} ProbeMotion;

/* EffectMotion as func_001A6118 and func_001A7F88 use it: a swaying effect driven by an angle */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 unk10;                      /* 0x10 */
    f32 vertical_speed;             /* 0x14 */
    f32 unk18;                      /* 0x18 */
    f32 unk1C;                      /* 0x1C */
    f32 angle;                      /* 0x20 */
    s8 done;                        /* 0x24 */
} SwayMotion;

/* EffectMotion as func_001A63C0 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 radius;                     /* 0x10 def->unkDC */
    f32 unk14;                      /* 0x14 */
    u8 on_ground;                   /* 0x18 */
} ProbeFlatMotion;

/* EffectMotion as func_001A6748 uses it */
typedef struct {
    Vector4 velocity;
    f32 speed;                      /* 0x10 */
    s32 unk14;                      /* 0x14 */
    u8 flag18;                      /* 0x18 */
} SpeedMotion;

/* EffectMotion as func_001A6828 and func_001ACA28 use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    u8 unk10;                       /* 0x10 */
    char pad11[0x0F];
} SpawnMotion;

/* EffectMotion as func_001A6A20 and func_001A6A90 use it: a speed and a flag */
typedef struct {
    Vector4 velocity;
    f32 speed;                      /* 0x10 */
    u8 flag14;                      /* 0x14 */
} SpeedFlagMotion;

/* EffectMotion as func_001A6BF8 uses it: an aim point toward the target */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 target_x;                   /* 0x10 */
    f32 unk14;                      /* 0x14 */
    f32 target_z;                   /* 0x18 */
    char pad1C[0x04];
    f32 turn;                       /* 0x20 how sharply it turns toward the aim point (func_001A6C98) */
    s8 aimed;                       /* 0x24 the aim point is set */
    u8 flag25;                      /* 0x25 */
    s8 ended;                       /* 0x26 the end effect is spawned */
} AimMotion;

/* EffectMotion as func_001A7548 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 unk10;                  /* 0x10 passed by address */
    int unk20;                      /* 0x20 */
    u16 unk24;                      /* 0x24 */
    char pad26[0x0A];
} SelectMotion;

/* EffectMotion as func_001A7A48, func_001A7AC8 and func_001A80E8 use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 bob_velocity;               /* 0x10 */
    f32 bob_limit;                  /* 0x14 */
    f32 bob_height;                 /* 0x18 */
    f32 distance;                   /* 0x1C */
    u16 countdown;                  /* 0x20 */
    u8 ended;                       /* 0x22 */
    u8 end_countdown;               /* 0x23 */
    u8 bob_rising;                  /* 0x24 */
    char pad25[0x0B];
} BobMotion;

/* EffectMotion as func_001A7E98 and func_001A90A0 use it: a random spin on top of the launch velocity */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 unk10;                      /* 0x10 random, centred on zero */
    f32 spin_rate;                  /* 0x14 */
    f32 unk18;                      /* 0x18 set to 1.0 */
    char pad1C;
    u8 unk1D;                       /* 0x1D set to 0 */
} SpinMotion;

/* EffectMotion as func_001A8218 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    int timer;                      /* 0x20 */
    char unk24[0x0C];               /* 0x24 passed by address */
} TimerMotion;

/* EffectMotion as func_001A8768 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    int unk20;                      /* 0x20 */
    s16 angle;                      /* 0x24 */
    s16 unk26;                      /* 0x26 */
    u8 unk28;                       /* 0x28 */
    char pad29[0x0F];
} AngleAimMotion;

/* EffectMotion as func_001A8928 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 unk10;                  /* 0x10 */
    int timer;                      /* 0x20 */
    u16 end_frame;                  /* 0x24 */
    u16 cooldown;                   /* 0x26 */
    char unk28[0x18];               /* 0x28 passed by address */
} CooldownTimerMotion;

/* EffectMotion as func_001A8BA8 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 unk10;                      /* 0x10 */
    u8 unk14;                       /* 0x14 */
} ChaserMotion;

/* EffectMotion as func_001A8E48 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    u16 count;                      /* 0x20 */
    u8 flag;                        /* 0x22 */
} CountedMotion;

/* EffectMotion as func_001A8ED0 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 unk10;                  /* 0x10 */
    u16 end_frame;                  /* 0x20 */
    s8 countdown;                   /* 0x22 frames left before the next step */
    char pad23[0x0D];
} EndCountdownMotion;

/* EffectMotion as func_001A8FF0 uses it: a copied vector and a scaled value */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 unk10;                  /* 0x10 copied from the target */
    char pad20[0x04];
    s16 unk24;                      /* 0x24 */
} ScaledMotion;

/* EffectMotion as func_001A9790 uses it: a heading in velocity.y, the frame counter and phase */
typedef struct {
    Vector4 velocity;               /* 0x00 velocity.y is the heading */
    int frame_stamp;                /* 0x10 the global spawn_slot counter when it started */
    int phase;                      /* 0x14 frame_stamp % interval */
    u8 unk18;                       /* 0x18 */
} PhaseMotion;

/* EffectMotion as func_001A9A28 uses it: an int step count */
typedef struct {
    int progress;                   /* 0x00 */
} ProgressMotion;

/* EffectMotion as func_001A9F98 uses it: the start frame and the flag set after it */
typedef struct {
    char pad00[0x25];
    u8 unk25;                       /* 0x25 set to 1 once started */
    u8 unk26;                       /* 0x26 the start frame */
} StartFrameMotion;

/* EffectMotion as func_001AA590 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 */
    u16 spawn_effect;               /* 0x20 */
    u16 end_frame;                  /* 0x22 */
    s8 spent;                       /* 0x24 */
} OneShotSpawnMotion;

/* EffectMotion as func_001AABA0 and func_001AAD40 use it: velocity, spin and acceleration */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 spin;                   /* 0x10 */
    Vector4 accel;                  /* 0x20 */
} SprayMotion;

/* EffectMotion as func_001AAEA0 uses it: a launch velocity, then the radius and flags */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    int unk10;                      /* 0x10 */
    f32 unk14;                      /* 0x14 */
    int radius_half;                /* 0x18 the definition's radius_half */
    u8 unk1C;                       /* 0x1C */
    char pad1D[0x03];
} RadiusMotion;

/* EffectMotion as func_001AAF58 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x08];
    int unk18;                      /* 0x18 compared with the frame counter */
    char unk1C[0x0C];               /* 0x1C passed by address */
} DelayedFallMotion;

/* EffectMotion as func_001AB018, func_001AB470 and func_001ADF40 use it: a heading velocity and a flag byte */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    u8 unk10;                       /* 0x10 cleared on spawn */
    char pad11[0x0F];
} HeadingMotion;

/* EffectMotion as func_001AB080 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char ending;                    /* 0x10 set once the end effect is spawned */
    char pad11[0x0F];
} EndingMotion;

/* EffectMotion as func_001AB188 uses it: a velocity and a countdown to the next target check */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    u8 countdown;                   /* 0x10 */
    u8 unk11;                       /* 0x11 */
    char pad12[0x0E];
} FollowCountMotion;

/* EffectMotion as func_001AB2C0 uses it: a bobbing velocity and the ending flag */
typedef struct {
    Vector4 velocity;               /* 0x00 (w is cleared each frame) */
    char pad10;
    char ending;                    /* 0x11 1 once the end has begun */
    char pad12[0x0E];
} BobbingMotion;

/* EffectMotion as func_001AB940 uses it: the spawn count, compared with the frame */
typedef struct {
    int spawn_count;                /* 0x00 (EffectActor+0x10) */
    char pad04[0x1C];
} SpawnCountMotion;

/* EffectMotion as func_001AB9C0 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    int start_frame;                /* 0x10 */
    f32 unk14;                      /* 0x14 */
    u8 unk18;                       /* 0x18 */
    char pad19[0x07];
} SpinBurstMotion;

/* EffectMotion as func_001ABCF8 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 scale;                      /* 0x10 */
    u8 unk14;                       /* 0x14 */
    char pad15[0x1B];
} LevelMotion;

/* EffectMotion as func_001ABE20 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 scale;                      /* 0x10 */
    char unk14[0x1C];               /* 0x14 passed by address */
} GrowingMotion;

/* EffectMotion as func_001ABEF8 and func_001ABFD0 use it: a float at 0x20 holding the distance to the target */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 unk10;                  /* 0x10 */
    f32 distance_to_target;         /* 0x20 */
    char pad24[0x0C];
} TargetDistanceMotion;

/* EffectMotion as func_001AC198 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x10];
    f32 unk20;                      /* 0x20 */
} LinkedMotion;

/* EffectMotion as func_001AC270 and func_001AE570 use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    int unk10;                      /* 0x10 */
} ResetMotion;

/* EffectMotion as func_001AC2C0 uses it */
typedef struct {
    char pad00[0xE0];
    f32 drift_x;
    f32 drift_y;
} DriftModel;

/* EffectMotion as func_001AC2C0 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 heading;                    /* 0x10 */
    char pad14[0x1C];
} HeadingAngleMotion;

/* EffectMotion as func_001AC8E8 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x10];
    u16 end_frame;                  /* 0x20 */
    u16 player_frame;               /* 0x22 */
    char unk24[0x0C];               /* 0x24 passed by address */
} PlayerChaseMotion;

/* EffectMotion as func_001ACCA0 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 scale_per_mille;            /* 0x10 */
    char pad14[0x0C];
} ScaleRateMotion;

/* EffectMotion as func_001ACE08 uses it: the spawn argument is kept after the velocity */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Entity *owner_entity;           /* 0x10 a copy of EffectActor.owner_entity */
    u8 unk14;                       /* 0x14 cleared on spawn */
} SavedArgMotion;

/* EffectMotion as func_001ACF70 uses it */
typedef struct {
    char pad00[4];
    u8 state;                       /* 0x04 */
} StateFlagMotion;

/* EffectMotion as func_001AD130 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 speed;                      /* 0x10 the target's velocity.x */
    s32 unk14;                      /* 0x14 */
    f32 unk18;                      /* 0x18 */
    u8 unk1C;                       /* 0x1C */
    char pad1D[0x03];
} TargetFollowerMotion;

/* EffectMotion as func_001AD1B8 uses it: falls after radius_half frames */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x0C];
    char unk1C[0x14];               /* 0x1C passed by address */
} FallingLateMotion;

/* EffectMotion as func_001AD4B8 uses it: turns toward a point */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    int end_frame;                  /* 0x20 */
    u8 done;                        /* 0x24 */
    char pad25[0x0B];
} TurnMotion;

/* EffectMotion as func_001AD558 uses it: ends in another effect at end_frame */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 unk10;                  /* 0x10 */
    int end_frame;                  /* 0x20 */
    s8 done;                        /* 0x24 */
    char pad25[0x0B];
} EndFrameMotion;

/* EffectMotion as func_001AD7F8 uses it: a spiral */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 start_angle;                /* 0x10 model heading plus EffectDef.unkE0 */
    int unk14;                      /* 0x14 */
    f32 unk18;                      /* 0x18 EffectDef.radius_half over half of keyframe_end */
    f32 unk1C;                      /* 0x1C EffectDef.interval over keyframe_end */
    u8 start_step;                  /* 0x20 rand() % 10 */
    u8 unk21;                       /* 0x21 */
    char pad22[0x0E];
} SpiralMotion;

/* EffectMotion as func_001AB670 and func_001AD8E8 use it: spins and sways */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 angle;                      /* 0x10 */
    f32 phase;                      /* 0x14 */
    f32 phase_step;                 /* 0x18 */
    f32 spin_per_frame;             /* 0x1C */
    u8 counter;                     /* 0x20 */
    s8 toggle;                      /* 0x21 */
    char pad22[0x0E];
} SwingMotion;

/* EffectMotion as func_001ADDD0 uses it: falls under gravity, spawns an effect on landing */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 vertical_speed;             /* 0x10 added to the model's height each frame */
    u16 landing_frame;              /* 0x14 */
    s8 landing_mode;                /* 0x16 5 spawns the effect id in radius_half, else in interval */
    char pad17[0x09];
} GravityMotion;

/* EffectMotion as func_001ADFC0 uses it: spawns an effect every reload frames */
typedef struct {
    int countdown;                  /* 0x00 frames until the next spawn */
    u16 reload_frames;              /* 0x04 */
    char pad06[0x1A];
} RepeaterMotion;

/* EffectMotion as func_001AB5C8 and func_001AE060 use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    f32 unk20;                      /* 0x20 random turn */
    f32 unk24;                      /* 0x24 */
} SpreadMotion;

/* EffectMotion as func_001AE118 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    f32 unk20;                      /* 0x20 spin angle, wrapped to +-pi */
    f32 unk24;                      /* 0x24 spin speed */
} SpinAngleMotion;

/* EffectMotion as func_001AE5C0 uses it */
typedef struct {
    char pad00[0xE4];
    f32 unkE4;                      /* 0xE4 */
} TiltModel;

/* Local view: the motion's spin angle after the velocity. */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 spin_angle;                 /* 0x10 */
} TiltMotion;

/* EffectMotion as func_001AE730 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    f32 unk20;                      /* 0x20 */
    f32 unk24;                      /* 0x24 */
} StartMotion;

/* EffectMotion as func_001AE7B0 uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    f32 yaw;                        /* 0x20 */
    f32 pitch;                      /* 0x24 */
} YawPitchMotion;

/* EffectMotion as func_001AEAB0 uses it: loops until a game flag is set */
typedef struct {
    int stopped;                    /* 0x00 set once D_0034AFD6 is 3 */
    char pad04[0x1C];
} LoopMotion;

/* EffectMotion as func_001AA590 uses it: spawns effect_id at end_frame or on a hit */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x10];
    u16 effect_id;                  /* 0x20 */
    u16 end_frame;                  /* 0x22 */
    s8 ended;                       /* 0x24 */
    char pad25[0x0B];
} DelayedMotion;

/* EffectMotion as func_001AE060 and func_001AE730 set it up: an end position and two floats */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 end_position;           /* 0x10 */
    f32 unk20;                      /* 0x20 */
    f32 unk24;                      /* 0x24 */
    char pad28[0x08];
} DriftMotion;

/* EffectMotion as func_001AA940 uses it: the distances the effect fades in over */
typedef struct {
    f32 range[2];                   /* 0x00 fade-in distance, full-size distance */
    char pad08[0x28];
} FadingMotion;

/* EffectMotion as func_001ACD18 uses it: a flag byte at the very start */
typedef struct {
    u8 grounded;                    /* 0x00 set when the model is dropped onto the entity */
    char pad01[0x2F];
} GroundedMotion;

/* EffectMotion as func_001ADF40 uses it: the heading in degrees, and as a word */
typedef struct {
    s32 heading_word;               /* 0x00 the heading, zero-extended */
    u16 heading;                    /* 0x04 degrees */
    char pad06[0x2A];
} HeadingWordMotion;

/* EffectMotion as func_001ADCA8 uses it: the frame the effect lands at */
typedef struct {
    char pad00[0x14];
    u16 landing_frame;              /* 0x14 */
    u8 unk16;                       /* 0x16 */
    char pad17[0x19];
} LandingFrameMotion;

/* EffectMotion as func_001A52B8 uses it: a flag byte right after the velocity */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    s8 unk10;                       /* 0x10 cleared when the model is lifted */
    char pad11[0x1F];
} LiftedMotion;

/* EffectMotion as func_001A7148 reads it from the effect that picked the targets: an entity and a distance per slot */
typedef struct {
    struct Entity *entity[4];       /* 0x00 */
    f32 distance[4];                /* 0x10 */
    s8 list_index[4];               /* 0x20 where the entity is in the list D_004467E0, -1: none (func_001A6EE8) */
    char pad24[0x0C];
} PickedMotion;

/* EffectMotion as func_001A72F0 uses it: scales the model, spawns the end effect */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 end_position;           /* 0x10 where the end effect appears */
    f32 scale_per_mille;            /* 0x20 */
    s8 ended;                       /* 0x24 */
    char pad25[0x0B];
} ScalingEndMotion;

/* EffectMotion as func_001ACFC0 uses it: a scale and a countdown byte */
typedef struct {
    f32 scale;                      /* 0x00 */
    s8 countdown;                   /* 0x04 frames the model is held still */
    char pad05[0x2B];
} ShockMotion;

/* EffectMotion as func_001A4128 (sits on another entity) uses it */
typedef struct {
    s32 radius_half;                /* 0x00 */
    s8 unk04;                       /* 0x04 */
    char pad05[0x1B];
} SnappedMotion;

/* EffectMotion as func_001AD558 uses it: spawns the end effect at end_frame */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 end_position;           /* 0x10 where the end effect appears */
    s32 end_frame;                  /* 0x20 */
    s8 ended;                       /* 0x24 also the start of the block passed by address */
    char pad25[0x0B];
} EndAtFrameMotion;

/* EffectMotion as func_001A73B0 (homes on a target) sets it up */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 aim;                    /* 0x10 */
    s32 timer;                      /* 0x20 */
    u16 flags;                      /* 0x24 */
    char pad26[0x0A];
} AimingMotion;

/* EffectMotion as func_001A7880 sets it up: the velocity, two random offsets and a scale */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 offset_x;                   /* 0x10 */
    f32 offset_y;                   /* 0x14 */
    char pad18[0x04];
    f32 scale;                      /* 0x1C */
    s16 unk20;                      /* 0x20 */
    char pad22;
    u8 unk23;                       /* 0x23 */
    u8 unk24;                       /* 0x24 */
    char pad25[0x0B];
} OffsetSprayMotion;

/* EffectMotion as func_001A7F88 uses it: grows along its velocity and bobs up and down */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 bob_speed;                  /* 0x10 added to the model's height each frame */
    f32 bob_limit;                  /* 0x14 the bob turns round at +- this speed */
    f32 velocity_scale;             /* 0x18 grows by EffectDef.scale_per_frame */
    s8 ended;                       /* 0x1C set by the hit check: the effect ends next frame */
    s8 bob_rising;                  /* 0x1D */
    char pad1E[0x12];
} BobbingGrowMotion;

/* EffectMotion as func_001AA040 reads it (set up by func_001A9E88 and func_001A9F98): a
   turning effect that hands over to a sub-effect and restarts from start_frame */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 aim;                    /* 0x10 where the sub-effect appears at end_frame */
    u16 end_effect;                 /* 0x20 */
    u16 end_frame;                  /* 0x22 */
    u8 countdown;                   /* 0x24 */
    s8 started;                     /* 0x25 set once a sub-effect was started */
    u8 start_frame;                 /* 0x26 the frame the effect restarts from */
    char pad27[0x09];
} RestartMotion;

/* EffectMotion as func_001AB5C8 and func_001AB670 use it: tilts to and fro while it falls */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 tilt_speed;                 /* 0x10 added to the model's rotation.x each frame */
    f32 tilt_accel;                 /* 0x14 */
    f32 fall_speed;                 /* 0x18 */
    f32 fall_accel;                 /* 0x1C */
    s8 tilting_back;                /* 0x20 */
    char pad21[0x0F];
} TiltFallMotion;

/* EffectMotion as func_001A90A0 and func_001A92A8 use it (func_001A8FF0 fills aim and
   arrival_frame): flies to a point, then sweeps on from there */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 aim;                    /* 0x10 */
    f32 sweep_turn;                 /* 0x20 */
    u16 arrival_frame;              /* 0x24 */
    s8 arrived;                     /* 0x26 */
    s8 has_aim;                     /* 0x27 the aim is a target or a point on the ground */
    s8 hit_spawned;                 /* 0x28 */
    s8 on_ground;                   /* 0x29 the aim has ground under it: the effect sweeps */
    s8 first_level;                 /* 0x2A spawned by a slot at level 1 */
    s8 level_step;                  /* 0x2B EffectDef.max_level less the slot's level */
    char pad2C[0x04];
} SweepMotion;

/* EffectMotion as func_001ACA28 uses it: one link of a chain of effects, each spawning the next */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 length;                     /* 0x10 how far on the next link starts */
    int links_left;                 /* 0x14 one less than the link before; 0 ends the chain */
    char pad18[0x18];
} ChainMotion;

/* EffectMotion as func_001A7B28 reads it: the BobMotion of func_001A7A48 with its bytes signed */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 bob_speed;                  /* 0x10 added to the model's height each frame */
    f32 bob_limit;                  /* 0x14 the bob turns round at +- this speed */
    f32 turn;                       /* 0x18 how sharply it turns toward its target; grows to EffectDef.unkE0 */
    f32 velocity_scale;             /* 0x1C grows by EffectDef.scale_per_frame */
    u16 countdown;                  /* 0x20 */
    s8 ended;                       /* 0x22 */
    s8 end_countdown;               /* 0x23 frames until it ends after another of its group did */
    s8 bob_rising;                  /* 0x24 */
    char pad25[0x0B];
} HomingBobMotion;

/* one 0xA0-byte effect of the pool D_003F3550 (0x180 of them) */
typedef struct EffectActor {
    char pad00[0x0C];
    f32 scale;                      /* 0x0C */
    EffectMotion motion;            /* 0x10 */
    s64 flags;                      /* 0x40 EFFECT_FLAG_* */
    void *owner;                    /* 0x48 */
    u16 frame;                      /* 0x4C */
    u16 effect_param;               /* 0x4E */
    ModelInstance *model;           /* 0x50 */
    EffectDef *def;                 /* 0x54 */
    char *unk58;                    /* 0x58 one of MapSlot.records; cleared when its map is unloaded (func_001A3040) */
    Entity *owner_entity;           /* 0x5C the entity that cast it; freed with it (func_001A32A8) */
    struct EffectActor *target;     /* 0x60 */
    Vector4 *start_position;        /* 0x64 */
    Vector4 *start_rotation;        /* 0x68 */
    int unk6C;                      /* 0x6C an Entity (EffectTarget) the effect homes on */
    Vector4 unk70;                  /* 0x70 */
    char pad80[0x10];
    int unk90;                      /* 0x90 */
    char pad94[0x0C];
} EffectActor;

/* EffectActor+0x00 as func_001AF0B0 writes it: the colour, whose alpha is EffectActor.scale */
typedef struct {
    f32 red;                        /* 0x00 */
    f32 green;                      /* 0x04 */
    f32 blue;                       /* 0x08 */
    f32 alpha;                      /* 0x0C */
} EffectActorColour;

#define EFFECT_COLOUR(effect) ((EffectActorColour *)(effect))

/* EffectActor.flags bits */
#define EFFECT_FLAG_LIVE (0x8000L << 41)           /* bit 56: the actor is in use */
#define EFFECT_FLAG_HAS_SLOTS (0x8000L << 42)      /* bit 57: free its spawner slots with it */
#define EFFECT_FLAG_HAS_TARGETERS (0x8000L << 43)  /* bit 58: free the effects aimed at it with it */

/* D_00502260[effect id]: an effect's definition-table entry (0x120 bytes) */
typedef struct {
    char pad00[0x108];
    u64 pad108 : 16;
    u8 track : 1;                   /* 0x108 bit 16: the actor is tracked by its object slot */
    u64 pad109 : 4;
    u8 placement : 2;               /* 0x108 bits 21-22: 0 and 3 take the spawn position, 1 and 2 place themselves */
    u64 pad10B : 41;
    char pad110[0x10];
} EffectTableEntry;

/* EffectActor.flags as bit-fields, for the stores that need them */
typedef struct {
    u64 pad00 : 56;
    u64 live : 1;                   /* bit 56: EFFECT_FLAG_LIVE */
    u64 has_slots : 1;              /* bit 57: EFFECT_FLAG_HAS_SLOTS */
    u64 has_targeters : 1;          /* bit 58: EFFECT_FLAG_HAS_TARGETERS */
    u8 bit59 : 1;
} EffectActorFlagBits;

#define EFFECT_FLAG_BITS(effect) ((EffectActorFlagBits *)&(effect)->flags)

/* EffectActor.flags split into the parts some functions read */
typedef struct {
    int unk40;                      /* 0x40 a countdown */
    s16 unk44;                      /* 0x44 */
    s8 unk46;                       /* 0x46 a signed counter */
    u8 unk47;                       /* 0x47 */
} EffectActorFlagParts;

#define EFFECT_FLAG_PARTS(effect) ((EffectActorFlagParts *)&(effect)->flags)

/* what EffectActor.unk6C points at: an Entity (kf_entity.h) as far as effects read it */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    char pad04[0x38];
    f32 height;                     /* 0x3C */
    char pad40[0x98];
    s64 flags;                      /* 0xD8 */
} EffectTarget;

/* one 0x50-byte slot of the effect spawner pool D_003F1750 (0x60 of them) */
typedef struct {
    Vector4 position;               /* 0x00 */
    Vector4 unk10;                  /* 0x10 */
    s64 flags;                      /* 0x20 low word: the EffectDef; EFFECT_SLOT_LIVE */
    Entity *owner_entity;           /* 0x28 passed on to the effect */
    struct EffectActor *owner_effect; /* 0x2C the effect that spawned the slot */
    void *unk30;                    /* 0x30 */
    void *unk34;                    /* 0x34 */
    void *owner;                    /* 0x38 */
    u16 effect_param;               /* 0x3C */
    char pad3E[0x12];
} EffectSlot;

/* EffectSlot.flags bit 32: the slot is in use */
#define EFFECT_SLOT_LIVE (0x8000L << 17)

/* EffectSlot.flags as the bit-fields the game writes */
typedef struct {
    u64 def : 32;
    u64 live : 1;
    u64 pad33 : 7;
    u64 level : 8;
    u64 unk26 : 8;
    u64 unk27 : 8;
} EffectSlotFlagBits;

#define EFFECT_SLOT_FLAG_BITS(slot) ((EffectSlotFlagBits *)&(slot)->flags)

/* EffectSlot.flags as the bytes some functions read */
typedef struct {
    EffectDef *def;                 /* 0x20 */
    u8 unk24;                       /* 0x24 */
    u8 level;                       /* 0x25 the spawner's level, from EffectDef.max_level */
    u8 unk26;                       /* 0x26 */
    u8 unk27;                       /* 0x27 */
} EffectSlotFlagParts;

#define EFFECT_SLOT_FLAG_PARTS(slot) ((EffectSlotFlagParts *)&(slot)->flags)

/* the whole EffectSlot as func_001A24D8 reads it: the bytes of the flag word as members of
   the slot itself (no address is taken, so gcc keeps no pointer to the word), and unk34 as
   the vector pointer it is */
typedef struct {
    char pad00[0x20];
    EffectDef *def;                 /* 0x20 */
    u8 unk24;                       /* 0x24 */
    u8 level;                       /* 0x25 the firings left */
    u8 unk26;                       /* 0x26 frames to the next firing */
    u8 unk27;                       /* 0x27 frames before the first firing, from EffectDef.unkFD */
    char pad28[0x0C];
    Vector4 *start_rotation;        /* 0x34 EffectSlot.unk34 */
    char pad38[0x18];
} EffectSlotView;

#define EFFECT_SLOT_VIEW(slot) ((EffectSlotView *)(slot))

/* FxTableEntry+0xAC: what casting the magic does to the player (func_001BC9E0) */
typedef struct {
    u16 unk00[6];                   /* 0x00 nonzero: PlayerState.timed_flags[i] is cleared */
    u16 unk0C[6];                   /* 0x0C goes to PlayerState.unk110 */
    s16 unk18[9];                   /* 0x18 goes to PlayerState.unk11C and unk122 */
    u16 unk2A[6];                   /* 0x2A goes to PlayerState.unk12E */
    s16 unk36[9];                   /* 0x36 goes to PlayerState.unk13A and unk140 */
    u16 duration;                   /* 0x48 frames the values above last */
    s16 unk4A;                      /* 0x4A healing in all; 999 and more: all of it */
    s16 unk4C;                      /* 0x4C healing given at once */
    s16 unk4E;                      /* 0x4E frames per point of the rest */
} FxTableStats;

/* FxTableEntry+0x34: one of the three hit checks of an effect, EffectDef.unk10B picks it (func_001B4F90) */
typedef struct {
    EntityParamsUnk240PartUnk00 strengths; /* 0x00 goes to Unk1B1470Request.unk50 */
    EntityParamsUnk240PartUnk12 unk12; /* 0x12 goes to Unk1B1470Request.unk62 */
    s16 unk1E;                      /* 0x1E goes to Unk1B1470Request.unk30 */
    s16 unk20;                      /* 0x20 goes to Unk1B1470Request.unk34 (shape 1) */
    u8 shape;                       /* 0x22 0 to 2: Unk1B1470Request.unk4C_0 */
    u8 unk23;                       /* 0x23 bits copied to Unk1B1470Request.unk4D (all but 0x40) */
    u8 unk24;                       /* 0x24 goes to EffectActorFlagParts.unk40 once the hit has landed */
    u8 unk25;                       /* 0x25 goes to Unk1B1470Request.unk4F */
    u8 unk26;                       /* 0x26 not 0: Unk1B1470Request.unk4C_3 */
    u8 unk27;                       /* 0x27 goes to Unk1B1470Request.unk70 */
} FxTableHitCheck;

/* D_00402DA8[]: a table entry of 0x110 bytes, indexed by effect id (func_001B4F30) */
typedef struct {
    char pad00[0x34];
    FxTableHitCheck hit_checks[3];  /* 0x34 */
    FxTableStats unkAC;             /* 0xAC */
    u16 unkFC;                      /* 0xFC taken from PlayerState.unkB8 to cast it (func_001BC9E0) */
    u16 unkFE;                      /* 0xFE divides how fast PlayerState.unkC0 fills (func_001B96C8) */
    u16 unk100;                     /* 0x100 */
    s8 unk102;                      /* 0x102 nonzero: the effect cannot be started */
    char pad103[0x01];
    u16 unk104;                     /* 0x104 goes to PlayerState.unkC8 when it is cast */
    char pad106[0x02];
    f32 unk108;                     /* 0x108 */
    char pad10C[0x04];
} FxTableEntry;

/* helpers every effect kind calls */
int func_001A1E28(int effect_id, Vector4 *position, Vector4 *rotation, Entity *owner_entity, u16 effect_param, void *owner);
int func_001A2838(int effect_id, EffectActor *effect);
int func_001A2928(int effect_id, EffectActor *effect, Vector4 *position);
void func_001A2A18(void *effect);
void func_001A2AF8(EffectSlot *slot);
void func_001A2B28(EffectActor *target);
void func_001A2BC8(EffectActor *owner);
void func_001A3E30(EffectActor *effect, EffectSlot *spawn_slot);
void func_001A4480(EffectActor *effect);
int func_001AEC48(EffectActor *effect);
int func_001AED58(EffectActor *effect);
int func_001AF0B0(EffectActor *effect);
int func_001AF4E0(EffectActor *effect);
void func_001AF538(float length, Vector4 *out, Vector4 *angles);
void func_001AF640(EffectActor *effect, EffectSlot *spawn_slot);
void func_001AF928(EffectActor *effect, EffectSlot *spawn_slot);
void func_001AFC88(Vector4 *velocity, Vector4 *direction, float speed, float turn);


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

/* a spawn request with a float at 0x18 and the position at 0x20 (func_002145F0, func_00214580) */
typedef struct {
    s32 pending;                    /* 0x00 cleared once the effect is spawned */
    char pad04[0x04];
    u8 kind;                        /* 0x08 */
    char pad09[0x07];
    u8 unk10;                       /* 0x10 */
    char pad11[0x07];
    f32 unk18;                      /* 0x18 */
    char pad1C[0x04];
    f32 pos_x;                      /* 0x20 */
    char pad24[0x04];
    f32 pos_y;                      /* 0x28 */
    char pad2C[0x04];
    f32 pos_z;                      /* 0x30 */
} SpawnRequestWide;

/* a task that feeds the cosine of an angle to a command (func_00214DC0) */
typedef struct {
    s32 active;                     /* 0x00 cleared after one step */
    char pad04[0x04];
    f32 angle;                      /* 0x08 radians */
    char command[4];                /* 0x0C the command func_00211D90 fills in */
} AngleTask;

#endif
