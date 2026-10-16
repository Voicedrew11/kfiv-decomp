/* Sound: the state of the sound driver layer (kf_sound.c, the EE side of FromSoftware's fssnd driver on
   the IOP): voices (header and sample data), sequences, reverb settings, the command list sent to the IOP
   each frame; and the types of the game's sound layer (kf_sound_game.c). */
#ifndef KF_SOUND_H
#define KF_SOUND_H

#include "common.h"
#include "kf_model.h"

/* one of the 16 voices, SoundState.voices[n]: a header (two copies on the IOP) and its sample data
   (an IOP copy at iop_heap, sent on to sound memory at unk0C) */
typedef struct {
    int unk00;                      /* 0x00 IOP block of the header, the stereo copy; 0 when the voice is free */
    int unk04;                      /* 0x04 IOP block of the mono copy (driver command 0x1070 makes it) */
    int iop_heap;                   /* 0x08 IOP copy of the samples, 0 when freed */
    int unk0C;                      /* 0x0C where the samples are in sound memory */
    int unk10;                      /* 0x10 size of each header copy */
    int unk14;                      /* 0x14 size of the block at iop_heap */
    s16 unk18;                      /* 0x18 the driver's id for the voice */
    char unk1A;                     /* 0x1A the transfer channel of the samples (commands 0x80, 0x90) */
    char unk1B;                     /* 0x1B */
    int unk1C;                      /* 0x1C 1: the stereo copy is in use, 0: the mono one */
} SoundVoice;

/* SoundState.banks[4]: a sequence (the .sq data of a BGM, on the IOP) and the voice it plays */
typedef struct {
    int iop_heap;                   /* 0x00 IOP memory, 0 when freed */
    int unk04;                      /* 0x04 the size of the data at iop_heap */
    int voice;                      /* 0x08 */
    int unk0C;                      /* 0x0C the driver's id for the sequence */
} SoundBank;

/* the request SoundDriver_VoiceLoadBind reads: the voice to reload and the heap to free */
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

/* the arguments of SoundDriver_LoadVoiceStep as its step functions get them */
typedef struct {
    int unk00;                      /* 0x00 the voice */
    int unk04;                      /* 0x04 the voice's first data */
    int unk08;                      /* 0x08 its size */
    char *unk0C;                    /* 0x0C where the next piece is read from */
    int unk10;                      /* 0x10 */
    int unk14;                      /* 0x14 */
    int unk18;                      /* 0x18 */
} SoundLoadArgs;

/* 0x18 bytes of reverb settings, copied whole (byte-aligned) */
typedef struct {
    char bytes[0x18];
} SoundEffectParams;

/* SoundEffectParams field by field: the reverb settings SoundDriver_AddReverb stores (the layout of
   sceSdEffectAttr: core, mode, depth, delay, feedback) */
typedef struct {
    int unk00;                      /* 0x00 the SPU2 core; -1 when the slot is free */
    int unk04;                      /* 0x04 the reverb mode, with 0x100 (clear the work area) added */
    s16 unk08;                      /* 0x08 */
    s16 unk0A;                      /* 0x0A */
    int unk0C;                      /* 0x0C */
    int unk10;                      /* 0x10 */
    s16 unk14;                      /* 0x14 */
    s16 unk16;                      /* 0x16 */
} SoundEffectFields;

/* sound_rpc_buffer: the record SoundDriver_Remote sends the sound driver; the reply lands on it */
typedef struct {
    void *self;                     /* 0x00 the record itself, then the driver's answer */
    int args[63];                   /* 0x04 */
} SoundRpcRequest;

/* sound_upload: a block being sent to the IOP in pieces of 0x4000 bytes (SoundDriver_StartUpload, SoundDriver_StepUpload) */
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

/* SoundState.unk240[n] with its signs: the note sounding in slot n (SoundDriver_NoteOn fills it) */
typedef struct {
    s8 voice;                       /* 0x00 index into SoundState.voices */
    u8 unk01;                       /* 0x01 */
    u8 unk02;                       /* 0x02 */
    s8 unk03;                       /* 0x03 1 while it sounds, -1 once stopped (SoundDriver_NoteOff) */
} SoundKey;

