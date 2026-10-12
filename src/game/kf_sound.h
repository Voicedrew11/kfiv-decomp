/* Sound: the driver state at *sound_state (voices, banks, effect settings,
   the IOP DMA of the status block) and the sound cue table. */
#ifndef KF_SOUND_H
#define KF_SOUND_H

#include "common.h"
#include "kf_model.h"

/* one of the 16 voices, SoundState.voices[n] */
typedef struct {
    int unk00;                      /* 0x00 */
    int unk04;                      /* 0x04 */
    int iop_heap;                   /* 0x08 IOP memory, 0 when freed */
    int unk0C;                      /* 0x0C */
    int unk10;                      /* 0x10 */
    int unk14;                      /* 0x14 size of the block at iop_heap */
    s16 unk18;                      /* 0x18 */
    char unk1A;                     /* 0x1A */
    char unk1B;                     /* 0x1B */
    int unk1C;                      /* 0x1C */
} SoundVoice;

/* SoundState.banks[4]: a bank and the voice it is bound to */
typedef struct {
    int iop_heap;                   /* 0x00 IOP memory, 0 when freed */
    int unk04;                      /* 0x04 func_001CE300's last argument (a size, it seems) */
    int voice;                      /* 0x08 */
    int unk0C;                      /* 0x0C */
} SoundBank;

/* the request func_001CCF80 reads: the voice to reload and the heap to free */
typedef struct {
    SoundVoice *voice;              /* 0x00 */
    int unk04;                      /* 0x04 */
    int unk08;                      /* 0x08 bytes still to send */
    int unk0C;                      /* 0x0C where the next piece goes */
    int heap_addr;                  /* 0x10 */
    int unk14;                      /* 0x14 */
    int unk18;                      /* 0x18 */
    int unk1C;                      /* 0x1C the running DMA transfer */
} SoundReloadRequest;

/* the arguments of func_001CD020 as its step functions get them */
typedef struct {
    int unk00;                      /* 0x00 the voice */
    int unk04;                      /* 0x04 the voice's first data */
    int unk08;                      /* 0x08 its size */
    char *unk0C;                    /* 0x0C where the next piece is read from */
    int unk10;                      /* 0x10 */
    int unk14;                      /* 0x14 */
    int unk18;                      /* 0x18 */
} SoundLoadArgs;

/* 0x18 bytes of effect settings, copied whole (byte-aligned) */
typedef struct {
    char bytes[0x18];
} SoundEffectParams;

/* SoundEffectParams as func_001CB778 clears it, field by field */
typedef struct {
    int unk00;                      /* 0x00 */
    int unk04;                      /* 0x04 */
    s16 unk08;                      /* 0x08 */
    s16 unk0A;                      /* 0x0A */
    int unk0C;                      /* 0x0C */
    int unk10;                      /* 0x10 */
    s16 unk14;                      /* 0x14 */
    s16 unk16;                      /* 0x16 */
} SoundEffectFields;

/* sound_rpc_buffer: the record func_001CBAE0 sends the sound driver; the reply lands on it */
typedef struct {
    void *self;                     /* 0x00 the record itself, then the driver's answer */
    int args[63];                   /* 0x04 */
} SoundRpcRequest;

/* sound_upload: a block being sent to the IOP in pieces of 0x4000 bytes (func_001CBC30, func_001CBD20) */
typedef struct {
    int dest;                       /* 0x00 */
    int src;                        /* 0x04 */
    int remaining;                  /* 0x08 */
    int padded_size;                /* 0x0C the piece's size rounded up to 16 */
    int piece_size;                 /* 0x10 */
    int dma_id;                     /* 0x14 */
    int active;                     /* 0x18 */
} SoundUpload;

/* sound_dma, sound_command_dma: the DMA descriptor handed to sceSifSetDma */
typedef struct {
    int src;                        /* 0x00 */
    int dest;                       /* 0x04 */
    int size;                       /* 0x08 */
    int mode;                       /* 0x0C */
} SoundDmaRequest;

/* SoundState.unk240[n] with its signs: the note sounding in slot n (func_001CE690 fills it) */
typedef struct {
    s8 voice;                       /* 0x00 index into SoundState.voices */
    u8 unk01;                       /* 0x01 */
    u8 unk02;                       /* 0x02 */
    s8 unk03;                       /* 0x03 1 while it sounds, -1 once stopped (func_001CE8B0) */
} SoundKey;

/* the sound driver state (sound_state_storage in kf_sound.c) */
typedef struct {
    SoundVoice voices[16];          /* 0x000 */
    SoundBank banks[4];             /* 0x200 */
    s8 unk240[126][4];              /* 0x240 all -1 after func_001CB778 */
    char pad438[0x08];
    SoundEffectParams effects[4];   /* 0x440 */
    SoundEffectParams effect;       /* 0x4A0 the one in use */
    void *load_addr;                /* 0x4B8 first IOP heap load's address */
    int unk4BC;                     /* 0x4BC */
    u32 unk4C0;                     /* 0x4C0 end of the used sound memory */
    int dma_src;                    /* 0x4C4 */
    int unk4C8;                     /* 0x4C8 */
    int dma_dest;                   /* 0x4CC */
    char unk4D0[0x06];              /* 0x4D0 */
    u16 active;                     /* 0x4D6 */
    char pad4D8[0x08];              /* the block in .bss is 0x4E0 bytes */
} SoundState;

