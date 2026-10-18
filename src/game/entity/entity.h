/* Entities: the live objects kept on singly linked lists. */
#ifndef KF_ENTITY_H
#define KF_ENTITY_H

#include "common.h"
#include "model/model.h"

#define ENTITY_DESPAWN_MARGIN           300.0f
#define ENTITY_VERTICAL_DESPAWN_MARGIN  150.0f

/* what Entity.spawn_info points at */
typedef struct {
    f32 spawn_x;                    /* 0x00 the block starts with the spawn position */
    f32 spawn_y;                    /* 0x04 */
    f32 spawn_z;                    /* 0x08 */
    char pad0C[0x04];
    f32 spawn_rotation_x;           /* 0x10 start of the rotation it spawns with (unk14 is its y) */
    f32 spawn_rotation_y;           /* 0x14 an angle (the facing it spawns with?) */
    f32 spawn_rotation_z;           /* 0x18 third rotation component (ScriptCmd_GetEntitySpawnRotation) */
    char pad1C[0x04];
    f32 spawn_scale[4];             /* 0x20 the scale it spawns with */
    u16 type_id;                    /* 0x30 which pool block holds its data */
    char pad32[0x02];
    s8 pool_size;                   /* 0x34 0-4: one pool block, else two */
    u8 barrier_level;               /* 0x35 strips of CollisionMesh.barrier_strips rated below it are skipped (EntityCollision_SphereAgainstMesh) */
    u16 death_script_id;            /* 0x36 what Script_Start gets when the entity dies */
    u64 spawn_flags;                /* 0x38 flags of the spawn: id, death limits, respawn, despawn */
    u8 spawn_mode;                  /* 0x40 the mode of this spawn: picks among the behaviours of its type, most often the think handler the set-up function installs */
    u8 variant;                     /* 0x41 picks the animation set */
    u8 map_object_id;               /* 0x42 ids of two map objects (see MapObject_GetById) */
    u8 second_map_object_id;        /* 0x43 id of the second map object it uses */
    u8 script_anim_frame;           /* 0x44 an animation frame number */
    char pad45[0x03];
    u16 vertical_despawn_range;     /* 0x48 0: no vertical limit */
    char pad4A[0x02];
    u16 wander_radius;              /* 0x4C radius round the spawn point it wanders and is held in */
    u16 talk_script_id;             /* 0x4E asked of Script_IsRunning before unk58 is used, 0: nothing */
    u16 respawn_distance;           /* 0x50 0: Entity_InitFromMapSpawns makes it 1800 */
    u8 respawn_chance;              /* 0x52 0: Entity_InitFromMapSpawns makes it 100 */
    char pad53;
    u16 wake_distance;              /* 0x54 how near the player has to come for a dormant entity to get up (EntityAi_Dormant) */
    u16 wander_height_range;        /* 0x56 half-range of the random height of picked wander points */
    u16 item_script_id;             /* 0x58 passed to Script_Start when an item is used on the entity, 0: nothing */
    u16 despawn_range;              /* 0x5A */
} EntitySpawnInfo;

/* EntitySpawnInfo.spawn_flags read through a union. The game's field is a union member:
   only then does the compiler keep earlier stores of other types ahead of the read. */
typedef union {
    u64 word;
    u32 halves[2];
    u8 bytes[8];                    /* [4]: spawn id for entity_by_spawn_id; [6]: 0 becomes 80 (Entity_InitFromMapSpawns) */
} EntitySpawnFlags;

#define ENTITY_SPAWN_FLAGS(entity) (((EntitySpawnFlags *)&(entity)->spawn_info->spawn_flags)->word)

struct Entity;

/* what EntityModel.level_tables points at: the ModelChannel (model/model.h) seen from Entity code */
typedef struct {
    char pad00[0x81];
    u8 motion_index;                /* 0x81 the motion playing (ModelChannel.index) */
    s16 frame;                      /* 0x82 the motion's current frame */
    s32 frame_fixed;                /* 0x84 the frame times 256 */
    s32 blend_left;                 /* 0x88 blend still to run; 0 once the motion plays on its own */
    u16 frame_step;                 /* 0x8C set from EntityParams.motion_speeds[] */
} EntityModelChannel;

/* one 0x100-byte element of what EntityModel.parts points at */
typedef struct {
    char pad00[0x50];
    f32 unk50[4];                   /* 0x50 */
    char pad60[0x08];
    u64 unk68;                      /* 0x68 flags */
    char pad70[0x90];
} EntityModelUnkF8;

/* what EntityModel.parts points at, seen as 0x80-byte elements: MutantWall_TurnPart2ToPlayer indexes
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

/* what Entity.model points at. ModelInstance (model/model.h) is a second view of the same block,
   see ENTITY_MODEL; the two are to be unified. */
typedef struct {
    char pad00[0xC0];
    f32 position[4];                /* 0xC0 */
    f32 rotation[4];                /* 0xD0 x, y (heading), z angles in radians */
    f32 scale[4];                   /* 0xE0 the model's x, y and z scale */
    char padF0[0x08];
    EntityModelUnkF8 *parts;        /* 0xF8 the model's part array (0x80 or 0x100 bytes per part) */
    EntityModelChannel *level_tables; /* 0xFC the model's animation channel (ModelInstance.level_tables) */
    char pad100[0x02];
    u16 draw_flags;                 /* 0x102 0x800: drawn with its own texture (same word as ModelInstance) */
    char pad104[0x30];
    s32 aim_part;                   /* 0x134 the part the model file marks 2; turned towards a target */
    void *look_at_part;             /* 0x138 the part look-at turns (the third the model file marks) */
    f32 radius;                     /* 0x13C the larger of Entity.collision_radius and unk34 */
    char pad140[0x1C];
    f32 alpha;                      /* 0x15C the model's alpha, 0x80 is opaque */
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
    u8 mp_damage;                   /* 0x1F mp the hit takes from the player */
    u16 radius_bonus;               /* 0x20 added to Entity.radius for the hit's radius */
    u16 half_angle_deg;             /* 0x22 half the arc of the hit, in degrees */
    u16 first_frame;                /* 0x24 first animation frame it is active on, 0: unused */
    u16 last_frame;                 /* 0x26 last animation frame it is active on */
} EntityParamsUnk240Part;

/* EntityParamsUnk240Part.unk1E as EntityAi_SendMeleeHit reads it: bit 1 of the halfword at 0x1E
   says unk1F is copied (reached by cast: a union changes gcc's scheduling) */
typedef struct {
    u16 flags;                      /* 0x1E bit 1: unk1F is copied */
} EntityParamsUnk240PartFlags;

#define ENTITY_PART_FLAGS(part) ((EntityParamsUnk240PartFlags *)&(part)->unk1E)

/* one of the 0x88-byte records at EntityParams+0x240 (EntityParams.melee_attack_third is the third) */
typedef struct {
    u8 attack_weight;               /* 0x00 weight in the six-attack draw; 0: the attack is not used */
    u8 hit_start_frame;             /* 0x01 animation frame the attack starts on, turned to the player up to it */
    u8 attack_delay;                /* 0x02 base frames before the next attack after this one */
    u8 hit_end_frame;               /* 0x03 animation frame the hit window ends on, held by flag bit 19 */
    u16 reach_min;                  /* 0x04 nearest the player may be for it, 0: no limit */
    u16 reach_max;                  /* 0x06 farthest the player may be for it */
    f32 reach_angle;                /* 0x08 widest angle to the player for it */
    u8 wear_armour_part;            /* 0x0C which piece of the player's armour the hit wears out */
    char pad0D;
    s16 heading_offset_deg;         /* 0x0E degrees its direction is off the entity's heading */
    EntityParamsUnk240Part hit_parts[3]; /* 0x10 the three hit parts of the attack: window, arc, power, ailments */
} EntityParamsUnk240;

