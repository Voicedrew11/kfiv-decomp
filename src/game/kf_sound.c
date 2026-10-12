#include "kf_load.h"
#include "kf_sound.h"

/* EE EABI varargs: the register save area a1..a7 is spilled to, as in libsdr.c */
typedef char *va_list;
#define va_start(ap, last) ((ap) = (char *)__builtin_next_arg(last) - 0x38)
#define va_arg(ap, type) (*(type *)((ap += 8) - 8))

static void func_001CBEA0(void);
void FlushCache(int); // match: void as in eekernel.h; an int result changes func_001CB778's registers
int sceSifBindRpc(int *, int, int);
int sceSifCheckStatRpc(int *);
int printf(const char *, ...);
int sceSifCallRpc(int *, int, int, char *, int, int *, int, int, int);
int sceSifSetDma(int *, int);
int sceSifDmaStat(int);
static int func_001CF070(const char *);
static int func_001CF110(char *, void *);
int sceSifAllocIopHeap(int);
int sceSifFreeIopHeap(int);
int func_001CBD20(void);
int puts(char *);
char *strcpy(char *, const char *);
int sceClose(int);
int sceLseek(int, int, int);
int sceOpen(const char *, int);
/* SDK: loads an IOP heap file, returns below zero until the IOP is ready */
int sceSifLoadIopHeap(char *name, void *addr);
extern char D_0043F640[];
extern char D_0043F688[];
extern int D_004469E8;
extern int D_00446704;
extern int (*const D_0043F670[])(int *, int *);
extern int D_00420940[];
extern char D_004469F0[];
extern int D_004469F8;
extern char D_00420A40[];


/* the step func_001CD020 runs keeps its source position here between calls */
static int voice_load_source;                   /* 0x4471C0 */
/* the bank func_001CDF90 is loading, step by step */
static int bank_load_index;                     /* 0x4471C4 */
static SoundBank *bank_load_bank;               /* 0x4471C8 */
static SoundVoice *bank_load_voice;             /* 0x4471CC */
static int bank_load_voice_data;                /* 0x4471D0 */
static SoundState *sound_state;                 /* 0x4471D4 points at sound_state_storage */

static int sound_command_list[256];             /* 0x5BEE80 commands for the driver, sent by func_001CEBE0 */
static SoundRpcRequest sound_rpc_buffer;        /* 0x5BF280 */
static SoundState sound_state_storage;          /* 0x5BF380 */
static SoundDmaRequest sound_dma;               /* 0x5BF860 */
static SoundUpload sound_upload;                /* 0x5BF870 */
static int sound_rpc_client[10];                /* 0x5BF890 sceSifClientData of the driver's RPC service */
static SoundDmaRequest sound_command_dma;       /* 0x5BF8B8 sends sound_command_list */

/* Starts the sound driver: binds its RPC service on the IOP, then resets the driver state,
   every voice, bank and effect slot. */
int func_001CB778(void) {
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
    sound_state->dma_src = func_001CBAE0(1, 0x10);
    *(int *)sound_state->unk4D0 = 0x3FFF; // match: the int at 0x4D0; the header has six chars there
    func_001CBAE0(1, 0x30, 0x3FFF, 0x3FFF);
    sound_state->load_addr = NULL;
    sound_state->unk4C8 = func_001CBAE0(1, 0xA0);
    // match: retail stores these three as pointers (the header has them as integers)
    *(int **)&sound_state->dma_dest = sound_command_list;
    sound_command_list[0] = 0;
    // match: as above
    *(char **)&sound_state->unk4BC = *(char **)&sound_state->unk4C0 = (char *)0x5010;
    *(u16 *)&sound_state->unk4D0[4] = 1; // match: the u16 at 0x4D4
    sound_state->active = 0;
    voice = sound_state->voices;
    for (i = 15; i != -1; i--, voice++) {
        voice->unk00 = voice->unk04 = voice->iop_heap = voice->unk0C = 0;
        voice->unk1B = 0;
        voice->unk18 = 0;
        voice->unk1A = 0;
        voice->unk14 = 0;
        voice->unk1C = 1;
    }
    bank = sound_state->banks;
    for (i = 3; i != -1; i--, bank++) {
        bank->iop_heap = 0;
        bank->unk0C = 0;
        bank->unk04 = 0;
    }
    entry = sound_state->unk240[0];
    for (i = 125; i != -1; i--, entry += 4) {
        entry[0] = entry[1] = entry[2] = entry[3] = -1;
    }
    effect = (SoundEffectFields *)sound_state->effects;
    for (i = 3; i != -1; i--, effect++) {
        effect->unk00 = -1;
        effect->unk04 = effect->unk08 = effect->unk0A = effect->unk0C = effect->unk10 = effect->unk14 = effect->unk16 = 0;
    }
    sound_upload.active = 0;
    FlushCache(0);
    return 0;
}

void func_001CB9A0(void) {
    SoundBank *bank;
    SoundVoice *voice;
    int i;

    func_001CBEA0();
    func_001CBAE0(1, 0x3030, 0);
    func_001CBAE0(1, 0x3030, 1);
    bank = sound_state->banks;
    for (i = 0; i < 4; i++) {
        if (bank->iop_heap != 0) {
            func_001CE408(i);
        }
        bank++;
    }
    voice = sound_state->voices;
    for (i = 0; i < 16; i++) {
        if (voice->unk00 != 0) {
            func_001CD6E0(i);
        }
        voice++;
    }
    func_001CBAE0(1, 0x20);
    FlushCache(0);
}

/* Polls the SIF RPC status until it is no longer 1, giving up after 0x989680 tries. */
static int func_001CBA58(void) {
    int retries = 0x989680;

    while (sceSifCheckStatRpc(sound_rpc_client) == 1) {
        if (retries < 0) {
            printf(D_0043F640);
            return -1;
        }
        retries--;
    }
    return 1;
}

/* The state of the sound driver's RPC client: 1 while a call is still running. */
int func_001CBAC0(void) {
    return sceSifCheckStatRpc(sound_rpc_client);
}

/* Sends a command and its arguments to the sound driver on the IOP (the game's own sceSdRemote)
   and returns the driver's answer. */
