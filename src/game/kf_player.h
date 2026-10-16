/* The player: the state block at player_state and the level/experience tables. */
#ifndef KF_PLAYER_H
#define KF_PLAYER_H

#include "common.h"
#include "kf_model.h"
#include "kf_game.h"
#include "kf_item.h"

/* the slots of PlayerState.equipment, in the order of the ini presets' equip_ keys */
#define EQUIP_SHIELD    0
#define EQUIP_HELM      1
#define EQUIP_ARMOR     2
#define EQUIP_GAUNTLET  3
#define EQUIP_BOOT      4
#define EQUIP_RING      5
#define EQUIP_BRACELET  6
#define EQUIP_AMULET    7

/* the whole Ancient set is held and worn: sword, shield, helmet, armor, gloves and greaves */
#define PLAYER_WEARS_ANCIENT_SET(player)                                                                          \
    ((player)->weapon == ITEM_WEAPON_INDEX(ITEM_ANCIENT_SWORD)                                                    \
     && (player)->equipment[EQUIP_SHIELD] == ITEM_WORN_INDEX(ITEM_ANCIENT_SHIELD)                                 \
     && (player)->equipment[EQUIP_HELM] == ITEM_WORN_INDEX(ITEM_ANCIENT_HELMET)                                   \
     && (player)->equipment[EQUIP_ARMOR] == ITEM_WORN_INDEX(ITEM_ANCIENT_ARMOR)                                   \
     && (player)->equipment[EQUIP_GAUNTLET] == ITEM_WORN_INDEX(ITEM_ANCIENT_GLOVES)                               \
     && (player)->equipment[EQUIP_BOOT] == ITEM_WORN_INDEX(ITEM_ANCIENT_GREAVES))

/* six values of a PlayerItemDef that the player takes over */
typedef struct {
    u16 values[6];                  /* 0x00 */
} PlayerItemRates;

/* nine values copied from the player into a shot */
typedef struct {
    s16 values[9];                  /* 0x00 */
} PlayerShotStats;

/* the 0x30-byte slot Shot_Alloc returns, as the player's magic fills it */
typedef struct {
    u8 is_magic_shot;               /* 0x00 1 when the shot is a magic's, 0 when it is the weapon's */
    char pad01[0x01];
    u16 strength_gauge;             /* 0x02 PlayerState.power_gauge */
    void *attack_request;           /* 0x04 the Unk1B1470Request (kf_entity.h) of a weapon's shot, in D_0057A350 */
    PlayerShotStats unk08;          /* 0x08 PlayerState.unk7E */
    char pad1A[0x06];
    u16 player_power;               /* 0x20 PlayerState.power */
    u16 player_magic;               /* 0x22 PlayerState.magic */
    s16 weapon_index;               /* 0x24 the weapon (PlayerState.weapon) */
    u16 magic_id;                   /* 0x26 the magic's number */
    char pad28[0x08];
} PlayerShot;

/* player_state. Files still declare it as float[]/int[]/char[] too, so the
   extern stays in each file until they agree; D_00413F84 is position.y and
   player_rotation is rotation. */
