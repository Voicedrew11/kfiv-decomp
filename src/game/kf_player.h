/* The player: the state block at D_00413F80 and the level/experience tables. */
#ifndef KF_PLAYER_H
#define KF_PLAYER_H

#include "common.h"
#include "kf_model.h"

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
    char pad5C[0x48];
    s32 unkA4;                      /* 0xA4 */
    s32 unkA8;                      /* 0xA8 */
    char padAC[0x08];
    s32 unkB4;                      /* 0xB4 */
    s32 unkB8;                      /* 0xB8 saved */
    s32 unkBC;                      /* 0xBC cleared when unk3C changes over a menu (func_001017A8) */
    s32 unkC0;                      /* 0xC0 cleared when unk264 is set */
    char padC4[0x10];
    s32 unkD4;                      /* 0xD4 saved */
    s32 unkD8;                      /* 0xD8 saved */
    char padDC[0xCC];
    u8 unk1A8;                      /* 0x1A8 nonzero: the main loop skips func_00210D48 */
    char pad1A9[0x03];
    f32 unk1AC;                     /* 0x1AC */
    f32 unk1B0;                     /* 0x1B0 */
    f32 unk1B4;                     /* 0x1B4 */
    f32 unk1B8;                     /* 0x1B8 */
    f32 unk1BC;                     /* 0x1BC */
    char pad1C0[0x04];
    f32 unk1C4;                     /* 0x1C4 added to the camera's height */
    u64 unk1C8;                     /* 0x1C8 status bits; bit 38 is tested */
    char pad1D0[0x0C];
    f32 unk1DC;                     /* 0x1DC */
    char pad1E0[0x10];
    f32 unk1F0;                     /* 0x1F0 added to the camera's height */
    char pad1F4[0x0C];
    s32 unk200;                     /* 0x200 */
    s32 unk204;                     /* 0x204 */
    s32 unk208;                     /* 0x208 */
    s32 unk20C;                     /* 0x20C */
    char pad210[0x10];
    Vector4 aim;                    /* 0x220 direction the player aims along, flattened to x/z */
    f32 aim_speed;                  /* 0x230 above 0: func_001005E8 draws func_00106FB8(0x40) */
    char pad234[0x1C];
    u8 unk250;                      /* 0x250 0xFF after a map change */
    char pad251[0x01];
    u16 map_index;                  /* 0x252 picks the MapSlot record */
    u32 unk254;                     /* 0x254 */
    char pad258[0x09];
    u8 unk261;                      /* 0x261 */
    char pad262[0x01];
    u8 unk263;                      /* 0x263 */
    u8 unk264;                      /* 0x264 set from unk3D, unk46 or the PlayerItemDef after a menu (func_001017A8) */
    char pad265[0x33];
    u8 unk298;                      /* 0x298 */
    char pad299[0x03];
    s32 unk29C;                     /* 0x29C nonzero: the screen is tinted by it (func_001005E8) */
    s32 unk2A0;                     /* 0x2A0 a total func_001B4EC8 adds to */
    char pad2A4[0x04];
    f32 unk2A8;                     /* 0x2A8 fade amount: > 0 toward white, < 0 toward black */
    char pad2AC[0x0C];
    u16 flag_timers[6];             /* 0x2B8 frames left, one per timed_flags entry */
    char pad2C4[0x20];
    s32 unk2E4;                     /* 0x2E4 above 0: unk2E8 counts (func_0018F648) */
    f32 unk2E8;                     /* 0x2E8 enemies whose unk60 is below it attack from half as far */
} PlayerState;

/* D_003682F0[160]: what has been gained per item (0x22 bytes each) */
typedef struct {
    char pad00[0x12];
    u8 level;                       /* 0x12 */
    u8 exp;                         /* 0x13 */
    u8 unk14;                       /* 0x14 */
    char pad15[0x0D];
} LevelRecord;

/* D_0041B9B8[]: 0x90-byte definitions, PlayerState.unk3C picks one */
typedef struct {
    char pad00[0x42];
    u16 unk42;                      /* 0x42 0xFFFF: none */
    char pad44[0x0E];
    u16 unk52;                      /* 0x52 0xFFFF: none */
    char pad54[0x0F];
    u8 unk63;                       /* 0x63 */
    char pad64[0x2C];
} PlayerItemDef;

/* D_004147F8[0x17C]: the fixed definitions, 0x40 bytes each. D_00417EF8 is the run of
   them from index 220 on (the items func_001B4DF8 reads). */
typedef struct {
    char pad00[0x30];
    u32 unk30_0 : 18;               /* 0x30 */
    u32 state : 2;                  /* 0x32 bits 2-3: 0 is set to 1 when the table is reset */
    u32 unk30_20 : 12;
    char pad34[0x03];
    u8 exp_per_level;               /* 0x37 */
    char pad38[0x04];
    u8 unk3C;                       /* 0x3C */
    char pad3D[0x03];
} LevelDef;

#endif
