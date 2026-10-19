/* Effect actors: short-lived effects that move a model and end in another effect. */
#ifndef KF_EFFECT_H
#define KF_EFFECT_H

#include "common.h"
#include "model/model.h"
#include "entity/entity.h"

#define EFFECT_SLOT_COUNT 0x60      /* spawner slots */
#define EFFECT_COUNT 0x180          /* effects (EffectActor) */
#define EFFECT_DEF_COUNT 0x400      /* entries of the definition table param/sfx.prm */

/* the effect's definition, EffectActor.def: one 0x120-byte entry of effect_table.
   Three channels are keyframed over the effect's life: keys_a (EffectKeys_ApplyScale), keys_b
   (EffectKeys_ApplySpin) and the colour (EffectKeys_ApplyColour), each at frames 0, two own keyframes and
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
    u8 spawn_batch_count;           /* 0xA5 how many effects the spawner makes each time it fires */
    u8 max_level;                   /* 0xA6 the spawner's level counts down from this */
    u8 spawn_interval;              /* 0xA7 frames between two firings of the spawner, 0: every frame */
    f32 rotation_base[3];           /* 0xA8 */
    f32 rotation_spread[3];         /* 0xB4 random +- range around rotation_base */
    f32 offset_base[3];             /* 0xC0 */
    f32 offset_spread[3];           /* 0xCC random +- range around offset_base */
    f32 speed;                      /* 0xD8 */
    f32 turn_rate;                  /* 0xDC per kind: spin, turn rate, spread */
    f32 ground_height;              /* 0xE0 per kind: height, drift, count */
    f32 spread_angle;               /* 0xE4 per kind: widest random turn or the aim pitch tolerance */
    f32 spin_rate;                  /* 0xE8 per kind: spin/heading rate a frame; elsewhere a count */
    u16 radius_half;                /* 0xEC */
    u16 interval;                   /* 0xEE frames between collision checks, or a range */
    u16 sub_effect_id;              /* 0xF0 effect ids passed to Effect_SpawnFromEffect */
    u16 child_effect_id;            /* 0xF2 effect spawned each frame or on hit; a link count for chains */
    u8 draw_hook_index;             /* 0xF4 with the placement bits, picks the model's draw routine from effect_draw_hooks */
    u8 unkF5;                       /* 0xF5 bit 0: drawn with the map's own light block; bit 1: not drawn in the second pass */
    u8 texture_anim_mode;           /* 0xF6 how the texture frames run: 0 loop, 1 once, 2 once and hold the last */
    u8 texture_frame_count;         /* 0xF7 how many texture frames, 0: not animated */
    u16 texture_frame_stride;       /* 0xF8 record ids between two texture frames */
    u16 texture_record_base;        /* 0xFA nonzero: an id that Effect_Init moves up by 0x1400; the first texture frame's record */
    u8 texture_frame_interval;      /* 0xFC effect frames per texture frame, less one */
    u8 spawn_start_delay;           /* 0xFD frames before the spawner's first firing */
    u16 model_index;                /* 0xFE index of the effect's model in effect_model_table */
    u16 kind_index;                 /* 0x100 index into the handler tables effect_kind_handlers and fx_table */
    u16 keyframe_end;               /* 0x102 last keyframe; kinds that loop restart here */
    f32 gravity;                    /* 0x104 */
    u16 max_frames;                 /* 0x108 0: no limit; the low bits of EFFECT_DEF_FLAGS */
    char pad10A;
    u8 hit_check_index;             /* 0x10B 0xFF: no hit check */
    u16 frame_spawn_effects[3];     /* 0x10C effect spawned when its frame in frame_spawn_frames comes */
    u16 frame_spawn_frames[3];      /* 0x112 frame each frame_spawn_effects entry is spawned at */
    u16 end_effect;                 /* 0x118 spawned when the effect ends */
    u16 sound_id;                   /* 0x11A not 0: sets EffectActor flag bit 59; passed to Sound_PlayTrackedKind */
    f32 scale_per_frame;            /* 0x11C */
} EffectDef;

/* the 64-bit word at EffectDef+0xF0 as Effect_DrawAll tests it (bits 40 and 41 are unkF5's bits 0 and 1) */
#define EFFECT_DEF_WORD_F0(def) (*(s64 *)&(def)->sub_effect_id)

/* the 64-bit flag word at EffectDef+0x108, whose low 16 bits are max_frames */
#define EFFECT_DEF_FLAGS(def) (*(s64 *)&(def)->max_frames)

/* the same word read through a union, which keeps the read behind any earlier store (EffectStart_PlaceModel) */
#define EFFECT_DEF_FLAG_WORD(def) (((EntityFlagWord *)&(def)->max_frames)->word)

/* EffectActor+0x10, which the update functions keep a pointer to. Each kind of
   effect uses the bytes after the velocity its own way; the *Motion types below
   are those views, reached by a cast (a union changes gcc's scheduling). */
typedef struct {
    Vector4 velocity;               /* 0x00 (EffectActor+0x10) */
    Vector4 unk10;                  /* 0x10 per-kind state */
    char unk20[0x10];               /* 0x20 */
} EffectMotion;