/* D_0038A130: the sound loads waiting to run, one at a time (func_00101618). Other files
   still declare it as int[]/char[], so the extern stays in each file. */
typedef struct {
    int unk00;                      /* 0x00 first argument of func_0019F628 */
    int unk04;                      /* 0x04 nonzero: a step 3 load is wanted; func_0019F628 clears it */
    int step;                       /* 0x08 0: idle, 1-3: which load is running */
    int unk0C;                      /* 0x0C nonzero: a step 1 load is wanted; func_001A1048 clears it */
    struct MapSlot *unk10;          /* 0x10 the map slot whose sound bank the step 1 load loads */
    struct MapSlot *unk14;          /* 0x14 the next unk10, taken once unk10 is NULL */
    int unk18;                      /* 0x18 nonzero: a step 2 load is wanted; func_001A0C18 clears it */
    int unk1C;                      /* 0x1C arguments of func_001A0C18 */
    int unk20;                      /* 0x20 */
    int unk24;                      /* 0x24 */
} SoundLoadQueue;

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

/* the data a SoundJob loads or plays: two buffers and their sizes. The two
   buffers have different pointer types in retail (the copies into a spec are
   scheduled as if they cannot alias), so they stay u8 * and void *. */
typedef struct {
    u8 *buffer_a;                   /* 0x00 */
    void *buffer_b;                 /* 0x04 */
    int unk08;                      /* 0x08 */
    int size_a;                     /* 0x0C */
    int size_b;                     /* 0x10 */
} SoundJobSpec;

/* four words that fill a SoundJobSpec: what MapSlot.unk34 points at */
typedef struct SoundSpecSource {
    int size_b;                     /* 0x00 */
    void *buffer_b;                 /* 0x04 */
    int size_a;                     /* 0x08 */
    u8 *buffer_a;                   /* 0x0C */
} SoundSpecSource;

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

/* the start arguments func_001A0C18 copies into a SoundJobSpec */
typedef struct {
    int size_b;                     /* 0x00 */
    char pad04[0x10];
    int buffer_b;                   /* 0x14 match: an address, read as an int */
    int buffer_a;                   /* 0x18 match: an address, read as an int */
    int size_a;                     /* 0x1C */
} SoundStartArgs;

/* SoundJobSpec as func_001A0C18, func_001A1048 and func_001A10D8 fill it in: the two buffers
   are pointers of two different types in the game, which is what lets gcc reorder the copies */
typedef struct {
    char *buffer_a;                 /* 0x00 */
    void *buffer_b;                 /* 0x04 */
    int unk08;                      /* 0x08 */
    int size_a;                     /* 0x0C */
    int size_b;                     /* 0x10 */
} SoundBankSpec;

/* SoundJobSpec as func_001A0C18 fills it in: there the two buffers are pointers of one type
   and the sizes are ints, which fixes the order of its loads and stores */
typedef struct {
    void *buffer_a;                 /* 0x00 */
    void *buffer_b;                 /* 0x04 */
    int unk08;                      /* 0x08 */
    int size_a;                     /* 0x0C */
    int size_b;                     /* 0x10 */
} SoundStartSpec;

/* what MapSlot.unk34 points at: the sound bank of a map, two buffers and their sizes */
typedef struct {
    int size_b;                     /* 0x00 */
    void *buffer_b;                 /* 0x04 */
    int size_a;                     /* 0x08 */
    char *buffer_a;                 /* 0x0C */
} SoundBankInfo;

/* SoundStartArgs with the buffers as the pointers they are (func_001A0C18) */
typedef struct {
    int size_b;                     /* 0x00 */
    char pad04[0x10];
    void *buffer_b;                 /* 0x14 */
    char *buffer_a;                 /* 0x18 */
    int size_a;                     /* 0x1C */
} SoundStartInfo;

/* a load buffer: a 64-byte aligned base, its limit and a fill count */
typedef struct {
    u32 base;                       /* 0x00 */
    u32 limit;                      /* 0x04 */
    int used;                       /* 0x08 */
} SoundFileBuffer;

/* D_0043EE78: the sound script state (0x78 bytes) */
typedef struct {
    u8 unk00;                       /* 0x00 */
    s8 cue_index;                   /* 0x01 index into the cue table */
    char pad02[0x76];
} SoundScriptState;

/* D_003EF710[128]: a sound channel (0x40 bytes); unk3A is -1 when it is free.
   D_00500170[128] are queued requests of the same shape; there unk38 is -1 when free. */
typedef struct {
    Vector4Arg origin;              /* 0x00 */
    Vector4Arg target;              /* 0x10 */
    char pad20[0x10];
    s32 unk30;                      /* 0x30 */
    f32 volume;                     /* 0x34 */
    s8 unk38;                       /* 0x38 */
    s8 unk39;                       /* 0x39 */
    s8 unk3A;                       /* 0x3A */
    s8 unk3B;                       /* 0x3B voice handed to func_001CE9A0 */
    s8 unk3C;                       /* 0x3C 1: a point at origin, 2: a segment origin..target; 0x63: restarts with the cue's volume */
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

/* kf_sound.c */
int func_001CBAE0(int blocking, ...);
int func_001CD6E0(u32 voice_index);
int func_001CE408(int bank_index);

#endif