/* the two blocks ParaStool_CheckPartHitInPlayerView fills for Attack_Run */
typedef struct {
    f32 position[4];                /* 0x00 */
    f32 previous_position[4];       /* 0x10 an earlier position: Attack_HitEntity takes the hit direction from it */
    f32 second_point[4];            /* 0x20 shape 2: a second point (Attack_RunEffectHitCheck) */
    f32 radius;                     /* 0x30 radius of the hit: sphere, sweep or cylinder */
    f32 height;                     /* 0x34 height of the hit's cylinder above its position */
    f32 facing;                     /* 0x38 heading the hit is aimed at, in radians */
    f32 half_angle;                 /* 0x3C half the hit's arc either side of facing */
    f32 pitch;                      /* 0x40 pitch, half_pitch, unk78 and highest_y: more of the shape of the player's swept requests (Attack_RunSwept) */
    f32 half_pitch;                 /* 0x44 half the swing's pitch window */
    struct Entity *owner;           /* 0x48 */
    u64 shape : 2;                  /* 0x4C which hit shape: 0 sphere, 3 swept, 1 and 2 do nothing */
    u64 unk4C_2 : 1;
    u64 knockback : 1;              /* not 0: the hit knocks the player back */
    u64 unk4C_4 : 4;
    u8 target_mask;                 /* 0x4D what it may hit: 1 player, 2 entities, 4 objects, 8/0x10 the map */
    u8 hit_armor_part;              /* 0x4E which worn piece a hit on the player wears out, 1 to 4 */
    u8 stun_frames;                 /* 0x4F copied to Entity.stun_frames by a hit of kind 1 */
    EntityParamsUnk240PartUnk00 unk50; /* 0x50 */
    EntityParamsUnk240PartUnk12 unk62; /* 0x62 */
    u8 map_object_damage_multiplier; /* 0x6E a weapon hit's damage to a map object is multiplied by it when the object asks (Attack_HitMapObject) */
    u8 double_hit_chance_percent;   /* 0x6F chance in percent that the hit counts double */
    u8 instant_kill_chance_percent; /* 0x70 chance in percent that the hit kills outright */
    u8 mp_damage;                   /* 0x71 taken off PlayerState.mp_now when the hit lands on the player (Attack_HitPlayer) */
    u16 player_hit_cooldown;        /* 0x72 not 0: the hit does nothing to the player */
    u8 landed_on_player;            /* 0x74 set once the hit has landed on the player */
    char pad75[0x03];
    f32 lowest_y;                   /* 0x78 lowest y of the player's swung hit window */
    f32 highest_y;                  /* 0x7C highest y of the player's swung hit window */
} Unk1B1470Request;

typedef struct {
    u8 is_magic_shot;               /* 0x00 0: the hit is a weapon's, 1: a magic's */
    char pad01;
    s16 strength_gauge;             /* 0x02 the power gauge's charge when the hit was launched */
    s32 attack_request;             /* 0x04 the request a weapon's shot reuses, 0 for a magic */
    char pad08[0x06];
    s16 worn_magic_bonus[6];        /* 0x0E per magic kind bonus from worn items, in fiftieths */
    char pad1A[0x02];
    s32 magic_use_counted;          /* 0x1C set once this hit has counted a magic use */
    s16 player_power;               /* 0x20 the attacker's power, in fiftieths of the strengths */
    s16 player_magic;               /* 0x22 the attacker's magic, in fiftieths of the strengths */
    s16 weapon_index;               /* 0x24 the weapon this hit wears and counts a use of */
    s16 magic_id;                   /* 0x26 the magic this hit counts a use of */
    char pad28[0x08];
} Unk1B1470Extra;

/* Unk1B1470Request with the bits at 0x4C as byte-sized fields: Attack_HitEntity reads unk4C_3 as a u8 */
typedef struct {
    char pad00[0x4C];
    u8 unk4C_0 : 3;                 /* 0x4C */
    u8 unk4C_3 : 1;
    u8 unk4C_4 : 4;
    char pad4D[0x33];
} __attribute__((aligned(8))) Unk1B1470RequestBits;

/* Unk1B1470Request with the four flag bytes from 0x4C as one 32-bit field, the way
   Attack_Run reads the shape in the low two bits */
typedef struct {
    char pad00[0x48];
    struct Entity *owner;           /* 0x48 */
    u64 flags : 32;                 /* 0x4C */
} Unk1B1470RequestWord;

/* Unk1B1470Request with the owner and the flag bytes after it as the one 64-bit word
   Attack_RunSphere and Attack_HitPlayer test bits of (the flag bytes are its upper half) */
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
    u8 magic_effect;                /* 0x00 passed to Attack_LaunchMagicEffect */
    u8 bone;                        /* 0x01 passed to Model_GetSkinnedBonePoint; 0xFF: use the model's direction */
    u16 frame;                      /* 0x02 animation frame it happens on, 0: unused */
} EntityParamsUnk3D8Launch;

/* a whole 0x1C-byte record at EntityParams+0x3D8 (EntityParamsUnk3D8 is its first word) */
typedef struct {
    u8 unk00;                       /* 0x00 */
    u8 turn_frames;                 /* 0x01 the entity turns to the player until the motion reaches this frame */
    u8 attack_delay;                /* 0x02 passed to EntityAi_SetAttackDelay when the attack ends */
    u8 unk03;                       /* 0x03 */
    char pad04[0x08];
    u8 unk0C;                       /* 0x0C */
    char pad0D[0x03];
    EntityParamsUnk3D8Launch launches[3]; /* 0x10 the three launches of the record */
} EntityParamsUnk3D8Full;

/* sub-struct at EntityParams+0x3D8 */
typedef struct {
    u8 unk00;                       /* 0x00 */
    u8 turn_frames;                 /* 0x01 the entity turns to the player until the motion reaches this frame */
    u8 attack_delay;                /* 0x02 passed to EntityAi_SetAttackDelay when the attack ends */
    u8 unk03;                       /* 0x03 */
} EntityParamsUnk3D8;

typedef struct {
    EntityParamsUnk3D8 record;      /* 0x00 */
    u16 reach_min;                  /* 0x04 nearest distance (past the radius) the attack is used at, 0: none */
    u16 reach_max;                  /* 0x06 farthest distance it is used at */
    f32 widest_angle;               /* 0x08 widest angle to the player for it */
    u8 unk0C;                       /* 0x0C */
    char pad0D;
    s16 heading_offset_deg;         /* 0x0E degrees its direction is off the entity's heading */
    u8 unk10;                       /* 0x10 */
    u8 unk11;                       /* 0x11 */
    char pad12[0x0A];
} EntityParamsUnk3D8Array;

/* one of the four bodies of an entity kind the player collides with piece by piece (EntityParams.body_points) */
/* the same slot as the motion code reads it: every flag a byte-wide bit-field (Motion_StepSlot) */
typedef struct {
    u8 index : 5;                   /* 0x00 which EntityParams.motion_ids entry (motion) it plays */
    u8 moves_entity : 1;            /* the root part's movement in the motion is handed to the entity */
    u8 bit6 : 1;
    u8 backwards : 1;               /* the motion plays from its last frame to its first */
    u8 frames_left;                 /* 0x01 frames left, counted down by EntityAi_StepSlotMotion */
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
    u16 radius;                     /* 0x04 its radius, before the model's x scale */
    u16 height;                     /* 0x06 its height, before the model's y scale */
    f32 hit_multiplier;             /* 0x08 a hit on this body counts this many times over (Attack_RunSphere) */
} EntityBodyPoint;

/* what Entity.params points at: per-kind tuning values. EntityController below is a second
   view of the same block; the two are to be unified. */