/* EffectMotion as EffectShot_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    u16 end_frame;                  /* 0x10 */
    char pad12[0x1E];
} ProjectileMotion;

/* EffectMotion as EffectImpactLob_Update uses it: falls under gravity */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    int mode;                       /* 0x10 3 or 5: spawn effect 0x15 on landing */
    u16 end_frame;                  /* 0x14 */
    s8 spawned;                     /* 0x16 a follow-up effect has been spawned */
    char unk17[0x19];               /* 0x17 passed by address */
} FallingMotion;

/* EffectMotion as EffectGroundFan_Update uses it: checks for hits every interval frames */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x04];
    int countdown;                  /* 0x14 frames to the next check */
    char unk18[0x18];               /* 0x18 passed by address */
} EmitterMotion;

/* EffectMotion as EffectBeam_Update, EffectRay_Update and EffectBeamStretch_Update use it: scales its model */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x10];
    f32 scale_per_mille;            /* 0x20 */
    char pad24[0x0C];
} ScalerMotion;

/* EffectMotion as EffectFall_Start, EffectFloorSkim_Start and others use it: turn toward a point */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    s16 unk20;                      /* 0x20 */
    s16 frames_to_impact;           /* 0x22 frames until the shot reaches what lies ahead */
    u8 unk24;                       /* 0x24 */
    u8 ends_next_frame;             /* 0x25 set once a hit spawned its effects: the effect ends next frame */
    u8 unk26;                       /* 0x26 */
    char pad27[0x09];
} TurningMotion;

/* EffectMotion as EffectHitWindow_Start uses it: the definition's collision radius and
   interval copied over the start of the velocity */
typedef struct {
    u16 radius_half;                /* 0x00 */
    u16 interval;                   /* 0x02 */
    char pad04[0x2C];
} PulseMotion;

/* EffectMotion as EffectLaunchFromOwner_Update (stays attached to another model) uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    ModelInstance **anchor;         /* 0x10 the model it is attached to */
    char unk14[0x1C];               /* 0x14 passed by address */
} AttachedMotion;

/* EffectMotion as EffectHomeOnEntity_Start uses it: follows EffectActor.target */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 speed;                      /* 0x10 the target's velocity.x */
    s32 unk14;                      /* 0x14 */
    f32 unk18;                      /* 0x18 */
    u8 unk1C;                       /* 0x1C */
    char pad1D[0x03];
    s8 ended;                       /* 0x20 set once the effect has ended */
    char pad21[0x0F];
} FollowerMotion;

/* EffectMotion as EffectDelayedShot_Start uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x0C];
    char unk1C;                     /* 0x1C */
    char pad1D[0x13];
} BurstMotion;

/* EffectMotion as EffectSplitShot_SpawnSharedBurst uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x16];
    u16 cooldown;                   /* 0x26 frames before the effect can fire again */
    char pad28[0x08];
} CooldownMotion;

/* EffectMotion as EffectFly_Start and EffectHandover_Update use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    int unk10;                      /* 0x10 */
    f32 unk14;                      /* 0x14 */
    char pad18[0x04];
    char unk1C;                     /* 0x1C */
} TimedMotion;

/* EffectMotion as EffectFly_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 fall_speed;                 /* 0x10 */
    f32 scale;                      /* 0x14 */
    f32 growth;                     /* 0x18 EffectFly_StartGrowing: what the model's scale is multiplied by each frame */
    char unk1C[0x14];               /* 0x1C passed by address */
} ScaledFallMotion;

/* EffectMotion as EffectAtTarget_Start and EffectAtTargetFloor_Start use it */
typedef struct {
    int countdown;                  /* 0x00 frames until the hit check runs (EffectDef.radius_half) */
    u8 unk04;                       /* 0x04 */
} TargetMotion;

/* EffectMotion as EffectAtTarget_Update, EffectAtTargetFloor_Update and others use it */
typedef struct {
    int countdown;                  /* 0x00 (EffectActor+0x10) */
    s8 no_floor;                    /* 0x04 1 when no floor was found under it */
    char pad05[0x0B];
} CountdownMotion;

/* EffectMotion as EffectEmitter_Start and EffectEmitter_Update use it */
typedef struct {
    u16 spawn_count;                /* 0x00 */
    u16 spawn_effect_id;            /* 0x02 copied from def->radius_half */
    u16 spawn_limit;                /* 0x04 copied from def->interval */
    u16 spawn_frames_base;          /* 0x06 copied from def->unkF0 */
    u16 next_spawn_frame;           /* 0x08 */
    u16 spawn_frames_span;          /* 0x0A copied from def->unkF2 */
} SpawnerMotion;

/* EffectMotion as EffectTrailHoming_Start, EffectTrailHoming_Update, EffectHomingChain_Start and others use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 turn_rate;                  /* 0x10 turn rate, copied from def->turn_rate; grows by EffectDef.ground_height while it hunts */
    int hit_cooldown;               /* 0x14 frames until it may be hit again */
    int wobbling;                   /* 0x18 1 once hit (it wobbles) */
    f32 wobble_x;                   /* 0x1C wobble offset x */
    f32 wobble_z;                   /* 0x20 wobble offset z */
    s8 ended;                       /* 0x24 set once the end effect is spawned */
    char pad25[0x0B];
} HomingMotion;

