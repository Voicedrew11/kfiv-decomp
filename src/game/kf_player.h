/* The player: the state block at D_00413F80 and the level/experience tables. */
#ifndef KF_PLAYER_H
#define KF_PLAYER_H

#include "common.h"
#include "kf_model.h"

/* six values of a PlayerItemDef that the player takes over */
typedef struct {
    u16 values[6];                  /* 0x00 */
} PlayerItemRates;

/* nine values copied from the player into a shot */
typedef struct {
    s16 values[9];                  /* 0x00 */
} PlayerShotStats;

/* the 0x30-byte slot func_001B1390 returns, as the player's magic fills it */
typedef struct {
    u8 unk00;                       /* 0x00 */
    char pad01[0x01];
    u16 unk02;                      /* 0x02 PlayerState.unkBC */
    void *unk04;                    /* 0x04 the Unk1B1470Request (kf_entity.h) of a weapon's shot, in D_0057A350 */
    PlayerShotStats unk08;          /* 0x08 PlayerState.unk7E */
    char pad1A[0x06];
    u16 unk20;                      /* 0x20 PlayerState.unkAC */
    u16 unk22;                      /* 0x22 PlayerState.unkB0 */
    s16 unk24;                      /* 0x24 the weapon (PlayerState.unk3C) */
    u16 unk26;                      /* 0x26 the magic's number */
    char pad28[0x08];
} PlayerShot;

/* D_00413F80. Files still declare it as float[]/int[]/char[] too, so the
   extern stays in each file until they agree; D_00413F84 is position.y and
   D_00413F90 is rotation. */