int func_001CBAE0(int blocking, ...) {
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
    result = func_001CBA58();
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

/* Starts sending size bytes at src to dest in sound memory, 0x4000 bytes at a time (func_001CBD20
   sends the rest). Returns 0, or -1 if the DMA could not be queued. */
static int func_001CBC30(int dest, int src, u32 size) {
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
    iop_buffer = sound_state->unk4C8;
    dma->src = sound_upload.src;
    dma->dest = iop_buffer;
    dma->size = sound_upload.padded_size;
    dma->mode = 0;
    FlushCache(0);
    dma_id = sceSifSetDma((int *)dma, 1);
    sound_upload.dma_id = dma_id;
    return (dma_id != 0) ? 0 : -1;
}

/* Keeps the upload func_001CBC30 started going: once the piece in flight has arrived it is
   written to sound memory and the next one is sent. Returns 1 when nothing is left to send. */
int func_001CBD20(void) {
    SoundDmaRequest *dma;
    int iop_buffer;
    int dma_id;

    if (sound_upload.active == 0) {
        return 1;
    }
    if (sceSifDmaStat(sound_upload.dma_id) >= 0) {
        return 0;
    }
    func_001CBAE0(1, 0xB0, sound_upload.dest, sound_upload.piece_size);
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
    iop_buffer = sound_state->unk4C8;
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
void func_001CBE60(void) {
    if (sound_state->active != TRUE) {
        func_001CBAE0(1, 0x40);
        sound_state->active = TRUE;
    }
}

/* Turns the sound output off if it is on. */
static void func_001CBEA0(void) {
    if (sound_state->active != FALSE) {
        func_001CBAE0(1, 0x50);
        sound_state->active = FALSE;
    }
}

/* Switches every loaded voice and bank to its second data block (unk04). */
void func_001CBED8(void) {
    SoundVoice *voice;
    SoundBank *bank;
    int i;

    if (sound_state->active != 0) {
        func_001CBAE0(1, 0x50);
    }
    *(u16 *)&sound_state->unk4D0[4] = 0; // match: the u16 at 0x4D4; the header has six chars there
    func_001CBAE0(1, 0xC0, 0);
    voice = sound_state->voices;
    for (i = 0; i < 16; i++) {
        voice->unk1C = 0;
        if (voice->unk00 == 0) {
            continue;
        }
        if (voice->unk0C == 0) {
            continue;
        }
        if (voice->unk04 == 0) {
            continue;
        }
        func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, voice->unk04, voice->unk1B);
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
        func_001CBAE0(1, 0x2070, bank->unk0C, voice->unk0C, voice->unk04, voice->unk1B, 0);
        bank++; // match: as above
    }
    if (sound_state->active != 0) {
        func_001CBAE0(1, 0x40);
    }
}

/* Switches every loaded voice and bank to its first data block (unk00). */
void func_001CC018(void) {
    SoundVoice *voice;
    SoundBank *bank;
    int i;

    if (sound_state->active != 0) {
        func_001CBAE0(1, 0x50);
    }
    *(u16 *)&sound_state->unk4D0[4] = 1; // match: the u16 at 0x4D4; the header has six chars there
    func_001CBAE0(1, 0xC0, 1);
    voice = sound_state->voices;
    for (i = 0; i < 16; i++) {
        voice->unk1C = 1;
        if (voice->unk00 == 0) {
            continue;
        }
        if (voice->unk0C == 0) {
            continue;
        }
        if (voice->unk04 == 0) {
            continue;
        }
        func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, voice->unk00, voice->unk1B);
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
        func_001CBAE0(1, 0x2070, bank->unk0C, voice->unk0C, voice->unk00, voice->unk1B, 1);
        bank++; // match: as above
    }
    if (sound_state->active != 0) {
        func_001CBAE0(1, 0x40);
    }
}