/* EffectMotion as EffectAimedHoming_Start uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 turn_rate;                  /* 0x10 turn rate, copied from def->turn_rate */
} PlayerHomingMotion;

/* EffectMotion as EffectShot_Start uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    u16 frames_to_impact;           /* 0x10 frames until the shot reaches what lies ahead */
} LaunchMotion;

/* EffectMotion as EffectLob_Start uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    int unk10;                      /* 0x10 */
    u8 unk14;                       /* 0x14 */
} DropMotion;

/* EffectMotion as EffectLob_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x04];
    s8 landed;                      /* 0x14 set once it has spawned its landing effects */
} LandedFlagMotion;

/* EffectMotion as EffectGroundChase_Start, EffectGroundChase_Update and others use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    float unk10;                    /* 0x10 */
    u8 links_left;                  /* 0x14 links left in the chain; the last one ends it */
    s8 state;                       /* 0x15 1: landed */
    char pad16[0x0A];
} LandingMotion;

/* EffectMotion as EffectChase_Start and EffectAimedChase_Start set it up: chases the entity in EffectActor.target_entity */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 turn;                       /* 0x10 how sharply it turns toward the entity; grows by EffectDef.ground_height */
    s8 ended;                       /* 0x14 set once the end effect is spawned */
    char pad15[0x0B];
} ChaseMotion;

/* EffectMotion as EffectGroundRun_Start sets it up and EffectGroundRun_Update uses it: runs along the ground */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 probe;                  /* 0x10 passed to Collision_CastRayAtMap; y is def->unkDC */
    f32 height;                     /* 0x20 def->unkDC: the ground is looked for from half of it down */
    f32 fall_speed;                 /* 0x24 */
    f32 hit_offset;                 /* 0x28 def->unkE0: how far below itself it checks for hits */
    s8 ended;                       /* 0x2C set when it starts with no ground under it; also passed by address */
    char pad2D[0x03];
} GroundRunMotion;

/* EffectMotion of the effect that winds along the floor (EffectGroundSway_*) */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 height;                     /* 0x10 EffectDef.turn_rate: the floor is looked for within it */
    f32 fall_speed;                 /* 0x14 */
    f32 sway_rate;                  /* 0x18 EffectDef.ground_height: the sway is the sine of this times angle */
    f32 angle_speed;                /* 0x1C EffectDef.spread_angle, grows by unkE8 each frame */
    f32 angle;                      /* 0x20 */
    s8 ended;                       /* 0x24 set when it starts with no floor under it */
} GroundSwayMotion;

/* EffectMotion of the effect that runs along the floor leaving effects (EffectGroundTrail_*) */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 height;                     /* 0x10 EffectDef.turn_rate: the floor is looked for within it */
    f32 fall_speed;                 /* 0x14 */
    s8 ended;                       /* 0x18 set when it starts with no floor under it */
} GroundTrailMotion;

/* EffectMotion as EffectOnFloor_Start uses it */
typedef struct {
    Vector4 velocity;
    f32 speed;                      /* 0x10 */
    s32 unk14;                      /* 0x14 */
    u8 flag18;                      /* 0x18 */
} SpeedMotion;

/* EffectMotion as EffectArc_Start and EffectChain_Start use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    u8 unk10;                       /* 0x10 */
    char pad11[0x0F];
} SpawnMotion;

/* EffectMotion as EffectLead_Start and EffectLead_Update use it: a speed and a flag */
typedef struct {
    Vector4 velocity;
    f32 speed;                      /* 0x10 */
    u8 flag14;                      /* 0x14 */
} SpeedFlagMotion;

/* EffectMotion as EffectFollow_AimAboveFloor uses it: an aim point toward the target */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 target_x;                   /* 0x10 */
    f32 target_y;                   /* 0x14 the height of the aim point: 600 above the floor */
    f32 target_z;                   /* 0x18 */
    char pad1C[0x04];
    f32 turn;                       /* 0x20 how sharply it turns toward the aim point (EffectFollow_Update) */
    s8 aimed;                       /* 0x24 the aim point is set */
    u8 flag25;                      /* 0x25 */
    s8 ended;                       /* 0x26 the end effect is spawned */
} AimMotion;

/* EffectMotion as EffectHomingChain_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 end_position;           /* 0x10 where the end effect appears */
    int links_left;                 /* 0x20 links left in the chain; the last link spawns the end effect */
    u16 unk24;                      /* 0x24 */
    char pad26[0x0A];
} SelectMotion;

/* EffectMotion as EffectHoverShot_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    int timer;                      /* 0x20 */
    char unk24[0x0C];               /* 0x24 passed by address */
} TimerMotion;

/* EffectMotion as EffectSplitShot_Start uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    int unk20;                      /* 0x20 */
    s16 angle;                      /* 0x24 */
    s16 unk26;                      /* 0x26 */
    u8 unk28;                       /* 0x28 */
    char pad29[0x0F];
} AngleAimMotion;

/* EffectMotion as EffectSplitShot_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 unk10;                  /* 0x10 */
    int timer;                      /* 0x20 */
    u16 end_frame;                  /* 0x24 */
    u16 cooldown;                   /* 0x26 */
    char unk28[0x18];               /* 0x28 passed by address */
} CooldownTimerMotion;