typedef struct {
    u32 model_number : 16;          /* 0x00 the chr/cNNNN.chr model file number, 0xFFFF: none */
    u32 entity_type : 8;            /* 0x02 index into entity_type_handlers */
    u32 unk03_0 : 1;                /* 0x03 */
    u32 movement_kind : 2;          /* 0x03 bits 25-26: how it moves and collides (ENTITY_PARAMS_MOVEMENT_KIND) */
    u32 sphere_collision : 1;       /* 0x03 bit 27: it collides as a sphere, not as a cylinder */
    u32 unk03_4 : 4;
    u16 body_radius;                /* 0x04 Entity.body_radius before scaling */
    u16 map_collision_radius;       /* 0x06 Entity.collision_radius before scaling */
    u16 map_collision_height;       /* 0x08 Entity.collision_height before scaling */
    u16 hit_points;                 /* 0x0A what Entity.hit_points is measured against */
    u16 body_volume;                /* 0x0C 0: Entity_InitFromMapSpawns makes it 70 */
    u16 experience_reward;          /* 0x0E passed to Player_AddExperience when the entity dies */
    u16 flags;                      /* 0x10 ENTITY_PARAMS_FLAG_ bits */
    char pad12[0x02];
    u8 combat_choice_weights[4];    /* 0x14 weights of the four actions, scaled by Entity.choice_weights */
    u16 attack_range;               /* 0x18 a distance to the player (see Npc_Think) */
    u16 attack_height_band;         /* 0x1A a height: ranged attacks need the player level with it (EntityAi_PickCombatChoice) */
    char pad1C[0x04];
    EntityBodyPoint body_points[4]; /* 0x20 what the player collides with for the kinds made of several bodies (Player_CollideWithEntities) */
    f32 speed_step_per_frame;       /* 0x50 how much its movement speeds change each frame */
    f32 max_speed;                  /* 0x54 the most the movement speeds it builds up reach */
    f32 turn_step_per_frame;        /* 0x58 how much its turn speed changes each frame */
    f32 turn_limit;                 /* 0x5C */
    u8 motion_ids[0x20];            /* 0x60 byte table */
    u16 motion_speeds[0x22];        /* 0x80 indexed by Entity.animation_index */
    u8 motion_blend_frames;         /* 0xC4 frames Motion_Start blends the new animation in over */
    u8 respawn_effect_bone;         /* 0xC5 bone index for Model_GetSkinnedBonePoint (EntityEvent_Respawn) */
    u16 respawn_effect_id;          /* 0xC6 effect started at that bone when it respawns, 0xFFFF: none */
    u8 attack_delay;                /* 0xC8 base frames before its next attack, plus a player-HP term */
    u8 critical_defence;            /* 0xC9 percent off a hit's chance to double or to kill outright */
    u8 dodge_chance;                /* 0xCA chance out of 500 that it dodges the player's swing */
    u8 guard_chance;                /* 0xCB chance out of 500 that it guards the player's swing */
    u16 radius;                     /* 0xCC Entity.radius before scaling */
    u16 guard_frames;               /* 0xCE frames the default guard holds animation 13 with flag bit 35 */
    u8 guard_physical_bonus;        /* 0xD0 added to each physical resistance while the guard flag is set */
    u8 guard_magic_bonus;           /* 0xD1 added to each magic resistance while the guard flag is set */
    u8 model_scale_percent;         /* 0xD2 the model's scale in percent */
    char padD3;
    s16 resist_slash;               /* 0xD4 unkD4 to resist_dark: nine resistances, 100 and up: immune (Attack_HitEntity) */
    s16 resist_hit;                 /* 0xD6 hit (blunt) resistance; 100 and up: immune */
    s16 resist_stab;                /* 0xD8 stab resistance; 100 and up: immune */
    s16 resist_fire;                /* 0xDA fire resistance; 100 and up: immune */
    s16 resist_earth;               /* 0xDC earth resistance */
    s16 resist_wind;                /* 0xDE wind resistance */
    s16 resist_water;               /* 0xE0 water resistance */
    s16 resist_light;               /* 0xE2 light resistance */
    s16 resist_dark;                /* 0xE4 dark resistance */
    u16 physical_defence;           /* 0xE6 added to each of the three physical resistances */
    u16 magic_defence;              /* 0xE8 added to each of the six magic resistances */
    u8 ailment_defence_1;           /* 0xEA unkEA to ailment_defence_4: percent taken off a hit's chance to start each of the four ailments (Attack_RollEntityAilments) */
    u8 ailment_defence_2;           /* 0xEB percent off the hit's chance to start ailment 2 of its four */
    u8 ailment_defence_3;           /* 0xEC percent off the hit's chance to start ailment 3 of its four */
    u8 ailment_defence_4;           /* 0xED percent off the hit's chance to start ailment 4 of its four */
    u16 body_height;                /* 0xEE Entity.height before scaling */
    u8 anim_effect_counts[0x20];    /* 0xF0 how many records anim_effect_offsets[n] leads to */
    u8 anim_sound_counts[0x20];     /* 0x110 how many records anim_sound_offsets[n] leads to */
    s32 anim_effect_offsets[0x20];  /* 0x130 offsets from entity_params to EntityParamsUnk130 records, by animation */
    s32 anim_sound_offsets[0x20];   /* 0x1B0 offsets from entity_params to EntityParamsUnk1B0 records, by animation */
    u16 gold;                       /* 0x230 how much gold it drops (Gold_DropFromEntity): half of it for sure, the rest by chance */
    u16 drop_item;                  /* 0x232 the item it may drop (MapItem_DropFromEnemy), 0xFFFF: none */
    u16 second_drop_item;           /* 0x234 the item it may drop when the first one's roll fails, 0xFFFF: none */
    s8 drop_chance;                 /* 0x236 percent */
    s8 second_drop_chance;          /* 0x237 percent */
    EntityUnaligned64 unk238;       /* 0x238 */
    EntityParamsUnk240 melee_attacks[2]; /* 0x240 indexed by flag bits 58-59 */
    char melee_attack_third[0x88];  /* 0x350 the third melee attack record, reached past unk240 */
    EntityParamsUnk3D8Array ranged_attacks[3]; /* 0x3D8 the three ranged attack records: weight, bone, frame, shots */
    u8 floor_probe_depth;           /* 0x42C how far below its position a floor may be to count (EntityCollision_WalkAgainstMesh) */
    u8 ledge_behavior;              /* 0x42D what it does at a ledge (EntityCollision_WalkAgainstMap): 0 walks off, 1 steps down at most Entity.collision_height, else stops */
    u16 sound_id;                   /* 0x42E id passed to Sound_PlayFalloff for its animation cues */
    s16 pitch_limit_deg;            /* 0x430 limit in degrees on the pitch of its aim arcs */
    s16 yaw_limit_deg;              /* 0x432 limit in degrees on the yaw of its aim arcs */
    u16 action_frames;              /* 0x434 frames its actions run for, loaded into the work timers */
    s16 near_distance;              /* 0x436 the nearer of the two distances it keeps from the player */
    s16 far_distance;               /* 0x438 the farther of the two distances it keeps from the player */
    s16 unk43A;                     /* 0x43A */
    s16 unk43C;                     /* 0x43C */
    s16 unk43E;                     /* 0x43E */
    u16 variant_effect_ids[8];      /* 0x440 indexed by flag bits 60-62; passed to Effect_StartSpawner */
} EntityParams;

/* EntityParams.flags; the low six are copied, inverted, into Entity flag bits 22-27 */
#define ENTITY_PARAMS_FLAG_1                              0x0001 /* clears Entity flag bit 22, which only ScriptCmd_GetEntityFlags reads */
#define ENTITY_PARAMS_FLAG_COLLIDES_WITH_WALLS            0x0002 /* the map's wall strips push it out; clear sets Entity bit 23, which turns them off */
#define ENTITY_PARAMS_FLAG_COLLIDES_WITH_SECOND_WALLS     0x0004 /* the map's second wall strips push it out; clear sets Entity bit 24, which turns them off */
#define ENTITY_PARAMS_FLAG_COLLIDES_WITH_MAP_OBJECTS      0x0008 /* the map objects stop it; clear sets Entity bit 25, which turns them off */
#define ENTITY_PARAMS_FLAG_COLLIDES_WITH_ENTITIES         0x0010 /* EntityCollision_PushApart pushes it out of the other entities; clear sets Entity bit 26, which skips it */
#define ENTITY_PARAMS_FLAG_COLLIDES_WITH_PLAYER           0x0020 /* the player collides with it; clear sets Entity bit 27, which makes the player walk through it */
#define ENTITY_PARAMS_FLAG_40                             0x0040 /* with ENTITY_PARAMS_FLAG_200: its model scale stays exact, unrandomised (Entity_InitFromMapSpawns) */
#define ENTITY_PARAMS_FLAG_TURNS_IN_PLACE                 0x0080 /* Npc_Think's states 4 and 6 turn it to the player where it stands */
#define ENTITY_PARAMS_FLAG_MOTION_DOES_NOT_MOVE           0x0100 /* Entity flag bits11 (the Motion_Step mode) is 0: its motions do not move it, its move command does */
#define ENTITY_PARAMS_FLAG_200                            0x0200 /* with ENTITY_PARAMS_FLAG_40: its model scale stays exact, unrandomised (Entity_InitFromMapSpawns) */
#define ENTITY_PARAMS_FLAG_HIT_BODY_POINTS                0x0400 /* Attack hits its EntityParams.body_points, not its cylinder or sphere */
#define ENTITY_PARAMS_FLAG_STOPS_WHEN_STOOD_ON            0x0800 /* clears Entity.move_command while the player stands on it */
#define ENTITY_PARAMS_FLAG_NO_KNOCKBACK                   0x1000 /* a hit leaves Entity.knockback_speed at 0 */
#define ENTITY_PARAMS_FLAG_LOOK_AT_PART_TURNS             0x2000 /* EntityAi_LookAtPoint turns EntityModel.look_at_part to the target */
#define ENTITY_PARAMS_FLAG_HALF_MODEL_ALPHA               0x4000 /* Entity.model_alpha is 0x40 rather than 0x80 */

/* record index of the 0x88-byte records at +0x240, reached by stepping the whole params pointer
   (the match keeps the index scaling apart from the +0x240) */
#define ENTITY_PARAMS_UNK240_RECORD(params, index) (((EntityParams *)((char *)(params) + (index) * sizeof(EntityParamsUnk240)))->melee_attacks[0])

/* what EntityParams.anim_effect_offsets[n] leads to: something Effect_StartSpawner starts at a bone on an animation frame */
typedef struct {
    u16 effect_id;                  /* 0x00 passed to Effect_StartSpawner */
    u16 frame;                      /* 0x02 the animation frame */
    u8 bone;                        /* 0x04 passed to Model_GetSkinnedBonePoint (and the one after it) */
    s8 use_variant;                 /* 0x05 not 0: EntityParams.variant_effect_ids picks what to start */
} EntityParamsUnk130;

