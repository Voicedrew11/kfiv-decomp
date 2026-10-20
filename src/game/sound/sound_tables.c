/* Sound bank buffers, load jobs and file names. */
#include "core/load.h"
#include "sound/sound.h"
#include "sound/sound_game.h"

/* the file name formats of the sound banks */
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

/* the two ways of starting a tracked sound (sound/sound_game.c) */
int Sound_PlayTracked(int sound, int unused, float *position);
int Sound_PlayTrackedAt(int sound, int unused, float *position);

// match: sound/sound.h's SoundFileBuffer is 0xC bytes; retail's bank slot is 0x20
typedef struct {
    SoundFileBuffer buffer;         /* 0x00 */
    int size_a;                     /* 0x0C */
    int size_b;                     /* 0x10 */
    int extra_size;                 /* 0x14 extra size of the BGM buffers */
    char pad18[0x08];               /* 0x18 */
} SoundBankSlot;

SoundBankSlot se_com_bank = { /* 0x3EF500 the se_com bank */
    .size_a = 0x2D000, .size_b = 0x1400,
};

SoundBankSlot sfx_com_bank = { /* 0x3EF520 the sfx_com bank */
    .size_a = 0x89800, .size_b = 0x2800,
};

SoundBankSlot bgm_buffers = { /* 0x3EF540 the BGM's buffers (bgm_load_job's) */
    .size_a = 0x4B000, .size_b = 0x1400, .extra_size = 0x5000,
};
asm(".globl bgm_header_buffer\nbgm_header_buffer = bgm_buffers + 0x4");  /* 0x3EF544 the BGM's second buffer (Sound_BgmReadHeader) */
asm(".globl bgm_sequence_buffer\nbgm_sequence_buffer = bgm_buffers + 0x8");  /* 0x3EF548 the BGM's third buffer (Sound_BgmReadSequence) */

SoundBankSlot sw_bank = { /* 0x3EF560 the sw bank (Sound_LoadSwBank) */
    .size_a = 0x1A400, .size_b = 0x1400,
};

SoundBankSlot mg_bank = { /* 0x3EF580 the mg bank (Sound_LoadMgBank) */
    .size_a = 0x8C00, .size_b = 0x1400,
};

SoundBankSpec map_bank_files = { /* 0x3EF5A0 the map bank's two files (Sound_LoadMapBank) */
    .size_a = 0x32000, .size_b = 0x1400,
};

// match: core/load.h's LoadJob is 0x28 bytes; retail's load job is 0x30 and 8-byte aligned
typedef struct {
    LoadJob job;                    /* 0x00 */
    int unk28;                      /* 0x28 -1 */
    int unk2C;                      /* 0x2C */
} __attribute__((aligned(8))) LoadJobSlot;

LoadJobSlot bgm_load_job = { /* 0x3EF5B8 the BGM load job */
    .job = {
        .buffers = (int *)&bgm_buffers, .name = bgm_bd_format, .header_format = bgm_hd_format,
        .name_format = bgm_sq_format, .file_handle = -1,
        /* 0x1C, inside the shared LoadJob's pad18: 0xBB810, the size of a BGM file */
        .file_size_area = { 0, 0, 0, 0, 0x10, 0xB8, 0x0B, 0, 0, 0, 0, 0 },
    },
    .unk28 = -1,
};

// match: sound/sound.h's SoundJob is 0x20 bytes; retail's sound job is 0x30, bank included
typedef struct {
    SoundJob job;                   /* 0x00 */
    SoundBank bank;                 /* 0x20 */
} SoundJobSlot;

SoundJobSlot se_com_job = { /* 0x3EF5E8 the se_com bank's job */
    .job = { .spec = (SoundJobSpec *)&se_com_bank, .name_a = se_com_bd_path, .name_b = se_com_hd_path,
             .result = 1, .voice_index = 1, .spu_dest = 0x5010 },
    .bank = { .iop_heap = 0, .iop_size = -1, .voice = -1 },
};

SoundJobSlot sfx_com_job = { /* 0x3EF618 the sfx_com bank's job */
    .job = { .spec = (SoundJobSpec *)&sfx_com_bank, .name_a = sfx_com_bd_path, .name_b = sfx_com_hd_path,
             .result = 2, .voice_index = 2, .spu_dest = 0x32010 },
    .bank = { .iop_heap = 0, .iop_size = -1, .voice = -1 },
};

SoundJobSlot map_bank_job = { /* 0x3EF648 the map bank's job (snd/map000) */
    .job = { .spec = (SoundJobSpec *)&map_bank_files, .name_a = map_bank_bd_path, .name_b = map_bank_hd_path,
             .result = 3, .voice_index = 3, .spu_dest = 0x106810 },
    .bank = { .iop_heap = 0, .iop_size = -1, .voice = -1 },
};

SoundJobSlot sw_bank_job = { /* 0x3EF678 the sw bank's job */
    .job = { .spec = (SoundJobSpec *)&sw_bank, .name_a = sw_bank_bd_format, .name_b = sw_bank_hd_format,
             .result = 4, .voice_index = 4, .spu_dest = 0x190010 },
    .bank = { .iop_heap = 0, .iop_size = -1, .voice = -1 },
};

SoundJobSlot mg_bank_job = { /* 0x3EF6A8 the mg bank's job */
    .job = { .spec = (SoundJobSpec *)&mg_bank, .name_a = mg_bank_bd_format, .name_b = mg_bank_hd_format,
             .result = 5, .voice_index = 5, .spu_dest = 0x1AA410 },
    .bank = { .iop_heap = 0, .iop_size = -1, .voice = -1 },
};

f32 sound_cos_polynomial[4] __attribute__((aligned(16))) = { /* 0x3EF6E0 what Sound_CosSin loads into VU0 */
    2.60188699e-06f, -0.000198074136f, 0.0083330255f, -0.166666567f,
};

u8 voice_ranges[8][2] = { /* 0x3EF6F0 the voice range of each group Sound_NextVoice walks */
    { 4, 5 }, { 6, 0x11 }, { 0x12, 0x39 }, { 0x3A, 0x53 },
    { 0x54, 0x63 }, { 0x64, 0x7D }, { 0, 0 }, { 0, 0 },
};

void *tracked_sound_players[4] = { /* 0x3EF700 the player Sound_PlayTrackedKind starts a tracked sound with */
    (void *)Sound_PlayTracked, (void *)Sound_PlayTrackedAt, NULL, NULL,
};

SoundChannel sound_channels[128] = { 0 }; /* 0x3EF710 the channels; sound_id is -1 when one is free */