/* EffectMotion as EffectLingerShot_Start uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    u16 count;                      /* 0x20 */
    u8 flag;                        /* 0x22 */
} CountedMotion;

/* EffectMotion as EffectLingerShot_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 end_position;           /* 0x10 where the end effect appears */
    u16 end_frame;                  /* 0x20 */
    s8 countdown;                   /* 0x22 frames left before the next step */
    char pad23[0x0D];
} EndCountdownMotion;

/* EffectMotion as EffectSweep_AimAt uses it: a copied vector and a scaled value */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 target_position;        /* 0x10 copied from the target */
    char pad20[0x04];
    s16 unk24;                      /* 0x24 */
} ScaledMotion;

/* EffectMotion as EffectGroundFan_Start uses it: a heading in velocity.y, the frame counter and phase */
typedef struct {
    Vector4 velocity;               /* 0x00 velocity.y is the heading */
    int frame_stamp;                /* 0x10 the global spawn_slot counter when it started */
    int phase;                      /* 0x14 frame_stamp % interval */
    u8 unk18;                       /* 0x18 */
} PhaseMotion;

/* EffectMotion as EffectLevelHeight_Update uses it: an int step count */
typedef struct {
    int progress;                   /* 0x00 */
} ProgressMotion;

/* EffectMotion as EffectHandover_StartNext uses it: the start frame and the flag set after it */
typedef struct {
    char pad00[0x25];
    u8 started;                     /* 0x25 set to 1 once started */
    u8 start_frame;                 /* 0x26 the start frame */
} StartFrameMotion;

/* EffectMotion as EffectBolt_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 */
    u16 spawn_effect;               /* 0x20 */
    u16 end_frame;                  /* 0x22 */
    s8 spent;                       /* 0x24 */
} OneShotSpawnMotion;

/* EffectMotion as EffectSpray_Start and EffectSpray_Update use it: velocity, spin and acceleration */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 spin;                   /* 0x10 */
    Vector4 accel;                  /* 0x20 */
} SprayMotion;

/* EffectMotion as EffectLateFall_Start uses it: a launch velocity, then the radius and flags */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    int unk10;                      /* 0x10 */
    f32 unk14;                      /* 0x14 */
    int radius_half;                /* 0x18 the definition's radius_half */
    u8 unk1C;                       /* 0x1C */
    char pad1D[0x03];
} RadiusMotion;

/* EffectMotion as EffectLateFall_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x08];
    int fall_frame;                 /* 0x18 the frame the fall starts at */
    char unk1C[0x0C];               /* 0x1C passed by address */
} DelayedFallMotion;

/* EffectMotion as EffectFallToFloor_Start, EffectHomeOnPlayer_Start and EffectRepeater_Start use it: a heading velocity and a flag byte */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    u8 unk10;                       /* 0x10 cleared on spawn */
    char pad11[0x0F];
} HeadingMotion;

/* EffectMotion as EffectFallToFloor_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char ending;                    /* 0x10 set once the end effect is spawned */
    char pad11[0x0F];
} EndingMotion;

/* EffectMotion as EffectFloorChase_Start uses it: a velocity and a countdown to the next target check */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    u8 countdown;                   /* 0x10 */
    u8 unk11;                       /* 0x11 */
    char pad12[0x0E];
} FollowCountMotion;

/* EffectMotion as EffectFloorChase_Update uses it: a bobbing velocity and the ending flag */
typedef struct {
    Vector4 velocity;               /* 0x00 (w is cleared each frame) */
    char pad10;
    char ending;                    /* 0x11 1 once the end has begun */
    char pad12[0x0E];
} BobbingMotion;

/* EffectMotion as EffectFloorDelay_Update uses it: the spawn count, compared with the frame */
typedef struct {
    int spawn_count;                /* 0x00 (EffectActor+0x10) */
    char pad04[0x1C];
} SpawnCountMotion;

/* EffectMotion as EffectTurnFan_Start uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    int start_frame;                /* 0x10 */
    f32 unk14;                      /* 0x14 */
    u8 unk18;                       /* 0x18 */
    char pad19[0x07];
} SpinBurstMotion;

/* EffectMotion as EffectLevelFan_Start uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 scale;                      /* 0x10 */
    u8 unk14;                       /* 0x14 */
    char pad15[0x1B];
} LevelMotion;

/* EffectMotion as EffectLevelFan_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 scale;                      /* 0x10 */
    char unk14[0x1C];               /* 0x14 passed by address */
} GrowingMotion;

/* EffectMotion as EffectRay_Start and EffectRay_StartAtPlayer use it: a float at 0x20 holding the distance to the target */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 hit_point;              /* 0x10 the hit point, or the far end of the ray */
    f32 distance_to_target;         /* 0x20 */
    char pad24[0x0C];
} TargetDistanceMotion;

/* EffectMotion as EffectBeamStretch_Start uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x10];
    f32 beam_length;                /* 0x20 the length of the beam that spawned it */
} LinkedMotion;

/* EffectMotion as EffectWobble_Start and EffectJitterWobble_Start use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    int wobble_angle;               /* 0x10 the wobble angle */
} ResetMotion;

