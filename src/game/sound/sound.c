/* The sound driver layer: the EE side of FromSoftware's sound driver on the IOP (fssnd; its RPC wrapper
   names itself fssnd_Remote). Commands go over SIF RPC (SoundDriver_Remote); voices (a header and its
   samples, the samples sent on to sound memory), sequences, notes queued into a command list that is sent
   once a frame, streams, reverb settings. */
#include "core/load.h"
#include "sound/sound.h"

/* EE EABI varargs: the register save area a1..a7 is spilled to, as in libsdr.c */
typedef char *va_list;
#define va_start(ap, last) ((ap) = (char *)__builtin_next_arg(last) - 0x38)
#define va_arg(ap, type) (*(type *)((ap += 8) - 8))

static void SoundDriver_DisableOutput(void);
void FlushCache(int); // match: void as in eekernel.h; an int result changes SoundDriver_Init's registers
int sceSifBindRpc(int *, int, int);
int sceSifCheckStatRpc(int *);
int printf(const char *, ...);
int sceSifCallRpc(int *, int, int, char *, int, int *, int, int, int);
int sceSifSetDma(int *, int);
int sceSifDmaStat(int);
static int SoundDriver_GetFileSize(const char *);
static int SoundDriver_LoadIopFile(char *, void *);
int sceSifAllocIopHeap(int);
int sceSifFreeIopHeap(int);
int puts(char *);
char *strcpy(char *, const char *);
int sceClose(int);
int sceLseek(int, int, int);
int sceOpen(const char *, int);
/* SDK: loads an IOP heap file, returns below zero until the IOP is ready */
int sceSifLoadIopHeap(char *name, void *addr);
extern char rpc_timeout_message[];
extern char voice_load_waiting_message[];
extern int voice_load_step;
extern int voice_load_failures;
extern int (*const voice_load_steps[])(int *, int *);
extern int sound_rpc_args[];
extern char sample_buffer_freed_message[];
extern int sequence_load_step;
extern char sound_rpc_file_name[];


/* SoundDriver_LoadVoiceStep keeps the position in the sample data here between calls */
static int voice_load_source;                   /* 0x4471C0 */
/* the sequence SoundDriver_LoadSequenceStep is loading, step by step */
static int bank_load_index;                     /* 0x4471C4 */
static SoundBank *bank_load_bank;               /* 0x4471C8 */
static SoundVoice *bank_load_voice;             /* 0x4471CC */
static int bank_load_voice_data;                /* 0x4471D0 */
static SoundState *sound_state;                 /* 0x4471D4 points at sound_state_storage */

static int sound_command_list[256];             /* 0x5BEE80 commands for the driver, sent by SoundDriver_FlushCommands */
static SoundRpcRequest sound_rpc_buffer;        /* 0x5BF280 */
static SoundState sound_state_storage;          /* 0x5BF380 */
static SoundDmaRequest sound_dma;               /* 0x5BF860 */
static SoundUpload sound_upload;                /* 0x5BF870 */
static int sound_rpc_client[10];                /* 0x5BF890 sceSifClientData of the driver's RPC service */
static SoundDmaRequest sound_command_dma;       /* 0x5BF8B8 sends sound_command_list */

/* Starts the sound driver: binds its RPC service on the IOP, then resets the driver state,
   every voice, sequence and reverb slot. Returns 0. */
int SoundDriver_Init(void) {
    SoundVoice *voice;
    SoundBank *bank;
    SoundEffectFields *effect;
    s8 *entry;
    int i;

    do {
        if (sceSifBindRpc(sound_rpc_client, 0x1010, 0) < 0) {
            while (TRUE) {
            }
        }
        for (i = 10000; i != -1; i--) {
        }
    } while (sound_rpc_client[9] == 0);
    FlushCache(0);
    sound_state = &sound_state_storage;
    sound_state->dma_src = SoundDriver_Remote(1, 0x10);
    *(int *)sound_state->unk4D0 = 0x3FFF; // match: the int at 0x4D0; the header has six chars there
    SoundDriver_Remote(1, 0x30, 0x3FFF, 0x3FFF);
    sound_state->load_addr = NULL;
    sound_state->iop_buffer = SoundDriver_Remote(1, 0xA0);
    // match: retail stores these three as pointers (the header has them as integers)
    *(int **)&sound_state->dma_dest = sound_command_list;
    sound_command_list[0] = 0;
    // match: as above
    *(char **)&sound_state->unk4BC = *(char **)&sound_state->spu_used_end = (char *)0x5010;
    *(u16 *)&sound_state->unk4D0[4] = 1; // match: the u16 at 0x4D4
    sound_state->active = 0;
    voice = sound_state->voices;
    for (i = 15; i != -1; i--, voice++) {
        voice->stereo_header_block = voice->mono_header_block = voice->iop_heap = voice->sample_spu_addr = 0;
        voice->unk1B = 0;
        voice->driver_voice_id = 0;
        voice->dma_channel = 0;
        voice->sample_size = 0;
        voice->stereo_selected = 1;
    }
    bank = sound_state->banks;
    for (i = 3; i != -1; i--, bank++) {
        bank->iop_heap = 0;
        bank->sequence_id = 0;
        bank->iop_size = 0;
    }
    entry = sound_state->note_keys[0];
    for (i = 125; i != -1; i--, entry += 4) {
        entry[0] = entry[1] = entry[2] = entry[3] = -1;
    }
    effect = (SoundEffectFields *)sound_state->effects;
    for (i = 3; i != -1; i--, effect++) {
        effect->core = -1;
        effect->mode = effect->depth_l = effect->depth_r = effect->delay = effect->feedback = effect->unk14 = effect->unk16 = 0;
    }
    sound_upload.active = 0;
    FlushCache(0);
    return 0;
}

/* Turns the sound output off, stops both streams (command 0x3030), frees every sequence and voice and
   sends command 0x20. main calls it before each Sound_Init, the movie players before a movie. */
void SoundDriver_ReleaseAll(void) {
    SoundBank *bank;
    SoundVoice *voice;
    int i;

    SoundDriver_DisableOutput();
    SoundDriver_Remote(1, 0x3030, 0);
    SoundDriver_Remote(1, 0x3030, 1);
    bank = sound_state->banks;
    for (i = 0; i < 4; i++) {
        if (bank->iop_heap != 0) {
            SoundDriver_FreeSequence(i);
        }
        bank++;
    }
    voice = sound_state->voices;
    for (i = 0; i < 16; i++) {
        if (voice->stereo_header_block != 0) {
            SoundDriver_FreeVoice(i);
        }
        voice++;
    }
    SoundDriver_Remote(1, 0x20);
    FlushCache(0);
}

/* Waits until the driver's last RPC call has finished: 1, or -1 after 0x989680 tries (it then prints
   "sceSifCheckStatRpc() == 1 in fssnd_Remote"). */
static int SoundDriver_WaitRpc(void) {
    int retries = 0x989680;

    while (sceSifCheckStatRpc(sound_rpc_client) == 1) {
        if (retries < 0) {
            printf(rpc_timeout_message);
            return -1;
        }
        retries--;
    }
    return 1;
}

/* The state of the sound driver's RPC client: 1 while a call is still running. */
int SoundDriver_IsBusy(void) {
    return sceSifCheckStatRpc(sound_rpc_client);
}

/* Sends a command and its arguments to the sound driver on the IOP (the game's own sceSdRemote;
   fssnd_Remote in its timeout message) and returns the driver's answer. Commands with 0x80000 send
   the record and take the reply into a buffer, those with 0x40000 send a buffer. */
int SoundDriver_Remote(int blocking, ...) {
    va_list ap;
    int command;
    int no_wait;
    int result;
    int i;

    va_start(ap, blocking);
    sound_rpc_buffer.self = &sound_rpc_buffer;
    command = va_arg(ap, int);
    // match: copies eight words although the save area only holds six more
    for (i = 0; i < 8; i++) {
        sound_rpc_buffer.args[i] = va_arg(ap, int);
    }
    no_wait = FALSE;
    if (blocking == 0) {
        no_wait = TRUE;
    }
    result = SoundDriver_WaitRpc();
    if (result < 0) {
        return result;
    }
    if (command & 0x80000) {
        sceSifCallRpc(sound_rpc_client, command, no_wait, (char *)&sound_rpc_buffer, 0xA0, (int *)sound_rpc_buffer.args[0], 0xA0, 0,
                      sound_rpc_buffer.args[0]);
    } else if (command & 0x40000) {
        sceSifCallRpc(sound_rpc_client, command, no_wait, (char *)sound_rpc_buffer.args[0], 0x40, (int *)&sound_rpc_buffer, 0x10, 0,
                      (int)sound_rpc_buffer.self);
        result = (int)sound_rpc_buffer.self;
    } else {
        sceSifCallRpc(sound_rpc_client, command, no_wait, (char *)&sound_rpc_buffer, 0x40, (int *)&sound_rpc_buffer, 0x10, 0,
                      (int)&sound_rpc_buffer);
        result = (int)sound_rpc_buffer.self;
    }
    return result;
}

/* Starts sending size bytes at src to dest in sound memory, 0x4000 bytes at a time (SoundDriver_StepUpload
   sends the rest). Returns 0, or -1 if the DMA could not be queued. */
static int SoundDriver_StartUpload(int dest, int src, u32 size) {
    SoundDmaRequest *dma;
    int iop_buffer;
    int dma_id;

    sound_upload.src = src;
    sound_upload.dest = dest;
    sound_upload.active = 1;
    sound_upload.remaining = size;
    if (size > 0x4000) {
        sound_upload.piece_size = sound_upload.padded_size = 0x4000;
    } else {
        sound_upload.padded_size = size;
        sound_upload.piece_size = size;
        if (size & 0xF) {
            do {
                sound_upload.padded_size++;
            } while (sound_upload.padded_size & 0xF);
        }
    }
    dma = &sound_dma;
    iop_buffer = sound_state->iop_buffer;
    dma->src = sound_upload.src;
    dma->dest = iop_buffer;
    dma->size = sound_upload.padded_size;
    dma->mode = 0;
    FlushCache(0);
    dma_id = sceSifSetDma((int *)dma, 1);
    sound_upload.dma_id = dma_id;
    return (dma_id != 0) ? 0 : -1;
}