/* The index of the first free voice, or -1 when all 16 are taken. */
int func_001CC160(void) {
    SoundVoice *voice = sound_state->voices;
    int i;

    for (i = 0; i < 16; i++, voice++) {
        if (voice->unk00 == 0) {
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
int func_001CC1A8(int voice_id, char *header_name, char *sample_name, int unk1B) {
    SoundVoice *voice = sound_state->voices;
    int voice_index;
    int block;
    int size;
    int i;

    for (i = 0; i < 16; i++, voice++) {
        if (voice->unk00 == 0) {
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
    voice->unk18 = voice_id;
    size = func_001CF070(header_name);
    if (size < 0) {
        return -1;
    }
    voice->unk10 = size;
    if (voice->unk00 != 0) {
        sceSifFreeIopHeap(voice->unk00);
    }
    if (voice->unk04 != 0) {
        sceSifFreeIopHeap(voice->unk04);
    }
    voice->unk00 = sceSifAllocIopHeap(size);
    voice->unk04 = sceSifAllocIopHeap(size);
    if (voice->unk00 == 0 || voice->unk04 == 0 || (u32)(voice->unk00 + voice->unk10) > 0x1FFFFF ||
        (u32)(voice->unk04 + voice->unk10) > 0x1FFFFF) {
        func_001CD6E0(voice_index);
        return -1;
    }
    if (func_001CF110(header_name, (void *)voice->unk00) != 0) {
        return -1; // match: the voice is not released here (the other loaders call func_001CD6E0 first)
    }
    func_001CBAE0(1, 0x1070, voice->unk04, voice->unk00, size);
    if (sample_name == NULL) {
        return voice_index;
    }
    size = func_001CF070(sample_name);
    if (size < 0) {
        func_001CD6E0(voice_index);
        return -1;
    }
    voice->unk14 = size;
    if (voice->unk0C == 0) {
        voice->unk0C = sound_state->unk4C0;
    }
    if ((u32)voice->unk0C >= sound_state->unk4C0) {
        sound_state->unk4C0 = voice->unk0C + voice->unk14;
    }
    voice->iop_heap = sceSifAllocIopHeap(size);
    if (voice->iop_heap == 0 || (u32)(voice->iop_heap + voice->unk14) > 0x1FFFFF) {
        func_001CD6E0(voice_index);
        return -1;
    }
    if (func_001CF110(sample_name, (void *)voice->iop_heap) != 0) {
        func_001CD6E0(voice_index);
        return -1;
    }
    if (0x1E7FBF - voice->unk0C < voice->unk14) {
        func_001CD6E0(voice_index);
        return -1;
    }
    if (voice->unk1C != 0) {
        block = voice->unk00;
    } else {
        block = voice->unk04;
    }
    if (func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, block, voice->unk1B) != 0) {
        func_001CD6E0(voice_index);
        return -1;
    }
    FlushCache(0);
    return voice_index;
}

/* Loads a voice from memory: header data to its two IOP blocks, sample data to sound memory.
   Returns the voice, -2 when none is free, -1 on failure. */
int func_001CC400(int voice_id, int header_data, int header_size, int sample_data, int sample_size, int unk1B) {
    SoundVoice *voice = sound_state->voices;
    int voice_index;
    int block;
    int i;

    for (i = 0; i < 16; i++, voice++) {
        if (voice->unk00 == 0) {
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
    voice->unk18 = voice_id;
    voice->unk1B = unk1B;
    while (header_size & 0xF) {
        header_size++;
    }
    voice->unk10 = header_size;
    if (voice->unk00 != 0) {
        sceSifFreeIopHeap(voice->unk00);
    }
    if (voice->unk04 != 0) {
        sceSifFreeIopHeap(voice->unk04);
    }
    voice->unk00 = sceSifAllocIopHeap(voice->unk10);
    voice->unk04 = sceSifAllocIopHeap(voice->unk10);
    if (voice->unk00 == 0 || voice->unk04 == 0 || (u32)(voice->unk00 + voice->unk10) > 0x1FFFFF ||
        (u32)(voice->unk04 + voice->unk10) > 0x1FFFFF) {
        func_001CD6E0(voice_index);
        return -1;
    }
    func_001CBC30(voice->unk00, header_data, voice->unk10);
    while (func_001CBD20() == 0) {
    }
    func_001CBAE0(1, 0x1070, voice->unk04, voice->unk00, voice->unk10);
    while (sample_size & 0xF) {
        sample_size++;
    }
    voice->unk14 = sample_size;
    if (voice->unk0C == 0) {
        voice->unk0C = sound_state->unk4C0;
    }
    if ((u32)voice->unk0C >= sound_state->unk4C0) {
        sound_state->unk4C0 = voice->unk0C + voice->unk14;
    }
    voice->iop_heap = sceSifAllocIopHeap(voice->unk14);
    if (voice->iop_heap == 0 || (u32)(voice->iop_heap + voice->unk14) > 0x1FFFFF) {
        func_001CD6E0(voice_index);
        return -1;
    }
    func_001CBC30(voice->iop_heap, sample_data, voice->unk14);
    while (func_001CBD20() == 0) {
    }
    if (0x1E7FBF - voice->unk0C < voice->unk14) {
        func_001CD6E0(voice_index);
        return -1;
    }
    if (voice->unk1C != 0) {
        block = voice->unk00;
    } else {
        block = voice->unk04;
    }
    if (func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, block, voice->unk1B) != 0) {
        func_001CD6E0(voice_index);
        return -1;
    }
    FlushCache(0);
    return voice_index;
}

/* Sets up a free voice with its own header data but the sample data of another voice.
   Returns the voice, -2 when none is free, -1 on failure. */
int func_001CC6A0(int voice_id, int source_index, int header_data, int header_size, int unk1B) {
    SoundVoice *source = &sound_state->voices[source_index];
    SoundVoice *voice = sound_state->voices;
    int voice_index;
    int block;
    int sample_size;
    int sample_addr;
    int i;

    for (i = 0; i < 16; i++, voice++) {
        if (voice->unk00 == 0) {
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
    voice->unk18 = voice_id;
    voice->unk1B = unk1B;
    while (header_size & 0xF) {
        header_size++;
    }
    voice->unk10 = header_size;
    if (voice->unk00 != 0) {
        sceSifFreeIopHeap(voice->unk00);
    }
    if (voice->unk04 != 0) {
        sceSifFreeIopHeap(voice->unk04);
    }
    voice->unk00 = sceSifAllocIopHeap(voice->unk10);
    voice->unk04 = sceSifAllocIopHeap(voice->unk10);
    if (voice->unk00 == 0 || voice->unk04 == 0 || (u32)(voice->unk00 + voice->unk10) > 0x1FFFFF ||
        (u32)(voice->unk04 + voice->unk10) > 0x1FFFFF) {
        func_001CD6E0(voice_index);
        return -1;
    }
    func_001CBC30(voice->unk00, header_data, voice->unk10);
    while (func_001CBD20() == 0) {
    }
    func_001CBAE0(1, 0x1070, voice->unk04, voice->unk00, voice->unk10);
    sample_size = source->unk14;
    sample_addr = source->unk0C;
    voice->unk14 = sample_size;
    voice->unk0C = sample_addr;
    voice->unk1A = source->unk1A;
    if (voice->unk1C != 0) {
        block = voice->unk00;
    } else {
        block = voice->unk04;
    }
    if (func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, block, voice->unk1B) != 0) {
        func_001CD6E0(voice_index);
        return -1;
    }
    FlushCache(0);
    return voice_index;
}

/* Loads a voice from memory like func_001CC400, but sends the sample data through a temporary
   IOP block, as large as the heap allows, a piece at a time.
   Returns the voice, -2 when none is free, -1 on failure. */
int func_001CC888(int voice_id, int header_data, int header_size, int sample_data, int sample_size, int unk1B) {
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
        if (voice->unk00 == 0) {
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
    voice->unk18 = voice_id;
    voice->unk1B = unk1B;
    while (header_size & 0xF) {
        header_size++;
    }
    voice->unk10 = header_size;
    if (voice->unk00 != 0) {
        sceSifFreeIopHeap(voice->unk00);
    }
    if (voice->unk04 != 0) {
        sceSifFreeIopHeap(voice->unk04);
    }
    voice->unk00 = sceSifAllocIopHeap(voice->unk10);
    voice->unk04 = sceSifAllocIopHeap(voice->unk10);
    if (voice->unk00 == 0 || voice->unk04 == 0 || (u32)(voice->unk00 + voice->unk10) > 0x1FFFFF ||
        (u32)(voice->unk04 + voice->unk10) > 0x1FFFFF) {
        func_001CD6E0(voice_index);
        return -1;
    }
    func_001CBC30(voice->unk00, header_data, voice->unk10);
    while (func_001CBD20() == 0) {
    }
    func_001CBAE0(1, 0x1070, voice->unk04, voice->unk00, voice->unk10);
    while (sample_size & 0xF) {
        sample_size++;
    }
    remaining = sample_size;
    voice->unk14 = remaining;
    if (voice->unk0C == 0) {
        voice->unk0C = sound_state->unk4C0;
    }
    if ((u32)voice->unk0C >= sound_state->unk4C0) {
        padded_size = voice->unk14;
        sound_state->unk4C0 = voice->unk0C + padded_size;
    } else {
        padded_size = voice->unk14; // match: read in both arms, so one load serves the test below
    }
    dest = voice->unk0C;
    if (0x1E7FBF - dest < padded_size) {
        func_001CD6E0(voice_index);
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
            func_001CBAE0(1, 0x80, voice->unk1A, 1);
            i = sceSifSetDma((int *)&sound_dma, 1); // match: the loop counter doubles as the DMA id
            if (i == 0) {
                sceSifFreeIopHeap(heap);
                func_001CD6E0(voice_index);
                return -1;
            }
            while (sceSifDmaStat(i) >= 0) {
            }
            sent = func_001CBAE0(1, 0x90, voice->unk1A, 0, heap, dest, sound_dma.size);
            if (sent != sound_dma.size) {
                sceSifFreeIopHeap(heap);
                func_001CD6E0(voice_index);
                return -1;
            }
            sample_data += sent;
            dest += sent;
            remaining -= sent;
        } while (remaining > 0);
    }
    func_001CBAE0(1, 0x80, voice->unk1A, 1);
    sceSifFreeIopHeap(heap);
    if (voice->unk1C != 0) {
        block = voice->unk00;
    } else {
        block = voice->unk04;
    }
    if (func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, block, voice->unk1B) != 0) {
        func_001CD6E0(voice_index);
        return -1;
    }
    FlushCache(0);
    return voice_index;
}

/* Sound load step: gives the voice up again. */
int func_001CCC08(int *args) {
    func_001CD6E0(args[0]);
    return 1;
}

/* Sound load step 0: clears the request, gets the voice's two data blocks on the IOP and
   uploads the first data to them. */
int func_001CCC28(SoundLoadArgs *args, SoundReloadRequest *request) {
    SoundVoice *voice;
    int voice_index;

    request->voice = NULL;
    request->unk04 = 0;
    request->unk08 = 0;
    *(void **)&request->unk0C = NULL; // match: unk0C and heap_addr are stored as pointers here
    *(void **)&request->heap_addr = NULL; // match: as above
    request->unk14 = 0;
    request->unk18 = 0;
    request->unk1C = 0;
    voice_index = args->unk00;
    voice = &sound_state->voices[voice_index];
    voice->unk18 = args->unk00;
    voice->unk1B = args->unk14;
    while (args->unk08 & 0xF) {
        args->unk08++;
    }
    voice->unk10 = args->unk08;
    voice->unk00 = sceSifAllocIopHeap(voice->unk10);
    voice->unk04 = sceSifAllocIopHeap(voice->unk10);
    if (voice->unk00 == 0 || voice->unk04 == 0 || (u32)(voice->unk00 + voice->unk10) > 0x1FFFFF ||
        (u32)(voice->unk04 + voice->unk10) > 0x1FFFFF) {
        func_001CD6E0(voice_index);
        return -1;
    }
    func_001CBC30(voice->unk00, args->unk04, voice->unk10);
    while (func_001CBD20() == 0) {
    }
    func_001CBAE0(1, 0x1070, voice->unk04, voice->unk00, voice->unk10);
    request->voice = voice;
    request->unk14 = voice_index;
    return 1;
}

/* Rounds the request size up to 16, then allocates the largest IOP heap block it can (halving from 1 MB). */
int func_001CCD78(IopLoadRequest *request, IopLoadContext *context) {
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

/* Sound load step: sends the next piece of the voice's data to IOP memory. */
int func_001CCDF0(SoundLoadArgs *args, SoundReloadRequest *request) {
    int dma_id;
    int heap_addr;
    u32 remaining;
    u32 piece_size;

    remaining = request->unk08;
    piece_size = request->unk18;
    if (piece_size < remaining) {
        sound_dma.size = piece_size;
    } else {
        sound_dma.size = remaining;
    }
    heap_addr = request->heap_addr;
    sound_dma.src = (int)args->unk0C;
    sound_dma.dest = heap_addr;
    sound_dma.mode = 0;
    if (func_001CBAE0(1, 0x80, request->voice->unk1A, 0) == 0) {
        printf(D_0043F688);
        return 0;
    }
    dma_id = sceSifSetDma((int *)&sound_dma, 1);
    if (dma_id == 0) {
        sceSifFreeIopHeap(request->heap_addr);
        func_001CD6E0(request->unk14);
        return -1;
    }
    request->unk1C = dma_id;
    return 1;
}

/* Sound load step: once the piece in flight has arrived it is written to sound memory;
   -1 goes back a step while more is left to send. */
int func_001CCEB8(SoundLoadArgs *args, SoundReloadRequest *request) {
    int sent;

    if (sceSifDmaStat(request->unk1C) >= 0) {
        return 0;
    }
    if (func_001CBAE0(1, 0x90, request->voice->unk1A, 0, request->heap_addr, request->unk0C, sound_dma.size) < 0) {
        sceSifFreeIopHeap(request->heap_addr);
        func_001CD6E0(request->unk14);
        return -1;
    }
    sent = sound_dma.size;
    args->unk0C += sent;
    request->unk0C += sent;
    request->unk08 -= sent;
    return (request->unk08 <= 0) ? 1 : -1;
}

int func_001CCF80(s32 unused, SoundReloadRequest *request) {
    SoundVoice *desc;
    s32 heap_src;

    desc = request->voice;
    if (func_001CBAE0(1, 0x80, desc->unk1A, 0) != 0) {
        sceSifFreeIopHeap(request->heap_addr);
        if (desc->unk1C != 0) {
            heap_src = desc->unk00;
        } else {
            heap_src = desc->unk04;
        }
        if (func_001CBAE0(1, 0x1010, desc->unk18, desc->unk0C, heap_src,
                          desc->unk1B) != 0) {
            func_001CD6E0(request->unk14);
            return -1;
        }
        FlushCache(0);
        return 1;
    }
    return 0;
}

/* Runs the handler for the current step with a packed argument block, then advances the step. */
int func_001CD020(int word0, int word1, int word2, int word3, int word4, int word5, int word6) {
    int args[7];
    int advance;

    if (D_004469E8 == 0) {
        voice_load_source = word3;
    }
    args[0] = word0;
    args[1] = word1;
    args[2] = word2;
    args[3] = voice_load_source;
    args[4] = word4;
    args[5] = word5;
    args[6] = word6;
    advance = D_0043F670[D_004469E8](args, D_00420940);
    voice_load_source = args[3];
    D_004469E8 += advance;
    if (D_004469E8 == 6) {
        D_004469E8 = 0;
    }
    if (D_004469E8 < 0) {
        D_004469E8 = 0;
        D_00446704++;
    }
    if (D_004469E8 == 0) {
        return D_00420940[5];
    }
    return -3;
}

/* Loads a voice from memory like func_001CC888, into a voice and a place in sound memory the
   caller chooses. Returns the voice, or -1 on failure. */
int func_001CD0E0(int voice_index, int header_data, int header_size, int sample_data, int sample_size, int unk1B,
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
    voice->unk18 = voice_index;
    while (header_size & 0xF) {
        header_size++;
    }
    voice->unk10 = header_size;
    if (voice->unk00 != 0) {
        sceSifFreeIopHeap(voice->unk00);
    }
    if (voice->unk04 != 0) {
        sceSifFreeIopHeap(voice->unk04);
    }
    voice->unk00 = sceSifAllocIopHeap(voice->unk10);
    voice->unk04 = sceSifAllocIopHeap(voice->unk10);
    if (voice->unk00 == 0 || voice->unk04 == 0 || (u32)(voice->unk00 + voice->unk10) > 0x1FFFFF ||
        (u32)(voice->unk04 + voice->unk10) > 0x1FFFFF) {
        func_001CD6E0(voice_index);
        return -1;
    }
    func_001CBC30(voice->unk00, header_data, voice->unk10);
    while (func_001CBD20() == 0) {
    }
    func_001CBAE0(1, 0x1070, voice->unk04, voice->unk00, voice->unk10);
    while (sample_size & 0xF) {
        sample_size++;
    }
    remaining = sample_size;
    voice->unk14 = remaining;
    voice->unk0C = dest;
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
            func_001CBAE0(1, 0x80, voice->unk1A, 1);
            i = sceSifSetDma((int *)&sound_dma, 1); // match: the loop counter doubles as the DMA id
            if (i == 0) {
                sceSifFreeIopHeap(heap);
                func_001CD6E0(voice_index);
                return -1;
            }
            while (sceSifDmaStat(i) >= 0) {
            }
            sent = func_001CBAE0(1, 0x90, voice->unk1A, 0, heap, write_addr, sound_dma.size);
            if (sent != sound_dma.size) {
                sceSifFreeIopHeap(heap);
                func_001CD6E0(voice_index);
                return -1;
            }
            sample_data += sent;
            write_addr += sent;
            remaining -= sent;
        } while (remaining > 0);
    }
    func_001CBAE0(1, 0x80, voice->unk1A, 1);
    sceSifFreeIopHeap(heap);
    if (voice->unk1C != 0) {
        block = voice->unk00;
    } else {
        block = voice->unk04;
    }
    if (func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, block, voice->unk1B) != 0) {
        func_001CD6E0(voice_index);
        return -1;
    }
    FlushCache(0);
    return voice_index;
}

/* Sets up a free voice from data that is already on the IOP: its first header block and its
   sample block. Returns the voice, -2 when none is free, -1 on failure. */
int func_001CD3D0(int voice_id, int header_block, int header_size, int sample_block, int sample_size, int unk1B) {
    SoundVoice *voice = sound_state->voices;
    int voice_index;
    int block;
    int i;

    for (i = 0; i < 16; i++, voice++) {
        if (voice->unk00 == 0) {
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
    voice->unk18 = voice_id;
    voice->unk1B = unk1B;
    voice->unk10 = header_size;
    voice->unk00 = header_block;
    if (header_block == 0) {
        return -1;
    }
    voice->unk04 = sceSifAllocIopHeap(header_size);
    if (voice->unk04 == 0) {
        return -1; // match: the voice is not released here (the other loaders call func_001CD6E0 first)
    }
    if ((u32)(voice->unk04 + voice->unk10) > 0x1FFFFF) {
        return -1; // match: the voice is not released here (the other loaders call func_001CD6E0 first)
    }
    func_001CBAE0(1, 0x1070, voice->unk04, voice->unk00, voice->unk10);
    voice->unk14 = sample_size;
    voice->iop_heap = sample_block;
    if (voice->unk0C == 0) {
        voice->unk0C = sound_state->unk4C0;
    }
    if ((u32)voice->unk0C >= sound_state->unk4C0) {
        sound_state->unk4C0 = voice->unk0C + voice->unk14;
    }
    if (voice->iop_heap == 0) {
        return -1; // match: the voice is not released here (the other loaders call func_001CD6E0 first)
    }
    if (0x1E7FBF - voice->unk0C < voice->unk14) {
        return -1; // match: the voice is not released here (the other loaders call func_001CD6E0 first)
    }
    if (voice->unk1C != 0) {
        block = voice->unk00;
    } else {
        block = voice->unk04;
    }
    if (func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, block, voice->unk1B) != 0) {
        return -1; // match: the voice is not released here (the other loaders call func_001CD6E0 first)
    }
    FlushCache(0);
    return voice_index;
}

/* Moves a voice to another channel and reloads its data there; 0 on success, -1 if the driver sent less. */
int func_001CD578(int voice_index, int channel) {
    SoundVoice *voice;

    voice = &sound_state->voices[voice_index];
    voice->unk1A = channel;
    return ((func_001CBAE0(1, 0x90, (char)channel, 0, voice->iop_heap, voice->unk0C, voice->unk14) ^ voice->unk14) == 0) ? 0 : -1;
}

/* Moves a voice to channel and to spu_addr in sound memory and reloads it; 0 on success, -1 on failure. */
int func_001CD5E0(int voice_index, char channel, u32 spu_addr) {
    SoundVoice *voice;
    int sent;
    u32 end_addr;

    voice = &sound_state->voices[voice_index];
    voice->unk1A = channel;
    if (spu_addr < 0x5010) {
        return -1;
    }
    if ((u32) (spu_addr + voice->unk14) > 0x1E7FBFU) {
        return -1;
    }
    voice->unk0C = spu_addr;
    if (func_001CBAE0(1, 0x1010, voice->unk18, spu_addr, voice->unk00, voice->unk1B) != 0) {
        return -1;
    }
    sent = func_001CBAE0(1, 0x90, voice->unk1A, 0, voice->iop_heap, voice->unk0C, voice->unk14);
    if (sent != voice->unk14) {
        return -1;
    }
    end_addr = spu_addr + sent;
    if (sound_state->unk4C0 < end_addr) {
        sound_state->unk4C0 = end_addr;
    }
    return 0;
}

int func_001CD6B8(int value) {
    return func_001CBAE0(1, 0x80, value, 0);
}

int func_001CD6E0(u32 voice_index) {
    SoundVoice *voice;
    u32 used_end;
    int block_base;
    int voices_left;

    if (voice_index >= 0x10U) {
        return -1;
    }
    voice = &sound_state->voices[voice_index];
    if (sound_state->unk4C0 == voice->unk0C + voice->unk14) {
        sound_state->unk4C0 = voice->unk0C;
    }
    voice->unk0C = 0;
    if (voice->iop_heap != 0) {
        if (sceSifFreeIopHeap(voice->iop_heap) != 0) {
            return -1;
        }
        voice->iop_heap = 0;
    }
    if (voice->unk00 != 0) {
        if (sceSifFreeIopHeap(voice->unk00) != 0) {
            return -1;
        }
        voice->unk00 = 0;
    }
    if (voice->unk04 != 0) {
        if (sceSifFreeIopHeap(voice->unk04) != 0) {
            return -1;
        }
        voice->unk04 = 0;
    }
    sound_state->unk4C0 = 0x5010;
    voices_left = 15;
    voice = sound_state->voices;
    do {
        used_end = sound_state->unk4C0;
        block_base = voice->unk0C;
        if ((u32)block_base >= used_end) {
            sound_state->unk4C0 = block_base + voice->unk14;
        }
        voice++;
        voices_left--;
    } while (voices_left >= 0);
    FlushCache(0);
    return 0;
}

/* Release the IOP heap block of a voice; returns -1 if none was held or the free failed. */
int func_001CD7E0(int voice_index) {
    SoundVoice *voice;

    voice = &sound_state->voices[voice_index];
    if (voice->iop_heap == 0) {
        return -1;
    }
    if (sceSifFreeIopHeap(voice->iop_heap) != 0) {
        return -1;
    }
    puts(D_004469F0);
    voice->iop_heap = 0;
    FlushCache(0);
    return 0;
}

/* Frees a voice's two data blocks on the IOP; -1 if either was missing or would not free. */
int func_001CD840(int voice_index) {
    SoundVoice *voice = &sound_state->voices[voice_index];
    int result = 0;

    if (voice->unk00 != 0) {
        if (sceSifFreeIopHeap(voice->unk00) != 0) {
            result = -1;
        } else {
            voice->unk00 = 0;
        }
    } else {
        result = -1;
    }
    if (voice->unk04 != 0) {
        if (sceSifFreeIopHeap(voice->unk04) != 0) {
            result = -1;
        } else {
            voice->unk04 = 0;
        }
    } else {
        result = -1;
    }
    FlushCache(0);
    return result;
}

/* Packs sound memory: sorts the voices by where their sample data sits, moves each one down
   to close the gaps, then rebinds every voice and bank. Returns -1 on failure. */
int func_001CD8C8(void) {
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
                    if ((u32)sorted[k]->unk0C > (u32)sorted[k + gap]->unk0C) {
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
        if (voice->unk0C == 0) {
            continue;
        }
        if (next_addr < (u32)voice->unk0C) {
            voice->unk0C = next_addr;
            func_001CBAE0(1, 0x80, voice->unk1A, 1);
            sent = func_001CBAE0(1, 0x90, voice->unk1A, 0, voice->iop_heap, voice->unk0C, voice->unk14);
            if (sent != voice->unk14) {
                return -1;
            }
            next_addr += sent;
            func_001CBAE0(1, 0x80, voice->unk1A, 1);
            if (voice->unk1C != 0) {
                block = voice->unk00;
            } else {
                block = voice->unk04;
            }
            if (func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, block, voice->unk1B) != 0) {
                return -1;
            }
        } else {
            next_addr = voice->unk0C + voice->unk14;
        }
    }
    sound_state->unk4C0 = next_addr;
    bank = sound_state->banks;
    for (i = 0; i < 4; i++, bank++) {
        if (bank->iop_heap == 0) {
            continue;
        }
        if (bank->voice == -1) {
            continue;
        }
        voice = &sound_state->voices[bank->voice];
        if (voice->unk1C != 0) {
            block = voice->unk00;
        } else {
            block = voice->unk04;
        }
        if (func_001CBAE0(1, 0x1010, bank->unk0C, voice->unk0C, block, voice->unk1B) != 0) {
            return -1;
        }
    }
    FlushCache(0);
    return 0;
}

/* Sets where in sound memory a voice's data goes. */
int func_001CDB58(u32 voice_index, int address) {
    if (voice_index >= 16) {
        return -1;
    }
    sound_state->voices[voice_index].unk0C = address;
    return 0;
}

void func_001CDB80(int voice_index, u8 value) {
    func_001CBAE0(1, 0x1040, sound_state->voices[voice_index].unk18, value);
}

void func_001CDBA0(int voice_index, u8 value) {
    func_001CBAE0(1, 0x1050, sound_state->voices[voice_index].unk18, value);
}

int func_001CDBC0(u32 voice_index, s16 value) {
    SoundVoice *voice;

    if (voice_index >= 0x11U) {
        return -1;
    }
    voice = &sound_state->voices[voice_index];
    if (voice->unk00 == 0) {
        return -1;
    }
    func_001CBAE0(1, 0x1060, voice->unk18, value);
    return 0;
}

int func_001CDC18(int voice_index) {
    SoundVoice *voice;
    SoundBank *bank;
    int banks_left;
    int result;

    voice = &sound_state->voices[voice_index];
    voice->unk1C = 0;
    bank = sound_state->banks;
    if (sound_state->active != 0) {
        func_001CBAE0(1, 0x50);
    }
    banks_left = 4;
    result = (func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, voice->unk04, voice->unk1B) != 0) ? -1 : 0;
    while (--banks_left != -1) {
        if (bank->voice == voice_index && func_001CBAE0(1, 0x2070, bank->unk0C, voice->unk0C, voice->unk04, voice->unk1B, 0) != 0) {
            result = -1;
            break;
        }
        bank++;
    }
    if (sound_state->active != 0) {
        func_001CBAE0(1, 0x40);
    }
    return result;
}

int func_001CDD20(int voice_index) {
    SoundVoice *voice = &sound_state->voices[voice_index];
    SoundBank *bank = sound_state->banks;
    int result = 0;
    int i;

    voice->unk1C = 1;
    if (sound_state->active != 0) {
        func_001CBAE0(1, 0x50);
    }
    if (func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, voice->unk00, voice->unk1B) != 0) {
        result = -1;
    }
    for (i = 4; --i != -1; bank++) {
        if (bank->voice == voice_index) {
            if (func_001CBAE0(1, 0x2070, bank->unk0C, voice->unk0C, voice->unk00, voice->unk1B, 1) != 0) {
                result = -1;
                break;
            }
        }
    }
    if (sound_state->active != 0) {
        func_001CBAE0(1, 0x40);
    }
    return result;
}

/* Loads a bank file to the IOP and binds it to a voice.
   Returns the bank, -2 when none is free, -1 on failure. */
int func_001CDE28(int bank_id, int voice_index, char *name) {
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
    bank->unk0C = bank_id;
    voice = &sound_state->voices[voice_index];
    bank->voice = voice_index;
    size = func_001CF070(name);
    if (size < 0) {
        return -1;
    }
    bank->unk04 = size;
    bank->iop_heap = sceSifAllocIopHeap(size);
    if (bank->iop_heap == 0) {
        return -1;
    }
    if ((u32)(bank->iop_heap + bank->unk04) > 0x1FFFFF) {
        return -1;
    }
    if (func_001CF110(name, (void *)bank->iop_heap) != 0) {
        return -1;
    }
    if (voice->unk1C != 0) {
        block = voice->unk00;
    } else {
        block = voice->unk04;
    }
    if (func_001CBAE0(1, 0x1010, bank->unk0C, voice->unk0C, block, voice->unk1B) != 0) {
        return -1;
    }
    if (func_001CBAE0(1, 0x2000, bank->unk0C, bank->iop_heap) != 0) {
        return -1;
    }
    FlushCache(0);
    return bank_index;
}

/* func_001CE170 in steps, to be called every frame until it stops returning -1: step 0 takes a free
   bank, allocates IOP memory and starts sending the data, step 1 waits for the transfer, step 2 hands
   bank and voice to the driver. Returns the bank's index when done, -2 when it failed. */
int func_001CDF90(int bank_id, int voice_index, int data, u32 size) {
    int index;
    int iop_heap;

    switch (D_004469F8) {
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
            D_004469F8 = 0;
            return -2;
        }
        if (bank_id == -1) {
            bank_id = bank_load_index;
        } else if ((u32)bank_id >= 4U) {
            return -1;
        }
        bank_load_bank->unk0C = bank_id;
        size = (size + 0xF) / 16 * 16;
        bank_load_bank->voice = voice_index;
        bank_load_voice = &sound_state->voices[voice_index];
        bank_load_bank->unk04 = size;
        iop_heap = sceSifAllocIopHeap(size);
        bank_load_bank->iop_heap = iop_heap;
        if (iop_heap == 0 || (u32)(iop_heap + bank_load_bank->unk04) > 0x1FFFFFU) {
            func_001CE408(bank_load_index);
            D_004469F8 = 0;
            return -2;
        }
        func_001CBC30(iop_heap, data, bank_load_bank->unk04);
        D_004469F8++;
        break;
    case 1:
        if (func_001CBD20() != 0) {
            D_004469F8++;
        }
        break;
    case 2:
        if (bank_load_voice->unk1C != 0) {
            bank_load_voice_data = bank_load_voice->unk00;
        } else {
            bank_load_voice_data = bank_load_voice->unk04;
        }
        if (func_001CBAE0(1, 0x1010, bank_load_bank->unk0C, bank_load_voice->unk0C, bank_load_voice_data, bank_load_voice->unk1B) != 0 ||
            func_001CBAE0(1, 0x2000, bank_load_bank->unk0C, bank_load_bank->iop_heap) != 0) {
            func_001CE408(bank_load_index);
            D_004469F8 = 0;
            return -2;
        }
        FlushCache(0);
        D_004469F8 = 0;
        break;
    }
    if (D_004469F8 != 0) {
        return -1;
    }
    return bank_load_index;
}