/* what EntityParams.anim_sound_offsets[n] leads to: something Sound_PlayFalloff starts on an animation frame */
typedef struct {
    u8 sound;                       /* 0x00 the sound Sound_PlayFalloff plays */
    u8 kind;                        /* 0x01 the kind passed to Sound_PlayFalloff */
    u16 frame;                      /* 0x02 the animation frame */
    u8 skip_while_ailing;           /* 0x04 not 0: left out while Entity.ailment_4_frames is set */
    char pad05[0x03];
} EntityParamsUnk1B0;

/* the first word of an EntityParams.ranged_attacks record, as the AI functions take it */
#define ENTITY_PARAMS_UNK3D8(params, index) (&(params)->ranged_attacks[index].record)
/* the three ranged attacks */
#define ENTITY_PARAMS_RANGED(params) ((params)->ranged_attacks)
#define ENTITY_PARAMS_RECORD(params, index) (&(params)->unk3D8[index])

/* EntityParams with the three ranged-attack records as a real array member: indexed this way the
   compiler adds the scaled index to the params pointer, base first (DarkKnight_PickAttack needs that) */
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
    u8 entity_type;                 /* 0x02 index into entity_type_handlers */
    u8 unk03;                       /* 0x03 */
} EntityParamsBytes;

/* the first word of EntityParams read whole (model_number, entity_type and the 0x03 bits together) */
typedef struct {
    u32 word;                       /* 0x00 */
} EntityParamsWord;

/* EntityParams.movement_kind read through the whole word, as the code tests it: 0 walks, 2 flies, 3 stays put */
#define ENTITY_PARAMS_MOVEMENT_MASK         0x6000000
#define ENTITY_PARAMS_MOVEMENT_WALK         0x0000000
#define ENTITY_PARAMS_MOVEMENT_1            0x2000000 /* not in param/chr.prm */
#define ENTITY_PARAMS_MOVEMENT_FLY          0x4000000
#define ENTITY_PARAMS_MOVEMENT_FIXED        0x6000000
#define ENTITY_PARAMS_MOVEMENT_KIND(params) (((EntityParamsWord *)(params))->word & ENTITY_PARAMS_MOVEMENT_MASK)

/* EntityParams.sphere_collision read through the whole word */
#define ENTITY_PARAMS_SPHERE_MASK           0x8000000
#define ENTITY_PARAMS_IS_SPHERE(params)     (((EntityParamsWord *)(params))->word & ENTITY_PARAMS_SPHERE_MASK)

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
       Entity.reaction_step is its step. It clears itself when it is done. */
    void (*reaction)(void);
    /* 0x10 the action the set-up function starts when the spawn has flag bit 58: wait for the
       player to come near, then get up */
    void (*dormant)(void);
    /* 0x14 the action EntityAi_Update starts when Entity.hit_points is 0 */
    void (*death)(void);
    /* 0x18 the action between the others: carries out the movement command in Entity.move_command.
       think ends by starting it; an action that has ended starts it when flag bit 5 is clear. */
    void (*move)(void);
    f32 turn_rate;                  /* 0x1C turn speed built up towards EntityParams.turn_limit */
    f32 look_part_angles[4];        /* 0x20 the look part's angles, updated in place each frame */
    f32 target_point[4];            /* 0x30 (Entity+0x110) a point to walk to (Entity.target points here) */
    f32 pose_blend_weight;          /* 0x40 weight Motion_BlendToPose blends the pose in with */
    f32 move_speed;                 /* 0x44 speed the movement builds up with EntityAi_Accelerate */
    f32 part_angle_x;               /* 0x48 a model part's x angle eased in animate (y speed in some) */
    f32 part_angle_y;               /* 0x4C a model part's y angle eased in animate (z speed in some) */
    f32 part_angle_z;               /* 0x50 a model part's z angle; heading or launch speed in places */
    f32 pitch_speed;                /* 0x54 y speed or pitch the type eases (a heading in some types) */
    f32 heading_speed;              /* 0x58 speed the type eases along the heading (a jump's y in some) */
    f32 keep_min_distance;          /* 0x5C the entity stays no nearer than this to the player */
    f32 keep_max_distance;          /* 0x60 the entity closes in while farther than this */
    char pad64[0x04];
    s16 pause_timer;                /* 0x68 frame countdown: attack pause, phase or animation length */
    s16 sub_timer;                  /* 0x6A one bit per entity 10..15 */
    s16 mode_timer;                 /* 0x6C frame countdown or value the think handler reloads */
    s16 reload_timer;               /* 0x6E countdown after which other work fields are reloaded */
    char pad70[0x08];
    s8 move_choice;                 /* 0x78 the way the entity closes in (0 turn in place, 1 back) */
    s8 turn_in_place;               /* 0x79 set while it must turn in place towards the player */
    char step_index;                /* 0x7A per-type step or state the think handler switches on */
    char action_flags;              /* 0x7B per-type flag byte: turn side, one-shot bits, amount */
    char action_counter;            /* 0x7C per-type counter, timer, half period or map object id */
    char pad7D[0x03];
    char motion_slot_0[0x08];       /* 0x80 (Entity+0x160) passed by address; an EntityAiMotionSlot */
    char motion_slot_1[0x08];       /* 0x88 (Entity+0x168) passed by address; an EntityAiMotionSlot */
} EntityAiState;

/* one of the two 8-byte slots EntityAi_StartSlotMotion fills */
typedef struct {
    u8 motion_bits;                 /* 0x00 motion index (5 bits) and flags, as EntityMotionSlotWord */
    u8 frames_left;                 /* 0x01 frames left, counted down by EntityAi_StepSlotMotion */
    s16 frame;                      /* 0x02 current frame */
    s32 frame_fixed;                /* 0x04 the frame times 256 */
} EntityAiMotionSlot;

/* Entity+0xE0 as the kinds handled at 0x1740A8..0x180000 use it */
typedef struct {
    void (*think)(struct Entity *entity, s32 run_now); /* 0x00 */
    char pad04[0x14];
    void (*move)(void);             /* 0x18 */
    s32 turn_rate;                  /* 0x1C turn speed built towards EntityParams.turn_limit */
    char pad20[0x10];
    f32 target_point[4];            /* 0x30 a point to move towards */
    char pad40[0x08];
    f32 point_x;                    /* 0x48 x of a stored point: the entity position or look point */
    f32 point_y;                    /* 0x4C y of that point (a part angle or heading in some types) */
    f32 point_z;                    /* 0x50 z of that point (a countdown or speed in some types) */
    char pad54[0x14];
    s16 action_frames;              /* 0x68 frames the current action has run (limit 211) */
    char pad6A[0x0E];
    s8 steer_to_target;             /* 0x78 0 while the action should steer towards the target */
    s8 climb_command;               /* 0x79 1/-1 climb, 2/-2 keep the height in bounds, 0 level out */
    s8 think_flag;                  /* 0x7A 0/1 flag or countdown the type's think handler keeps */
    char pad7B[0x05];
    EntityAiMotionSlot motion_slot_0;  /* 0x80 */
    EntityAiMotionSlot motion_slot_1;  /* 0x88 */
} EntityAiStateKind1740A8;

/* Entity+0xE0 as the flying kind set up by DarkBird_Init lays it out */
typedef struct {
    void (*think)(struct Entity *entity, s32 run_now); /* 0x00 */
    char pad04[0x14];
    void (*move)(void);             /* 0x18 */
    f32 turn_rate;                  /* 0x1C turn speed */
    char pad20[0x24];
    f32 side_speed;                 /* 0x44 speed sideways */
    f32 forward_speed;              /* 0x48 speed forwards */
    f32 pitch_speed;                /* 0x4C pitch speed */
    char pad50[0x28];
    s8 unk78;                       /* 0x78 */
    s8 climb_command;               /* 0x79 climb (above 0), dive (below 0) or level out (0) */
    char pad7A[0x06];
    EntityAiMotionSlot motion_slot_0;  /* 0x80 */
    EntityAiMotionSlot motion_slot_1;  /* 0x88 */
    char pad90[0x10];
} EntityAiStateKind1719F8;

/* Entity+0xE0 as the kind set up by ExpeditionSoldier_Init lays it out */
typedef struct {
    void (*think)(struct Entity *entity, s32 run_now); /* 0x00 */
    char pad04[0x14];
    void (*move)(void);             /* 0x18 */
    s32 turn_rate;                  /* 0x1C turn rate of the move command, cleared when one starts */
    f32 unk20[4];                   /* 0x20 */
    f32 target_point[4];            /* 0x30 where the entity walks to (Entity.target points here) */
    char pad40[0x3A];
    s8 map_object_42_state;         /* 0x7A state of map object spawn_info->unk42 */
    s8 map_object_43_state;         /* 0x7B state of map object spawn_info->unk43 */
    s8 map_object_to_operate_id;    /* 0x7C id of the map object to operate */
    char pad7D[0x23];
} EntityAiStateKind178358;