/* Keeps the upload SoundDriver_StartUpload started going: once the piece in flight has arrived it is
   written to sound memory and the next one is sent. Returns 1 when nothing is left to send. */
int SoundDriver_StepUpload(void) {
    SoundDmaRequest *dma;
    int iop_buffer;
    int dma_id;

    if (sound_upload.active == 0) {
        return 1;
    }
    if (sceSifDmaStat(sound_upload.dma_id) >= 0) {
        return 0;
    }
    SoundDriver_Remote(1, 0xB0, sound_upload.dest, sound_upload.piece_size);
    sound_upload.remaining -= sound_upload.piece_size;
    if (sound_upload.remaining <= 0) {
        sound_upload.active = 0;
        return 1;
    }
    sound_upload.src += 0x4000;
    sound_upload.dest += 0x4000;
    if (sound_upload.remaining > 0x4000) {
        sound_upload.piece_size = sound_upload.padded_size = 0x4000;
    } else {
        sound_upload.padded_size = sound_upload.remaining;
        sound_upload.piece_size = sound_upload.remaining;
        if (sound_upload.remaining & 0xF) {
            do {
                sound_upload.padded_size++;
            } while (sound_upload.padded_size & 0xF);
        }
    }
    dma = &sound_dma;
    iop_buffer = sound_state->iop_buffer;
    dma->src = sound_upload.src;
    dma->dest = iop_buffer;
    dma->size = sound_upload.padded_size;
    dma->mode = 0;
    FlushCache(0);
    dma_id = sceSifSetDma((int *)dma, 1);
    sound_upload.dma_id = dma_id;
    return 0;
}

/* Turns the sound output on, once. */
void SoundDriver_EnableOutput(void) {
    if (sound_state->active != TRUE) {
        SoundDriver_Remote(1, 0x40);
        sound_state->active = TRUE;
    }
}

/* Turns the sound output off if it is on. */
static void SoundDriver_DisableOutput(void) {
    if (sound_state->active != FALSE) {
        SoundDriver_Remote(1, 0x50);
        sound_state->active = FALSE;
    }
}

/* Mono output (the sound option): tells the driver (command 0xC0, 0) and switches every loaded voice
   and sequence to its mono header copy (unk04); SoundDriver_NoteOn then centres every pan. */
void SoundDriver_SetMono(void) {
    SoundVoice *voice;
    SoundBank *bank;
    int i;

    if (sound_state->active != 0) {
        SoundDriver_Remote(1, 0x50);
    }
    *(u16 *)&sound_state->unk4D0[4] = 0; // match: the u16 at 0x4D4; the header has six chars there
    SoundDriver_Remote(1, 0xC0, 0);
    voice = sound_state->voices;
    for (i = 0; i < 16; i++) {
        voice->stereo_selected = 0;
        if (voice->stereo_header_block == 0) {
            continue;
        }
        if (voice->sample_spu_addr == 0) {
            continue;
        }
        if (voice->mono_header_block == 0) {
            continue;
        }
        SoundDriver_Remote(1, 0x1010, voice->driver_voice_id, voice->sample_spu_addr, voice->mono_header_block, voice->unk1B);
        voice++; // match: retail only steps to the next voice after a reload
    }
    bank = sound_state->banks;
    for (i = 0; i < 4; i++) {
        if (bank->iop_heap == 0) {
            continue;
        }
        if (bank->voice == -1) {
            continue;
        }
        voice = &sound_state->voices[bank->voice];
        SoundDriver_Remote(1, 0x2070, bank->sequence_id, voice->sample_spu_addr, voice->mono_header_block, voice->unk1B, 0);
        bank++; // match: as above
    }
    if (sound_state->active != 0) {
        SoundDriver_Remote(1, 0x40);
    }
}

/* Stereo output (the sound option): tells the driver (command 0xC0, 1) and switches every loaded voice
   and sequence to its stereo header copy (unk00). */
void SoundDriver_SetStereo(void) {
    SoundVoice *voice;
    SoundBank *bank;
    int i;

    if (sound_state->active != 0) {
        SoundDriver_Remote(1, 0x50);
    }
    *(u16 *)&sound_state->unk4D0[4] = 1; // match: the u16 at 0x4D4; the header has six chars there
    SoundDriver_Remote(1, 0xC0, 1);
    voice = sound_state->voices;
    for (i = 0; i < 16; i++) {
        voice->stereo_selected = 1;
        if (voice->stereo_header_block == 0) {
            continue;
        }
        if (voice->sample_spu_addr == 0) {
            continue;
        }
        if (voice->mono_header_block == 0) {
            continue;
        }
        SoundDriver_Remote(1, 0x1010, voice->driver_voice_id, voice->sample_spu_addr, voice->stereo_header_block, voice->unk1B);
        voice++; // match: retail only steps to the next voice after a reload
    }
    bank = sound_state->banks;
    for (i = 0; i < 4; i++) {
        if (bank->iop_heap == 0) {
            continue;
        }
        if (bank->voice == -1) {
            continue;
        }
        voice = &sound_state->voices[bank->voice];
        SoundDriver_Remote(1, 0x2070, bank->sequence_id, voice->sample_spu_addr, voice->stereo_header_block, voice->unk1B, 1);
        bank++; // match: as above
    }
    if (sound_state->active != 0) {
        SoundDriver_Remote(1, 0x40);
    }
}

/* The index of the first free voice, or -1 when all 16 are taken. */
int SoundDriver_FindFreeVoice(void) {
    SoundVoice *voice = sound_state->voices;
    int i;

    for (i = 0; i < 16; i++, voice++) {
        if (voice->stereo_header_block == 0) {
            break;
        }
    }
    if (!(i < 16)) {
        return -1;
    }
    return i;
}

/* Loads a voice from files: the header file to its two IOP blocks and, if one is named, the
   sample file. Returns the voice, -2 when none is free, -1 on failure. */
int SoundDriver_LoadVoiceFiles(int voice_id, char *header_name, char *sample_name, int unk1B) {
    SoundVoice *voice = sound_state->voices;
    int voice_index;
    int block;
    int size;
    int i;

    for (i = 0; i < 16; i++, voice++) {
        if (voice->stereo_header_block == 0) {
            voice_index = i;
            break;
        }
    }
    if (!(i < 16)) {
        return -2;
    }
    if (voice_id == -1) {
        voice_id = voice_index;
    }
    voice->unk1B = unk1B;
    voice->driver_voice_id = voice_id;
    size = SoundDriver_GetFileSize(header_name);
    if (size < 0) {
        return -1;
    }
    voice->header_size = size;
    if (voice->stereo_header_block != 0) {
        sceSifFreeIopHeap(voice->stereo_header_block);
    }
    if (voice->mono_header_block != 0) {
        sceSifFreeIopHeap(voice->mono_header_block);
    }
    voice->stereo_header_block = sceSifAllocIopHeap(size);
    voice->mono_header_block = sceSifAllocIopHeap(size);
    if (voice->stereo_header_block == 0 || voice->mono_header_block == 0 || (u32)(voice->stereo_header_block + voice->header_size) > 0x1FFFFF ||
        (u32)(voice->mono_header_block + voice->header_size) > 0x1FFFFF) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    if (SoundDriver_LoadIopFile(header_name, (void *)voice->stereo_header_block) != 0) {
        return -1; // match: the voice is not released here (the other loaders call SoundDriver_FreeVoice first)
    }
    SoundDriver_Remote(1, 0x1070, voice->mono_header_block, voice->stereo_header_block, size);
    if (sample_name == NULL) {
        return voice_index;
    }
    size = SoundDriver_GetFileSize(sample_name);
    if (size < 0) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    voice->sample_size = size;
    if (voice->sample_spu_addr == 0) {
        voice->sample_spu_addr = sound_state->spu_used_end;
    }
    if ((u32)voice->sample_spu_addr >= sound_state->spu_used_end) {
        sound_state->spu_used_end = voice->sample_spu_addr + voice->sample_size;
    }
    voice->iop_heap = sceSifAllocIopHeap(size);
    if (voice->iop_heap == 0 || (u32)(voice->iop_heap + voice->sample_size) > 0x1FFFFF) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    if (SoundDriver_LoadIopFile(sample_name, (void *)voice->iop_heap) != 0) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    if (0x1E7FBF - voice->sample_spu_addr < voice->sample_size) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    if (voice->stereo_selected != 0) {
        block = voice->stereo_header_block;
    } else {
        block = voice->mono_header_block;
    }
    if (SoundDriver_Remote(1, 0x1010, voice->driver_voice_id, voice->sample_spu_addr, block, voice->unk1B) != 0) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    FlushCache(0);
    return voice_index;
}

/* Loads a voice from memory: header data to its two IOP blocks, sample data to sound memory.
   Returns the voice, -2 when none is free, -1 on failure. */