typedef struct {
    Vector4 position;               /* 0x00 */
    Vector4 rotation;               /* 0x10 y is the heading */
    s32 unk20;                      /* 0x20 unk20 to unk2C are saved one by one (func_00103C18, func_00104EC8) */
    s32 unk24;                      /* 0x24 */
    s32 unk28;                      /* 0x28 */
    s32 unk2C;                      /* 0x2C */
    s16 timed_flags[6];             /* 0x30 held at 0 while the matching timer runs */
    u8 unk3C;                       /* 0x3C row of D_0041B9B8 (0x90 bytes each) */
    u8 unk3D;                       /* 0x3D passed to func_001A1260 at start-up */
    u8 equipment[8];                /* 0x3E equipped items, from definition 220 on; [7] 0x9C: enemy attacks reach half as far (func_0018F648) */
    u16 unk46;                      /* 0x46 */
    u8 unk48;                       /* 0x48 */
    char pad49[0x03];
    s32 unk4C;                      /* 0x4C saved */
    s32 unk50;                      /* 0x50 saved */
    s32 unk54;                      /* 0x54 saved */
    s32 unk58;                      /* 0x58 */
    s32 unk5C;                      /* 0x5C the most unkB8 can be */
    PlayerShotStats unk60;          /* 0x60 unk60 to unkA3 and unkDC are worked out by func_001BA310 */
    PlayerItemRates unk72;          /* 0x72 at most 100 each */
    PlayerShotStats unk7E;          /* 0x7E copied into the shot a cast magic launches (func_001BC9E0) */
    PlayerShotStats unk90;          /* 0x90 */
    char padA2[0x02];
    s32 unkA4;                      /* 0xA4 */
    s32 unkA8;                      /* 0xA8 */
    s32 unkAC;                      /* 0xAC how fast unkBC fills (at least 20 is used) */
    s32 unkB0;                      /* 0xB0 how fast unkC0 fills (at least 20 is used) */
    s32 unkB4;                      /* 0xB4 */
    s32 unkB8;                      /* 0xB8 saved */
    s32 unkBC;                      /* 0xBC cleared when unk3C changes over a menu (func_001017A8) */
    s32 unkC0;                      /* 0xC0 cleared when unk264 is set */
    s32 unkC4;                      /* 0xC4 frames unkBC waits before it fills again */
    s32 unkC8;                      /* 0xC8 frames unkC0 waits before it fills again */
    s32 unkCC;                      /* 0xCC how much of a HUD bar is filled (func_001D19D8), out of unkD0 */
    s32 unkD0;                      /* 0xD0 0: the bar is not drawn */
    s32 unkD4;                      /* 0xD4 saved */
    s32 unkD8;                      /* 0xD8 saved */
    u16 unkDC[6];                   /* 0xDC at most 100 each */
    s32 unkE8;                      /* 0xE8 unkE8 to unk10C are worked out by func_001BAE88 from what is worn */
    s32 unkEC;                      /* 0xEC nonzero: unkB8 drops every 30 frames (func_001B96C8) */
    u32 unkF0;                      /* 0xF0 nonzero: unkB4 rises every that many frames */
    u32 unkF4;                      /* 0xF4 nonzero: unkB4 drops every that many frames */
    u32 unkF8;                      /* 0xF8 nonzero: unkB8 rises every that many frames */
    u32 unkFC;                      /* 0xFC nonzero: unkB8 drops every that many frames */
    s32 unk100;                     /* 0x100 percent of the damage dealt that is added to unkB4 (func_001B2F70) */
    s32 unk104;                     /* 0x104 percent of the damage dealt that is added to unkB8 */
    s32 unk108;                     /* 0x108 nonzero: the player moves and turns at half speed (func_001B7EE0) */
    s32 unk10C;                     /* 0x10C 1 while item 0x15F is worn (func_001BB020) */
    u16 unk110[6];                  /* 0x110 cleared when unk14C[i] runs out */
    s16 unk11C[3];                  /* 0x11C cleared with unk188[i] when unk158[i] runs out */
    s16 unk122[6];                  /* 0x122 nonzero: unk18E[i] competes for the weapon trail kind (func_001C06E8) */
    u16 unk12E[6];                  /* 0x12E cleared when unk16A[i] runs out */
    s16 unk13A[3];                  /* 0x13A cleared when unk176[i] runs out */
    s16 unk140[6];                  /* 0x140 cleared when unk17C[i] runs out */
    u16 unk14C[6];                  /* 0x14C frames left for unk110[i] */
    s16 unk158[3];                  /* 0x158 frames left for unk11C[i] and unk188[i] */
    s16 unk15E[6];                  /* 0x15E frames left for unk122[i] and unk18E[i] */
    u16 unk16A[6];                  /* 0x16A frames left for unk12E[i] */
    s16 unk176[3];                  /* 0x176 frames left for unk13A[i] */
    s16 unk17C[6];                  /* 0x17C frames left for unk140[i] */
    s16 unk188[3];                  /* 0x188 */
    s16 unk18E[6];                  /* 0x18E the largest one picks the weapon trail kind */
    char pad19A[0x02];
    s32 unk19C;                     /* 0x19C counts down to 0 every frame */
    s32 unk1A0;                     /* 0x1A0 counts down to 0 every frame */
    s16 unk1A4;                     /* 0x1A4 */
    s16 unk1A6;                     /* 0x1A6 */
    u8 unk1A8;                      /* 0x1A8 nonzero: the main loop skips func_00210D48 */
    char pad1A9[0x03];
    f32 unk1AC;                     /* 0x1AC */
    f32 unk1B0;                     /* 0x1B0 */
    f32 unk1B4;                     /* 0x1B4 */
    f32 unk1B8;                     /* 0x1B8 */
    f32 unk1BC;                     /* 0x1BC */
    s32 unk1C0;                     /* 0x1C0 */
    f32 unk1C4;                     /* 0x1C4 added to the camera's height */
    u64 unk1C8;                     /* 0x1C8 status bits; bit 38 is tested */
    f32 unk1D0;                     /* 0x1D0 set with status bit 33 when a step up is too high to take at once (func_001BF350) */
    f32 unk1D4;                     /* 0x1D4 16.67 then */
    f32 unk1D8;                     /* 0x1D8 position.y when the player left the ground: the height a fall is measured from */
    f32 unk1DC;                     /* 0x1DC */
    s32 unk1E0;                     /* 0x1E0 nonzero with unk1E4 4: the fifth row of the movement tables is used */
    s32 unk1E4;                     /* 0x1E4 */
    s32 unk1E8;                     /* 0x1E8 a frame count: every 60th frame the ground may poison the player (func_001B9078) */
    s32 unk1EC;                     /* 0x1EC nonzero: unkB4 is held at 0 and the player dies (func_001B9078) */
    f32 unk1F0;                     /* 0x1F0 added to the camera's height */
    f32 unk1F4;                     /* 0x1F4 how deep the dip after a landing is: half the fall speed, at most 150 */
    s32 unk1F8;                     /* 0x1F8 how many frames the dip takes */
    s32 unk1FC;                     /* 0x1FC the frame of the dip; unk1F0 follows half a sine wave */
    s32 unk200;                     /* 0x200 */
    s32 unk204;                     /* 0x204 */
    s32 unk208;                     /* 0x208 */
    s32 unk20C;                     /* 0x20C */
    u8 unk210;                      /* 0x210 3: the second row of the movement tables is used; 6: see func_001BEA50 */
    u8 unk211;                      /* 0x211 set to 6 with unk210 before the floors are looked for (func_001BDDC0) */
    char pad212[0x02];
    f32 unk214;                     /* 0x214 */
    char pad218[0x08];
    Vector4 aim;                    /* 0x220 direction the player aims along, flattened to x/z */
    f32 aim_speed;                  /* 0x230 above 0: func_001005E8 draws func_00106FB8(0x40) */
    s32 unk234;                     /* 0x234 frames unk238 and unk23C keep growing */
    f32 unk238;                     /* 0x238 */
    f32 unk23C;                     /* 0x23C */
    Vector4 unk240;                 /* 0x240 a velocity added to the position every frame: the push of the floor (MapRecord.unk2E) */
    u8 unk250;                      /* 0x250 0xFF after a map change */
    char pad251[0x01];
    u16 map_index;                  /* 0x252 picks the MapSlot record */
    u32 unk254;                     /* 0x254 */
    u8 unk258;                      /* 0x258 a step counter: func_001B7C90 advances it, func_001B7CC8 clears it */
    u8 unk259;                      /* 0x259 set when a cast magic launches a shot (func_001BC9E0) */
    u8 unk25A;                      /* 0x25A */
    u8 unk25B;                      /* 0x25B cleared every frame before the item's handler runs (func_001B6FA0) */
    u8 unk25C;                      /* 0x25C frames left in which the first magic of the weapon may be asked for (func_001B70F8) */
    u8 unk25D;                      /* 0x25D set when a swing starts: the second magic of the weapon may be asked for */
    u8 unk25E;                      /* 0x25E the second magic: 1 asked for, 2 cast */
    u8 unk25F;                      /* 0x25F the first magic: 1 asked for, 2 cast */
    u8 unk260_0 : 1;                /* 0x260 set while the sound of drawing the weapon plays (func_001B7918) */
    u8 unk260_1 : 1;                /* set once the first magic's motion frame is reached (func_001B70F8) */
    u8 unk260_2 : 1;                /* tested for the second magic, never set (func_001B70F8) */
    u8 unk260_3 : 5;
    u8 unk261;                      /* 0x261 */
    u8 unk262;                      /* 0x262 set once by func_001BC0E0 */
    u8 unk263;                      /* 0x263 */
    u8 unk264;                      /* 0x264 set from unk3D, unk46 or the PlayerItemDef after a menu (func_001017A8) */
    char pad265[0x03];
    PlayerShot unk268;              /* 0x268 the swing of the weapon in hand, passed to func_001B1470 (func_001B70F8) */
    u8 unk298;                      /* 0x298 */
    char pad299[0x03];
    s32 unk29C;                     /* 0x29C nonzero: the screen is tinted by it (func_001005E8) */
    s32 unk2A0;                     /* 0x2A0 a total func_001B4EC8 adds to */
    char pad2A4[0x04];
    f32 unk2A8;                     /* 0x2A8 fade amount: > 0 toward white, < 0 toward black */
    char pad2AC[0x04];
    s32 unk2B0;                     /* 0x2B0 moved one toward 0 every unk2B4 frames; unkB4 moves with it */
    s32 unk2B4;                     /* 0x2B4 frames per step of unk2B0; cleared when unk2B0 reaches 0 */
    u16 flag_timers[6];             /* 0x2B8 frames left, one per timed_flags entry */
    u32 unk2C4;                     /* 0x2C4 the frame counter D_00446ED4 when timed_flags[0] was started */
    u32 unk2C8;                     /* 0x2C8 the same for timed_flags[1] to [5] */
    u32 unk2CC;                     /* 0x2CC */
    u32 unk2D0;                     /* 0x2D0 */
    u32 unk2D4;                     /* 0x2D4 */
    u32 unk2D8;                     /* 0x2D8 */
    s32 unk2DC;                     /* 0x2DC above 0: func_001BB2E8(3) runs every 90 frames, not every 30 */
    s32 unk2E0;                     /* 0x2E0 frames magic 0x3D to 0x3F last */
    s32 unk2E4;                     /* 0x2E4 above 0: unk2E8 counts (func_0018F648) */
    f32 unk2E8;                     /* 0x2E8 enemies whose unk60 is below it attack from half as far */
    s32 unk2EC;                     /* 0x2EC frames magic 0x52 to 0x54 last */
    s32 unk2F0;                     /* 0x2F0 10, 20 or 30 for magic 0x52, 0x53, 0x54 */
    char pad2F4[0x0C];              /* the block is 0x300 bytes (func_001B5810 clears and copies that many) */
} PlayerState;