typedef struct {
    Vector4 position;               /* 0x00 */
    Vector4 rotation;               /* 0x10 y is the heading */
    s32 hp_base;                    /* 0x20 ini "hp": the most HP before what is worn; hp_base to magic_base are saved one by one (Game_WriteSaveImage, Game_ReadSaveImage) */
    s32 mp_base;                    /* 0x24 ini "mp" */
    s32 power_base;                 /* 0x28 ini "power"; raised by "Physical UP" */
    s32 magic_base;                 /* 0x2C ini "magic"; raised by "Magic Power UP" */
    s16 timed_flags[6];             /* 0x30 held at 0 while the matching timer runs */
    u8 weapon;                      /* 0x3C ini "equip_weapon": the weapon in hand, its item number less ITEM_WEAPON_FIRST (its row of weapon_defs) */
    u8 equipped_magic;              /* 0x3D ini "equip_magic": the magic in hand; passed to Sound_LoadMgBank at start-up */
    u8 equipment[8];                /* 0x3E what is worn, by EQUIP_ slot: the item number less ITEM_WORN_FIRST */
    u16 shortcut;                   /* 0x46 ini "equip_shortcut": the item or magic of the shortcut */
    u8 shortcut_type;               /* 0x48 ini "equip_shortcut_type": 0 none, 1 an item, 2 a magic */
    char pad49[0x03];
    s32 gold;                       /* 0x4C ini "gold"; saved */
    s32 experience;                 /* 0x50 ini "exp"; saved */
    s32 level;                      /* 0x54 ini "level"; saved */
    s32 hp_max;                     /* 0x58 the most hp_now can be: hp_base with what is worn (the Widda's Ring adds a fifth) */
    s32 mp_max;                     /* 0x5C the most mp_now can be */
    PlayerShotStats unk60;          /* 0x60 unk60 to unkA3 and unkDC are worked out by Player_RecomputeStats */
    PlayerItemRates unk72;          /* 0x72 at most 100 each */
    PlayerShotStats unk7E;          /* 0x7E copied into the shot a cast magic launches (Player_CastMagic) */
    PlayerShotStats unk90;          /* 0x90 */
    char padA2[0x02];
    s32 weight;                     /* 0xA4 the weight of what is held and worn (item_name_defs unk3E, in pounds) */
    s32 max_weight;                 /* 0xA8 the most the player may carry; weight above it slows him */
    s32 power;                      /* 0xAC power_base in effect (the Power Seed adds a tenth); how fast unkBC fills (at least 20 is used) */
    s32 magic;                      /* 0xB0 magic_base in effect (the Widda Needle adds a tenth); how fast unkC0 fills (at least 20 is used) */
    s32 hp_now;                     /* 0xB4 the HP the player has (the healing items add to it) */
    s32 mp_now;                     /* 0xB8 the MP the player has; saved */
    s32 power_gauge;                /* 0xBC cleared when weapon changes over a menu (GameLoop_OpenRequestedMenu) */
    s32 magic_gauge;                /* 0xC0 cleared when unk264 is set */
    s32 power_gauge_delay;          /* 0xC4 frames unkBC waits before it fills again */
    s32 magic_gauge_delay;          /* 0xC8 frames unkC0 waits before it fills again */
    s32 breath_now;                 /* 0xCC how much of a HUD bar is filled (Hud_Draw), out of unkD0 */
    s32 breath_max;                 /* 0xD0 0: the bar is not drawn */
    s32 weapon_use_count;           /* 0xD4 saved; every 100th swing gives "Physical UP" (Player_CountWeaponUse) */
    s32 magic_use_count;            /* 0xD8 saved; every 100th magic gives "Magic Power UP" (Player_CountMagicUse) */
    u16 ailment_defence[6];         /* 0xDC at most 100 each */
    s32 unkE8;                      /* 0xE8 unkE8 to unk10C are worked out by Player_SumWornBonuses from what is worn */
    s32 worn_drain_active;          /* 0xEC nonzero: mp drops every 30 frames (Player_UpdateStatus) */
    u32 hp_regen_period;            /* 0xF0 nonzero: hp rises every that many frames */
    u32 hp_drain_period;            /* 0xF4 nonzero: hp drops every that many frames */
    u32 mp_regen_period;            /* 0xF8 nonzero: mp rises every that many frames */
    u32 mp_drain_period;            /* 0xFC nonzero: mp drops every that many frames */
    s32 damage_to_hp_percent;       /* 0x100 percent of the damage dealt that is added to hp (Attack_HitEntity) */
    s32 damage_to_mp_percent;       /* 0x104 percent of the damage dealt that is added to mp */
    s32 movement_halved;            /* 0x108 nonzero: the player moves and turns at half speed (Player_Move) */
    s32 ring_of_sight_active;       /* 0x10C 1 while the Ring of Sight is worn (Player_AddItemBonuses) */
    u16 ailment_attack_bonus[6];    /* 0x110 cleared when ailment_attack_bonus_timer[i] runs out */
    s16 physical_strength_bonus[3]; /* 0x11C cleared with physical_bonus_stamp[i] when physical_strength_bonus_timer[i] runs out */
    s16 magic_strength_bonus[6];    /* 0x122 nonzero: magic_bonus_stamp[i] competes for the weapon trail kind (WeaponTrail_PickKind) */
    u16 ailment_defence_bonus[6];   /* 0x12E cleared when ailment_defence_bonus_timer[i] runs out */
    s16 physical_defence_bonus[3];  /* 0x13A cleared when physical_defence_bonus_timer[i] runs out */
    s16 magic_defence_bonus[6];     /* 0x140 cleared when magic_defence_bonus_timer[i] runs out */
    u16 ailment_attack_bonus_timer[6]; /* 0x14C frames left for ailment_attack_bonus[i] */
    s16 physical_strength_bonus_timer[3]; /* 0x158 frames left for physical_strength_bonus[i] and physical_bonus_stamp[i] */
    s16 magic_strength_bonus_timer[6]; /* 0x15E frames left for magic_strength_bonus[i] and magic_bonus_stamp[i] */
    u16 ailment_defence_bonus_timer[6]; /* 0x16A frames left for ailment_defence_bonus[i] */
    s16 physical_defence_bonus_timer[3]; /* 0x176 frames left for physical_defence_bonus[i] */
    s16 magic_defence_bonus_timer[6]; /* 0x17C frames left for magic_defence_bonus[i] */
    s16 physical_bonus_stamp[3];    /* 0x188 cast order of the magic that set physical_strength_bonus */
    s16 magic_bonus_stamp[6];       /* 0x18E the largest one picks the weapon trail kind */
    char pad19A[0x02];
    s32 power_seed_timer;           /* 0x19C frames the Power Seed lasts; counts down to 0 every frame */
    s32 widda_needle_timer;         /* 0x1A0 frames the Widda Needle lasts; counts down to 0 every frame */
    s16 hp_bar_level;               /* 0x1A4 how full the drawn HP bar is, out of 49 */
    s16 mp_bar_level;               /* 0x1A6 the mp the drawn MP bar moves toward */
    u8 death_timer;                 /* 0x1A8 nonzero: the main loop skips Script_UpdateAll */
    char pad1A9[0x03];
    f32 fall_speed;                 /* 0x1AC downward speed gravity builds, in world y */
    f32 strafe_speed;               /* 0x1B0 sideways walking speed, before the heading is applied */
    f32 walk_speed;                 /* 0x1B4 forward and backward walking speed */
    f32 look_speed;                 /* 0x1B8 how fast the view pitches up and down */
    f32 turn_speed;                 /* 0x1BC how fast the player turns */
    s32 stride_phase;               /* 0x1C0 0 to STRIDE_LENGTH phase of the walk stride */
    f32 head_bob_offset;            /* 0x1C4 added to the camera's height */
    u64 status_bits;                /* 0x1C8 status bits; bit 38 is tested */
    f32 step_remainder;             /* 0x1D0 set with status bit 33 when a step up is too high to take at once (Player_CollideWithMapObjects) */
    f32 step_per_frame;             /* 0x1D4 16.67 then */
    f32 airborne_start_y;           /* 0x1D8 position.y when the player left the ground: the height a fall is measured from */
    f32 water_surface_y;            /* 0x1DC height of the water surface the player is under */
    s32 ground_effect_active;       /* 0x1E0 nonzero with unk1E4 4: the fifth row of the movement tables is used */
    s32 ground_effect_kind;         /* 0x1E4 what the ground does: 1-3 poison, 4 damage 10, 5 damage */
    s32 ground_effect_frames;       /* 0x1E8 a frame count: every 60th frame the ground may poison the player (Player_ApplyGroundEffects) */
    s32 ground_kills;               /* 0x1EC nonzero: hp is held at 0 and the player dies (Player_ApplyGroundEffects) */
    f32 landing_dip_offset;         /* 0x1F0 added to the camera's height */
    f32 landing_dip_depth;          /* 0x1F4 how deep the dip after a landing is: half the fall speed, at most 150 */
    s32 landing_dip_frames;         /* 0x1F8 how many frames the dip takes */
    s32 landing_dip_frame;          /* 0x1FC the frame of the dip; unk1F0 follows half a sine wave */
    s32 landing_slow_timer;         /* 0x200 frames left of half speed after a hard landing */
    s32 stepped_on_entity;          /* 0x204 the entity the player was put on top of */
    s32 floor_map_object;           /* 0x208 the map object the player stands on */
    s32 touched_map_object;         /* 0x20C the map object mesh wall the player touched */
    u8 floor_material;              /* 0x210 3: the second row of the movement tables is used; 6: see Player_FindFloorMaterial */
    u8 unk211;                      /* 0x211 set to 6 with unk210 before the floors are looked for (Player_CollideWithMap) */
    char pad212[0x02];
    f32 unk214;                     /* 0x214 */
    char pad218[0x08];
    Vector4 aim;                    /* 0x220 direction the player is knocked back along, flattened to x/z (Player_KnockBackFromHit) */
    f32 aim_speed;                  /* 0x230 knock-back speed, lost by weight every frame; above 0: Game_BlendPreviousFrame(0x40) is drawn */
    s32 view_tilt_frames;           /* 0x234 frames unk238 and unk23C keep growing */
    f32 view_tilt_pitch;            /* 0x238 pitch a hit threw the view by */
    f32 view_tilt_roll;             /* 0x23C second view tilt of a hit, grown with the pitch */
    Vector4 unk240;                 /* 0x240 a velocity added to the position every frame: the push of the floor (MapRecord.push_acceleration) */
    u8 record_flag_block;           /* 0x250 0xFF after a map change */
    char pad251[0x01];
    u16 map_index;                  /* 0x252 picks the MapSlot record */
    u32 frames_since_item_action;   /* 0x254 frames since the item in hand last moved */
    u8 item_action_step;            /* 0x258 a step counter: Player_PlayEquipSound advances it, Player_FinishWeaponMotion clears it */
    u8 cast_launched_shot;          /* 0x259 set when a cast magic launches a shot (Player_CastMagic) */
    u8 magic2_asked;                /* 0x25A nonzero: the weapon's second magic was asked for this swing */
    u8 magic1_asked;                /* 0x25B cleared every frame before the item's handler runs (Player_UpdateActions) */
    u8 magic1_window;               /* 0x25C frames left in which the first magic of the weapon may be asked for (Player_SwingWeapon) */
    u8 magic2_armed;                /* 0x25D set when a swing starts: the second magic of the weapon may be asked for */
    u8 magic2_state;                /* 0x25E the second magic: 1 asked for, 2 cast */
    u8 magic1_state;                /* 0x25F the first magic: 1 asked for, 2 cast */
    u8 draw_sound_played : 1;       /* 0x260 set while the sound of drawing the weapon plays (Player_DrawBow) */
    u8 magic1_motion_reached : 1;   /* set once the first magic's motion frame is reached (Player_SwingWeapon) */
    u8 magic2_motion_reached : 1;   /* tested for the second magic, never set (Player_SwingWeapon) */
    u8 unk260_3 : 5;
    u8 weapon_bounced;              /* 0x261 nonzero: the weapon bounced this swing and hits no more */
    u8 struck_solid_object;         /* 0x262 set once by Player_StrikeSolidObject */
    u8 swing_ended;                 /* 0x263 nonzero: the swing's hits are over (map change, pause) */
    u8 charging_magic;              /* 0x264 set from equipped_magic, shortcut or the PlayerItemDef after a menu (GameLoop_OpenRequestedMenu) */
    char pad265[0x03];
    PlayerShot unk268;              /* 0x268 the swing of the weapon in hand, passed to Attack_Run (Player_SwingWeapon) */
    u8 draw_motion_complete;        /* 0x298 nonzero: the draw motion reached the frame that looses the shot */
    char pad299[0x03];
    s32 mp_drain_tint;              /* 0x29C nonzero: the screen is tinted by it (GameLoop_DrawFrame) */
    s32 pending_mp_drain;           /* 0x2A0 a total Attack_DrainPlayerMp adds to */
    char pad2A4[0x04];
    f32 screen_fade;                /* 0x2A8 fade amount: > 0 toward white, < 0 toward black */
    char pad2AC[0x04];
    s32 hp_step_remaining;          /* 0x2B0 moved one toward 0 every unk2B4 frames; hp moves with it */
    s32 hp_step_period;             /* 0x2B4 frames per step of hp_step_remaining; cleared when unk2B0 reaches 0 */
    u16 flag_timers[6];             /* 0x2B8 frames left, one per timed_flags entry */
    u32 ailment0_start_frame;       /* 0x2C4 the frame counter D_00446ED4 when timed_flags[0] was started */
    u32 ailment1_start_frame;       /* 0x2C8 the same for timed_flags[1] to [5] */
    u32 ailment2_start_frame;       /* 0x2CC the frame count timed_flags[2] was started at */
    u32 ailment3_start_frame;       /* 0x2D0 the frame count timed_flags[3] was started at */
    u32 ailment4_start_frame;       /* 0x2D4 the frame count timed_flags[4] was started at */
    u32 ailment5_start_frame;       /* 0x2D8 the frame count timed_flags[5] was started at */
    s32 ailment0_slow_timer;        /* 0x2DC above 0: Player_TakeDamageNoRumble(3) runs every 90 frames, not every 30 */
    s32 light_spell_timer;          /* 0x2E0 frames magic 0x3D to 0x3F last */
    s32 intimidate_timer;           /* 0x2E4 above 0: unk2E8 counts (EntityAi_PickMeleeAttack) */
    f32 intimidate_range;           /* 0x2E8 enemies whose unk60 is below it attack from half as far */
    s32 rage_timer;                 /* 0x2EC frames magic 0x52 to 0x54 last */
    s32 rage_power_bonus;           /* 0x2F0 10, 20 or 30 for magic 0x52, 0x53, 0x54 */
    char pad2F4[0x0C];              /* the block is 0x300 bytes (Player_ResetForNewGame clears and copies that many) */
} PlayerState;

