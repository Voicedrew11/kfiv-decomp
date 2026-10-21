/* Sound: the state of the sound driver layer (sound/sound.c, the EE side of FromSoftware's fssnd driver on
   the IOP): voices (header and sample data), sequences, reverb settings, the command list sent to the IOP
   each frame; and the types of the game's sound layer (sound/sound_game.c). */
#ifndef KF_SOUND_H
#define KF_SOUND_H

#include "common.h"
#include "model/model.h"

/* SoundDriver_Remote command bits: the reply is taken into a buffer; a buffer is sent with the command */
#define SOUND_COMMAND_REPLY        0x80000
#define SOUND_COMMAND_SENDS_BUFFER 0x40000

/* a MIDI data byte: 7 bits, the low half of a 14-bit bend or value */
#define MIDI_DATA_MASK             0x7F

/* one of the 16 voices, SoundState.voices[n]: a header (two copies on the IOP) and its sample data
   (an IOP copy at iop_heap, sent on to sound memory at sample_spu_addr) */
typedef struct {
    int stereo_header_block;        /* 0x00 IOP block of the header, the stereo copy; 0 when the voice is free */
    int mono_header_block;          /* 0x04 IOP block of the mono copy (driver command 0x1070 makes it) */
    int iop_heap;                   /* 0x08 IOP copy of the samples, 0 when freed */
    int sample_spu_addr;            /* 0x0C where the samples are in sound memory */
    int header_size;                /* 0x10 size of each header copy */
    int sample_size;                /* 0x14 size of the block at iop_heap */
    s16 driver_voice_id;            /* 0x18 the driver's id for the voice */
    char dma_channel;               /* 0x1A the transfer channel of the samples (commands 0x80, 0x90) */
    char voice_flags;               /* 0x1B flag sent with the voice load commands */
    int stereo_selected;            /* 0x1C 1: the stereo copy is in use, 0: the mono one */
} SoundVoice;

/* SoundState.banks[4]: a sequence (the .sq data of a BGM, on the IOP) and the voice it plays */
typedef struct {
    int iop_heap;                   /* 0x00 IOP memory, 0 when freed */
    int iop_size;                   /* 0x04 the size of the data at iop_heap */
    int voice;                      /* 0x08 */
    int sequence_id;                /* 0x0C the driver's id for the sequence */
} SoundBank;

/* the request SoundDriver_VoiceLoadBind reads: the voice to reload and the heap to free */
typedef struct {
    SoundVoice *voice;              /* 0x00 */
    int cleared_on_header_start;    /* 0x04 zeroed when a header load starts */
    int bytes_remaining;            /* 0x08 bytes still to send */
    int next_spu_addr;              /* 0x0C where the next piece goes in sound memory */
    int heap_addr;                  /* 0x10 */
    int voice_index;                /* 0x14 the voice being loaded */
    int piece_size;                 /* 0x18 the size of the piece being sent */
    int dma_id;                     /* 0x1C the running DMA transfer */
} SoundReloadRequest;

/* the arguments of SoundDriver_LoadVoiceStep as its step functions get them */
typedef struct {
    int voice_index;                /* 0x00 the voice */
    int header_data;                /* 0x04 the voice's first data */
    int header_size;                /* 0x08 its size */
    char *next_sample_data;         /* 0x0C where the next piece is read from */
    int sample_size;                /* 0x10 the samples' size */
    int voice_flags;                /* 0x14 copied into the voice's flags */
    int spu_dest;                   /* 0x18 where the samples go in sound memory */
} SoundLoadArgs;

/* 0x18 bytes of reverb settings, copied whole (byte-aligned) */
typedef struct {
    char bytes[0x18];
} SoundEffectParams;

/* SoundEffectParams field by field: the reverb settings SoundDriver_AddReverb stores (the layout of
   sceSdEffectAttr: core, mode, depth, delay, feedback) */