int SoundDriver_LoadVoice(int voice_id, int header_data, int header_size, int sample_data, int sample_size, int unk1B) {
    SoundVoice *voice = sound_state->voices;
    int voice_index;
    int block;
    int i;

    for (i = 0; i < 16; i++, voice++) {
        if (voice->stereo_header_block == 0) {
            voice_index = i;
            break;
        }
    }
    if (!(i < 16)) {
        return -2;
    }
    if (voice_id == -1) {
        voice_id = voice_index;
    } else if ((u32)voice_id >= 16) {
        return -1;
    }
    voice->driver_voice_id = voice_id;
    voice->unk1B = unk1B;
    while (header_size & 0xF) {
        header_size++;
    }
    voice->header_size = header_size;
    if (voice->stereo_header_block != 0) {
        sceSifFreeIopHeap(voice->stereo_header_block);
    }
    if (voice->mono_header_block != 0) {
        sceSifFreeIopHeap(voice->mono_header_block);
    }
    voice->stereo_header_block = sceSifAllocIopHeap(voice->header_size);
    voice->mono_header_block = sceSifAllocIopHeap(voice->header_size);
    if (voice->stereo_header_block == 0 || voice->mono_header_block == 0 || (u32)(voice->stereo_header_block + voice->header_size) > 0x1FFFFF ||
        (u32)(voice->mono_header_block + voice->header_size) > 0x1FFFFF) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    SoundDriver_StartUpload(voice->stereo_header_block, header_data, voice->header_size);
    while (SoundDriver_StepUpload() == 0) {
    }
    SoundDriver_Remote(1, 0x1070, voice->mono_header_block, voice->stereo_header_block, voice->header_size);
    while (sample_size & 0xF) {
        sample_size++;
    }
    voice->sample_size = sample_size;
    if (voice->sample_spu_addr == 0) {
        voice->sample_spu_addr = sound_state->spu_used_end;
    }
    if ((u32)voice->sample_spu_addr >= sound_state->spu_used_end) {
        sound_state->spu_used_end = voice->sample_spu_addr + voice->sample_size;
    }
    voice->iop_heap = sceSifAllocIopHeap(voice->sample_size);
    if (voice->iop_heap == 0 || (u32)(voice->iop_heap + voice->sample_size) > 0x1FFFFF) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    SoundDriver_StartUpload(voice->iop_heap, sample_data, voice->sample_size);
    while (SoundDriver_StepUpload() == 0) {
    }
    if (0x1E7FBF - voice->sample_spu_addr < voice->sample_size) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    if (voice->stereo_selected != 0) {
        block = voice->stereo_header_block;
    } else {
        block = voice->mono_header_block;
    }
    if (SoundDriver_Remote(1, 0x1010, voice->driver_voice_id, voice->sample_spu_addr, block, voice->unk1B) != 0) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    FlushCache(0);
    return voice_index;
}

/* Sets up a free voice with its own header data but the sample data of another voice.
   Returns the voice, -2 when none is free, -1 on failure. */
int SoundDriver_LoadVoiceSharedSamples(int voice_id, int source_index, int header_data, int header_size, int unk1B) {
    SoundVoice *source = &sound_state->voices[source_index];
    SoundVoice *voice = sound_state->voices;
    int voice_index;
    int block;
    int sample_size;
    int sample_addr;
    int i;

    for (i = 0; i < 16; i++, voice++) {
        if (voice->stereo_header_block == 0) {
            voice_index = i;
            break;
        }
    }
    if (!(i < 16)) {
        return -2;
    }
    if (voice_id == -1) {
        voice_id = voice_index;
    } else if ((u32)voice_id >= 16) {
        return -1;
    }
    voice->driver_voice_id = voice_id;
    voice->unk1B = unk1B;
    while (header_size & 0xF) {
        header_size++;
    }
    voice->header_size = header_size;
    if (voice->stereo_header_block != 0) {
        sceSifFreeIopHeap(voice->stereo_header_block);
    }
    if (voice->mono_header_block != 0) {
        sceSifFreeIopHeap(voice->mono_header_block);
    }
    voice->stereo_header_block = sceSifAllocIopHeap(voice->header_size);
    voice->mono_header_block = sceSifAllocIopHeap(voice->header_size);
    if (voice->stereo_header_block == 0 || voice->mono_header_block == 0 || (u32)(voice->stereo_header_block + voice->header_size) > 0x1FFFFF ||
        (u32)(voice->mono_header_block + voice->header_size) > 0x1FFFFF) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    SoundDriver_StartUpload(voice->stereo_header_block, header_data, voice->header_size);
    while (SoundDriver_StepUpload() == 0) {
    }
    SoundDriver_Remote(1, 0x1070, voice->mono_header_block, voice->stereo_header_block, voice->header_size);
    sample_size = source->sample_size;
    sample_addr = source->sample_spu_addr;
    voice->sample_size = sample_size;
    voice->sample_spu_addr = sample_addr;
    voice->dma_channel = source->dma_channel;
    if (voice->stereo_selected != 0) {
        block = voice->stereo_header_block;
    } else {
        block = voice->mono_header_block;
    }
    if (SoundDriver_Remote(1, 0x1010, voice->driver_voice_id, voice->sample_spu_addr, block, voice->unk1B) != 0) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    FlushCache(0);
    return voice_index;
}

/* Loads a voice from memory like SoundDriver_LoadVoice, but sends the sample data through a temporary
   IOP block, as large as the heap allows, a piece at a time.
   Returns the voice, -2 when none is free, -1 on failure. */
int SoundDriver_LoadVoiceBuffered(int voice_id, int header_data, int header_size, int sample_data, int sample_size, int unk1B) {
    SoundVoice *voice = sound_state->voices;
    u32 heap_size = 0x100000;
    int voice_index;
    int block;
    int i;
    int remaining;
    int dest;
    int heap;
    int sent;
    int padded_size;

    for (i = 0; i < 16; i++, voice++) {
        if (voice->stereo_header_block == 0) {
            voice_index = i;
            break;
        }
    }
    if (!(i < 16)) {
        return -2;
    }
    if (voice_id == -1) {
        voice_id = voice_index;
    } else if ((u32)voice_id >= 17) {
        return -1;
    }
    voice->driver_voice_id = voice_id;
    voice->unk1B = unk1B;
    while (header_size & 0xF) {
        header_size++;
    }
    voice->header_size = header_size;
    if (voice->stereo_header_block != 0) {
        sceSifFreeIopHeap(voice->stereo_header_block);
    }
    if (voice->mono_header_block != 0) {
        sceSifFreeIopHeap(voice->mono_header_block);
    }
    voice->stereo_header_block = sceSifAllocIopHeap(voice->header_size);
    voice->mono_header_block = sceSifAllocIopHeap(voice->header_size);
    if (voice->stereo_header_block == 0 || voice->mono_header_block == 0 || (u32)(voice->stereo_header_block + voice->header_size) > 0x1FFFFF ||
        (u32)(voice->mono_header_block + voice->header_size) > 0x1FFFFF) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    SoundDriver_StartUpload(voice->stereo_header_block, header_data, voice->header_size);
    while (SoundDriver_StepUpload() == 0) {
    }
    SoundDriver_Remote(1, 0x1070, voice->mono_header_block, voice->stereo_header_block, voice->header_size);
    while (sample_size & 0xF) {
        sample_size++;
    }
    remaining = sample_size;
    voice->sample_size = remaining;
    if (voice->sample_spu_addr == 0) {
        voice->sample_spu_addr = sound_state->spu_used_end;
    }
    if ((u32)voice->sample_spu_addr >= sound_state->spu_used_end) {
        padded_size = voice->sample_size;
        sound_state->spu_used_end = voice->sample_spu_addr + padded_size;
    } else {
        padded_size = voice->sample_size; // match: read in both arms, so one load serves the test below
    }
    dest = voice->sample_spu_addr;
    if (0x1E7FBF - dest < padded_size) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    do {
        heap_size >>= 1;
    } while ((heap = sceSifAllocIopHeap(heap_size)) == 0);
    if (sample_size > 0) {
        do {
            if (heap_size < (u32)remaining) {
                sound_dma.size = heap_size;
            } else {
                sound_dma.size = remaining;
            }
            sound_dma.src = sample_data;
            sound_dma.dest = heap;
            sound_dma.mode = 0;
            SoundDriver_Remote(1, 0x80, voice->dma_channel, 1);
            i = sceSifSetDma((int *)&sound_dma, 1); // match: the loop counter doubles as the DMA id
            if (i == 0) {
                sceSifFreeIopHeap(heap);
                SoundDriver_FreeVoice(voice_index);
                return -1;
            }
            while (sceSifDmaStat(i) >= 0) {
            }
            sent = SoundDriver_Remote(1, 0x90, voice->dma_channel, 0, heap, dest, sound_dma.size);
            if (sent != sound_dma.size) {
                sceSifFreeIopHeap(heap);
                SoundDriver_FreeVoice(voice_index);
                return -1;
            }
            sample_data += sent;
            dest += sent;
            remaining -= sent;
        } while (remaining > 0);
    }
    SoundDriver_Remote(1, 0x80, voice->dma_channel, 1);
    sceSifFreeIopHeap(heap);
    if (voice->stereo_selected != 0) {
        block = voice->stereo_header_block;
    } else {
        block = voice->mono_header_block;
    }
    if (SoundDriver_Remote(1, 0x1010, voice->driver_voice_id, voice->sample_spu_addr, block, voice->unk1B) != 0) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    FlushCache(0);
    return voice_index;
}

/* SoundDriver_LoadVoiceStep's first step: frees whatever the voice held. Returns 1 (on). */
int SoundDriver_VoiceLoadRelease(int *args) {
    SoundDriver_FreeVoice(args[0]);
    return 1;
}

/* SoundDriver_LoadVoiceStep: clears the request, takes the voice's two header blocks on the IOP and
   sends the header there (the mono copy made by command 0x1070). 1 (on), or -1 on failure. */
int SoundDriver_VoiceLoadSendHeader(SoundLoadArgs *args, SoundReloadRequest *request) {
    SoundVoice *voice;
    int voice_index;

    request->voice = NULL;
    request->unk04 = 0;
    request->bytes_remaining = 0;
    *(void **)&request->next_spu_addr = NULL; // match: next_spu_addr and heap_addr are stored as pointers here
    *(void **)&request->heap_addr = NULL; // match: as above
    request->voice_index = 0;
    request->piece_size = 0;
    request->dma_id = 0;
    voice_index = args->voice_index;
    voice = &sound_state->voices[voice_index];
    voice->driver_voice_id = args->voice_index;
    voice->unk1B = args->unk14;
    while (args->header_size & 0xF) {
        args->header_size++;
    }
    voice->header_size = args->header_size;
    voice->stereo_header_block = sceSifAllocIopHeap(voice->header_size);
    voice->mono_header_block = sceSifAllocIopHeap(voice->header_size);
    if (voice->stereo_header_block == 0 || voice->mono_header_block == 0 || (u32)(voice->stereo_header_block + voice->header_size) > 0x1FFFFF ||
        (u32)(voice->mono_header_block + voice->header_size) > 0x1FFFFF) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    SoundDriver_StartUpload(voice->stereo_header_block, args->header_data, voice->header_size);
    while (SoundDriver_StepUpload() == 0) {
    }
    SoundDriver_Remote(1, 0x1070, voice->mono_header_block, voice->stereo_header_block, voice->header_size);
    request->voice = voice;
    request->voice_index = voice_index;
    return 1;
}