/* D_00414660[7]: colour and life of the weapon trail, one per kind (D_00447188) */
typedef struct {
    s32 red;                        /* 0x00 */
    s32 green;                      /* 0x04 */
    s32 blue;                       /* 0x08 */
    s32 alpha;                      /* 0x0C of the newest point; it fades with age */
    s64 blend;                      /* 0x10 GS ALPHA_1 value */
    s32 lifetime;                   /* 0x18 frames a point stays queued */
    char pad1C[0x04];
} WeaponTrailKind;

/* D_00414580 (0x48 bytes): the colours laid over the screen for what happens to the player; each
   strength is the alpha func_001B6800 draws with */
typedef struct {
    s32 unk00;                      /* 0x00 set to 10 by func_001BB360 */
    s32 unk04;                      /* 0x04 red (dark red while unk14 shows) */
    s32 unk08;                      /* 0x08 green, shown when kind is 0 */
    s32 unk0C;                      /* 0x0C dark yellow, shown when kind is 1 */
    s32 unk10;                      /* 0x10 black, subtracted */
    s32 unk14;                      /* 0x14 purple, shown when kind is 3 */
    s32 unk18;                      /* 0x18 white, shown when kind is 4 */
    s32 red;                        /* 0x1C the colour shown when kind is 5 */
    s32 green;                      /* 0x20 */
    s32 blue;                       /* 0x24 */
    s32 unk28;                      /* 0x28 its strength */
    s32 kind;                       /* 0x2C which of the tints is shown */
    s32 unk30;                      /* 0x30 black, subtracted */
    s32 unk34;                      /* 0x34 0x80 to 0xA0: rises with MapDrawState.unk2A90 (func_001B69D0) */
    char pad38[0x10];
} PlayerScreenTint;