/* weapon_trail_kinds[7]: colour and life of the weapon trail, one per kind (D_00447188) */
typedef struct {
    s32 red;                        /* 0x00 */
    s32 green;                      /* 0x04 */
    s32 blue;                       /* 0x08 */
    s32 alpha;                      /* 0x0C of the newest point; it fades with age */
    s64 blend;                      /* 0x10 GS ALPHA_1 value */
    s32 lifetime;                   /* 0x18 frames a point stays queued */
    char pad1C[0x04];
} WeaponTrailKind;

/* player_screen_tint (0x48 bytes): the colours laid over the screen for what happens to the player; each
   strength is the alpha Player_DrawScreenTints draws with */
typedef struct {
    s32 damage_flash_frames;        /* 0x00 set to 10 by Player_FlashDamage */
    s32 damage_flash_alpha;         /* 0x04 red (dark red while unk14 shows) */
    s32 green_tint_alpha;           /* 0x08 green, shown when kind is 0 */
    s32 dark_yellow_tint_alpha;     /* 0x0C dark yellow, shown when kind is 1 */
    s32 darkness_tint_alpha;        /* 0x10 black, subtracted */
    s32 purple_tint_alpha;          /* 0x14 purple, shown when kind is 3 */
    s32 white_tint_alpha;           /* 0x18 white, shown when kind is 4 */
    s32 red;                        /* 0x1C the colour shown when kind is 5 */
    s32 green;                      /* 0x20 */
    s32 blue;                       /* 0x24 */
    s32 cycling_tint_alpha;         /* 0x28 its strength */
    s32 kind;                       /* 0x2C which of the tints is shown */
    s32 death_fade_tint_alpha;      /* 0x30 black, subtracted */
    s32 magic_glow_strength;        /* 0x34 0x80 to 0xA0: rises with MapDrawState.screen_tint_amount (Player_UpdateScreenTints) */
    char pad38[0x10];
} PlayerScreenTint;

