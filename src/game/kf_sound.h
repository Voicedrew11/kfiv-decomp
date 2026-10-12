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
    char unk1A;                     /* 0x1A */
    char unk1B;                     /* 0x1B */
    int unk1C;                      /* 0x1C */
} SoundVoice;

/* SoundState.banks[4]: a bank and the voice it is bound to */
typedef struct {
    int iop_heap;                   /* 0x00 IOP memory, 0 when freed */
    char pad04[0x04];
    int voice;                      /* 0x08 */
    int unk0C;                      /* 0x0C */
} SoundBank;

/* the request func_001CCF80 reads: the voice to reload and the heap to free */
typedef struct {
    SoundVoice *voice;              /* 0x00 */
    char pad04[0x0C];
    int heap_addr;                  /* 0x10 */
    int unk14;                      /* 0x14 */
} SoundReloadRequest;

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
    void *load_addr;                /* 0x4B8 first IOP heap load's address */
    char pad4BC[0x08];
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

/* D_0043EE78: the sound script state (0x78 bytes) */
typedef struct {
    u8 unk00;                       /* 0x00 */
    s8 cue_index;                   /* 0x01 index into the cue table */
    char pad02[0x76];
} SoundScriptState;

/* D_003EF710[128]: a sound channel (0x40 bytes); unk3A is -1 when it is free.
   D_00500170[128] are queued requests of the same shape; there unk38 is -1 when free. */
typedef struct {
    f32 origin[3];                  /* 0x00 */
    char pad0C[0x04];
    f32 target[3];                  /* 0x10 */
    char pad1C[0x14];
    s32 unk30;                      /* 0x30 */
    f32 volume;                     /* 0x34 */
    s8 unk38;                       /* 0x38 */
    s8 unk39;                       /* 0x39 */
    s8 unk3A;                       /* 0x3A */
    s8 unk3B;                       /* 0x3B voice handed to func_001CE9A0 */
    s8 unk3C;                       /* 0x3C 0x63: restarts with the cue's volume */
    char pad3D[0x03];
} SoundChannel;

/* the voice record func_0021C350 sets up for a stream upload */
typedef struct {
    s32 unk00;                      /* 0x00 */
    char pad04[0x40];
    s32 unk44;                      /* 0x44 */
    char pad48[0x08];
    s32 unk50;                      /* 0x50 */
    char pad54[0x04];
    s32 unk58;                      /* 0x58 */
} StreamVoice;

#endif