/* PlayerState.unk1C8 as the bit-fields the game sets */
typedef struct {
    u64 pad0 : 33;
    u64 bit33 : 1;
    u64 bit34 : 1;                  /* set: func_001BD420 does nothing */
    u64 bit35 : 1;                  /* running: the run button is held while moving (func_001B5A48) */
    u64 bit36 : 1;                  /* on an object set up by func_001BD420 (a ladder, it seems) */
    u64 pad37 : 1;
    u64 bit38 : 1;                  /* cleared at the start of every played frame, with bit39 */
    u64 bit39 : 1;
    u64 pad40 : 24;
} PlayerStatusBits;

/* the start of a PlayerState with unk1C8 as a word and bit-fields: accesses through it are based on the
   block's address, as retail's are in func_001B5A48 */
typedef struct {
    char pad000[0x1C8];
    s32 step_sounded;               /* 0x1C8 set once the footstep of the current stride (PlayerState.unk1C0) has sounded */
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

#define PLAYER_STATUS_BITS(player) ((PlayerStatusBits *)&(player)->unk1C8)

/* D_0043E130[100]: the experience a level needs (PlayerState.unk50 against unk54) and what reaching it adds */
typedef struct {
    s32 experience;                 /* 0x00 */
    u8 unk04;                       /* 0x04 added to PlayerState.unk20 */
    u8 unk05;                       /* 0x05 added to PlayerState.unk24 */
    u8 unk06;                       /* 0x06 added to PlayerState.unk28 */
    u8 unk07;                       /* 0x07 added to PlayerState.unk2C */
} PlayerLevelStep;

/* D_0041E118[160]: 0x40-byte definitions read from a data file (func_00112648), one per LevelRecord of D_003682F0 */
typedef struct {
    s16 unk00[9];                   /* 0x00 the base of the nine values; LevelRecord.unk00 is added (func_001BB728) */
    u16 unk12[6];                   /* 0x12 added to PlayerState.unkDC while the item is worn (func_001BB020) */
    char pad1E[0x01];
    u8 bonus_kinds[2];              /* 0x1F what wearing the item does, 0: nothing (the switch of func_001BB020) */
    u8 bonus_amounts[2];            /* 0x21 */
    char pad23[0x1C];
    u8 unk3F;                       /* 0x3F added to PlayerItemDef.unk7C for the shield in hand (func_001D94F8) */
} LevelRecordDef;

/* D_003682F0[160]: what has been gained per item (0x22 bytes each) */
typedef struct {
    s16 unk00[9];                   /* 0x00 added to the PlayerItemDef's row (func_001BA310) */
    u8 level;                       /* 0x12 */
    u8 exp;                         /* 0x13 */
    u8 unk14;                       /* 0x14 */
    char pad15[0x01];
    s16 unk16;                      /* 0x16 ini "item_": the third number times 20, 0 to 480 (func_00207D68) */
    u8 unk18;                       /* 0x18 nonzero: func_001ED9A8 lists the item among those whose time unk19 counts */
    char unk19[8];                  /* 0x19 a ClockTime (kf_game.h), not aligned */
    char pad21[0x01];
} LevelRecord;

/* D_0041B9B8[]: 0x90-byte definitions, PlayerState.unk3C picks one */
typedef struct {
    PlayerShotStats unk00[3];       /* 0x00 one row per LevelRecord.unk14 */
    PlayerItemRates unk36;          /* 0x36 */
    u16 unk42;                      /* 0x42 0xFFFF: none */
    u16 unk44;                      /* 0x44 the swing's motion frame on which magic unk42 is cast (func_001B70F8) */
    u16 unk46;                      /* 0x46 0xFFFF: none */
    u16 unk48;                      /* 0x48 the motion frame from which PlayerState.unk260_1 is set */
    u16 unk4A;                      /* 0x4A first and last motion frame on which magic unk42 may be asked for */
    u16 unk4C;                      /* 0x4C */
    u16 unk4E;                      /* 0x4E frames magic unk42 stays (D_00447114) */
    char pad50[0x02];
    u16 unk52;                      /* 0x52 0xFFFF: none */
    u16 unk54;                      /* 0x54 as unk44 to unk4E, for magic unk52 */
    u16 unk56;                      /* 0x56 */
    u16 unk58;                      /* 0x58 */
    u16 unk5A;                      /* 0x5A */
    u16 unk5C;                      /* 0x5C */
    u16 unk5E;                      /* 0x5E */
    char pad60[0x02];
    u8 unk62;                       /* 0x62 index into D_00369830: nonzero there lets the item launch its shot (func_001B77E0) */
    u8 unk63;                       /* 0x63 */
    u8 bonus_kinds[2];              /* 0x64 as LevelRecordDef.bonus_kinds */
    u8 bonus_amounts[2];            /* 0x66 */
    u16 unk68;                      /* 0x68 shown on the trade screen in tenths of a foot (func_001E7010) */
    char pad6A[0x02];
    f32 unk6C;                      /* 0x6C twice Unk1B1470Request.unk44 of a swing, before D_00446240 (func_001B70F8) */
    f32 unk70;                      /* 0x70 twice Unk1B1470Request.unk3C of a swing, before D_00446240 */
    f32 unk74;                      /* 0x74 how far a swing reaches, less 100 */
    u16 unk78;                      /* 0x78 the motion frame from which a held shot is ready (func_001B7918) */
    u16 unk7A;                      /* 0x7A the frame the drawing motion stops at */
    u8 unk7C;                       /* 0x7C which D_005CEE60 motion the weapon shown on the equipment screen plays (func_001D94F8) */
    u8 unk7D;                       /* 0x7D which D_0042A290 function draws it there */
    u8 unk7E;                       /* 0x7E goes to PlayerState.unkC4 when a swing starts (func_001B70F8) */
    u8 unk7F;                       /* 0x7F copied to Unk1B1470Request.unk6E */
    u16 unk80;                      /* 0x80 0xFFFF: no sound while drawing */
    u16 unk82;                      /* 0x82 the motion frame the drawing sound starts on */
    char pad84[0x02];
    u8 unk86;                       /* 0x86 copied to Unk1B1470Request.unk6F */
    s8 unk87;                       /* 0x87 unk87 to unk89 become D_00414740 (x, y, z) when the item's model is loaded */
    s8 unk88;                       /* 0x88 */
    s8 unk89;                       /* 0x89 */
    u16 unk8A;                      /* 0x8A the least PlayerState.unkAC for magic unk42 (func_001B70F8) */
    u16 unk8C;                      /* 0x8C the least PlayerState.unkB0 for magic unk42 */
    char pad8E[0x02];
} PlayerItemDef;

/* D_00414780, D_004147A0: points 30 and 31 of the held item's model, this frame's and the frame before's */
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

/* D_004147F8[0x17C]: the fixed definitions, 0x40 bytes each. D_00417EF8 is the run of
   them from index 220 on (the items func_001B4DF8 reads). */
typedef struct {
    char pad00[0x30];
    u32 unk30_0 : 18;               /* 0x30 */
    u32 state : 2;                  /* 0x32 bits 2-3: 0 is set to 1 when the table is reset */
    u32 unk30_20 : 12;
    char pad34[0x02];
    u8 unk36;                       /* 0x36 minutes that must pass since LevelRecord.unk19 (func_001ED9A8) */
    u8 exp_per_level;               /* 0x37 */
    char pad38[0x04];
    u8 unk3C;                       /* 0x3C */
    char pad3D[0x01];
    u16 unk3E;                      /* 0x3E summed into PlayerState.unkA4 for everything worn */
} LevelDef;

/* D_0042B810[]: the keys of ini/NNNN.ini ("hp=", "equip_weapon=", "item_", ...) and what reads each; a NULL handler ends it */
typedef struct {
    char key[0x40];                 /* 0x00 the start of the line */
    void (*handler)(char *);        /* 0x40 gets the text after the "=" */
} IniKeyHandler;

#endif