/* PlayerState.status_bits as the bit-fields the game sets */
typedef struct {
    u64 pad0 : 33;
    u64 bit33 : 1;
    u64 bit34 : 1;                  /* set: Ladder_Start does nothing */
    u64 bit35 : 1;                  /* running: the run button is held while moving (Player_Update) */
    u64 bit36 : 1;                  /* on an object set up by Ladder_Start (a ladder, it seems) */
    u64 pad37 : 1;
    u64 bit38 : 1;                  /* cleared at the start of every played frame, with bit39 */
    u64 bit39 : 1;
    u64 pad40 : 24;
} PlayerStatusBits;

/* the start of a PlayerState with unk1C8 as a word and bit-fields: accesses through it are based on the
   block's address, as retail's are in Player_Update */
typedef struct {
    char pad000[0x1C8];
    s32 step_sounded;               /* 0x1C8 set once the footstep of the current stride (PlayerState.stride_phase) has sounded */
    u64 pad32 : 1;                  /* 0x1CC the bits of PlayerStatusBits */
    u64 bit33 : 1;
    u64 bit34 : 1;
    u64 bit35 : 1;
    u64 bit36 : 1;
    u64 pad37 : 1;
    u64 bit38 : 1;
    u64 bit39 : 1;
    u64 pad40 : 24;
} PlayerStatusView;