/* Loads a voice's data into the first free bank: allocates IOP memory for it, sends the data there
   (waiting until the transfer is done) and hands bank and voice to the driver.
   Returns the bank's index, -2 when all four are taken, -1 on any other failure. */
int func_001CE170(int bank_id, int voice_index, int data, int size) {
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
    bank->unk0C = bank_id;
    bank->voice = voice_index;
    while (size & 0xF) {
        size++;
    }
    bank->unk04 = size;
    iop_heap = sceSifAllocIopHeap(size);
    bank->iop_heap = iop_heap;
    if (iop_heap == 0 || (u32)(iop_heap + bank->unk04) > 0x1FFFFFU) {
        func_001CE408(free_index);
        return -1;
    }
    func_001CBC30(iop_heap, data, bank->unk04);
    while (func_001CBD20() == 0) {
    }
    if (voice->unk1C != 0) {
        heap_src = voice->unk00;
    } else {
        heap_src = voice->unk04;
    }
    if (func_001CBAE0(1, 0x1010, bank->unk0C, voice->unk0C, heap_src, voice->unk1B) != 0 ||
        func_001CBAE0(1, 0x2000, bank->unk0C, bank->iop_heap) != 0) {
        func_001CE408(free_index);
        return -1;
    }
    FlushCache(0);
    return free_index;
}