/* Entity+0xE0 as the kind set up by func_00150588 lays it out */
typedef struct {
    char pad00[0x1C];
    s32 unk1C;                      /* 0x1C */
} EntityAiStateKind150588;

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
    f32 turn_rate;                  /* 0x1C turn speed built towards EntityParams.turn_limit */
    f32 unk20[4];                   /* 0x20 */
    char pad30[0x10];
    f32 unk40;                      /* 0x40 */
    f32 unk44;                      /* 0x44 */
    char pad48[0x22];
    s16 climb_point_index;          /* 0x6A which of the 5 climb_points it heads for next */
    s16 unk6C;                      /* 0x6C */
    s16 reload_timer;               /* 0x6E countdown after which other work fields are reloaded */
    char pad70[0x08];
    s8 move_choice;                 /* 0x78 the way it closes in (0 or -1), as EntityAiState.move_choice */
    s8 turn_in_place;               /* 0x79 set while it must turn in place towards the player */
    s8 player_far;                  /* 0x7A 1 once the player is over radius+1100 away, 0 again under radius+600 */
    s8 sliding;                     /* 0x7B 1 while a slide is under way (the next choice slides to a stop) */
    s8 magic_alert_timer;           /* 0x7C 30 frames counted down after the player launches a magic shot */
    char pad7D[0x23];
} __attribute__((aligned(8))) EntityAiStateKind168160;

/* an 8-byte slot of the work area as the bit-fields the init functions set */
typedef struct {
    u8 index : 5;                   /* what EntityAi_StartSlotMotion looks up */
    u64 bit5 : 1;
    u64 bit6 : 1;
    u64 pad7 : 1;
    u8 frames_left;                 /* 0x01 frames left */
    s16 frame;                      /* 0x02 current frame */
    s32 frame_fixed;                /* 0x04 the frame times 256 */
} EntityAiMotionSlotBits;

/* Entity+0xE0 as the kinds set up by func_00151D30 and its neighbours use it */
typedef struct {
    char pad00[0x80];
    EntityAiMotionSlotBits motion_slot_0; /* 0x80 */
    EntityAiMotionSlotBits motion_slot_1; /* 0x88 */
} EntityAiStateKind151D30;

/* Entity+0xE0 as the kind set up by DarkSorcerer_Init lays it out */
typedef struct {
    char pad00[0x1C];
    f32 turn_rate;                  /* 0x1C turn speed built towards EntityParams.turn_limit */
    char pad20[0x24];
    f32 move_speed;                 /* 0x44 speed the movement builds up with EntityAi_Accelerate */
    f32 vertical_speed;             /* 0x48 up/down speed eased by the up and down commands */
    f32 forward_speed;              /* 0x4C forward speed eased by the forward and back commands */
    char pad50[0x50];
} EntityAiStateKind16BF98;

/* Entity+0xE0 as the kind set up by func_0015F8C0 lays it out. The area is
   8-byte aligned: only then is "unk68 == 0 && unk6A == 0" one lwu. */
typedef struct {
    char pad00[0x68];
    s16 vanish_frames;              /* 0x68 frames left while it is vanished (draw flag 0x800, params.action_frames to start) */
    s16 vanish_cooldown;            /* 0x6A frames before it may vanish again (params.far_distance to start) */
    s16 think_timer;                /* 0x6C countdown reloaded from params.unk43A */
    char pad6E[0x0A];
    s8 unk78;                       /* 0x78 */
    s8 unk79;                       /* 0x79 */
    s8 rising;                      /* 0x7A 1 while it climbs (above 45 over the floor) until under 35 */
    s8 dipped_low;                  /* 0x7B set once it has dropped under 35 over the floor */
    s8 combat_mode;                 /* 0x7C which combat think state 2 runs: 0 normal, 1 circle near, else keep away */
    s8 fading_out;                  /* 0x7D set while it fades out before vanishing */
    s8 fade_frames;                 /* 0x7E frames of the fade-out left (from 20) */
    char pad7F[0x21];
} __attribute__((aligned(8))) EntityAiStateKind15F8C0;

/* Entity+0xE0 as the kind set up by func_00162BC8 lays it out */
typedef struct {
    char pad00[0x30];
    f32 effect_angles[4];           /* 0x30 pitch and yaw passed to EntityEvent_StartEffectAtPoint */
    char pad40[0x2E];
    s16 unk6E;                      /* 0x6E */
    char pad70[0x10];
    f32 first_aim[4];               /* 0x80 pitch and yaw of the first aimed effect */
    f32 second_aim[4];              /* 0x90 pitch and yaw of the second aimed effect */
} EntityAiStateKind162BC8;

/* Entity+0xE0 as the kind set up by Onyx_Init lays it out */
typedef struct {
    char pad00[0x68];
    s16 action_weights[3];          /* 0x68 weights of the first three actions (see Onyx_PickAttack) */
    char pad6E[0x32];
} EntityAiStateKind17FC60;

/* Entity+0xE0 as the kind set up by func_00131390 lays it out */
typedef struct {
    void (*think)(struct Entity *entity, s32 run_now); /* 0x00 */
    char pad04[0x14];
    void (*move)(void);             /* 0x18 */
    f32 turn_rate;                  /* 0x1C turn speed used by EntityAi_TurnToHeading */
    char pad20[0x4A];
    s16 player_cast_timer;          /* 0x6A counts down; reloaded from EntityParams.action_frames */
    char pad6C[0x0E];
    s8 unk7A;                       /* 0x7A */
    s8 alternate_anim_flag;         /* 0x7B picks animation 6 or 7, flipped each time it plays */
    s8 distance_band;               /* 0x7C distance band to the player, 0 (far) to 4 (near) */
    s8 distance_band_timer;         /* 0x7D frames until unk7C is picked again */
    s8 afterimage_active;           /* 0x7E set by the actions that play animation 6 or 7 */
    char pad7F[0x21];
} EntityAiStateKind131390;

/* Entity+0xE0 as the kind set up by func_0012ACA0 lays it out */
typedef struct {
    char pad00[0x1C];
    f32 turn_rate;                  /* 0x1C turn speed built towards EntityParams.turn_limit */
    char pad20[0x20];
    f32 neck_bend[7];               /* 0x40 how far each of the five neck joints (model parts 30 to 34) is bent, [2] to [6] */
    char pad5C[0x44];
} EntityAiStateKind12ACA0;

/* what Entity.params points at (the entity's controller / type data), see ENTITY_CONTROLLER.
   EntityParams above is a second view of the same block; the two are to be unified. */
typedef struct {
    char pad00[0x10];
    u16 flags;                      /* 0x10 ENTITY_PARAMS_FLAG_ bits */
    u16 state_timer_reload;         /* 0x12 reload value for Entity.timer */
    char pad14[0x04];
    u16 attack_range;               /* 0x18 how near the player has to be for it to notice and attack */
    u16 attack_height_band;         /* 0x1A height band around its middle the player is seen in */
    f32 attack_arc;                 /* 0x1C widest angle off its heading the player is seen in, radians */
    char pad20[0x3C];
    f32 turn_limit;                 /* 0x5C */
    char pad60[0x64];
    u8 motion_blend_frames;         /* 0xC4 frames Motion_Start blends a new animation in over */
    u8 respawn_effect_bone;         /* 0xC5 bone the effect starts at when the entity respawns */
    u16 respawn_effect_id;          /* 0xC6 respawn effect id, 0xFFFF: none */
    char padC8[0x368];
    s16 pitch_limit_deg;            /* 0x430 limit in degrees on the pitch of its look-at arcs */
    char pad432[0x02];
    s16 action_frames;              /* 0x434 frames its actions run for, loaded into work timers */
    char pad436[0x02];
    s16 far_distance;               /* 0x438 the farther of the two distances it keeps from the player */
    s16 entity_group_index;         /* 0x43A index of its group entry (entity_group_state, origins) */
} EntityController;

/* EntityController.flags: the type needs two pool blocks (0x100000 bytes) */
#define CONTROLLER_FLAG_TWO_BLOCKS  0x8000