/* the sound driver state (sound_state_storage in kf_sound.c) */
typedef struct {
    SoundVoice voices[16];          /* 0x000 */
    SoundBank banks[4];             /* 0x200 */
    s8 unk240[126][4];              /* 0x240 all -1 after SoundDriver_Init */
    char pad438[0x08];
    SoundEffectParams effects[4];   /* 0x440 stored reverb settings */
    SoundEffectParams effect;       /* 0x4A0 the one sent to the driver last */
    void *load_addr;                /* 0x4B8 first IOP heap load's address */
    int unk4BC;                     /* 0x4BC */
    u32 unk4C0;                     /* 0x4C0 end of the used sound memory */
    int dma_src;                    /* 0x4C4 */
    int unk4C8;                     /* 0x4C8 */
    int dma_dest;                   /* 0x4CC */
    char unk4D0[0x06];              /* 0x4D0 u16 at 0x4D4: 1 stereo output, 0 mono */
    u16 active;                     /* 0x4D6 */
    char pad4D8[0x08];              /* the block in .bss is 0x4E0 bytes */
} SoundState;

/* sound_load_queue: the sound loads waiting to run, one at a time (GameLoop_RunSoundLoads). Other files
   still declare it as int[]/char[], so the extern stays in each file. */
typedef struct {
    int unk00;                      /* 0x00 first argument of Sound_LoadBgm */
    int unk04;                      /* 0x04 nonzero: a step 3 load is wanted; Sound_LoadBgm clears it */
    int step;                       /* 0x08 0: idle, 1-3: which load is running */
    int unk0C;                      /* 0x0C nonzero: a step 1 load is wanted; Sound_LoadMapBankStep clears it */
    struct MapSlot *unk10;          /* 0x10 the map slot whose sound bank the step 1 load loads */
    struct MapSlot *unk14;          /* 0x14 the next unk10, taken once unk10 is NULL */
    int unk18;                      /* 0x18 nonzero: a step 2 load is wanted; Sound_LoadEntityBankStep clears it */
    int unk1C;                      /* 0x1C arguments of Sound_LoadEntityBankStep */
    int unk20;                      /* 0x20 */
    int unk24;                      /* 0x24 */
} SoundLoadQueue;

/* a voice's envelope as SoundDriver_GetAdsr unpacks it: the two SPU2 ADSR registers (ADSR1 in the upper
   half: attack mode and rate, decay rate, sustain level; ADSR2 below: sustain mode and rate, release mode and
   rate), the rates turned round */
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

/* bgm_cues[], map_bank_cues[]: a reverb setting from rev_para.dat, the arguments of SoundDriver_AddReverb */
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

/* a sound job (Sound_EntityBankLoadVoice to Sound_LoadJob): its data, two file names, the
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

/* the start arguments Sound_LoadEntityBankStep copies into a SoundJobSpec */
typedef struct {
    int size_b;                     /* 0x00 */
    char pad04[0x10];
    int buffer_b;                   /* 0x14 match: an address, read as an int */
    int buffer_a;                   /* 0x18 match: an address, read as an int */
    int size_a;                     /* 0x1C */
} SoundStartArgs;

/* SoundJobSpec as Sound_LoadEntityBankStep, Sound_LoadMapBankStep and Sound_LoadMapBank fill it in: the two buffers
   are pointers of two different types in the game, which is what lets gcc reorder the copies */
typedef struct {
    char *buffer_a;                 /* 0x00 */
    void *buffer_b;                 /* 0x04 */
    int unk08;                      /* 0x08 */
    int size_a;                     /* 0x0C */
    int size_b;                     /* 0x10 */
} SoundBankSpec;

/* SoundJobSpec as Sound_LoadEntityBankStep fills it in: there the two buffers are pointers of one type
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

/* SoundStartArgs with the buffers as the pointers they are (Sound_LoadEntityBankStep) */
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

/* map_reverb_cue_indices: 0x78 cue indices (0 to 3) into map_bank_cues; Sound_MapBankAddReverb copies
   the whole table and takes entry 1 (cue_index) */
typedef struct {
    u8 unk00;                       /* 0x00 */
    s8 cue_index;                   /* 0x01 index into the cue table */
    char pad02[0x76];
} SoundScriptState;

/* sound_channels[128]: a sound channel (0x40 bytes); unk3A is -1 when it is free.
   tagged_requests[128] are queued requests of the same shape; there unk38 is -1 when free. */