/* SoundDriver_LoadVoiceStep: rounds the sample size up to 16, notes it and the place in sound memory,
   then takes the largest IOP block it can (halving from 1 MB) to send the samples through. Returns 1. */
int SoundDriver_VoiceLoadAllocBuffer(IopLoadRequest *request, IopLoadContext *context) {
    u32 heap_size = 0x100000;
    IopLoadBuffer *buffer;
    s32 unk18;
    s32 aligned;

    buffer = context->buffer;
    aligned = ((u32)(request->size + 0xF) >> 4) << 4;
    request->size = aligned;
    context->size = aligned;
    buffer->size = aligned;
    unk18 = request->unk18;
    buffer->unk0C = unk18;
    context->unk0C = unk18;
    do {
        heap_size >>= 1;
    } while ((context->heap = sceSifAllocIopHeap(heap_size)) == 0);
    context->heap_size = heap_size;
    return 1;
}

/* SoundDriver_LoadVoiceStep: once the last transfer to sound memory has finished, starts the DMA of the
   next piece of the samples to the IOP block. 1 (on), 0 while waiting ("waiting..."), -1 on failure. */
int SoundDriver_VoiceLoadSendPiece(SoundLoadArgs *args, SoundReloadRequest *request) {
    int dma_id;
    int heap_addr;
    u32 remaining;
    u32 piece_size;

    remaining = request->bytes_remaining;
    piece_size = request->piece_size;
    if (piece_size < remaining) {
        sound_dma.size = piece_size;
    } else {
        sound_dma.size = remaining;
    }
    heap_addr = request->heap_addr;
    sound_dma.src = (int)args->next_sample_data;
    sound_dma.dest = heap_addr;
    sound_dma.mode = 0;
    if (SoundDriver_Remote(1, 0x80, request->voice->dma_channel, 0) == 0) {
        printf(voice_load_waiting_message);
        return 0;
    }
    dma_id = sceSifSetDma((int *)&sound_dma, 1);
    if (dma_id == 0) {
        sceSifFreeIopHeap(request->heap_addr);
        SoundDriver_FreeVoice(request->voice_index);
        return -1;
    }
    request->dma_id = dma_id;
    return 1;
}

/* SoundDriver_LoadVoiceStep: once the piece has arrived on the IOP, sends it on to sound memory
   (command 0x90). 1 when all is sent, -1 (back a step) while more is left, 0 while the DMA runs. */
int SoundDriver_VoiceLoadWritePiece(SoundLoadArgs *args, SoundReloadRequest *request) {
    int sent;

    if (sceSifDmaStat(request->dma_id) >= 0) {
        return 0;
    }
    if (SoundDriver_Remote(1, 0x90, request->voice->dma_channel, 0, request->heap_addr, request->next_spu_addr, sound_dma.size) < 0) {
        sceSifFreeIopHeap(request->heap_addr);
        SoundDriver_FreeVoice(request->voice_index);
        return -1;
    }
    sent = sound_dma.size;
    args->next_sample_data += sent;
    request->next_spu_addr += sent;
    request->bytes_remaining -= sent;
    return (request->bytes_remaining <= 0) ? 1 : -1;
}

/* SoundDriver_LoadVoiceStep's last step: once the transfer to sound memory has finished, frees the IOP
   block and hands the voice to the driver (command 0x1010). 1 when done, 0 while waiting, -1 on failure. */
int SoundDriver_VoiceLoadBind(s32 unused, SoundReloadRequest *request) {
    SoundVoice *desc;
    s32 heap_src;

    desc = request->voice;
    if (SoundDriver_Remote(1, 0x80, desc->dma_channel, 0) != 0) {
        sceSifFreeIopHeap(request->heap_addr);
        if (desc->stereo_selected != 0) {
            heap_src = desc->stereo_header_block;
        } else {
            heap_src = desc->mono_header_block;
        }
        if (SoundDriver_Remote(1, 0x1010, desc->driver_voice_id, desc->sample_spu_addr, heap_src,
                          desc->unk1B) != 0) {
            SoundDriver_FreeVoice(request->voice_index);
            return -1;
        }
        FlushCache(0);
        return 1;
    }
    return 0;
}

/* SoundDriver_LoadVoiceAt a step a call, to be called until it stops returning -3: runs the current step
   (voice_load_steps: free the voice, send the header, take an IOP block, send a piece of the samples to it, send
   the piece on to sound memory, bind the voice) and moves on by what it returns. Once done, returns the
   voice the request holds; a failed step counts in voice_load_failures. */
int SoundDriver_LoadVoiceStep(int voice_index, int header_data, int header_size, int sample_data, int sample_size,
                              int unk1B, int dest) {
    int args[7];
    int advance;

    if (voice_load_step == 0) {
        voice_load_source = sample_data;
    }
    args[0] = voice_index;
    args[1] = header_data;
    args[2] = header_size;
    args[3] = voice_load_source;
    args[4] = sample_size;
    args[5] = unk1B;
    args[6] = dest;
    advance = voice_load_steps[voice_load_step](args, sound_rpc_args);
    voice_load_source = args[3];
    voice_load_step += advance;
    if (voice_load_step == 6) {
        voice_load_step = 0;
    }
    if (voice_load_step < 0) {
        voice_load_step = 0;
        voice_load_failures++;
    }
    if (voice_load_step == 0) {
        return sound_rpc_args[5];
    }
    return -3;
}

/* Loads a voice from memory like SoundDriver_LoadVoiceBuffered, into a voice and a place in sound memory the
   caller chooses. Returns the voice, or -1 on failure. */
int SoundDriver_LoadVoiceAt(int voice_index, int header_data, int header_size, int sample_data, int sample_size, int unk1B,
                  int dest) {
    SoundVoice *voice = &sound_state->voices[voice_index];
    u32 heap_size = 0x100000;
    int block;
    int i;
    int remaining;
    int heap;
    int sent;
    int write_addr;

    voice->unk1B = unk1B;
    voice->driver_voice_id = voice_index;
    while (header_size & 0xF) {
        header_size++;
    }
    voice->header_size = header_size;
    if (voice->stereo_header_block != 0) {
        sceSifFreeIopHeap(voice->stereo_header_block);
    }
    if (voice->mono_header_block != 0) {
        sceSifFreeIopHeap(voice->mono_header_block);
    }
    voice->stereo_header_block = sceSifAllocIopHeap(voice->header_size);
    voice->mono_header_block = sceSifAllocIopHeap(voice->header_size);
    if (voice->stereo_header_block == 0 || voice->mono_header_block == 0 || (u32)(voice->stereo_header_block + voice->header_size) > 0x1FFFFF ||
        (u32)(voice->mono_header_block + voice->header_size) > 0x1FFFFF) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    SoundDriver_StartUpload(voice->stereo_header_block, header_data, voice->header_size);
    while (SoundDriver_StepUpload() == 0) {
    }
    SoundDriver_Remote(1, 0x1070, voice->mono_header_block, voice->stereo_header_block, voice->header_size);
    while (sample_size & 0xF) {
        sample_size++;
    }
    remaining = sample_size;
    voice->sample_size = remaining;
    voice->sample_spu_addr = dest;
    write_addr = dest;
    do {
        heap_size >>= 1;
    } while ((heap = sceSifAllocIopHeap(heap_size)) == 0);
    if (sample_size > 0) {
        do {
            if (heap_size < (u32)remaining) {
                sound_dma.size = heap_size;
            } else {
                sound_dma.size = remaining;
            }
            sound_dma.src = sample_data;
            sound_dma.dest = heap;
            sound_dma.mode = 0;
            SoundDriver_Remote(1, 0x80, voice->dma_channel, 1);
            i = sceSifSetDma((int *)&sound_dma, 1); // match: the loop counter doubles as the DMA id
            if (i == 0) {
                sceSifFreeIopHeap(heap);
                SoundDriver_FreeVoice(voice_index);
                return -1;
            }
            while (sceSifDmaStat(i) >= 0) {
            }
            sent = SoundDriver_Remote(1, 0x90, voice->dma_channel, 0, heap, write_addr, sound_dma.size);
            if (sent != sound_dma.size) {
                sceSifFreeIopHeap(heap);
                SoundDriver_FreeVoice(voice_index);
                return -1;
            }
            sample_data += sent;
            write_addr += sent;
            remaining -= sent;
        } while (remaining > 0);
    }
    SoundDriver_Remote(1, 0x80, voice->dma_channel, 1);
    sceSifFreeIopHeap(heap);
    if (voice->stereo_selected != 0) {
        block = voice->stereo_header_block;
    } else {
        block = voice->mono_header_block;
    }
    if (SoundDriver_Remote(1, 0x1010, voice->driver_voice_id, voice->sample_spu_addr, block, voice->unk1B) != 0) {
        SoundDriver_FreeVoice(voice_index);
        return -1;
    }
    FlushCache(0);
    return voice_index;
}

/* Sets up a free voice from data that is already on the IOP: its first header block and its
   sample block. Returns the voice, -2 when none is free, -1 on failure. */
int SoundDriver_BindIopVoice(int voice_id, int header_block, int header_size, int sample_block, int sample_size, int unk1B) {
    SoundVoice *voice = sound_state->voices;
    int voice_index;
    int block;
    int i;

    for (i = 0; i < 16; i++, voice++) {
        if (voice->stereo_header_block == 0) {
            voice_index = i;
            break;
        }
    }
    if (!(i < 16)) {
        return -2;
    }
    if (voice_id == -1) {
        voice_id = voice_index;
    } else if ((u32)voice_id >= 17) {
        return -1;
    }
    voice->driver_voice_id = voice_id;
    voice->unk1B = unk1B;
    voice->header_size = header_size;
    voice->stereo_header_block = header_block;
    if (header_block == 0) {
        return -1;
    }
    voice->mono_header_block = sceSifAllocIopHeap(header_size);
    if (voice->mono_header_block == 0) {
        return -1; // match: the voice is not released here (the other loaders call SoundDriver_FreeVoice first)
    }
    if ((u32)(voice->mono_header_block + voice->header_size) > 0x1FFFFF) {
        return -1; // match: the voice is not released here (the other loaders call SoundDriver_FreeVoice first)
    }
    SoundDriver_Remote(1, 0x1070, voice->mono_header_block, voice->stereo_header_block, voice->header_size);
    voice->sample_size = sample_size;
    voice->iop_heap = sample_block;
    if (voice->sample_spu_addr == 0) {
        voice->sample_spu_addr = sound_state->spu_used_end;
    }
    if ((u32)voice->sample_spu_addr >= sound_state->spu_used_end) {
        sound_state->spu_used_end = voice->sample_spu_addr + voice->sample_size;
    }
    if (voice->iop_heap == 0) {
        return -1; // match: the voice is not released here (the other loaders call SoundDriver_FreeVoice first)
    }
    if (0x1E7FBF - voice->sample_spu_addr < voice->sample_size) {
        return -1; // match: the voice is not released here (the other loaders call SoundDriver_FreeVoice first)
    }
    if (voice->stereo_selected != 0) {
        block = voice->stereo_header_block;
    } else {
        block = voice->mono_header_block;
    }
    if (SoundDriver_Remote(1, 0x1010, voice->driver_voice_id, voice->sample_spu_addr, block, voice->unk1B) != 0) {
        return -1; // match: the voice is not released here (the other loaders call SoundDriver_FreeVoice first)
    }
    FlushCache(0);
    return voice_index;
}