/* Entity.move_command bits, one per move, along the model's own axes (it faces -z, its right is -x) */
#define MOVE_COMMAND_FORWARD        0x1     /* moves along the model's facing (-z) at the move speed, animation 3 */
#define MOVE_COMMAND_BACK           0x2     /* moves along +z, animation 5 */
#define MOVE_COMMAND_STEP_LEFT      0x4     /* moves along +x, its left, animation 6 */
#define MOVE_COMMAND_STEP_RIGHT     0x8     /* moves along -x, its right, animation 7 */
#define MOVE_COMMAND_DOWN           0x10    /* moves along -y, animation 8 */
#define MOVE_COMMAND_UP             0x20    /* moves along +y, animation 9 */
#define MOVE_COMMAND_RUN            0x40    /* doubles the move speed and picks the run animation 4 */
#define MOVE_COMMAND_TURN_LEFT      0x100   /* turns towards -yaw: the turn rate steps down to -turn_limit */
#define MOVE_COMMAND_TURN_RIGHT     0x200   /* turns towards +yaw */
#define MOVE_COMMAND_TURN_FAST      0x400   /* doubles this frame's turn (some handlers: for the run bit too) */
#define MOVE_COMMAND_TURN_TO_TARGET 0x800   /* AcidCrawler_Move turns to Entity.target (EntityAi_TurnToPoint) instead */
#define MOVE_COMMAND_ANIMATION_26   0x1000  /* plays animation 26 instead of standing: Npc watching a player in front */
#define MOVE_COMMAND_ANIMATION_27   0x2000  /* plays animation 27: Npc watching a player to its left */
#define MOVE_COMMAND_ANIMATION_28   0x4000  /* plays animation 28: Npc watching a player to its right */
#define MOVE_COMMAND_8000           0x8000  /* AcidCrawler_Move plays animation 17; no C code sets it */
/* groups of the direction and turn bits the move handlers test together */
#define MOVE_COMMAND_FORWARD_BACK   0x3     /* the forward/back axis */
#define MOVE_COMMAND_SIDEWAYS       0xC     /* the step left/right axis */
#define MOVE_COMMAND_UP_DOWN        0x30    /* the up/down axis */
#define MOVE_COMMAND_DIRECTIONS     0x3F    /* all six direction bits: the command moves the entity */
#define MOVE_COMMAND_TURNS          0x300   /* either turn bit */

typedef struct Entity {
    EntityModel *model;             /* 0x00 position at +0xC0; ENTITY_MODEL() is the ModelInstance view */
    char pad04[0x08];
    EntityParams *params;           /* 0x0C ENTITY_CONTROLLER() is the EntityController view */
    EntitySpawnInfo *spawn_info;    /* 0x10 */
    void *map_record;               /* 0x14 the map record (MapRecord *) the entity stands on */
    struct Entity *prev;            /* 0x18 */
    struct Entity *next;            /* 0x1C */
    u16 ailment_1_frames;           /* 0x20 a timer, set to 1800 by EntityCollision_ApplySurfaceEffect */
    char pad22[0x02];
    u16 ailment_2_frames;           /* 0x24 not 0: attacks reach half as far (EntityAi_PickMeleeAttack) */
    u16 ailment_tint_index;         /* 0x26 1-4: which of the timers ailment_1_frames, ailment_2_frames, ailment_3_frames, unk2A tints the model (Entity_DrawAll) */
    u16 ailment_3_frames;           /* 0x28 frames the third entity ailment lasts; slows its moves */
    u16 ailment_4_frames;           /* 0x2A frames the fourth entity ailment lasts */
    u16 hit_points;                 /* 0x2C hit points; at 0 the death action starts */
    u16 last_hit_damage;            /* 0x2E capped at EntityParams.hit_points; Entity.knockback_speed is set from it (Entity_UpdateAll) */
    f32 collision_radius;           /* 0x30 radius used against the map mesh and map objects */
    f32 collision_height;           /* 0x34 half-height used against the map mesh */
    f32 radius;                     /* 0x38 */
    f32 height;                     /* 0x3C */
    f32 knockback_damping;          /* 0x40 EntityParams.body_volume scaled three ways (a volume or weight?) */
    f32 body_radius;                /* 0x44 EntityParams.unk04 scaled */
    f32 floor_lift;                 /* 0x48 how far above the floor it is kept (see EntityCollision_SphereAgainstMap) */
    f32 knockback_speed;            /* 0x4C speed the knockback pushes the entity along at */
    s32 update_func;                /* 0x50 the type's per-frame update (entity_type_handlers) */
    void (*handler)(struct Entity *entity); /* 0x54 the current action; EntityAi_Update calls it every frame while flag bit 6 is set and no reaction is */
    void (*prev_handler)(struct Entity *entity); /* 0x58 the action before the last EntityAi_StartAction */
    void *target;                   /* 0x5C */
    f32 distance_to_player_3d;      /* 0x60 distance to the player, height included */
    f32 distance_to_player;         /* 0x64 */
    f32 heading_to_player;          /* 0x68 heading (yaw) from the entity to the player */
    f32 knockback_heading;          /* 0x6C the heading Entity.knockback_speed pushes it along (Entity_UpdateAll) */
    f32 prev_position[4];           /* 0x70 the model's position before this frame's move */
    u16 hit_cooldown_frames;        /* 0x80 frames after a hit during which hits are ignored */
    u16 stun_frames;                /* 0x82 frames the entity's whole update is skipped */
    f32 move_delta_x;               /* 0x84 this frame's x move, added to the position */
    f32 move_delta_y;               /* 0x88 this frame's y move, added to the position */
    f32 move_delta_z;               /* 0x8C this frame's z move, added to the position */
    f32 turn_delta_pitch;           /* 0x90 this frame's x rotation, added to the model's */
    f32 turn_delta_yaw;             /* 0x94 this frame's y rotation (heading), added to the model's */
    f32 turn_delta_roll;            /* 0x98 this frame's z rotation, added to the model's */
    u16 timer;                      /* 0x9C frames left in state 1 (EntityAi_StepState) */
    u16 choice_change_frame;        /* 0x9E frame counter when the current combat choice was picked */
    u16 action_timer;               /* 0xA0 a timer for the think functions and the actions; EntityAi_Update counts it down */
    u8 attack_delay_frames;         /* 0xA2 frames before the next attack: set when an attack or a reaction ends (EntityAi_SetAttackDelay), counted down by EntityAi_Update */
    u8 hit_flash_frames;            /* 0xA3 frames the model is drawn red after a hit */
    f32 wall_push[3];               /* 0xA4 a push applied while flag bit 45 or 46 is set */
    f32 entity_push[3];             /* 0xB0 a push applied while flag bit 48 is set */
    u8 choice_weights[4];           /* 0xBC weights of the four combat choices, 0x40 to 0xC0 */
    u8 attack_weights[6];           /* 0xC0 weights of the six attacks, 0x40 to 0xC0 */
    u16 busy_flags;                 /* 0xC6 the running action or reaction, ENTITY_BUSY_ bits; think waits while set */
    u16 move_command;               /* 0xC8 this frame's MOVE_COMMAND_ bits for the move action; EntityAi_Update clears it */
    s8 action_step;                 /* 0xCA step of the current action; EntityAi_StartAction puts it back to 0 */
    s8 reaction_step;               /* 0xCB step of the reaction (EntityAiState.reaction); EntityAi_SetReaction puts it back to 0 */
    /* 0xCC what the entity knows of the player (EntityAi_StepState): 0 nothing, 2 the player is noticed
       (flag bit 33), 1 it was and Entity.timer still runs. Scripts and some types set other values. */
    s8 state;
    s8 combat_choice;               /* 0xCD which of the four combat choices is running */
    char prev_state;                /* 0xCE state before the last change */
    s8 prev_combat_choice;          /* 0xCF combat choice before the last change */
    u8 prev_motion_id;              /* 0xD0 the motion of the animation before (Motion_Start) */
    s8 animation_index;             /* 0xD1 the current animation (Motion_Start) */
    char sound_bank;                /* 0xD2 copied from its pool block */
    char pool_slot_index;           /* 0xD3 pool block index, -1: none */
    char work_area_index;           /* 0xD4 work area index, -1: none */
    char group_id;                  /* 0xD5 */
    u8 model_alpha;                 /* 0xD6 the model's alpha, 0x80 (0x40 for some kinds) */
    u8 spawn_index;                 /* 0xD7 spawn record index; staggers the periodic checks */
    u64 flags;                      /* 0xD8 */
    EntityAiState ai_state;        /* 0xE0 the AI handlers and their work fields */
    char pad170[0x10];              /* 0x170 the record is 0x180 bytes */
} Entity;

/* Entity.busy_flags bits; 0x1000 and up are actions of the entity type's own */
#define ENTITY_BUSY_ATTACK          0x1     /* an attack (EntityAi_MeleeAttack, EntityAi_RangedAttack) */
#define ENTITY_BUSY_REACTION_LIGHT  0x2     /* the reaction to a light hit (EntityAi_ReactionLight, EntityAi_ReactionRewind) */
#define ENTITY_BUSY_REACTION_HEAVY  0x4     /* the reaction to a heavy hit (EntityAi_ReactionHeavy) */
#define ENTITY_BUSY_DODGE           0x8     /* a dodge (EntityAi_Dodge) */
#define ENTITY_BUSY_GUARD           0x10    /* a guard (EntityAi_Guard) */
#define ENTITY_BUSY_DORMANT         0x20    /* the dormant action (EntityAi_Dormant) */
#define ENTITY_BUSY_DEATH           0x40    /* the death action (EntityAi_Death) */
#define ENTITY_BUSY_STAND_ONCE      0x200   /* EntityAi_StandOnce: animation 0 (standing) once */
#define ENTITY_BUSY_1000            0x1000  /* an action of the type's own */
#define ENTITY_BUSY_2000            0x2000  /* an action of the type's own */
#define ENTITY_BUSY_4000            0x4000  /* an action of the type's own */

