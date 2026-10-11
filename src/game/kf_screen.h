/* Screen effects: fades, alpha-blended overlays and the screen wobble table. */
#ifndef KF_SCREEN_H
#define KF_SCREEN_H

#include "common.h"

/* D_0038D680: the screen fade */
typedef struct {
    char pad00[0x28];
    int timer;                      /* 0x28 frames left */
} ScreenFade;

/* what func_0010AEB8 draws, with its alpha at 0x15C */
typedef struct {
    char pad00[0x15C];
    f32 alpha;                      /* 0x15C below 128: drawn without z writes */
} OverlaySprite;

/* D_00471DE0[16]: the screen wobble, one sine wave per entry */
typedef struct {
    f32 amp_x;                      /* 0x00 */
    f32 amp_y;                      /* 0x04 */
    s16 x;                          /* 0x08 amp_x * sin(phase), 4.12 fixed point */
    s16 y;                          /* 0x0A */
    f32 phase;                      /* 0x0C */
} WaveOscillator;

/* what D_00446978 points at */
typedef struct {
    char pad00[0x82];
    u16 unk82;                      /* 0x82 */
} DisplaySettings;

typedef struct {
    char pad00[0xFC];
    DisplaySettings *settings;      /* 0xFC */
} DisplayContext;

#endif