typedef struct {
    int core;                       /* 0x00 the SPU2 core; -1 when the slot is free */
    int mode;                       /* 0x04 the reverb mode, with 0x100 (clear the work area) added */
    s16 depth_l;                    /* 0x08 the left depth */
    s16 depth_r;                    /* 0x0A the right depth */
    int delay;                      /* 0x0C the reverb delay */
    int feedback;                   /* 0x10 the reverb feedback */
    s16 aux_level_l;                /* 0x14 fifth reverb parameter, stored */
    s16 aux_level_r;                /* 0x16 sixth reverb parameter, stored */
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

/* SoundState.note_keys[n] with its signs: the note sounding in slot n (SoundDriver_NoteOn fills it) */
typedef struct {
    s8 voice;                       /* 0x00 index into SoundState.voices */
    u8 channel;                     /* 0x01 the MIDI channel of the note */
    u8 note;                        /* 0x02 the note number */
    s8 sounding;                    /* 0x03 1 while it sounds, -1 once stopped */
} SoundKey;

/* the sound driver state (sound_state_storage in sound/sound.c) */
typedef struct {
    SoundVoice voices[16];          /* 0x000 */
    SoundBank banks[4];             /* 0x200 */
    s8 note_keys[126][4];           /* 0x240 the note slots, 4 bytes each (SoundKey): all -1 after SoundDriver_Init */
    char pad438[0x08];
    SoundEffectParams effects[4];   /* 0x440 stored reverb settings */
    SoundEffectParams effect;       /* 0x4A0 the one sent to the driver last */
    void *load_addr;                /* 0x4B8 first IOP heap load's address */
    int sample_area_start;          /* 0x4BC where the samples start in sound memory */
    u32 spu_used_end;               /* 0x4C0 end of the used sound memory */
    int dma_src;                    /* 0x4C4 */
    int iop_buffer;                 /* 0x4C8 the IOP buffer the streams are read into */
    int dma_dest;                   /* 0x4CC */
    char volume_and_stereo[0x06];   /* 0x4D0 six bytes: a volume int and a stereo u16 */
    u16 active;                     /* 0x4D6 */
    char pad4D8[0x08];              /* the block in .bss is 0x4E0 bytes */
} SoundState;

/* sound_load_queue: the sound loads waiting to run, one at a time (GameLoop_RunSoundLoads) */
typedef struct {
    int bgm_id;                     /* 0x00 first argument of Sound_LoadBgm */
    int bgm_load_pending;           /* 0x04 nonzero: a step 3 load is wanted; Sound_LoadBgm clears it */
    int step;                       /* 0x08 0: idle, 1-3: which load is running */
    int map_bank_load_pending;      /* 0x0C nonzero: a step 1 load is wanted; Sound_LoadMapBankStep clears it */
    struct MapSlot *pending_map_slot; /* 0x10 the map slot whose sound bank the step 1 load loads */
    struct MapSlot *next_map_slot;  /* 0x14 the next pending_map_slot, taken once pending_map_slot is NULL */
    int entity_bank_load_pending;   /* 0x18 nonzero: a step 2 load is wanted; Sound_LoadEntityBankStep clears it */
    int entity_bank_args;           /* 0x1C arguments of Sound_LoadEntityBankStep */
    int entity_slot_index;          /* 0x20 entity pool slot the step 2 load binds its voice to */
    int entity_slot_voice_out;      /* 0x24 pool slot whose voice byte takes the loaded voice index */
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
    s16 depth_l;                    /* 0x04 the left depth */
    s16 depth_r;                    /* 0x06 the right depth */
    s16 delay;                      /* 0x08 the reverb delay */
    s16 feedback;                   /* 0x0A the reverb feedback */
    s16 aux_level_l;                /* 0x0C fifth reverb parameter from rev_para */
    s16 aux_level_r;                /* 0x0E sixth reverb parameter from rev_para */
} SoundCue;

/* the data a SoundJob loads or plays: two buffers and their sizes. The two
   buffers have different pointer types in retail (the copies into a spec are
   scheduled as if they cannot alias), so they stay u8 * and void *. */
typedef struct {
    u8 *buffer_a;                   /* 0x00 */
    void *buffer_b;                 /* 0x04 */
    int pad08;                      /* 0x08 */
    int size_a;                     /* 0x0C */
    int size_b;                     /* 0x10 */
} SoundJobSpec;

/* four words that fill a SoundJobSpec: what MapSlot.sound_bank points at */
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
    int voice_index;                /* 0x18 the voice the load binds */
    int spu_dest;                   /* 0x1C where the samples go in sound memory */
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
    int pad08;                      /* 0x08 */
    int size_a;                     /* 0x0C */
    int size_b;                     /* 0x10 */
} SoundStartSpec;