/* Entity.flags bits. The code sets them as masks, as shifts or through ENTITY_FLAG_BITS. */
#define ENTITY_FLAG_ACTIVE_LIST              0x1                   /* on the active entities list: Entity_LinkActive sets it, Entity_UnlinkActive clears it */
#define ENTITY_FLAG_LIVE                     0x2                   /* alive: attacks, pushes and Effect_IsEntityUntargetable skip an entity without it */
#define ENTITY_FLAG_DYING_MASK               0xC                   /* 0 alive; 1 when a death action starts, 2 as it fades, 3 once it is off the world */
#define ENTITY_FLAG_DYING_1                  0x4                   /* that field at 1, which the start of a death action sets */
#define ENTITY_FLAG_NO_RESPAWN               0x10                  /* EntityAi_ShouldRespawn returns FALSE while set; Entity_ClearFlag10InSlot clears it */
#define ENTITY_FLAG_OWN_THINK                0x20                  /* EntityAi_Update runs the entity type's own think handler while set (set for every spawn) */
#define ENTITY_FLAG_HANDLER_RUNS             0x40                  /* EntityAi_Update calls Entity.handler every frame while set */
#define ENTITY_FLAG_RESPAWNABLE              0x80                  /* the spawn may come back: EntityAi_ShouldRespawn needs it, the spawn flags set it */
#define ENTITY_FLAG_100                      0x100                 /* EntityEvent_Respawn sets it with the live bits, Entity_SetSpawnedFlags clears it */
#define ENTITY_FLAG_LOOK_AT_TARGET           0x200                 /* the head follows the target (EntityAi_LookAtPlayer's look_at_target) */
#define ENTITY_FLAG_400                      0x400                 /* set with bit 9 before an attack is picked, cleared by the on_hit handlers */
#define ENTITY_FLAG_MOTION_STEP_MODE_MASK    0x3800                /* the mode (0 to 2) Motion_Step and Motion_StepSlot are given */
#define ENTITY_FLAG_MOTION_STEP_MODE_1       0x800                 /* that mode at 1, which the start of every action sets */
#define ENTITY_FLAG_ANIMATES                 0x4000                /* the animate handler blends or steps the motion while set */
#define ENTITY_FLAG_8000                     0x8000                /* read by Motion_Step: the motion stands on its first frame */
#define ENTITY_FLAG_NOT_TARGETABLE           0x10000               /* Effect_IsEntityUntargetable calls it flagged out: an entity holding an animation has it */
#define ENTITY_FLAG_DRAW_AFTERIMAGE          0x20000               /* Entity_DrawAll draws the pool block's model again for the entities that have it */
#define ENTITY_FLAG_40000                    0x40000               /* set and cleared by script commands 257 and 258, and by the unseen actions */
#define ENTITY_FLAG_80000                    0x80000               /* set once the ranged attack is past its launch frame; a light hit reads it to rewind */
#define ENTITY_FLAG_100000                   0x100000              /* an attack skips an entity with it set (the target loop of the attack requests) */
#define ENTITY_FLAG_200000                   0x200000              /* Effect_IsEntityUntargetable excludes it; set with bits 18 and 20 while unseen */
#define ENTITY_FLAG_400000                   0x400000              /* set when EntityParams.flags bit 0 is clear */
#define ENTITY_FLAG_NO_WALL_COLLISION        0x800000              /* set when EntityParams.flags bit 1 (ENTITY_PARAMS_FLAG_COLLIDES_*) is clear */
#define ENTITY_FLAG_NO_SECOND_WALL_COLLISION 0x1000000             /* set when EntityParams.flags bit 2 (ENTITY_PARAMS_FLAG_COLLIDES_*) is clear */
#define ENTITY_FLAG_NO_MAP_OBJECT_COLLISION  0x2000000             /* set when EntityParams.flags bit 3 (ENTITY_PARAMS_FLAG_COLLIDES_*) is clear */
#define ENTITY_FLAG_NO_ENTITY_COLLISION      0x4000000             /* set when EntityParams.flags bit 4 (ENTITY_PARAMS_FLAG_COLLIDES_*) is clear */
#define ENTITY_FLAG_NO_PLAYER_COLLISION      0x8000000             /* set when EntityParams.flags bit 5 (ENTITY_PARAMS_FLAG_COLLIDES_*) is clear */
#define ENTITY_FLAG_NO_FLOOR_LIFT            0x10000000            /* the collision leaves Entity.floor_lift out of the height while set */
#define ENTITY_FLAG_20000000                 0x20000000            /* a value the entity types pass to their think functions (Npc, Bell, DarkOne) */
#define ENTITY_FLAG_MAP_RECORD_INACTIVE      0x40000000            /* set while the map record under the entity is not active (Entity_IsMapRecordActive) */
#define ENTITY_FLAG_TURN_IN_PLACE            0x80000000ULL         /* set, the think functions would only turn on the spot instead of walking */
#define ENTITY_FLAG_PLAYER_IN_REACH          0x100000000ULL        /* EntityAi_SensePlayer sets it while the player is inside the reach, arc and height */
#define ENTITY_FLAG_PLAYER_NOTICED           0x200000000ULL        /* EntityAi_SensePlayer sets it (noticed) by a chance that grows as the player comes nearer */
#define ENTITY_FLAG_MOTION_ENDED             0x400000000ULL        /* set from Motion_Step's result: the motion has blended in and played to its end */
#define ENTITY_FLAG_GUARDING                 0x800000000ULL        /* the guard bonuses are added to the defences while set */
#define ENTITY_FLAG_HIT_STRENGTH_MASK        0x3000000000ULL       /* 1 below 33.3334 percent of EntityParams.hit_points, else 2 */
#define ENTITY_FLAG_HIT_SOURCE_MASK          0xC000000000ULL       /* 1 the player's weapon, 2 the player's magic, 0 anything else */
#define ENTITY_FLAG_RAISE_ATTACK_WEIGHT      0x20000000000ULL      /* Attack_HitPlayer sets it with a weapon hit; EntityAi_Update raises that weight once */
#define ENTITY_FLAG_RAISE_RANGED_WEIGHT      0x40000000000ULL      /* Attack_HitPlayer sets it with a magic hit; EntityAi_Update raises the ranged weights once */
#define ENTITY_FLAG_HIT_BY_PLAYER            0x80000000000ULL      /* set when the player's attack hits the entity; it wakes the dormant entity types */
#define ENTITY_FLAG_ON_FLOOR                 0x100000000000ULL     /* set where the collision puts the entity on the floor */
#define ENTITY_FLAG_WALL_PUSH                0x200000000000ULL     /* set where the collision pushed the entity out of a wall */
#define ENTITY_FLAG_400000000000             0x400000000000ULL     /* set with bit 45; Motion_Step reads it */
#define ENTITY_FLAG_OBJECT_PUSH              0x800000000000ULL     /* set where a map object pushed the entity */
#define ENTITY_FLAG_ENTITY_PUSH              0x1000000000000ULL    /* set where another entity pushed it (Entity.entity_push) */
#define ENTITY_FLAG_PLAYER_PUSH              0x2000000000000ULL    /* set where the player pushed it */
#define ENTITY_FLAG_ON_TOP                   0x4000000000000ULL    /* set when the collision result is "stands on it" (hit 2) */
#define ENTITY_FLAG_SKIP_FLOOR_MASK          0x4000000400000L      /* the collision then leaves the mesh's floor strips out (bits 22 and 50) */
#define ENTITY_FLAG_LIFT_UP                  0x8000000000000ULL    /* EntityAi_UpdateCollision raises Entity.floor_lift to 40 while set */
#define ENTITY_FLAG_LEDGE_STOP               0x10000000000000ULL   /* set where a ledge made the entity stop at its previous position */
#define ENTITY_FLAG_FAR_FROM_SPAWN           0x20000000000000ULL   /* EntityAi_CheckFarFromSpawn(3D) sets it beyond EntitySpawnInfo.wander_radius */
#define ENTITY_FLAG_MELEE_0_HIT              0x40000000000000ULL   /* the first melee check already hit (EntityAi_CheckMeleeHit) */
#define ENTITY_FLAG_MELEE_1_HIT              0x80000000000000ULL   /* the second melee check already hit */
#define ENTITY_FLAG_MELEE_2_HIT              0x100000000000000ULL  /* the third melee check already hit */
#define ENTITY_FLAG_RANGED_ATTACK            0x200000000000000ULL  /* the attack EntityAi_PickAttack picked is a ranged one */
#define ENTITY_FLAG_ATTACK_INDEX_MASK        0xC00000000000000ULL  /* the ranged record the attack uses */
#define ENTITY_FLAG_FLOOR_MATERIAL_MASK      0x7000000000000000ULL /* the floor material Collision_GetFloorMaterial read */
#define ENTITY_FLAG_SIGHT_BLOCKED            0x8000000000000000ULL /* something stands between the entity and the player (EntityAi_SensePlayer) */