/* EffectMotion as EffectWobble_Update uses it */
typedef struct {
    char pad00[0xE0];
    f32 drift_x;
    f32 drift_y;
} DriftModel;

/* EffectMotion as EffectWobble_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 heading;                    /* 0x10 */
    char pad14[0x1C];
} HeadingAngleMotion;

/* EffectMotion as EffectSurroundPlayer_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x10];
    u16 end_frame;                  /* 0x20 */
    u16 player_frame;               /* 0x22 */
    char unk24[0x0C];               /* 0x24 passed by address */
} PlayerChaseMotion;

/* EffectMotion as EffectChain_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 scale_per_mille;            /* 0x10 */
    char pad14[0x0C];
} ScaleRateMotion;

/* EffectMotion as EffectLaunchFromOwner_Start uses it: the spawn argument is kept after the velocity */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Entity *owner_entity;           /* 0x10 a copy of EffectActor.owner_entity */
    u8 unk14;                       /* 0x14 cleared on spawn */
} SavedArgMotion;

/* EffectMotion as EffectBowDraw_Start uses it */
typedef struct {
    char pad00[4];
    u8 state;                       /* 0x04 */
} StateFlagMotion;

/* EffectMotion as EffectInheritSpeed_Start uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 speed;                      /* 0x10 the target's velocity.x */
    s32 unk14;                      /* 0x14 */
    f32 unk18;                      /* 0x18 */
    u8 unk1C;                       /* 0x1C */
    char pad1D[0x03];
} TargetFollowerMotion;

/* EffectMotion as EffectInheritSpeed_Update uses it: falls after radius_half frames */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x0C];
    char unk1C[0x14];               /* 0x1C passed by address */
} FallingLateMotion;

/* EffectMotion as EffectImpactShot_Start uses it: turns toward a point */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    int end_frame;                  /* 0x20 */
    u8 done;                        /* 0x24 */
    char pad25[0x0B];
} TurnMotion;

/* EffectMotion as EffectImpactShot_Update uses it: ends in another effect at end_frame */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 end_position;           /* 0x10 where the end effect appears */
    int end_frame;                  /* 0x20 */
    s8 done;                        /* 0x24 */
    char pad25[0x0B];
} EndFrameMotion;

/* EffectMotion as EffectSpiral_Start uses it: a spiral */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 start_angle;                /* 0x10 model heading plus EffectDef.ground_height */
    int unk14;                      /* 0x14 */
    f32 unk18;                      /* 0x18 EffectDef.radius_half over half of keyframe_end */
    f32 unk1C;                      /* 0x1C EffectDef.interval over keyframe_end */
    u8 start_step;                  /* 0x20 rand() % 10 */
    u8 unk21;                       /* 0x21 */
    char pad22[0x0E];
} SpiralMotion;

/* EffectMotion as EffectTiltFall_Update and EffectSpiral_Update use it: spins and sways */
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

/* EffectMotion as EffectDrop_Update uses it: falls under gravity, spawns an effect on landing */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 vertical_speed;             /* 0x10 added to the model's height each frame */
    u16 landing_frame;              /* 0x14 */
    s8 landing_mode;                /* 0x16 5 spawns the effect id in radius_half, else in interval */
    char pad17[0x09];
} GravityMotion;

/* EffectMotion as EffectRepeater_Update uses it: spawns an effect every reload frames */
typedef struct {
    int countdown;                  /* 0x00 frames until the next spawn */
    u16 reload_frames;              /* 0x04 */
    char pad06[0x1A];
} RepeaterMotion;

/* EffectMotion as EffectTiltFall_Start and EffectCorkscrew_Start use it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    f32 random_turn;                /* 0x20 random turn */
    f32 spin_rate;                  /* 0x24 spin rate */
} SpreadMotion;

/* EffectMotion as EffectCorkscrew_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    f32 spin_angle;                 /* 0x20 spin angle, wrapped to +-pi */
    f32 spin_speed;                 /* 0x24 spin speed */
} SpinAngleMotion;

/* EffectMotion as EffectJitterWobble_Update uses it */
typedef struct {
    char pad00[0xE4];
    f32 scale_y;                    /* 0xE4 the y scale */
} TiltModel;

/* Local view: the motion's spin angle after the velocity. */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 spin_angle;                 /* 0x10 */
} TiltMotion;

/* EffectMotion as EffectJitterOrbit_Start uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    f32 unk20;                      /* 0x20 */
    f32 unk24;                      /* 0x24 */
} StartMotion;

/* EffectMotion as EffectJitterOrbit_Update uses it */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char unk10[0x10];               /* 0x10 passed by address */
    f32 yaw;                        /* 0x20 */
    f32 pitch;                      /* 0x24 */
} YawPitchMotion;

/* EffectMotion as EffectLoopUntilFlag_Update uses it: loops until a game flag is set */
typedef struct {
    int stopped;                    /* 0x00 set once loop_effect_stop_state is 3 */
    char pad04[0x1C];
} LoopMotion;