typedef struct {
    Vector4Arg origin;              /* 0x00 */
    Vector4Arg target;              /* 0x10 */
    char pad20[0x10];
    s32 unk30;                      /* 0x30 */
    f32 volume;                     /* 0x34 */
    s8 unk38;                       /* 0x38 */
    s8 unk39;                       /* 0x39 */
    s8 unk3A;                       /* 0x3A */
    s8 unk3B;                       /* 0x3B voice handed to SoundDriver_SetNoteControl */
    s8 unk3C;                       /* 0x3C 1: a point at origin, 2: a segment origin..target; 0x63: restarts with the cue's volume */
    char pad3D[0x03];
} SoundChannel;

/* kf_sound.c */
/* Starts the sound driver and resets its state; returns 0 */
int SoundDriver_Init(void);
/* Turns the output off, stops both streams and frees every sequence and voice */
void SoundDriver_ReleaseAll(void);
/* The state of the driver's RPC client: nonzero while a call is still running */
int SoundDriver_IsBusy(void);
/* Sends a command and its arguments to the driver on the IOP and returns its answer */
int SoundDriver_Remote(int blocking, ...);
/* Keeps an upload to the IOP going; 1 once nothing is left to send */
int SoundDriver_StepUpload(void);
/* Turns the sound output on, once */
void SoundDriver_EnableOutput(void);
/* Mono output: every voice and sequence on its mono header copy, every pan centred */
void SoundDriver_SetMono(void);
/* Stereo output: every voice and sequence on its stereo header copy */
void SoundDriver_SetStereo(void);
/* Loads a voice from memory into a given voice and place in sound memory, a step a call; -3 until done */
int SoundDriver_LoadVoiceStep(int voice_index, int header_data, int header_size, int sample_data, int sample_size,
                              int unk1B, int dest);
/* Loads a voice from memory into a given voice and place in sound memory; the voice, or -1 */
int SoundDriver_LoadVoiceAt(int voice_index, int header_data, int header_size, int sample_data, int sample_size,
                            int unk1B, int dest);
/* Nonzero once the sample transfer on a channel has finished */
int SoundDriver_IsTransferDone(int channel);
/* Frees a voice: its sample copy and both header copies */
int SoundDriver_FreeVoice(u32 voice_index);
/* Frees the IOP copy of a voice's samples once they are in sound memory */
int SoundDriver_FreeSampleBuffer(int voice_index);
/* Sets the volume of a voice */
int SoundDriver_SetVoiceVolume(u32 voice_index, s16 value);
/* Loads a sequence from memory into the first free sequence slot, a step a call; -1 until done */
int SoundDriver_LoadSequenceStep(int bank_id, int voice_index, int data, u32 size);
/* Frees a sequence's IOP memory */
int SoundDriver_FreeSequence(int bank_index);
/* Starts a sequence playing */
int SoundDriver_PlaySequence(u32 bank_index, int value);
/* Stops a sequence */
int SoundDriver_StopSequence(u32 bank_index);
/* Sets the volume of a sequence */
int SoundDriver_SetSequenceVolume(int bank_index, s16 value);
/* Queues a note on a voice in note slot `slot`, with its volume, bend and pan */
int SoundDriver_NoteOn(u32 slot, int voice_index, char channel, char program, char note, char volume, s16 bend, char pan);
/* Queues the end of the note in a slot */
int SoundDriver_NoteOff(u32 key_index);
/* Queues a volume (kind 0), pan (1) or bend (2) change for the note in a slot */
int SoundDriver_SetNoteControl(int kind, u32 slot, int value);
/* Sends the queued note commands to the IOP; once a frame */
int SoundDriver_FlushCommands(void);
/* Starts streaming a file on a stream */
int SoundDriver_PlayStreamFile(int stream, char *file_name, int value);
/* Stores reverb settings in a free slot; the slot, or -2 */
int SoundDriver_AddReverb(int effect_flags, s16 param1, s16 param2, s16 param3, s16 param4, s16 param5, s16 param6,
                          int effect_id);
/* Frees a reverb slot */
int SoundDriver_FreeReverb(u32 effect_index);
/* Sends a reverb slot's settings to the driver */
void SoundDriver_ApplyReverb(u32 effect_index);

#endif