/* Entity with the work area at 0xE0 as plain bytes: the init functions clear it
   one byte at a time, and only an array member gives their code */
#define ENTITY_WORK_AREA_SIZE 0xA0

typedef struct {
    char pad00[0xE0];
    u8 work_bytes[ENTITY_WORK_AREA_SIZE]; /* 0xE0 */
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

/* Entity.flags with bits 36-37 and 38-39 as two fields, the way Attack_HitEntity sets them on a hit */
typedef struct {
    u64 pad0 : 36;
    u32 hit_strength : 2;           /* bits 36-37: 1 below 33.3334 percent of EntityParams.hit_points, else 2 */
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

/* the block at player_state: the player (position first). Files declare
   `extern PlayerData player_state;` themselves: older files still use an array. */
typedef struct {
    f32 position[4];                /* 0x00 */
    char pad10[0x04];
    f32 heading;                    /* 0x14 the player's heading angle, PlayerState.rotation.y */
    char pad18[0x1EC];
    Entity *stood_on_entity;        /* 0x204 the entity the player was put on top of when climbing */
    char pad208[0x4A];
    u16 map_record_index;           /* 0x252 picks the MapSlot record the player is in */
    char pad254[0x05];
    u8 magic_shot_launched;         /* 0x259 set when a cast magic launches a shot, cleared each frame */
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
   EntityFrameTable (AncientKing_RangedAttack0, AncientKing_LaunchShotsNearPlayer) */
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

/* one of the 0x80-byte records MapSlot.records (map/map.h) points at */
typedef struct {
    char pad00[0x1C];
    f32 push_heading;               /* 0x1C the heading its floor pushes the player along (Player_Move) */
    char pad20[0x0C];
    u16 push_max_speed;             /* 0x2C the most speed the push gives, 0: no push */
    u16 push_acceleration;          /* 0x2E how much speed the push adds per frame, 0: no push */
    char pad30[0x02];
    u16 collision_mesh_index;       /* 0x32 0xFFFF: unused */
    char pad34[0x02];
    u8 draw_block_index;            /* 0x36 which MapSlotUnk0C block the entities standing on it are drawn with */
    u8 light_block_index;           /* 0x37 which MapSlotUnk10Words block they are drawn with */
    u64 record_flags;               /* 0x38 flags; 0x4000000: EntityEvent_StartFrameEffects does its work */
    char pad40[0x0E];
    u8 prop_id;                     /* 0x4E which prop of the map it is, 0: none */
    char pad4F[0x11];
    f32 lowest[4];                  /* 0x60 lowest x, y, z it covers */
    f32 highest[4];                 /* 0x70 highest x, y, z it covers */
} MapRecord;

/* one of the 0x80-byte records MapSlot.light_blocks (map/map.h) points at, indexed by byte 3 of
   EntitySpawnInfo.spawn_flags */
typedef struct {
    char pad00[0x04];
    f32 light1_direction_x;         /* 0x04 x of light 1's direction in this light block */
    char pad08[0x0C];
    f32 light1_direction_y;         /* 0x14 y of light 1's direction in this light block */
    char pad18[0x0C];
    f32 light1_direction_z;         /* 0x24 z of light 1's direction in this light block */
    char pad28[0x28];
    f32 light1_red;                 /* 0x50 red of light 1's colour, 0 to 1 */
    f32 light1_green;               /* 0x54 green of light 1's colour, 0 to 1 */
    f32 light1_blue;                /* 0x58 blue of light 1's colour, 0 to 1 */
    char pad5C[0x14];
    f32 ambient_red;                /* 0x70 red of the block's ambient colour (its fourth colour) */
    f32 ambient_green;              /* 0x74 green of the block's ambient colour */
    f32 ambient_blue;               /* 0x78 blue of the block's ambient colour */
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

/* Entity.move_command (the movement command bits) read through a union: the value then does not survive a
   store of another type, and Scorpion_Move reads the field again after it (see ENTITY_FLAGS) */
typedef union {
    u16 all;
} EntityCommand;
#define ENTITY_COMMAND(entity) (((EntityCommand *)&(entity)->move_command)->all)

/* entity_group_state[EntityController.entity_group_index]: state shared by a group of entities */
typedef struct {
    char unk00;                     /* 0x00 */
    char unk01;                     /* 0x01 */
    char member_died;               /* 0x02 set when a member dies */
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

/* one block of the entity pool, entity_pool_slots[5] (Entity_ResetAllLists.c's
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
    s16 entity_count;               /* 0x04 entities using it */
    u8 reuse_delay;                 /* 0x06 frames before the slot can be reused, 2 once its last entity leaves */
    u8 sound_bank;                  /* 0x07 copied to Entity.sound_bank */
    int age;                        /* 0x08 frame_counter when last released; the oldest is reused first */
    int base_address;               /* 0x0C */
} EntityPoolSlot;

extern EntityPoolSlot entity_pool_slots[ENTITY_POOL_SLOT_COUNT];

/* the first 0x20 bytes of a loaded type file (file_load_buffer), kept in entity_type_file_header while the file's
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
    char *model_offset;             /* 0x04 handed to Model_SetUpSkinned with the entity's model. The two are
                                       pointer typed: only then does Entity_TryActivate load them after base_address */
    char *motion_offset;            /* 0x08 handed to ModelMotion_LoadFile */
    s32 texture_data;               /* 0x0C becomes ModelInstance.texture_data */
    s32 pool_model_offset;          /* 0x10 the slot's own model (EntityPool_BuildBlockModel), 0: none */
} EntityPoolData;

/* a node of the linked_point_pool pool (0x200 of them, chained through prev/next): a point
   that eases toward a target over a duration or follows another vector. The two
   active lists and free list are owned by model/linked_point.c. */
typedef struct LinkedPoint {
    Vector4 position;               /* 0x00 w is kept when following; w is 1 / duration while easing */
    Vector4 target;                 /* 0x10 w is 1.0 */
    Vector4 *follow;                /* 0x20 NULL: stays put */
    f32 progress_scale;             /* 0x24 1 / duration squared */
    f32 duration;                   /* 0x28 */
    u16 handle;                     /* 0x2C index in the pool */
    u8 active;                      /* 0x2E */
    u8 list_index;                  /* 0x2F which linked_point_heads list it is on */
    u8 kind_a;                      /* 0x30 */
    s8 kind_b;                      /* 0x31 */
    s16 unk32;                      /* 0x32 */
    char pad34[0x04];
    struct LinkedPoint *prev;       /* 0x38 */
    struct LinkedPoint *next;       /* 0x3C */
} LinkedPoint;

#define LINKED_POINT_COUNT 0x200

extern LinkedPoint linked_point_pool[LINKED_POINT_COUNT];

/* the scratchpad as the movement handlers of entity types 65 and 71 use it: a velocity and the
   matrix that turns it to the model's heading */
#define ENTITY_SCRATCH_VECTOR ((f32 *)0x70000000)
#define ENTITY_SCRATCH_MATRIX ((f32 *)0x70000010)

/* upstream's typed views of the two blocks an Entity points at */
#define ENTITY_MODEL(entity) ((ModelInstance *)(entity)->model)
#define ENTITY_CONTROLLER(entity) ((EntityController *)(entity)->params)

/* a whole model instance (0x1C0 bytes) as func_00131390 copies it to fire_lord_model[0] and [1]
   (afterimages drawn at half alpha): the game's struct is 8-byte aligned, the copy moves
   doublewords */
typedef struct {
    ModelInstance model;
} __attribute__((aligned(8))) ModelInstanceBlock;

extern Entity *active_entities_head;          /* head of the active entities' list (list C of entity/entity_pool.c) */

float Entity_SpawnDistanceToPlayer(Entity *entity);    /* distance to the player */

/* what ladder_object refers to: a record with a kind id at 0x30 (Ladder_PlayStepSound) */
typedef struct {
    char pad00[0x30];
    u16 kind;                       /* 0x30 kind id, picks the step sound (Ladder_PlayStepSound) */
    char pad32[0x1E];
    s16 step_count;                 /* 0x50 how many steps there are between the two end points (Ladder_Start) */
    s16 frames_per_step;            /* 0x52 frames one step takes */
} ReferencedObject;

/* ladder_object: the map object the player is climbing */
typedef struct {
    ModelInstance *model;           /* 0x00 its get_point gives the two end points (Ladder_Start) */
    char pad04[0x04];
    ReferencedObject *definition;   /* 0x08 the object's definition */
    char pad0C[0x04];
    s32 flags;                      /* 0x10 flags; 0x10000 is tested by Ladder_WaitOnRung */
} ObjectReference;

void Entity_ResetAllLists(void);
void Entity_FreeAllInGroup(int group_id);
void Entity_DespawnOutOfRange(void);

#endif