/* EffectMotion as EffectBolt_Update uses it: spawns effect_id at end_frame or on a hit */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    char pad10[0x10];
    u16 effect_id;                  /* 0x20 */
    u16 end_frame;                  /* 0x22 */
    s8 ended;                       /* 0x24 */
    char pad25[0x0B];
} DelayedMotion;

/* EffectMotion as EffectCorkscrew_Start and EffectJitterOrbit_Start set it up: an end position and two floats */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 end_position;           /* 0x10 */
    f32 velocity_scale;             /* 0x20 factor the drift is scaled by; grows by EffectDef.scale_per_frame */
    f32 unk24;                      /* 0x24 */
    char pad28[0x08];
} DriftMotion;

/* EffectMotion as EffectBoneFade_Update uses it: the distances the effect fades in over */
typedef struct {
    f32 range[2];                   /* 0x00 fade-in distance, full-size distance */
    char pad08[0x28];
} FadingMotion;

/* EffectMotion as EffectOwnerFloor_Start uses it: a flag byte at the very start */
typedef struct {
    u8 no_floor;                    /* 0x00 set when EffectOwnerFloor_Start finds no floor */
    char pad01[0x2F];
} GroundedMotion;

/* EffectMotion as EffectRepeater_Start uses it: the heading in degrees, and as a word */
typedef struct {
    s32 heading_word;               /* 0x00 the heading, zero-extended */
    u16 heading;                    /* 0x04 degrees */
    char pad06[0x2A];
} HeadingWordMotion;

/* EffectMotion as EffectDrop_Start uses it: the frame the effect lands at */
typedef struct {
    char pad00[0x14];
    u16 landing_frame;              /* 0x14 */
    u8 unk16;                       /* 0x16 */
    char pad17[0x19];
} LandingFrameMotion;

/* EffectMotion as EffectFall_Start uses it: a flag byte right after the velocity */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    s8 ends_next_frame;             /* 0x10 cleared when the model is lifted; set once a hit spawns the end effect */
    char pad11[0x1F];
} LiftedMotion;

/* EffectMotion as EffectBeam_Start reads it from the effect that picked the targets: an entity and a distance per slot */
typedef struct {
    struct Entity *entity[4];       /* 0x00 */
    f32 distance[4];                /* 0x10 */
    s8 list_index[4];               /* 0x20 where the entity is in the list active_entities_head, -1: none (EffectPickTargets_Start) */
    char pad24[0x0C];
} PickedMotion;

/* EffectMotion as EffectBeam_Update uses it: scales the model, spawns the end effect */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 end_position;           /* 0x10 where the end effect appears */
    f32 scale_per_mille;            /* 0x20 */
    s8 ended;                       /* 0x24 */
    char pad25[0x0B];
} ScalingEndMotion;

/* EffectMotion as EffectBowDraw_Update uses it: a scale and a countdown byte */
typedef struct {
    f32 scale;                      /* 0x00 */
    s8 countdown;                   /* 0x04 frames the model is held still */
    char pad05[0x2B];
} ShockMotion;

/* EffectMotion as EffectAtTarget_Start (sits on another entity) uses it */
typedef struct {
    s32 radius_half;                /* 0x00 */
    s8 no_floor;                    /* 0x04 1 when no floor was found under it */
    char pad05[0x1B];
} SnappedMotion;

/* EffectMotion as EffectImpactShot_Update uses it: spawns the end effect at end_frame */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 end_position;           /* 0x10 where the end effect appears */
    s32 end_frame;                  /* 0x20 */
    s8 ended;                       /* 0x24 also the start of the block passed by address */
    char pad25[0x0B];
} EndAtFrameMotion;

/* EffectMotion as EffectHomingChain_Start (homes on a target) sets it up */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    Vector4 aim;                    /* 0x10 */
    s32 timer;                      /* 0x20 */
    u16 flags;                      /* 0x24 */
    char pad26[0x0A];
} AimingMotion;

/* EffectMotion as EffectBobFly_Start and EffectBobFly_Update use it: grows along its velocity and bobs up and down */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 bob_speed;                  /* 0x10 added to the model's height each frame */
    f32 bob_limit;                  /* 0x14 the bob turns round at +- this speed */
    f32 velocity_scale;             /* 0x18 grows by EffectDef.scale_per_frame */
    s8 ended;                       /* 0x1C set by the hit check: the effect ends next frame */
    s8 bob_rising;                  /* 0x1D */
    char pad1E[0x12];
} BobbingGrowMotion;

/* EffectMotion as EffectHandover_Update reads it (set up by EffectHandover_Start and EffectHandover_StartNext): a
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

/* EffectMotion as EffectTiltFall_Start and EffectTiltFall_Update use it: tilts to and fro while it falls */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 tilt_speed;                 /* 0x10 added to the model's rotation.x each frame */
    f32 tilt_accel;                 /* 0x14 */
    f32 fall_speed;                 /* 0x18 */
    f32 fall_accel;                 /* 0x1C */
    s8 tilting_back;                /* 0x20 */
    char pad21[0x0F];
} TiltFallMotion;

/* EffectMotion as EffectSweep_Start and EffectSweep_Update use it (EffectSweep_AimAt fills aim and
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

/* EffectMotion as EffectChain_Start uses it: one link of a chain of effects, each spawning the next */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 length;                     /* 0x10 how far on the next link starts */
    int links_left;                 /* 0x14 one less than the link before; 0 ends the chain */
    char pad18[0x18];
} ChainMotion;