/* Sets a voice's transfer channel and sends its samples from the IOP copy to sound memory again;
   0 on success, -1 if the driver sent less. */
int SoundDriver_ResendVoiceSamples(int voice_index, int channel) {
    SoundVoice *voice;

    voice = &sound_state->voices[voice_index];
    voice->dma_channel = channel;
    return ((SoundDriver_Remote(1, 0x90, (char)channel, 0, voice->iop_heap, voice->sample_spu_addr, voice->sample_size) ^ voice->sample_size) == 0) ? 0 : -1;
}

/* Moves a voice's samples to spu_addr in sound memory (sent again from the IOP copy over channel) and
   rebinds the voice; 0 on success, -1 on failure. */
int SoundDriver_MoveVoiceSamples(int voice_index, char channel, u32 spu_addr) {
    SoundVoice *voice;
    int sent;
    u32 end_addr;

    voice = &sound_state->voices[voice_index];
    voice->dma_channel = channel;
    if (spu_addr < 0x5010) {
        return -1;
    }
    if ((u32) (spu_addr + voice->sample_size) > 0x1E7FBFU) {
        return -1;
    }
    voice->sample_spu_addr = spu_addr;
    if (SoundDriver_Remote(1, 0x1010, voice->driver_voice_id, spu_addr, voice->stereo_header_block, voice->unk1B) != 0) {
        return -1;
    }
    sent = SoundDriver_Remote(1, 0x90, voice->dma_channel, 0, voice->iop_heap, voice->sample_spu_addr, voice->sample_size);
    if (sent != voice->sample_size) {
        return -1;
    }
    end_addr = spu_addr + sent;
    if (sound_state->spu_used_end < end_addr) {
        sound_state->spu_used_end = end_addr;
    }
    return 0;
}

/* Asks the driver (command 0x80 without waiting) whether the sample transfer on a channel has finished;
   nonzero once it has. Command 0x80 with 1 waits for it instead. */
int SoundDriver_IsTransferDone(int channel) {
    return SoundDriver_Remote(1, 0x80, channel, 0);
}

/* Frees a voice: its IOP sample copy and both header copies, then finds the end of the used sound memory
   again from the voices left. 0, or -1 for a bad index or a failed free. */
int SoundDriver_FreeVoice(u32 voice_index) {
    SoundVoice *voice;
    u32 used_end;
    int block_base;
    int voices_left;

    if (voice_index >= 0x10U) {
        return -1;
    }
    voice = &sound_state->voices[voice_index];
    if (sound_state->spu_used_end == voice->sample_spu_addr + voice->sample_size) {
        sound_state->spu_used_end = voice->sample_spu_addr;
    }
    voice->sample_spu_addr = 0;
    if (voice->iop_heap != 0) {
        if (sceSifFreeIopHeap(voice->iop_heap) != 0) {
            return -1;
        }
        voice->iop_heap = 0;
    }
    if (voice->stereo_header_block != 0) {
        if (sceSifFreeIopHeap(voice->stereo_header_block) != 0) {
            return -1;
        }
        voice->stereo_header_block = 0;
    }
    if (voice->mono_header_block != 0) {
        if (sceSifFreeIopHeap(voice->mono_header_block) != 0) {
            return -1;
        }
        voice->mono_header_block = 0;
    }
    sound_state->spu_used_end = 0x5010;
    voices_left = 15;
    voice = sound_state->voices;
    do {
        used_end = sound_state->spu_used_end;
        block_base = voice->sample_spu_addr;
        if ((u32)block_base >= used_end) {
            sound_state->spu_used_end = block_base + voice->sample_size;
        }
        voice++;
        voices_left--;
    } while (voices_left >= 0);
    FlushCache(0);
    return 0;
}

/* Frees the IOP copy of a voice's samples, which is no longer needed once they are in sound memory
   (prints "@#$%"); 0, or -1 if none was held or the free failed. */
int SoundDriver_FreeSampleBuffer(int voice_index) {
    SoundVoice *voice;

    voice = &sound_state->voices[voice_index];
    if (voice->iop_heap == 0) {
        return -1;
    }
    if (sceSifFreeIopHeap(voice->iop_heap) != 0) {
        return -1;
    }
    puts(sample_buffer_freed_message);
    voice->iop_heap = 0;
    FlushCache(0);
    return 0;
}

/* Frees a voice's two header copies on the IOP; -1 if either was missing or would not free. */
int SoundDriver_FreeVoiceHeaders(int voice_index) {
    SoundVoice *voice = &sound_state->voices[voice_index];
    int result = 0;

    if (voice->stereo_header_block != 0) {
        if (sceSifFreeIopHeap(voice->stereo_header_block) != 0) {
            result = -1;
        } else {
            voice->stereo_header_block = 0;
        }
    } else {
        result = -1;
    }
    if (voice->mono_header_block != 0) {
        if (sceSifFreeIopHeap(voice->mono_header_block) != 0) {
            result = -1;
        } else {
            voice->mono_header_block = 0;
        }
    } else {
        result = -1;
    }
    FlushCache(0);
    return result;
}

/* Packs sound memory: sorts the voices by where their sample data sits, moves each one down
   to close the gaps, then rebinds every voice and bank. Returns -1 on failure. */
int SoundDriver_CompactMemory(void) {
    SoundVoice *sorted[16];
    char moved[16]; // match: cleared and never read
    SoundVoice *voice;
    SoundVoice *swap;
    SoundBank *bank;
    u32 next_addr;
    int block;
    int sent;
    int gap;
    int i;
    int start;
    int k;

    for (i = 15; i != -1; i--) {
        sorted[i] = &sound_state->voices[i];
        moved[i] = 0;
    }
    for (gap = 8; gap != 0; gap /= 2) {
        for (start = 0; start < gap; start++) {
            for (i = start + gap; i < 16; i += gap) {
                for (k = i - gap; k >= start; k -= gap) {
                    if ((u32)sorted[k]->sample_spu_addr > (u32)sorted[k + gap]->sample_spu_addr) {
                        swap = sorted[k];
                        sorted[k] = sorted[k + gap];
                        sorted[k + gap] = swap;
                        continue;
                    }
                    break;
                }
            }
        }
    }
    next_addr = sound_state->unk4BC;
    for (i = 0; i < 16; i++) {
        voice = sorted[i];
        if (voice->iop_heap == 0) {
            continue;
        }
        if (voice->sample_spu_addr == 0) {
            continue;
        }
        if (next_addr < (u32)voice->sample_spu_addr) {
            voice->sample_spu_addr = next_addr;
            SoundDriver_Remote(1, 0x80, voice->dma_channel, 1);
            sent = SoundDriver_Remote(1, 0x90, voice->dma_channel, 0, voice->iop_heap, voice->sample_spu_addr, voice->sample_size);
            if (sent != voice->sample_size) {
                return -1;
            }
            next_addr += sent;
            SoundDriver_Remote(1, 0x80, voice->dma_channel, 1);
            if (voice->stereo_selected != 0) {
                block = voice->stereo_header_block;
            } else {
                block = voice->mono_header_block;
            }
            if (SoundDriver_Remote(1, 0x1010, voice->driver_voice_id, voice->sample_spu_addr, block, voice->unk1B) != 0) {
                return -1;
            }
        } else {
            next_addr = voice->sample_spu_addr + voice->sample_size;
        }
    }
    sound_state->spu_used_end = next_addr;
    bank = sound_state->banks;
    for (i = 0; i < 4; i++, bank++) {
        if (bank->iop_heap == 0) {
            continue;
        }
        if (bank->voice == -1) {
            continue;
        }
        voice = &sound_state->voices[bank->voice];
        if (voice->stereo_selected != 0) {
            block = voice->stereo_header_block;
        } else {
            block = voice->mono_header_block;
        }
        if (SoundDriver_Remote(1, 0x1010, bank->sequence_id, voice->sample_spu_addr, block, voice->unk1B) != 0) {
            return -1;
        }
    }
    FlushCache(0);
    return 0;
}

/* Sets where in sound memory a voice's data goes. */
int SoundDriver_SetVoiceAddress(u32 voice_index, int address) {
    if (voice_index >= 16) {
        return -1;
    }
    sound_state->voices[voice_index].sample_spu_addr = address;
    return 0;
}

/* Sets the priority of a voice's synth port (command 0x1040: FSIOPSND writes the byte to the first field
   of the port's modhsyn environment, which modhsyn adds to each note's priority when it has to take a
   sounding voice away for a new note). Nothing calls it. */
void SoundDriver_SetVoicePriority(int voice_index, u8 priority) {
    SoundDriver_Remote(1, 0x1040, sound_state->voices[voice_index].driver_voice_id, priority);
}

/* Sets how many SPU2 voices a voice's synth port may sound at once (command 0x1050: FSIOPSND writes the
   byte to the second field of the port's modhsyn environment; at that count modhsyn takes the port's own
   lowest-priority voice for a new note). Nothing calls it. */