/* Takes the first free bank for a voice's data in IOP memory and hands both to the driver.
   Returns the bank's index, -2 when all four are taken, -1 on any other failure. */
int func_001CE300(int bank_id, int voice_index, int iop_heap, int size) {
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
    bank->unk0C = bank_id;
    bank->unk04 = size;
    bank->voice = voice_index;
    bank->iop_heap = iop_heap;
    if (iop_heap == 0) {
        return -1;
    }
    if (voice->unk1C != 0) {
        heap_src = voice->unk00;
    } else {
        heap_src = voice->unk04;
    }
    if (func_001CBAE0(1, 0x1010, bank->unk0C, voice->unk0C, heap_src, voice->unk1B) != 0) {
        return -1;
    }
    if (func_001CBAE0(1, 0x2000, bank->unk0C, bank->iop_heap) != 0) {
        return -1;
    }
    FlushCache(0);
    return free_index;
}

int func_001CE408(int i) {
    SoundBank *bank = &sound_state->banks[i];

    if (bank->iop_heap == 0) {
        return -1;
    }
    if (sceSifFreeIopHeap(bank->iop_heap) != 0) {
        return -1;
    }
    bank->unk04 = bank->iop_heap = 0;
    FlushCache(0);
    return 0;
}

int func_001CE468(u32 bank_index, int value) {
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
    return func_001CBAE0(1, 0x2030, bank->unk0C, value) == 0 ? 0 : -1;
}