/* what MapSlot.sound_bank points at: the sound bank of a map, two buffers and their sizes */
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

/* a bank's slot: its buffer and the two file sizes (SoundFileBuffer alone is the 0xC bytes the loaders use) */
typedef struct {
    SoundFileBuffer buffer;         /* 0x00 */
    int size_a;                     /* 0x0C */
    int size_b;                     /* 0x10 */
    int extra_size;                 /* 0x14 extra size of the BGM buffers */
    char pad18[0x08];               /* 0x18 */
} SoundBankSlot;

/* a job's slot: the job and the bank it loads */
typedef struct {
    SoundJob job;                   /* 0x00 */
    SoundBank bank;                 /* 0x20 */
} SoundJobSlot;

/* map_reverb_cue_indices: 0x78 cue indices (0 to 3) into map_bank_cues; Sound_MapBankAddReverb copies
   the whole table and takes entry 1 (cue_index) */
typedef struct {
    u8 pad00;                       /* 0x00 */
    s8 cue_index;                   /* 0x01 index into the cue table */
    char pad02[0x76];
} SoundScriptState;

/* sound_channels[128]: a sound channel (0x40 bytes); sound_id is -1 when it is free.
   tagged_requests[128] are queued requests of the same shape; there voice is -1 when free
   and mode and kind hold each other's values. */
typedef struct {
    Vector4Arg origin;              /* 0x00 */
    Vector4Arg target;              /* 0x10 */
    char pad20[0x10];
    s32 volume_level;               /* 0x30 the volume the driver is given, 0 to 127 */
    f32 volume;                     /* 0x34 */
    s8 voice;                       /* 0x38 the voice the sound plays (a queued request: 1 when queued, -1 when free) */
    s8 mode;                        /* 0x39 the mode the caller passed (0 to 8) */
    s8 sound_id;                    /* 0x3A the sound number; -1 when the channel is free */
    s8 note_slot;                   /* 0x3B the note slot the channel plays in (a queued request keeps its voice here) */
    s8 kind;                        /* 0x3C 1: a point at origin, 2: a segment origin..target; 0x63: restarts with the cue's volume */
    char pad3D[0x03];
} SoundChannel;

/* sound/sound.c */
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

/* sound/sound_load_queue.c */
extern int bgm_load_pending[3];

/* sound/sound_rpc.c */
extern int sound_rpc_args[];
extern char sound_rpc_file_name[];

/* sound/sound_tables.c */
extern int bgm_header_buffer[];
extern int bgm_sequence_buffer[];
extern SoundBankSpec map_bank_files;
extern f32 sound_cos_polynomial[4];
extern u8 voice_ranges[][2];
extern void *tracked_sound_players[];
extern SoundChannel sound_channels[];

/* data still in asm */
extern char bgm_sq_format[];
extern char bgm_hd_format[];
extern char bgm_bd_format[];
extern char se_com_hd_path[];
extern char se_com_bd_path[];
extern char sfx_com_hd_path[];
extern char sfx_com_bd_path[];
extern char map_bank_hd_path[];
extern char map_bank_bd_path[];
extern char sw_bank_hd_format[];
extern char sw_bank_bd_format[];
extern char mg_bank_hd_format[];
extern char mg_bank_bd_format[];
extern char rpc_timeout_message[];
extern char voice_load_waiting_message[];
extern int voice_load_failures;
extern int voice_load_step;
extern char sample_buffer_freed_message[];
extern int sequence_load_step;

/* sound/sound_load_queue.c */
extern SoundLoadQueue sound_load_queue;

#endif