void SoundDriver_SetVoiceMaxPolyphony(int voice_index, u8 max_voices) {
    SoundDriver_Remote(1, 0x1050, sound_state->voices[voice_index].driver_voice_id, max_voices);
}

/* Sets a voice's volume (command 0x1060; Sound_SetBgmVolume passes 0..0x7F). 0, or -1 for a bad index
   or a free voice. */
int SoundDriver_SetVoiceVolume(u32 voice_index, s16 value) {
    SoundVoice *voice;

    if (voice_index >= 0x11U) {
        return -1;
    }
    voice = &sound_state->voices[voice_index];
    if (voice->stereo_header_block == 0) {
        return -1;
    }
    SoundDriver_Remote(1, 0x1060, voice->driver_voice_id, value);
    return 0;
}

/* SoundDriver_SetMono for one voice and the sequences that play it: output off while the voice is
   rebound to its mono header copy. 0, or -1 if the driver refused. */
int SoundDriver_SetVoiceMono(int voice_index) {
    SoundVoice *voice;
    SoundBank *bank;
    int banks_left;
    int result;

    voice = &sound_state->voices[voice_index];
    voice->stereo_selected = 0;
    bank = sound_state->banks;
    if (sound_state->active != 0) {
        SoundDriver_Remote(1, 0x50);
    }
    banks_left = 4;
    result = (SoundDriver_Remote(1, 0x1010, voice->driver_voice_id, voice->sample_spu_addr, voice->mono_header_block, voice->unk1B) != 0) ? -1 : 0;
    while (--banks_left != -1) {
        if (bank->voice == voice_index && SoundDriver_Remote(1, 0x2070, bank->sequence_id, voice->sample_spu_addr, voice->mono_header_block, voice->unk1B, 0) != 0) {
            result = -1;
            break;
        }
        bank++;
    }
    if (sound_state->active != 0) {
        SoundDriver_Remote(1, 0x40);
    }
    return result;
}

/* SoundDriver_SetStereo for one voice and the sequences that play it. 0, or -1 if the driver refused. */
int SoundDriver_SetVoiceStereo(int voice_index) {
    SoundVoice *voice = &sound_state->voices[voice_index];
    SoundBank *bank = sound_state->banks;
    int result = 0;
    int i;

    voice->stereo_selected = 1;
    if (sound_state->active != 0) {
        SoundDriver_Remote(1, 0x50);
    }
    if (SoundDriver_Remote(1, 0x1010, voice->driver_voice_id, voice->sample_spu_addr, voice->stereo_header_block, voice->unk1B) != 0) {
        result = -1;
    }
    for (i = 4; --i != -1; bank++) {
        if (bank->voice == voice_index) {
            if (SoundDriver_Remote(1, 0x2070, bank->sequence_id, voice->sample_spu_addr, voice->stereo_header_block, voice->unk1B, 1) != 0) {
                result = -1;
                break;
            }
        }
    }
    if (sound_state->active != 0) {
        SoundDriver_Remote(1, 0x40);
    }
    return result;
}

/* Loads a sequence file to the IOP and hands it to the driver with the voice it plays (commands 0x1010,
   0x2000). Returns the sequence slot, -2 when none is free, -1 on failure. */
int SoundDriver_LoadSequenceFile(int bank_id, int voice_index, char *name) {
    SoundBank *bank = sound_state->banks;
    SoundVoice *voice;
    int bank_index;
    int block;
    int size;
    int i;

    for (i = 0; i < 4; i++, bank++) {
        if (bank->iop_heap == 0) {
            bank_index = i;
            break;
        }
    }
    if (!(i < 4)) {
        return -2;
    }
    if (bank_id == -1) {
        bank_id = bank_index;
    } else if ((u32)bank_id >= 4) {
        return -1;
    }
    bank->sequence_id = bank_id;
    voice = &sound_state->voices[voice_index];
    bank->voice = voice_index;
    size = SoundDriver_GetFileSize(name);
    if (size < 0) {
        return -1;
    }
    bank->iop_size = size;
    bank->iop_heap = sceSifAllocIopHeap(size);
    if (bank->iop_heap == 0) {
        return -1;
    }
    if ((u32)(bank->iop_heap + bank->iop_size) > 0x1FFFFF) {
        return -1;
    }
    if (SoundDriver_LoadIopFile(name, (void *)bank->iop_heap) != 0) {
        return -1;
    }
    if (voice->stereo_selected != 0) {
        block = voice->stereo_header_block;
    } else {
        block = voice->mono_header_block;
    }
    if (SoundDriver_Remote(1, 0x1010, bank->sequence_id, voice->sample_spu_addr, block, voice->unk1B) != 0) {
        return -1;
    }
    if (SoundDriver_Remote(1, 0x2000, bank->sequence_id, bank->iop_heap) != 0) {
        return -1;
    }
    FlushCache(0);
    return bank_index;
}

/* SoundDriver_LoadSequence in steps, to be called every frame until it stops returning -1: step 0 takes a
   free sequence slot, allocates IOP memory and starts sending the data, step 1 waits for the transfer, step 2
   hands sequence and voice to the driver. Returns the slot when done, -2 when it failed. */
int SoundDriver_LoadSequenceStep(int bank_id, int voice_index, int data, u32 size) {
    int index;
    int iop_heap;

    switch (sequence_load_step) {
    case 0:
        bank_load_bank = sound_state->banks;
        for (index = 0; index < 4; index++) {
            if (bank_load_bank->iop_heap == 0) {
                bank_load_index = index;
                break;
            }
            bank_load_bank++;
        }
        if (index >= 4) {
            sequence_load_step = 0;
            return -2;
        }
        if (bank_id == -1) {
            bank_id = bank_load_index;
        } else if ((u32)bank_id >= 4U) {
            return -1;
        }
        bank_load_bank->sequence_id = bank_id;
        size = (size + 0xF) / 16 * 16;
        bank_load_bank->voice = voice_index;
        bank_load_voice = &sound_state->voices[voice_index];
        bank_load_bank->iop_size = size;
        iop_heap = sceSifAllocIopHeap(size);
        bank_load_bank->iop_heap = iop_heap;
        if (iop_heap == 0 || (u32)(iop_heap + bank_load_bank->iop_size) > 0x1FFFFFU) {
            SoundDriver_FreeSequence(bank_load_index);
            sequence_load_step = 0;
            return -2;
        }
        SoundDriver_StartUpload(iop_heap, data, bank_load_bank->iop_size);
        sequence_load_step++;
        break;
    case 1:
        if (SoundDriver_StepUpload() != 0) {
            sequence_load_step++;
        }
        break;
    case 2:
        if (bank_load_voice->stereo_selected != 0) {
            bank_load_voice_data = bank_load_voice->stereo_header_block;
        } else {
            bank_load_voice_data = bank_load_voice->mono_header_block;
        }
        if (SoundDriver_Remote(1, 0x1010, bank_load_bank->sequence_id, bank_load_voice->sample_spu_addr, bank_load_voice_data, bank_load_voice->unk1B) != 0 ||
            SoundDriver_Remote(1, 0x2000, bank_load_bank->sequence_id, bank_load_bank->iop_heap) != 0) {
            SoundDriver_FreeSequence(bank_load_index);
            sequence_load_step = 0;
            return -2;
        }
        FlushCache(0);
        sequence_load_step = 0;
        break;
    }
    if (sequence_load_step != 0) {
        return -1;
    }
    return bank_load_index;
}

/* Loads a sequence from memory into the first free of the four sequence slots: allocates IOP memory for
   it, sends the data there (waiting until the transfer is done) and hands sequence and voice to the driver.
   Returns the slot, -2 when all four are taken, -1 on any other failure. */
int SoundDriver_LoadSequence(int bank_id, int voice_index, int data, int size) {
    SoundState *state = sound_state;
    SoundBank *bank;
    SoundVoice *voice;
    int index;
    int free_index;
    int heap_src;
    int iop_heap;

    bank = state->banks;
    for (index = 0; index < 4; index++) {
        if (bank->iop_heap == 0) {
            free_index = index;
            break;
        }
        bank++;
    }
    if (index >= 4) {
        return -2;
    }
    if (bank_id == -1) {
        bank_id = free_index;
    } else if ((u32)bank_id >= 4U) {
        return -1;
    }
    voice = &state->voices[voice_index];
    bank->sequence_id = bank_id;
    bank->voice = voice_index;
    while (size & 0xF) {
        size++;
    }
    bank->iop_size = size;
    iop_heap = sceSifAllocIopHeap(size);
    bank->iop_heap = iop_heap;
    if (iop_heap == 0 || (u32)(iop_heap + bank->iop_size) > 0x1FFFFFU) {
        SoundDriver_FreeSequence(free_index);
        return -1;
    }
    SoundDriver_StartUpload(iop_heap, data, bank->iop_size);
    while (SoundDriver_StepUpload() == 0) {
    }
    if (voice->stereo_selected != 0) {
        heap_src = voice->stereo_header_block;
    } else {
        heap_src = voice->mono_header_block;
    }
    if (SoundDriver_Remote(1, 0x1010, bank->sequence_id, voice->sample_spu_addr, heap_src, voice->unk1B) != 0 ||
        SoundDriver_Remote(1, 0x2000, bank->sequence_id, bank->iop_heap) != 0) {
        SoundDriver_FreeSequence(free_index);
        return -1;
    }
    FlushCache(0);
    return free_index;
}

/* Takes the first free sequence slot for sequence data already in IOP memory and hands it and its voice
   to the driver. Returns the slot, -2 when all four are taken, -1 on any other failure. */
int SoundDriver_BindIopSequence(int bank_id, int voice_index, int iop_heap, int size) {
    SoundState *state = sound_state;
    SoundBank *bank;
    SoundVoice *voice;
    int index;
    int free_index;
    int heap_src;

    bank = state->banks;
    for (index = 0; index < 4; index++) {
        if (bank->iop_heap == 0) {
            free_index = index;
            break;
        }
        bank++;
    }
    if (index >= 4) {
        return -2;
    }
    if (bank_id == -1) {
        bank_id = free_index;
    } else if ((u32)bank_id >= 4U) {
        return -1;
    }
    voice = &state->voices[voice_index];
    bank->sequence_id = bank_id;
    bank->iop_size = size;
    bank->voice = voice_index;
    bank->iop_heap = iop_heap;
    if (iop_heap == 0) {
        return -1;
    }
    if (voice->stereo_selected != 0) {
        heap_src = voice->stereo_header_block;
    } else {
        heap_src = voice->mono_header_block;
    }
    if (SoundDriver_Remote(1, 0x1010, bank->sequence_id, voice->sample_spu_addr, heap_src, voice->unk1B) != 0) {
        return -1;
    }
    if (SoundDriver_Remote(1, 0x2000, bank->sequence_id, bank->iop_heap) != 0) {
        return -1;
    }
    FlushCache(0);
    return free_index;
}

