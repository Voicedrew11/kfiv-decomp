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
    char pad3D[0x16F];
    f32 unk1AC;                     /* 0x1AC */
    f32 unk1B0;                     /* 0x1B0 */
    f32 unk1B4;                     /* 0x1B4 */
    char pad1B8[0x9A];
    u16 map_index;                  /* 0x252 picks the MapSlot record */
    char pad254[0x0D];
    u8 unk261;                      /* 0x261 */
    char pad262[0x56];
    u16 flag_timers[6];             /* 0x2B8 frames left, one per timed_flags entry */
} PlayerState;

/* D_003682F0[160]: what has been gained per item (0x22 bytes each) */
typedef struct {
    char pad00[0x12];
    u8 level;                       /* 0x12 */
    u8 exp;                         /* 0x13 */
    char pad14[0x0E];
} LevelRecord;

/* D_00417EF8[]: the fixed definition per item (0x40 bytes each) */
typedef struct {
    char pad00[0x37];
    u8 exp_per_level;               /* 0x37 */
    char pad38[0x08];
} LevelDef;

#endif