#define PLAYER_STATUS(player) ((PlayerStatusView *)(player))

#define PLAYER_STATUS_BITS(player) ((PlayerStatusBits *)&(player)->status_bits)

/* D_0043E130[100]: the experience a level needs (PlayerState.experience against level) and what reaching it adds */
typedef struct {
    s32 experience;                 /* 0x00 */
    u8 hp_gain;                     /* 0x04 added to PlayerState.hp_base */
    u8 mp_gain;                     /* 0x05 added to PlayerState.mp_base */
    u8 power_gain;                  /* 0x06 added to PlayerState.power_base */
    u8 magic_gain;                  /* 0x07 added to PlayerState.magic_base */
} PlayerLevelStep;

/* worn_defs[160]: param/item_pct.prm (Map_LoadParamFiles), a row per item from ITEM_WORN_FIRST, one per LevelRecord of worn_item_level_records */
typedef struct {
    s16 unk00[9];                   /* 0x00 the base of the nine values; LevelRecord.unk00 is added (Player_AddWornItemValues) */
    u16 unk12[6];                   /* 0x12 added to PlayerState.ailment_defence while the item is worn (Player_AddItemBonuses) */
    char pad1E[0x01];
    u8 bonus_kinds[2];              /* 0x1F what wearing the item does, 0: nothing (the switch of Player_AddItemBonuses) */
    u8 bonus_amounts[2];            /* 0x21 */
    char pad23[0x1C];
    u8 unk3F;                       /* 0x3F added to PlayerItemDef.equip_menu_motion for the shield in hand (Menu_UpdateEquip) */
} LevelRecordDef;