/* EffectMotion of the bobbing homers of kind 24 (EffectBobHoming_*) */
typedef struct {
    Vector4 velocity;               /* 0x00 */
    f32 bob_speed;                  /* 0x10 added to the model's height each frame */
    f32 bob_limit;                  /* 0x14 the bob turns round at +- this speed */
    f32 turn;                       /* 0x18 how sharply it turns toward its target; grows to EffectDef.ground_height */
    f32 velocity_scale;             /* 0x1C grows by EffectDef.scale_per_frame */
    u16 countdown;                  /* 0x20 */
    s8 ended;                       /* 0x22 */
    s8 end_countdown;               /* 0x23 frames until it ends after another of its group did */
    s8 bob_rising;                  /* 0x24 */
    char pad25[0x0B];
} HomingBobMotion;

/* one 0xA0-byte effect of effect_pool (0x180 of them) */
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
    char *map_record;               /* 0x58 one of MapSlot.records; cleared when its map is unloaded (Effect_FreeForMap) */
    Entity *owner_entity;           /* 0x5C the entity that cast it; freed with it (Effect_FreeForOwner) */
    struct EffectActor *target;     /* 0x60 */
    Vector4 *start_position;        /* 0x64 */
    Vector4 *start_rotation;        /* 0x68 */
    int target_entity;              /* 0x6C the Entity the effect homes on (EffectTarget view) */
    Vector4 saved_position;         /* 0x70 position kept with the model's, moved by Effect_TransformAll and Effect_OffsetAll */
    char pad80[0x10];
    int reserved_frames;            /* 0x90 frames the actor stays reserved once it is freed */
    char pad94[0x0C];
} EffectActor;

/* EffectActor+0x00 as EffectKeys_ApplyColour writes it: the colour, whose alpha is EffectActor.scale */
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

/* effect_table[effect id]: an effect's definition-table entry (0x120 bytes) */
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
    int frames_left;                /* 0x40 frames left before the effect's spawn check runs again */
    s16 linked_point_index;         /* 0x44 the linked point it holds, -1: none */
    s8 sound_track;                 /* 0x46 the tracked sound it plays, -1: none */
    u8 unk47;                       /* 0x47 */
} EffectActorFlagParts;

#define EFFECT_FLAG_PARTS(effect) ((EffectActorFlagParts *)&(effect)->flags)

/* what EffectActor.target_entity points at: an Entity (entity/entity.h) as far as effects read it */
typedef struct {
    ModelInstance *model;           /* 0x00 */
    char pad04[0x38];
    f32 height;                     /* 0x3C */
    char pad40[0x98];
    s64 flags;                      /* 0xD8 */
} EffectTarget;

/* one 0x50-byte slot of the effect spawner pool, effect_slots (0x60 of them) */
typedef struct {
    Vector4 position;               /* 0x00 */
    Vector4 rotation;               /* 0x10 the spawn rotation: pitch x, heading y (the origin's when placed by the spawner) */
    s64 flags;                      /* 0x20 low word: the EffectDef; EFFECT_SLOT_LIVE */
    Entity *owner_entity;           /* 0x28 passed on to the effect */
    struct EffectActor *owner_effect; /* 0x2C the effect that spawned the slot */
    void *start_position;           /* 0x30 pointer to the position the spawned effects start at, when the def keeps it */
    void *start_rotation;           /* 0x34 pointer to the rotation the spawned effects start with, when the def keeps it */
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
    u64 bit33 : 1;                  /* copied from EffectActor flag bit 59 */
    u64 pad34 : 6;
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
    u8 frames_to_firing;            /* 0x26 frames to the next firing */
    u8 start_delay_frames;          /* 0x27 frames before the first firing, from EffectDef.spawn_start_delay */
} EffectSlotFlagParts;

#define EFFECT_SLOT_FLAG_PARTS(slot) ((EffectSlotFlagParts *)&(slot)->flags)

/* the whole EffectSlot as Effect_RunSpawner reads it: the bytes of the flag word as members of
   the slot itself (no address is taken, so gcc keeps no pointer to the word), and start_rotation as
   the vector pointer it is */
typedef struct {
    char pad00[0x20];
    EffectDef *def;                 /* 0x20 */
    u8 unk24;                       /* 0x24 */
    u8 level;                       /* 0x25 the firings left */
    u8 frames_to_firing;            /* 0x26 frames to the next firing */
    u8 start_delay_frames;          /* 0x27 frames before the first firing, from EffectDef.spawn_start_delay */
    char pad28[0x0C];
    Vector4 *start_rotation;        /* 0x34 EffectSlot.start_rotation */
    char pad38[0x18];
} EffectSlotView;

#define EFFECT_SLOT_VIEW(slot) ((EffectSlotView *)(slot))

