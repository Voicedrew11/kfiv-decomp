/* Sound: the driver state at *D_004471D4 (voices, banks, effect settings,
   the IOP DMA of the status block) and the sound cue table. */
#ifndef KF_SOUND_H
#define KF_SOUND_H

#include "common.h"

/* one of the 16 voices, SoundState.voices[n] */
typedef struct {
    int unk00;                      /* 0x00 */
    int unk04;                      /* 0x04 */
    int iop_heap;                   /* 0x08 IOP memory, 0 when freed */
    int unk0C;                      /* 0x0C */
    int unk10;                      /* 0x10 */
    char pad14[0x04];
    s16 unk18;                      /* 0x18 */
    char pad1A;
    char unk1B;                     /* 0x1B */
    int unk1C;                      /* 0x1C */
} SoundVoice;

/* SoundState.banks[4]: a bank and the voice it is bound to */
typedef struct {
    char pad00[0x08];
    int voice;                      /* 0x08 */
    int unk0C;                      /* 0x0C */
} SoundBank;

/* 0x18 bytes of effect settings, copied whole (byte-aligned) */
typedef struct {
    char bytes[0x18];
} SoundEffectParams;

/* what D_004471D4 points at. Most files still declare D_004471D4 as char *,
   so the extern stays in each file. */
typedef struct {
    SoundVoice voices[16];          /* 0x000 */
    SoundBank banks[4];             /* 0x200 */
    char pad240[0x200];
    SoundEffectParams effects[4];   /* 0x440 */
    SoundEffectParams effect;       /* 0x4A0 the one in use */
    char pad4B8[0x0C];
    int dma_src;                    /* 0x4C4 */
    int unk4C8;                     /* 0x4C8 */
    int dma_dest;                   /* 0x4CC */
    char unk4D0[0x06];              /* 0x4D0 */
    u16 active;                     /* 0x4D6 */
} SoundState;

/* a voice status word as func_001CF1F0 unpacks it */
typedef struct {
    u32 bits0_4 : 5;
    u32 bit5 : 1;
    u32 bits6_12 : 7;
    u32 bits13_15 : 3;
    u32 bits16_19 : 4;
    u32 bits20_23 : 4;
    u32 bits24_30 : 7;
    u32 bit31 : 1;
} SoundVoiceStatus;

/* D_004FF558[]: a sound cue, the arguments of func_001CEE78 */
typedef struct {
    int flags;                      /* 0x00 */
    s16 unk04;                      /* 0x04 */
    s16 unk06;                      /* 0x06 */
    s16 unk08;                      /* 0x08 */
    s16 unk0A;                      /* 0x0A */
    s16 unk0C;                      /* 0x0C */
    s16 unk0E;                      /* 0x0E */
} SoundCue;

/* the data a SoundJob loads or plays: two buffers and their sizes */
typedef struct {
    int buffer_a;                   /* 0x00 */
    int buffer_b;                   /* 0x04 */
    int unk08;                      /* 0x08 */
    int size_a;                     /* 0x0C */
    int size_b;                     /* 0x10 */
} SoundJobSpec;

/* a sound job (func_001A0B10 to func_001A1458): its data, two file names, the
   driver's result and two more arguments for the driver call */
typedef struct {
    SoundJobSpec *spec;             /* 0x00 */
    char *name_a;                   /* 0x04 sprintf format or file name */
    char *name_b;                   /* 0x08 */
    char pad0C[0x04];
    int result;                     /* 0x10 the voice or step the driver returned */
    char pad14[0x04];
    int unk18;                      /* 0x18 */
    int unk1C;                      /* 0x1C */
} SoundJob;

/* a 0x60-byte sound command (D_0043EE78) */
typedef struct {
    char pad00;
    u8 kind;                        /* 0x01 */
    char pad02[0x32];
    int *params;                    /* 0x34 four words for a SoundJobSpec */
    char pad38[0x28];
} SoundCommand;

/* the start arguments func_001A0C18 copies into a SoundJobSpec */
typedef struct {
    int unk00;                      /* 0x00 */
    char pad04[0x10];
    int unk14;                      /* 0x14 */
    int unk18;                      /* 0x18 */
    int unk1C;                      /* 0x1C */
} SoundStartArgs;

/* a load buffer: a 64-byte aligned base, its limit and a fill count */
typedef struct {
    u32 base;                       /* 0x00 */
    u32 limit;                      /* 0x04 */
    int used;                       /* 0x08 */
} LoadBuffer;

#endif