/* Sends driver command 0x2010 with bank bank_index's second word; -1 for a bad index. */
int func_001CE4D0(u32 bank_index) {
    SoundBank *bank;

    if (bank_index >= 5U) {
        return -1;
    }
    bank = &sound_state->banks[bank_index];
    return func_001CBAE0(1, 0x2010, bank->unk0C);
}

int func_001CE510(u32 bank_index) {
    SoundBank *bank;

    if (bank_index >= 5) {
        return -1;
    }
    bank = &sound_state->banks[bank_index];
    if (bank->iop_heap == 0) {
        return -1;
    }
    return func_001CBAE0(1, 0x2040, bank->unk0C) == 0 ? 0 : -1;
}

/* Sends driver command 0x2050 with bank bank_index's second word if the bank holds IOP memory; -1 if not. */
int func_001CE568(u32 bank_index) {
    SoundBank *bank;

    if (bank_index >= 5U) {
        return -1;
    }
    bank = &sound_state->banks[bank_index];
    if (bank->iop_heap == 0) {
        return -1;
    }
    return (func_001CBAE0(1, 0x2050, bank->unk0C) == 0) ? 0 : -1;
}

/* Sends bank bank_index's second word to the driver if the bank holds IOP memory; -1 if not. */
int func_001CE5C0(u32 bank_index) {
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
    return (func_001CBAE0(1, 0x2060, bank->unk0C) == 0) ? 0 : -1;
}