/* FxTableEntry+0xAC: what casting the magic does to the player (Player_CastMagic) */
typedef struct {
    u16 clears_timed_flag[6];       /* 0x00 nonzero: PlayerState.timed_flags[i] is cleared */
    u16 ailment_attack_bonus[6];    /* 0x0C goes to PlayerState.ailment_attack_bonus */
    s16 strength_bonus[9];          /* 0x18 goes to PlayerState.physical_strength_bonus and magic_strength_bonus */
    u16 ailment_defence_bonus[6];   /* 0x2A goes to PlayerState.ailment_defence_bonus */
    s16 defence_bonus[9];           /* 0x36 goes to PlayerState.physical_defence_bonus and magic_defence_bonus */
    u16 duration;                   /* 0x48 frames the values above last */
    s16 heal_total;                 /* 0x4A healing in all; 999 and more: all of it */
    s16 heal_at_once;               /* 0x4C healing given at once */
    s16 heal_step_period;           /* 0x4E frames per point of the rest */
} FxTableStats;

/* FxTableEntry+0x34: one of the three hit checks of an effect, EffectDef.hit_check_index picks it (Attack_RunEffectHitCheck) */
typedef struct {
    EntityParamsUnk240PartUnk00 strengths; /* 0x00 goes to Unk1B1470Request.unk50 */
    EntityParamsUnk240PartUnk12 unk12; /* 0x12 goes to Unk1B1470Request.unk62 */
    s16 radius;                     /* 0x1E the hit's radius (shape 0 and 1) */
    s16 height;                     /* 0x20 the hit's height (shape 1) */
    u8 shape;                       /* 0x22 0 to 2: Unk1B1470Request.shape */
    u8 target_mask;                 /* 0x23 target bits, copied to the request's target_mask (all but 0x40) */
    u8 frames_left_on_hit;          /* 0x24 frames the effect's frames_left is set to once the hit has landed */
    u8 stun_frames;                 /* 0x25 frames the hit stuns for */
    u8 knockback;                   /* 0x26 not 0: the hit knocks back */
    u8 instant_kill_chance_percent; /* 0x27 chance of an instant kill, in percent */
} FxTableHitCheck;

/* fx_table[]: a table entry of 0x110 bytes, indexed by effect id (Attack_LaunchMagicEffect) */
typedef struct {
    char pad00[0x34];
    FxTableHitCheck hit_checks[3];  /* 0x34 */
    FxTableStats stats;             /* 0xAC what casting the magic does to the player (Player_CastMagic) */
    u16 mp_cost;                    /* 0xFC taken from PlayerState.mp_now to cast it (Player_CastMagic) */
    u16 gauge_charge_divisor;       /* 0xFE divides how fast PlayerState.magic_gauge fills (Player_UpdateStatus) */
    u16 effect_id;                  /* 0x100 the effect the spell spawns when it is cast */
    s8 item_menu_magic;             /* 0x102 nonzero: the effect cannot be started */
    char pad103[0x01];
    u16 gauge_refill_delay;         /* 0x104 goes to PlayerState.magic_gauge_delay when it is cast */
    char pad106[0x02];
    f32 effect_depth_below;         /* 0x108 how far below the caster the magic's effect is placed */
    char pad10C[0x04];
} FxTableEntry;

extern EffectActor effect_pool[EFFECT_COUNT]; /* the effect pool (effect/effect.c) */
extern int effect_sound_count;                       /* live effects that hold a sound slot */

/* effect/effect.c: the spawner slot and effect pools */
void Effect_Init(void);
void Effect_DebugFreeAllButPlayer(void);
void Effect_TransformAll(void *transform);
int Effect_StartSpawner(int effect_id, void *position, void *rotation, void *owner_entity);
int Effect_Spawn(int effect_id, Vector4 *position, Vector4 *rotation, Entity *owner_entity, u16 effect_param, void *owner);
void Effect_UpdateAll(void);
void Effect_UploadTextures(void);
void Effect_DrawAll(int pass);
int Effect_SpawnFromEffect(int effect_id, EffectActor *effect);
int Effect_SpawnFromEffectAt(int effect_id, EffectActor *effect, Vector4 *position);
void Effect_Free(EffectActor *effect);
void Effect_FreeSpawner(EffectSlot *slot);
void Effect_FreeTargeting(EffectActor *target);
void Effect_FreeSpawnersOf(EffectActor *owner);
void Effect_OffsetAll(void *offset);
void Effect_FreeForMap(int map);
void Effect_FreeForOwner(Entity *owner);
void Effect_FreeUntaggedForOwner(Entity *owner);
void Effect_FreeByParam(int effect_param);
void Effect_FreePlayerWeaponKinds(void);
void Effect_FreePlayerMagic(void);
void Effect_FreeSpawnersByParam(int effect_param);
int Effect_IsDefRangeInUse(int first_def, int last_def);

/* helpers every effect kind calls */
void EffectFly_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLoop_Update(EffectActor *effect);
int EffectKeys_ApplyScale(EffectActor *effect);
int EffectKeys_ApplySpin(EffectActor *effect);
int EffectKeys_ApplyColour(EffectActor *effect);
int EffectHit_Check(EffectActor *effect);
void EffectAngle_ToVector(float length, Vector4 *out, Vector4 *angles);
void EffectStart_PlaceModel(EffectActor *effect, EffectSlot *spawn_slot);
void EffectStart_TurnModel(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSteer_Toward(Vector4 *velocity, Vector4 *direction, float speed, float turn);

#endif
