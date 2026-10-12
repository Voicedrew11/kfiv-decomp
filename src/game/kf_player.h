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
    char pad20[0x10];
    s16 timed_flags[6];             /* 0x30 held at 0 while the matching timer runs */
    u8 unk3C;                       /* 0x3C row of D_0041B9B8 (0x90 bytes each) */
    char pad3D[0x05];
    u8 unk42;                       /* 0x42 */
    char pad43[0x02];
    u8 unk45;                       /* 0x45 0x9C: enemy attacks reach half as far (func_0018F648) */
    char pad46[0x12];
    s32 unk58;                      /* 0x58 */
    char pad5C[0x48];
    s32 unkA4;                      /* 0xA4 */
    s32 unkA8;                      /* 0xA8 */
    char padAC[0x08];
    s32 unkB4;                      /* 0xB4 */
    char padB8[0x04];
    s32 unkBC;                      /* 0xBC */
    char padC0[0xEC];
    f32 unk1AC;                     /* 0x1AC */
    f32 unk1B0;                     /* 0x1B0 */
    f32 unk1B4;                     /* 0x1B4 */
    char pad1B8[0x10];
    u64 unk1C8;                     /* 0x1C8 status bits; bit 38 is tested */
    char pad1D0[0x0C];
    f32 unk1DC;                     /* 0x1DC */
    char pad1E0[0x20];
    s32 unk200;                     /* 0x200 */
    s32 unk204;                     /* 0x204 */
    char pad208[0x18];
    Vector4 aim;                    /* 0x220 direction the player aims along, flattened to x/z */
    f32 aim_speed;                  /* 0x230 */
    char pad234[0x1E];
    u16 map_index;                  /* 0x252 picks the MapSlot record */
    char pad254[0x0D];
    u8 unk261;                      /* 0x261 */
    char pad262[0x01];
    u8 unk263;                      /* 0x263 */
    char pad264[0x34];
    u8 unk298;                      /* 0x298 */
    char pad299[0x07];
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
    char pad14[0x0E];
} LevelRecord;

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