/* Sends driver command 0x1060 with bank bank_index's second word and a signed 16-bit value. */
int func_001CE620(int bank_index, s16 value) {
    return func_001CBAE0(1, 0x1060, sound_state->banks[bank_index].unk0C, value);
}

/* Sends driver command 0x2080 with bank bank_index's second word and an unsigned 16-bit value. */
int func_001CE658(int bank_index, u16 value) {
    return func_001CBAE0(1, 0x2080, sound_state->banks[bank_index].unk0C, value);
}

/* Starts a note in slot `slot` on a voice: queues, in the buffer the IOP reads, an optional program
   change, the pan (0x3F unless the voice allows one), the bend as two 7-bit halves, the volume and the
   note itself. Returns the slot; -1: no driver or bad slot, -2: the buffer is full. */
int func_001CE690(u32 slot, int voice_index, char channel, char program, char note, char volume, s16 bend, char pan) {
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
    key = (SoundKey *)state->unk240[slot & 0x7F];
    key->unk03 = 1;
    key->voice = voice_index;
    key->unk02 = note;
    key->unk01 = channel;
    *out++ = 0xFF;
    *out++ = voice->unk18;
    *out++ = length - 3;
    if (program != -1) {
        *out++ = channel - 0x40;
        *out++ = program;
    }
    // match: the u16 at 0x4D4; the header has six chars there
    if (*(u16 *)&sound_state->unk4D0[4] == 0 || voice->unk1C == 0) {
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

int func_001CE8B0(u32 key_index) {
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
    key = (SoundKey *)sound_state->unk240[key_index];
    *write_ptr++ = 0xFF;
    // match: byte 0x18 of the voice (low byte of unk18), read as a byte
    *write_ptr++ = ((u8 *)sound_state + (key->voice << 5))[0x18];
    *write_ptr++ = 7;
    *write_ptr++ = 0xFD;
    *write_ptr++ = 0x10;
    *write_ptr++ = key->unk01;
    *write_ptr++ = key->unk02;
    *write_ptr++ = key_index;
    write_ptr[1] = write_ptr[0] = 0;
    *command_list += 0xA;
    key->unk03 = -1;
    return 0;
}

/* Queues a 10-byte command for the note in slot `slot` in the buffer the IOP reads:
   kind 0 and 1 send `value` as they are (1 forces 0x3F unless the voice allows a volume),
   kind 2 sends it as two 7-bit halves. -1: no driver or bad slot, -2: the buffer is full. */
int func_001CE9A0(int kind, u32 slot, int value) {
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
    key = (SoundKey *)state->unk240[slot];
    voice = &state->voices[key->voice];
    *out++ = 0xFF;
    *out++ = voice->unk18;
    *out++ = 7;
    switch (kind) {
    case 0:
        *out++ = 0xFD;
        *out++ = 0;
        *out++ = key->unk01;
        *out++ = key->unk02;
        *out++ = slot;
        out[0] = value;
        out[1] = 0;
        break;
    case 1:
        // match: the u16 at 0x4D4; the header has six chars there
        if (*(u16 *)&sound_state->unk4D0[4] == 0 || voice->unk1C == 0) {
            value = 0x3F;
        }
        *out++ = 0xFD;
        *out++ = 1;
        *out++ = key->unk01;
        *out++ = key->unk02;
        *out++ = slot;
        out[0] = value;
        out[1] = 0;
        break;
    case 2:
        *out++ = 0xFD;
        *out++ = kind;
        *out++ = key->unk01;
        *out++ = key->unk02;
        *out++ = slot;
        out[0] = value & 0x7F;
        out[1] = ((value >> 7) + 0x40) & 0x7F;
        break;
    }
    *queue_size += 0xA;
    return 0;
}

int func_001CEB38(int voice_index, char first_value, char second_value) {
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
    *write_ptr++ = (u8)sound_state->voices[voice_index].unk18;
    *write_ptr++ = 2;
    write_ptr[0] = (char)(first_value - 0x40);
    write_ptr[1] = second_value;
    *command_list += 5;
    return 0;
}

/* Starts a DMA transfer with a 16-byte aligned size and waits for it, then clears the uncached word. */
int func_001CEBE0(void) {
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
    func_001CBAE0(1, 0x1080);
    return 0;
}

int func_001CECB8(int stream, char *file_name, int value) {
    strcpy(D_00420A40, file_name);
    stream &= 1;
    sceSifCallRpc(sound_rpc_client, stream | 0x3000, 0, D_00420A40, 0x100, (int *)&sound_rpc_buffer, 1, 0, 0);
    if (sound_rpc_buffer.self == 0) {
        if (func_001CBAE0(1, 0x3040, stream, value) != 0) {
            func_001CBAE0(1, 0x3030, stream);
            return -1;
        }
        return stream;
    }
    return -1;
}

int func_001CED78(int stream, char *file_name, int value) {
    if (func_001CBAE0(1, 0x3040, stream, value) != 0) {
        func_001CBAE0(1, 0x3030, stream);
        return -1;
    }
    strcpy(D_00420A40, file_name);
    sceSifCallRpc(sound_rpc_client, stream | 0x30A0, 0, D_00420A40, 0x100, (int *)&sound_rpc_buffer, 1, 0, 0);
    if (sound_rpc_buffer.self != 0) {
        func_001CBAE0(1, 0x3030, stream);
        return -1;
    }
    return stream;
}

/* Sends driver command 0x30C0 for a stream with two values; returns the stream, or -1 when the driver refuses. */
int func_001CEE30(int stream, int first_value, int second_value) {
    return (func_001CBAE0(1, 0x30C0, stream, first_value, second_value) == 0) ? stream : -1;
}

/* Stores an effect setting in the first free of the four slots; returns the slot, or -2 when all are taken. */
int func_001CEE78(int effect_flags, s16 param1, s16 param2, s16 param3, s16 param4, s16 param5, s16 param6, int effect_id) {
    SoundEffectFields *effect;
    int i;
    int slot;

    for (effect = (SoundEffectFields *)sound_state->effects, i = 0; i < 4; i++, effect++) {
        if (effect->unk00 != -1) {
            continue;
        }
        slot = i;
        break;
    }
    if (!(i < 4)) {
        return -2;
    }
    effect->unk00 = effect_id;
    effect->unk04 = effect_flags | 0x100;
    effect->unk08 = param1;
    effect->unk0A = param2;
    effect->unk0C = param3;
    effect->unk10 = param4;
    effect->unk14 = param5;
    effect->unk16 = param6;
    FlushCache(0);
    return slot;
}

int func_001CEF48(u32 effect_index) {
    SoundEffectFields *effect;

    if (effect_index >= 5U) {
        return -1;
    }
    effect = (SoundEffectFields *)&sound_state->effects[effect_index];
    if (effect->unk00 == -1) {
        return -1;
    }
    effect->unk00 = -1;
    FlushCache(0);
    return 0;
}

void func_001CEFA0(u32 effect_index) {
    if (effect_index < 5) {
        sound_state->effect = sound_state->effects[effect_index];
        func_001CBAE0(1, 0x40070, &sound_state->effect);
    }
}

/* Copy entry index (0-4) of the table into the current record, then pass it to func_001CBAE0. */
void func_001CF008(u32 index) {
    if (index < 5) {
        sound_state->effect = sound_state->effects[index];
        func_001CBAE0(0, 0x40070, &sound_state->effect);
    }
}

/* Opens the file (retrying until it works), takes its size, and rounds it up to a multiple of 64. */
static int func_001CF070(const char *name) {
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

static int func_001CF110(char *name, void *addr) {
    if (sound_state->load_addr == NULL) {
        sound_state->load_addr = addr;
    }
    while (sceSifLoadIopHeap(name, addr) < 0) {
    }
    return 0;
}

void func_001CF168(void *buffer, int selector) {
    u8 *out_bytes;
    int *word;
    int words_left;

    func_001CBAE0(1, 0x1090, selector);
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

/* Sends slot voice_index's two words with param; the reply is re-packed into *status (some fields inverted). */
void func_001CF1F0(int voice_index, int param, SoundVoiceStatus *status) {
    SoundVoice *slot;
    u32 reply, inverted;

    slot = &sound_state->voices[voice_index];
    reply = func_001CBAE0(1, 0x110, slot->unk00, slot->unk10, param);
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

/* Send voice's two words with value, and status with its bit fields re-packed (some inverted). */
void func_001CF2E8(int voice_index, int value, u32 status) {
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
    func_001CBAE0(1, 0x120, voice->unk00, voice->unk10, value, packed);
}