/* Frees a sequence's IOP memory; 0, or -1 if it held none or the free failed. */
int SoundDriver_FreeSequence(int i) {
    SoundBank *bank = &sound_state->banks[i];

    if (bank->iop_heap == 0) {
        return -1;
    }
    if (sceSifFreeIopHeap(bank->iop_heap) != 0) {
        return -1;
    }
    bank->iop_size = bank->iop_heap = 0;
    FlushCache(0);
    return 0;
}

/* Starts a sequence playing (command 0x2030; the BGM loader passes 0 once the sequence is loaded).
   0, or -1 with the output off, a bad index or an empty slot. */
int SoundDriver_PlaySequence(u32 bank_index, int value) {
    SoundBank *bank;

    if (sound_state->active == 0) {
        return -1;
    }
    if (bank_index >= 5) {
        return -1;
    }
    bank = &sound_state->banks[bank_index];
    if (bank->iop_heap == 0) {
        return -1;
    }
    return SoundDriver_Remote(1, 0x2030, bank->sequence_id, value) == 0 ? 0 : -1;
}

/* Whether a sequence is playing (command 0x2010: FSIOPSND answers 1 while it is playing or paused, 0 when
   it is stopped or only loaded); -1 for a bad index. Nothing calls it. */
int SoundDriver_IsSequencePlaying(u32 bank_index) {
    SoundBank *bank;

    if (bank_index >= 5U) {
        return -1;
    }
    bank = &sound_state->banks[bank_index];
    return SoundDriver_Remote(1, 0x2010, bank->sequence_id);
}

/* Stops a sequence (command 0x2040; the BGM loader calls it before freeing the old BGM).
   0, or -1 for a bad index or an empty slot. */
int SoundDriver_StopSequence(u32 bank_index) {
    SoundBank *bank;

    if (bank_index >= 5) {
        return -1;
    }
    bank = &sound_state->banks[bank_index];
    if (bank->iop_heap == 0) {
        return -1;
    }
    return SoundDriver_Remote(1, 0x2040, bank->sequence_id) == 0 ? 0 : -1;
}

/* Pauses a sequence (command 0x2050: FSIOPSND turns the modmidi play switch off without rewinding, unlike
   the stop, and marks the sequence paused). 0, or -1 for a bad index, an empty slot or a refusal. Nothing
   calls it. */
int SoundDriver_PauseSequence(u32 bank_index) {
    SoundBank *bank;

    if (bank_index >= 5U) {
        return -1;
    }
    bank = &sound_state->banks[bank_index];
    if (bank->iop_heap == 0) {
        return -1;
    }
    return (SoundDriver_Remote(1, 0x2050, bank->sequence_id) == 0) ? 0 : -1;
}

/* Resumes a paused sequence (command 0x2060: FSIOPSND turns the modmidi play switch back on where it
   stopped, marks the sequence playing and sets its tempo again). 0, or -1 with the output off, a bad index,
   an empty slot or a refusal. Nothing calls it. */
int SoundDriver_ResumeSequence(u32 bank_index) {
    SoundBank *bank;

    if (sound_state->active == 0) {
        return -1;
    }
    if (bank_index >= 5U) {
        return -1;
    }
    bank = &sound_state->banks[bank_index];
    if (bank->iop_heap == 0) {
        return -1;
    }
    return (SoundDriver_Remote(1, 0x2060, bank->sequence_id) == 0) ? 0 : -1;
}

/* Sets a sequence's volume (command 0x1060, as for a voice): the BGM fades. */
int SoundDriver_SetSequenceVolume(int bank_index, s16 value) {
    return SoundDriver_Remote(1, 0x1060, sound_state->banks[bank_index].sequence_id, value);
}

/* Sets a sequence's tempo (command 0x2080: FSIOPSND clamps the value by the sequence's own tempo and
   passes it to sceMidi_MidiSetRelativeTempo, and keeps it for the resume). Returns the driver's answer.
   Nothing calls it. */
int SoundDriver_SetSequenceTempo(int bank_index, u16 tempo) {
    return SoundDriver_Remote(1, 0x2080, sound_state->banks[bank_index].sequence_id, tempo);
}

/* Starts a note in slot `slot` on a voice: queues, in the command list SoundDriver_FlushCommands sends,
   an optional program change, the pan (0x3F, centre, with mono output), the bend as two 7-bit halves,
   the volume and the note itself. Returns the slot; -1: output off or bad slot, -2: the list is full. */
int SoundDriver_NoteOn(u32 slot, int voice_index, char channel, char program, char note, char volume, s16 bend, char pan) {
    SoundState *state = sound_state;
    int *queue_size;
    int queued;
    int offset;
    int length;
    u8 *out;
    SoundKey *key;
    SoundVoice *voice;

    if (state->active == 0) {
        return -1;
    }
    queue_size = (int *)(state->dma_dest | 0x20000000);
    length = 0x19;
    if (program != -1) {
        length = 0x1B;
    }
    queued = *queue_size;
    if ((u32)(queued + length) >= 0x3FC) {
        return -2;
    }
    offset = queued + 4;
    out = (u8 *)(((int)queue_size + offset) | 0x20000000);
    voice = &state->voices[voice_index];
    if (slot >= 0x7E) {
        return -1;
    }
    key = (SoundKey *)state->note_keys[slot & 0x7F];
    key->sounding = 1;
    key->voice = voice_index;
    key->note = note;
    key->channel = channel;
    *out++ = 0xFF;
    *out++ = voice->driver_voice_id;
    *out++ = length - 3;
    if (program != -1) {
        *out++ = channel - 0x40;
        *out++ = program;
    }
    // match: the u16 at 0x4D4; the header has six chars there
    if (*(u16 *)&sound_state->unk4D0[4] == 0 || voice->stereo_selected == 0) {
        pan = 0x3F;
    }
    *out++ = 0xF9;
    *out++ = 1;
    *out++ = channel;
    *out++ = pan;
    *out++ = 0;
    *out++ = 0xF9;
    *out++ = 2;
    *out++ = channel;
    *out++ = bend & 0x7F;
    *out++ = ((bend >> 7) + 0x40) & 0x7F;
    *out++ = 0xF9;
    *out++ = 0;
    *out++ = channel;
    *out++ = volume;
    *out++ = 0;
    *out++ = 0xFD;
    *out++ = 0x10;
    *out++ = channel;
    *out++ = note;
    *out++ = slot;
    out[0] = 0x7F;
    out[1] = 0;
    *queue_size += length;
    return slot;
}

/* Ends the note in a slot: queues its note command with velocity 0 and marks the slot stopped.
   0; -1: output off or bad slot, -2: the list is full. */
int SoundDriver_NoteOff(u32 key_index) {
    u8 *write_ptr;
    int *command_list;
    SoundKey *key;
    int list_used;
    int write_offset;

    if (sound_state->active == 0) {
        return -1;
    }
    if (key_index >= 0x7E) {
        return -1;
    }
    command_list = (int *)(sound_state->dma_dest | 0x20000000);
    list_used = *command_list;
    if ((u32)(list_used + 0xA) >= 0x3FC) {
        return -2;
    }
    write_offset = list_used + 4;
    write_ptr = (u8 *)(((int)command_list + write_offset) | 0x20000000);
    key = (SoundKey *)sound_state->note_keys[key_index];
    *write_ptr++ = 0xFF;
    // match: byte 0x18 of the voice (low byte of driver_voice_id), read as a byte
    *write_ptr++ = ((u8 *)sound_state + (key->voice << 5))[0x18];
    *write_ptr++ = 7;
    *write_ptr++ = 0xFD;
    *write_ptr++ = 0x10;
    *write_ptr++ = key->channel;
    *write_ptr++ = key->note;
    *write_ptr++ = key_index;
    write_ptr[1] = write_ptr[0] = 0;
    *command_list += 0xA;
    key->sounding = -1;
    return 0;
}

/* Changes the note in slot `slot`: queues a 10-byte command, kind 0 its volume, 1 its pan (0x3F with mono
   output), 2 its bend (as two 7-bit halves). 0; -1: output off or bad slot, -2: the list is full. */
int SoundDriver_SetNoteControl(int kind, u32 slot, int value) {
    SoundState *state = sound_state;
    int *queue_size;
    int queued;
    int offset;
    u8 *out;
    SoundKey *key;
    SoundVoice *voice;

    if (state->active == 0) {
        return -1;
    }
    if (slot >= 0x7E) {
        return -1;
    }
    queue_size = (int *)(state->dma_dest | 0x20000000);
    queued = *queue_size;
    if ((u32)(queued + 0xA) >= 0x3FC) {
        return -2;
    }
    offset = queued + 4;
    out = (u8 *)(((int)queue_size + offset) | 0x20000000);
    key = (SoundKey *)state->note_keys[slot];
    voice = &state->voices[key->voice];
    *out++ = 0xFF;
    *out++ = voice->driver_voice_id;
    *out++ = 7;
    switch (kind) {
    case 0:
        *out++ = 0xFD;
        *out++ = 0;
        *out++ = key->channel;
        *out++ = key->note;
        *out++ = slot;
        out[0] = value;
        out[1] = 0;
        break;
    case 1:
        // match: the u16 at 0x4D4; the header has six chars there
        if (*(u16 *)&sound_state->unk4D0[4] == 0 || voice->stereo_selected == 0) {
            value = 0x3F;
        }
        *out++ = 0xFD;
        *out++ = 1;
        *out++ = key->channel;
        *out++ = key->note;
        *out++ = slot;
        out[0] = value;
        out[1] = 0;
        break;
    case 2:
        *out++ = 0xFD;
        *out++ = kind;
        *out++ = key->channel;
        *out++ = key->note;
        *out++ = slot;
        out[0] = value & 0x7F;
        out[1] = ((value >> 7) + 0x40) & 0x7F;
        break;
    }
    *queue_size += 0xA;
    return 0;
}