/* worn_item_level_records[160]: what has been gained per item (0x22 bytes each) */
typedef struct {
    s16 unk00[9];                   /* 0x00 added to the PlayerItemDef's row (Player_RecomputeStats) */
    u8 level;                       /* 0x12 */
    u8 exp;                         /* 0x13 */
    u8 unk14;                       /* 0x14 */
    char pad15[0x01];
    s16 unk16;                      /* 0x16 ini "item_": the third number times 20, 0 to 480 (Ini_SetItem) */
    u8 unk18;                       /* 0x18 nonzero: Smith_BuildItemRings lists the item among those whose time unk19 counts */
    union {
        char bytes[8];              /* 0x19 stored through this view (Smith_UpdateUpgradeConfirm, Smith_UpdateListWithConfirm) */
        ClockTime time;             /* 0x19 when it was last upgraded; not aligned */
    } unk19;
    char pad21[0x01];
} LevelRecord;

/* weapon_defs[70]: param/item_wpn.prm (Map_LoadParamFiles), 0x90 bytes per weapon from ITEM_WEAPON_FIRST; PlayerState.weapon picks one */
typedef struct {
    PlayerShotStats attack_by_level[3]; /* 0x00 one row per LevelRecord.unk14 */
    PlayerItemRates unk36;          /* 0x36 */
    u16 first_magic_id;             /* 0x42 0xFFFF: none */
    u16 first_magic_cast_frame;     /* 0x44 the swing's motion frame on which magic unk42 is cast (Player_SwingWeapon) */
    u16 unk46;                      /* 0x46 0xFFFF: none */
    u16 first_magic_latch_frame;    /* 0x48 the motion frame from which PlayerState.magic1_motion_reached is set */
    u16 first_magic_window_start_frame; /* 0x4A first and last motion frame on which magic unk42 may be asked for */
    u16 first_magic_window_end_frame; /* 0x4C last motion frame on which the first magic may be asked for */
    u16 first_magic_effect_frames;  /* 0x4E frames magic unk42 stays (D_00447114) */
    char pad50[0x02];
    u16 second_magic_id;            /* 0x52 0xFFFF: none */
    u16 second_magic_cast_frame;    /* 0x54 as unk44 to first_magic_effect_frames, for magic unk52 */
    u16 unk56;                      /* 0x56 */
    u16 second_magic_latch_frame;   /* 0x58 motion frame past which the second magic's latch is set */
    u16 second_magic_window_start_frame; /* 0x5A first motion frame on which the second magic may be asked for */
    u16 second_magic_window_end_frame; /* 0x5C last motion frame on which the second magic may be asked for */
    u16 second_magic_effect_frames; /* 0x5E frames the second magic's effect stays */
    char pad60[0x02];
    u8 ammo_item_index;             /* 0x62 index into item_counts: nonzero there lets the item launch its shot (Player_FireProjectile) */
    u8 action_handler_index;        /* 0x63 picks item_action_handlers: 0 and 1 swing, 2 shoots */
    u8 bonus_kinds[2];              /* 0x64 as LevelRecordDef.bonus_kinds */
    u8 bonus_amounts[2];            /* 0x66 */
    u16 weapon_length_cm;           /* 0x68 shown on the trade screen in tenths of a foot (Trade_UpdateStockList) */
    char pad6A[0x02];
    f32 swing_pitch_arc_degrees;    /* 0x6C twice Unk1B1470Request.half_pitch of a swing, before D_00446240 (Player_SwingWeapon) */
    f32 swing_yaw_arc_degrees;      /* 0x70 twice Unk1B1470Request.half_angle of a swing, before D_00446240 */
    f32 swing_reach;                /* 0x74 how far a swing reaches, less 100 */
    u16 draw_ready_frame;           /* 0x78 the motion frame from which a held shot is ready (Player_DrawBow) */
    u16 draw_last_frame;            /* 0x7A the frame the drawing motion stops at */
    u8 equip_menu_motion;           /* 0x7C which D_005CEE60 motion the weapon shown on the equipment screen plays (Menu_UpdateEquip) */
    u8 equip_menu_draw_index;       /* 0x7D which D_0042A290 function draws it there */
    u8 swing_gauge_delay_frames;    /* 0x7E goes to PlayerState.power_gauge_delay when a swing starts (Player_SwingWeapon) */
    u8 map_object_damage_multiplier; /* 0x7F copied to Unk1B1470Request.map_object_damage_multiplier */
    u16 draw_sound_id;              /* 0x80 0xFFFF: no sound while drawing */
    u16 draw_sound_frame;           /* 0x82 the motion frame the drawing sound starts on */
    char pad84[0x02];
    u8 double_hit_chance_percent;   /* 0x86 copied to Unk1B1470Request.double_hit_chance_percent */
    s8 held_offset_x;               /* 0x87 unk87 to unk89 become held_item_offsets (x, y, z) when the item's model is loaded */
    s8 held_offset_y;               /* 0x88 y offset of the model of the item in the hand */
    s8 held_offset_z;               /* 0x89 z offset of the model of the item in the hand */
    u16 first_magic_min_power;      /* 0x8A the least PlayerState.power for magic unk42 (Player_SwingWeapon) */
    u16 first_magic_min_magic;      /* 0x8C the least PlayerState.magic for magic unk42 */
    char pad8E[0x02];
} PlayerItemDef;

/* held_item_arm_points, held_item_arm_points_prev: points 30 and 31 of the held item's model, this frame's and the frame before's */
typedef struct {
    VuVector4 points[2];
} PlayerArmPoints;

/* an item_arm file (the model the player holds): up to three parts, each present when its size is not 0 */
typedef struct {
    s32 texture_size;               /* 0x00 */
    s32 model_size;                 /* 0x04 */
    s32 motion_size;                /* 0x08 */
    char pad0C[0x08];
    s32 texture_offset;             /* 0x14 from the start of the file */
    s32 model_offset;               /* 0x18 */
    s32 motion_offset;              /* 0x1C */
} PlayerArmFile;

/* item_name_defs[ITEM_COUNT]: param/item_nam.prm, 0x40 bytes per item, starting with its name. D_00417EF8 is
   the run of them from ITEM_WORN_FIRST on (the items Attack_WearWornItem reads). */
typedef struct {
    char pad00[0x30];
    u32 file_number : 16;           /* 0x30 the number in its model file's name */
    u32 unk30_16 : 2;
    u32 state : 2;                  /* 0x32 bits 2-3: 0 is set to 1 when the table is reset */
    u32 upgrade_material_kind : 12; /* which smith material upgrades the item: bone, scale or claw */
    char pad34[0x02];
    u8 upgrade_wait_minutes;        /* 0x36 minutes that must pass since LevelRecord.unk19 (Smith_BuildItemRings) */
    u8 exp_per_level;               /* 0x37 */
    char pad38[0x04];
    u8 unk3C;                       /* 0x3C */
    char pad3D[0x01];
    u16 weight_tenths_of_pound;     /* 0x3E summed into PlayerState.weight for everything worn */
} LevelDef;

/* D_0042B810[]: the keys of ini/NNNN.ini ("hp=", "equip_weapon=", "item_", ...) and what reads each; a NULL handler ends it */
typedef struct {
    char key[0x40];                 /* 0x00 the start of the line */
    void (*handler)(char *);        /* 0x40 gets the text after the "=" */
} IniKeyHandler;

#endif