/* Queues a program change (0xC0 + channel, program) for a voice, as SoundDriver_NoteOn does before a note.
   Nothing calls it. 0; -1: output off, -2: the list is full. */
int SoundDriver_ProgramChange(int voice_index, char first_value, char second_value) {
    int *command_list;
    int list_used;
    int write_offset;
    u8 *write_ptr;

    if (sound_state->active == 0) {
        return -1;
    }
    command_list = (int *)(sound_state->dma_dest | 0x20000000);
    list_used = *command_list;
    if ((u32)(list_used + 5) >= 0x3FC) {
        return -2;
    }
    write_offset = list_used + 4;
    write_ptr = (u8 *)(((int)command_list + write_offset) | 0x20000000);
    *write_ptr++ = 0xFF;
    *write_ptr++ = (u8)sound_state->voices[voice_index].driver_voice_id;
    *write_ptr++ = 2;
    write_ptr[0] = (char)(first_value - 0x40);
    write_ptr[1] = second_value;
    *command_list += 5;
    return 0;
}

/* Sends the queued note commands (sound_command_list, its byte count first) to the driver's buffer on the
   IOP, waits for the DMA, empties the list and has the driver run it (command 0x1080). Once a frame.
   0, or -1 with the output off or when the DMA could not start. */
int SoundDriver_FlushCommands(void) {
    int *flag;
    int size;
    int transfer_id;
    int dest;

    if (sound_state->active == 0) {
        return -1;
    }
    flag = (int *)(sound_state->dma_dest | 0x20000000);
    size = *flag + 4;
    while (size & 0xF) {
        size++;
    }
    dest = sound_state->dma_dest;
    sound_command_dma.dest = sound_state->dma_src;
    sound_command_dma.src = dest;
    sound_command_dma.size = size;
    sound_command_dma.mode = 0;
    transfer_id = sceSifSetDma((int *)&sound_command_dma, 1);
    if (transfer_id == 0) {
        return -1;
    }
    while (sceSifDmaStat(transfer_id) >= 0) {
    }
    *flag = 0;
    SoundDriver_Remote(1, 0x1080);
    return 0;
}

/* Opens a file as stream `stream & 1` (command 0x3000 with the name: FSIOPSND's adsOpen opens and reads
   the file), then gives the stream its buffer (0x3040); stops it (0x3030) if that fails. The other order
   than SoundDriver_PlayStreamFile. Nothing calls it. Returns the stream, or -1. */
int SoundDriver_OpenStreamFile(int stream, char *file_name, int value) {
    strcpy(sound_rpc_file_name, file_name);
    stream &= 1;
    sceSifCallRpc(sound_rpc_client, stream | 0x3000, 0, sound_rpc_file_name, 0x100, (int *)&sound_rpc_buffer, 1, 0, 0);
    if (sound_rpc_buffer.self == 0) {
        if (SoundDriver_Remote(1, 0x3040, stream, value) != 0) {
            SoundDriver_Remote(1, 0x3030, stream);
            return -1;
        }
        return stream;
    }
    return -1;
}

/* Starts a stream (command 0x3040) and sends it the name of the file to play (command 0x30A0 + stream);
   stops it again (0x3030) if either fails. The title screen music (TITLE.ADS). Returns the stream, or -1. */
int SoundDriver_PlayStreamFile(int stream, char *file_name, int value) {
    if (SoundDriver_Remote(1, 0x3040, stream, value) != 0) {
        SoundDriver_Remote(1, 0x3030, stream);
        return -1;
    }
    strcpy(sound_rpc_file_name, file_name);
    sceSifCallRpc(sound_rpc_client, stream | 0x30A0, 0, sound_rpc_file_name, 0x100, (int *)&sound_rpc_buffer, 1, 0, 0);
    if (sound_rpc_buffer.self != 0) {
        SoundDriver_Remote(1, 0x3030, stream);
        return -1;
    }
    return stream;
}

/* Sets a stream to play straight from the disc (command 0x30C0: FSIOPSND's adsSetWithLsn takes a buffer
   of buffer_size bytes and reads from sector `sector` with sceCdRead, with no file name). Returns the
   stream, or -1 when the driver refuses. Nothing calls it. */
int SoundDriver_SetStreamSector(int stream, int sector, int buffer_size) {
    return (SoundDriver_Remote(1, 0x30C0, stream, sector, buffer_size) == 0) ? stream : -1;
}

/* Stores reverb settings (a rev_para.dat entry: mode, depths, delay, feedback) for SPU2 core effect_id in
   the first free of the four slots; returns the slot, or -2 when all are taken. */
int SoundDriver_AddReverb(int effect_flags, s16 param1, s16 param2, s16 param3, s16 param4, s16 param5, s16 param6, int effect_id) {
    SoundEffectFields *effect;
    int i;
    int slot;

    for (effect = (SoundEffectFields *)sound_state->effects, i = 0; i < 4; i++, effect++) {
        if (effect->core != -1) {
            continue;
        }
        slot = i;
        break;
    }
    if (!(i < 4)) {
        return -2;
    }
    effect->core = effect_id;
    effect->mode = effect_flags | 0x100;
    effect->depth_l = param1;
    effect->depth_r = param2;
    effect->delay = param3;
    effect->feedback = param4;
    effect->unk14 = param5;
    effect->unk16 = param6;
    FlushCache(0);
    return slot;
}

/* Frees a reverb slot; 0, or -1 for a bad index or a free slot. */
int SoundDriver_FreeReverb(u32 effect_index) {
    SoundEffectFields *effect;

    if (effect_index >= 5U) {
        return -1;
    }
    effect = (SoundEffectFields *)&sound_state->effects[effect_index];
    if (effect->core == -1) {
        return -1;
    }
    effect->core = -1;
    FlushCache(0);
    return 0;
}

/* Sends the settings of a reverb slot to the driver (command 0x40070), waiting for the call. */
void SoundDriver_ApplyReverb(u32 effect_index) {
    if (effect_index < 5) {
        sound_state->effect = sound_state->effects[effect_index];
        SoundDriver_Remote(1, 0x40070, &sound_state->effect);
    }
}

/* SoundDriver_ApplyReverb without waiting for the call. Nothing calls it. */
void SoundDriver_ApplyReverbNoWait(u32 index) {
    if (index < 5) {
        sound_state->effect = sound_state->effects[index];
        SoundDriver_Remote(0, 0x40070, &sound_state->effect);
    }
}

/* Opens the file (retrying until it works), takes its size, and rounds it up to a multiple of 64. */
static int SoundDriver_GetFileSize(const char *name) {
    int handle;
    int size;

    do {
        do {
            handle = sceOpen(name, 1);
        } while (handle < 0);
        size = sceLseek(handle, 0, 2);
        sceClose(handle);
    } while (size < 0);
    if (size & 0x3F) {
        do {
            size += 1;
        } while (size & 0x3F);
    }
    return size;
}

/* Loads a file into IOP memory at addr (sceSifLoadIopHeap, retried until it works) and remembers the
   first address it loaded to. Returns 0. */
static int SoundDriver_LoadIopFile(char *name, void *addr) {
    if (sound_state->load_addr == NULL) {
        sound_state->load_addr = addr;
    }
    while (sceSifLoadIopHeap(name, addr) < 0) {
    }
    return 0;
}

/* Copies the status of the 16 MIDI channels of synth port `port` to buffer, one byte each (command 0x1090:
   FSIOPSND answers with sceHSyn_GetChStat, whose bits say whether the channel has voices in its lists).
   Nothing calls it. */
void SoundDriver_GetChannelStatus(void *buffer, int port) {
    u8 *out_bytes;
    int *word;
    int words_left;

    SoundDriver_Remote(1, 0x1090, port);
    out_bytes = buffer;
    word = (int *)&sound_rpc_buffer;
    words_left = 3;
    do {
        out_bytes[0] = *((u8 *)word);
        out_bytes[1] = *(((u8 *)word) + 1);
        out_bytes[2] = (((u32)(*word)) & 0xFF0000) >> 16;
        out_bytes[3] = (((u32)(*word)) & 0xFF000000) >> 24;
        word++;
        out_bytes += 4;
        words_left--;
    } while (words_left >= 0);
}

/* Reads an envelope from a voice's header (command 0x110 with the header, its size and param) into
   *status: the SPU2 ADSR pair, the rate fields turned round (0x7F - rate and so on). */
void SoundDriver_GetAdsr(int voice_index, int param, SoundVoiceStatus *status) {
    SoundVoice *slot;
    u32 reply, inverted;

    slot = &sound_state->voices[voice_index];
    reply = SoundDriver_Remote(1, 0x110, slot->stereo_header_block, slot->header_size, param);
    inverted = ~reply;
    status->bit31 = reply >> 31;
    status->bits24_30 = inverted >> 24;
    status->bits20_23 = inverted >> 20;
    status->bits16_19 = reply >> 16;
    status->bits13_15 = reply >> 13;
    status->bits6_12 = inverted >> 6;
    status->bit5 = reply >> 5;
    status->bits0_4 = inverted;
}

/* Writes an envelope into a voice's header (command 0x120): the ADSR pair in status, the rates turned
   round as SoundDriver_GetAdsr reads them. */
void SoundDriver_SetAdsr(int voice_index, int value, u32 status) {
    SoundVoice *voice;
    u32 packed;

    voice = &sound_state->voices[voice_index];
    packed = (status >> 31) << 31;
    packed |= ((((status >> 24) & 0x7F) ^ 0x7F) << 24);
    packed |= ((((status >> 20) & 0xF) ^ 0xF) << 20);
    packed |= status & 0xF0000;
    packed |= status & 0xE000;
    packed |= ((((status >> 6) & 0x7F) ^ 0x7F) << 6);
    packed |= status & 0x20;
    packed |= (status & 0x1F) ^ 0x1F;
    SoundDriver_Remote(1, 0x120, voice->stereo_header_block, voice->header_size, value, packed);
}
