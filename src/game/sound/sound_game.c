/* The game's sound layer over the driver (sound/sound.c): voices, volumes, sounds played at a
   point and panned by where it lies from the player, tagged (looping) sounds, BGM, bank loading and the
   reverb settings of rev_para.dat. */
#include "core/load.h"
#include "common.h"
#include "sound/sound.h"
#include "player/player.h"
#include "model/model.h"
#include "core/file.h"
#include "map/map.h"
#include "effect/effect.h"
#include "sound/sound_game.h"

int File_IsLocked(void);
int File_Sync(int, int);

extern LoadJob bgm_load_job;
extern SoundJob se_com_job;
extern SoundJob sfx_com_job;
extern SoundFileBuffer se_com_bank;
extern SoundFileBuffer sfx_com_bank;
extern SoundChannel sound_channels[];
extern char sound_heap_base[];
extern int D_004468BC;
extern float D_004468C0;
extern int D_004468C4;
extern s8 map_reverb_set;
extern char map_sounds_enabled;
extern char reverb_change_pending;
extern s8 map_reverb_volume;
extern u8 loaded_bgm;
extern u8 bgm_load_running;
extern s8 positional_sounds_muted;
extern u8 loading_bgm;
void *memset(void *, int, u32);
extern u8 game_options[];
extern f32 sound_cos_polynomial[4];
extern u8 voice_ranges[][2];
extern u8 voice_range_cursors[8];
extern SoundJob map_bank_job;
extern SoundJob sw_bank_job;
extern SoundJob mg_bank_job;
extern u8 music_volume[];
extern int bgm_fade_level;
extern void *tracked_sound_players[];
extern PlayerState player_state;
extern u8 sfx_volume[];
double sqrt(double);
int rand(void);
float atan2f(float, float);
extern int tagged_bend;
extern int tagged_bend_high;
extern int tagged_bend_low;
extern char tagged_bend_falling;
extern int bgm_load_step;
extern int bgm_load_can_yield;
extern char *sound_bank_area;
extern int bgm_bd_size;
extern int sound_heap_next;
void File_Lock(void);
int File_GetSize(int);
int sprintf(char *, char *, ...);
void File_Unlock(void);
extern int bgm_header_buffer[];
extern int bgm_hd_size;
int File_Open(char *name, int nowait, int pack);
int File_Read(int file, void *buf, int size);
extern int bgm_sequence_buffer[];
extern int bgm_sq_size;
extern u8 bgm_reverb_slot;
extern int heap_file_read_step;
extern int heap_file_handle;
int File_Close(int file);
extern s32 map_bank_file_read_step;
extern s32 map_bank_file_handle;
extern MapHeader *map_slot0_header[];
int FlushCache(int);
int sceCdBreak(void);
extern int (*const bgm_load_steps[])(LoadJob *, s32);
extern char rev_para_path[];
extern s32 sound_file_load_step;
extern s32 (*const sound_file_load_steps[])(void);
extern int entity_bank_load_step;
extern int (*entity_bank_load_steps[])(void);
/* the falloff players Sound_PlayFalloff picks from (the Sfx and Map ones take no bank) */
extern int (*falloff_sound_players[])(int, int); // match: an int result, though the players are void
extern int map_bank_load_step;
extern SoundScriptState map_reverb_cue_indices;
extern u8 map_reverb_slot;
extern int D_004468B4;
extern int (*map_bank_load_steps[])(void);
extern SoundBankSpec map_bank_files;
extern SoundFileBuffer sw_bank;
extern SoundFileBuffer mg_bank;
extern int sound_load_queue[0x28 / 4];
extern MapSlot map_slots[];
extern int current_map_slot;

static char sound_file_path[0x80];         /* 0x4FF3C0 a file name for sprintf */
static char map_bank_file_path[0x80];         /* 0x4FF440 the map bank's file name */
static SoundStartSpec sound_load_spec;     /* 0x4FF4C0 the spec of the sound file load */
static SoundJob sound_load_job;           /* 0x4FF4D8 the sound file load job */
static u8 D_004FF4F8[0x18];           /* 0x4FF4F8 */
static SoundBankSpec map_bank_spec;      /* 0x4FF510 the spec of the map bank load */
static SoundJob map_bank_load_job;           /* 0x4FF528 the map bank load job */
static u8 D_004FF548[0x10];           /* 0x4FF548 */
static char bgm_cues[0x400];        /* 0x4FF558 the reverb settings of each BGM (SoundCue), 0x40 of them */
static char map_bank_cues[0x800];        /* 0x4FF958 the reverb settings of the maps (SoundCue), 0x80 of them */
static char tagged_voice_flags[0x18];         /* 0x500158 a claim flag per voice 0x54..0x6B */
static SoundChannel tagged_requests[0x80]; /* 0x500170 the queued tagged requests */
static s32 tracked_voices[30][2];         /* 0x502170 the tracked voices */

static void Sound_StartStopTrackedVoices(void);
static void Sound_CosSin(f32 angle, f32 *cos_out, f32 *sin_out);
static float Sound_Delta(f32 *out, f32 *point_a, f32 *point_b);
static int Sound_NextVoice(int slot);
static s8 Sound_ClaimTaggedVoice(void);
static void Sound_SetUpMixer(void);
static void Sound_PlayBgmSequence(int value);
static void Sound_SetBgmFade(s32 level);
static int Sound_TaggedNearestPoint(s32 index, Vector4 *nearest);
static f32 Sound_SegmentNearestPointXZ(Vector4 *start, Vector4 *end, Vector4 *nearest, f32 start_dx, f32 start_dz, f32 end_dx, f32 end_dz, f32 back_dx, f32 back_dz, f32 seg_dx, f32 seg_dz);
static void Sound_StartQueued(void);
static int Sound_PlayCommonFixedSlot(char sound);
static int Sound_ReadHeapFileStep(LoadJob *job, int index);
static void Sound_CancelBgmLoad(LoadJob *job);
static int Sound_StepBgmLoad(LoadJob *job, s32 state);
static int Sound_LoadReverbParams(void);
static s32 Sound_RunFileLoad(void);
static int Sound_RunEntityBankLoad(void *sequence, int index);
static int Sound_RunMapBankLoad(void *sequence, int kind);
static int Sound_LoadFilePair(char *name_a, char *name_b, SoundJobSpec *spec, int first_arg, int second_arg);
static int Sound_StepUpload(void);
static int Sound_UpdateMapReverbVolume(void);

/* Starts the sound driver and resets the sound state: loads the se_com and sfx_com banks into the heap,
   reads rev_para.dat, and frees every channel, queued request and claimed voice. */
void Sound_Init(void) {
    int i;
    u32 heap_start;

    D_004468BC = 0x40;
    D_004468C0 = 0.013f;
    D_004468C4 = 1;
    reverb_change_pending = 0;
    map_reverb_volume = 0;
    map_sounds_enabled = 0;
    loaded_bgm = 0xFF;
    bgm_load_running = 0;
    positional_sounds_muted = 0;
    map_reverb_set = 0;
    loading_bgm = 0xFF;
    memset(tracked_voices, 0, 0xD0);
    SoundDriver_Init();
    SoundDriver_Remote(1, 0x100, 0xA, 0x800);
    SoundDriver_EnableOutput();
    se_com_bank.used = 0;
    se_com_bank.base = heap_start = ((u32)sound_heap_base >> 6) << 6;
    se_com_bank.limit = heap_start + 0xA0000;
    Sound_LoadJob(&se_com_job);
    sfx_com_bank.base = se_com_bank.base;
    sfx_com_bank.limit = se_com_bank.limit;
    Sound_LoadJob(&sfx_com_job);
    for (i = 0; i < 0x80; i++) {
        sound_channels[i].sound_id = -1;
    }
    Sound_LoadReverbParams();
    for (i = 0; i < 0x80; i++) {
        tagged_requests[i].voice = -1;
    }
    for (i = 0; i < 0x10; i++) {
        tagged_voice_flags[i] = -1;
    }
    bgm_load_job.voice_index = 0;
    bgm_load_job.sequence_index = 0;
}

/* Starts and at once stops sound 0 of the sfx_com bank on each tracked voice (0x64..0x7D). Nothing calls
   it. */
static void Sound_StartStopTrackedVoices(void) {
    s32 i;

    for (i = 0x64; i < 0x7E; i++) {
        SoundDriver_NoteOn(i, sfx_com_job.result, 0, 0, 0x3C, game_options[2], 0, 0x3F);
        SoundDriver_NoteOff(i);
    }
}

/* Applies the stereo option (game_options[0]) to the driver: SoundDriver_SetStereo when it is set,
   SoundDriver_SetMono when not. */
void Sound_ApplyStereoOption(void) {
    if (game_options[0] != 0) {
        SoundDriver_SetStereo();
        return;
    }
    SoundDriver_SetMono();
}

/* Once a frame: starts the queued tagged sounds; then, unless a reverb change is pending (reverb_change_pending),
   updates the tagged channels and the map's reverb volume and sends the frame's note commands; otherwise
   waits for the driver to finish the change. */
void Sound_Update(void) {
    Sound_StartQueued();
    if (reverb_change_pending == 0) {
        Sound_UpdateTagged();
        Sound_UpdateMapReverbVolume();
        SoundDriver_FlushCommands();
        return;
    }
    if (SoundDriver_IsBusy() == 0) {
        reverb_change_pending = 0;
    }
}

/* Writes the cosine and sine of an angle within +-pi (VU0 macro code: a polynomial for the cosine in pi/2 -
   |angle|, the sine its square root complement, negated for a negative angle). The first output is the
   cosine. */
static void Sound_CosSin(f32 angle, f32 *cos_out, f32 *sin_out) {
    __asm__ volatile(
        "mtc1 $0, $f0\n"
        "c.lt.s %0, $f0\n"
        "li.s $f0, 1.5707964\n"
        "bc1f 0f\n"
        "add.s %0, $f0, %0\n"
        "li $7, 1\n"
        "j 1f\n"
        "0:\n"
        "sub.s %0, $f0, %0\n"
        "move $7, $0\n"
        "1:\n"
        "mfc1 $8, %0\n"
        "qmtc2 $8, $vf6\n"
        "la $8, sound_cos_polynomial\n"
        "lqc2 $vf5, 0($8)\n"
        "vmr32.w $vf6, $vf6\n"
        "vaddx.x $vf4, $vf0, $vf6x\n"
        "vmul.x $vf6, $vf6, $vf6\n"
        "vmulx.yzw $vf4, $vf4, $vf0x\n"
        "vmulw.xyzw $vf8, $vf5, $vf6w\n"
        "vsub.xyzw $vf5, $vf0, $vf0\n"
        "vmulx.xyzw $vf8, $vf8, $vf6x\n"
        "vmulx.xyz $vf8, $vf8, $vf6x\n"
        "vaddw.x $vf4, $vf4, $vf8w\n"
        "vmulx.xy $vf8, $vf8, $vf6x\n"
        "vaddz.x $vf4, $vf4, $vf8z\n"
        "vmulx.x $vf8, $vf8, $vf6x\n"
        "vaddy.x $vf4, $vf4, $vf8y\n"
        "vaddx.x $vf4, $vf4, $vf8x\n"
        "vaddx.xy $vf4, $vf5, $vf4x\n"
        "sqc2 $vf4, 0(%2)\n"
        "vmul.x $vf7, $vf4, $vf4\n"
        "vsubx.w $vf7, $vf0, $vf7x\n"
        "vsqrt Q, $vf7w\n"
        "vwaitq\n"
        "vaddq.x $vf7, $vf0, Q\n"
        "bnez $7, 2f\n"
        "vaddx.x $vf4, $vf5, $vf7x\n"
        "b 3f\n"
        "2:\n"
        "vsubx.x $vf4, $vf5, $vf7x\n"
        "3:\n"
        "sqc2 $vf4, 0(%3)\n"
        : "=f"(angle)
        : "0"(angle), "r"(cos_out), "r"(sin_out)
        : "$f0", "$7", "$8", "memory");
}

/* Writes point_a - point_b to out and returns its length (VU0 macro code). */
static float Sound_Delta(f32 *out, f32 *point_a, f32 *point_b) {
    f32 length[4];

    __asm__ volatile(
        "vaddw.xyz $vf31, $vf0, $vf0w\n"
        "lqc2 $vf1, 0(%1)\n"
        "lqc2 $vf2, 0(%2)\n"
        "vsub.xyzw $vf3, $vf1, $vf2\n"
        "vmul.xyzw $vf4, $vf3, $vf3\n"
        "vmulax.x ACC, $vf3, $vf3x\n"
        "vmadday.x ACC, $vf31, $vf4y\n"
        "vmaddz.x $vf5, $vf31, $vf4z\n"
        "vsqrt Q, $vf5x\n"
        "vwaitq\n"
        "vmulq.x $vf6, $vf31, Q\n"
        "sqc2 $vf3, 0(%0)\n"
        "sqc2 $vf6, 0(%3)\n"
        : : "r"(out), "r"(point_a), "r"(point_b), "r"(length) : "memory");
    return length[0];
}

/* Steps the cursor of voice group `group` round its range (voice_ranges) and returns the next voice. */
static int Sound_NextVoice(int slot) {
    s8 index = slot;
    u8 low = voice_ranges[index][0];
    s8 high = voice_ranges[index][1];
    s8 offset = voice_range_cursors[index] - low + 1;
    int next = offset % (high - (s8)low + 1) + low;

    voice_range_cursors[index] = next;
    return (s8)next;
}

/* Claims the first free of the 16 voices 0x54..0x63 for a tagged sound and returns it. */
static s8 Sound_ClaimTaggedVoice(void) {
    s8 *flag;
    s32 i;

    flag = tagged_voice_flags;
    for (i = 0; i < 16; i++, flag++) {
        if (*flag < 0) {
            *flag = 1;
            return i + 0x54;
        }
    }
}

/* Sets up the SPU2 mixer: command 0x100 (0xA, 0x800) as Sound_Init sends it, then through command 0xD0
   (an SPU2 parameter, encoded as libsd's sceSdSetParam does) the mixer switches of both cores (0xFC0,
   0xFCC) and the input volume of core 1 (left and right at 0x7FFF). Nothing calls it. */
static void Sound_SetUpMixer(void) {
    SoundDriver_Remote(1, 0x100, 0xA, 0x800);
    SoundDriver_Remote(1, 0xD0, 0x800, 0xFC0);
    SoundDriver_Remote(1, 0xD0, 0x801, 0xFCC);
    SoundDriver_Remote(1, 0xD0, 0xD81, 0x7FFF);
    SoundDriver_Remote(1, 0xD0, 0xE81, 0x7FFF);
}

/* Starts the BGM's sequence (bgm_load_job.sequence_index) playing, with value. Nothing calls it. */
static void Sound_PlayBgmSequence(int value) {
    SoundDriver_PlaySequence(bgm_load_job.sequence_index, value);
}

/* Sets the BGM sequence's volume (SoundDriver_SetSequenceVolume) to level, at most 0x7F: the fades of
   the BGM load steps. */
static void Sound_SetBgmFade(s32 level) {
    SoundDriver_SetSequenceVolume(bgm_load_job.sequence_index, (s16)(level < 0x80 ? level : 0x7F));
}

/* Sets the BGM voice's volume (SoundDriver_SetVoiceVolume) to level, kept within 0..0x7F: the music volume option. */
void Sound_SetBgmVolume(s32 level) {
    if (level < 0) {
        level = 0;
    }
    if (level > 0x7F) {
        level = 0x7F;
    }
    SoundDriver_SetVoiceVolume(bgm_load_job.voice_index, level);
}

/* Sets one volume (SoundDriver_SetVoiceVolume) on the voices of the five sound banks and on voices 6 to 9. */
void Sound_SetBankVolumes(int level) {
    s16 short_level = level;

    SoundDriver_SetVoiceVolume(se_com_job.result, short_level);
    SoundDriver_SetVoiceVolume(mg_bank_job.result, short_level);
    SoundDriver_SetVoiceVolume(sw_bank_job.result, short_level);
    SoundDriver_SetVoiceVolume(map_bank_job.result, short_level);
    SoundDriver_SetVoiceVolume(sfx_com_job.result, short_level);
    SoundDriver_SetVoiceVolume(6, short_level);
    SoundDriver_SetVoiceVolume(7, short_level);
    SoundDriver_SetVoiceVolume(8, short_level);
    SoundDriver_SetVoiceVolume(9, short_level);
}

/* Sets the BGM fade level (bgm_fade_level) to the music volume option (music_volume). */
void Sound_SyncBgmLevel(void) {
    bgm_fade_level = music_volume[0];
}

/* Starts a tracked sound with the player of `kind` in tracked_sound_players (0 Sound_PlayTracked, 1
   Sound_PlayTrackedAt). */
char Sound_PlayTrackedKind(int index, int first, int second, int third, int fourth) {
    int (*handler)(int, int, int, char);

    handler = tracked_sound_players[index];
    return handler(first, second, third, (char)fourth);
}

/* Starts a sound on the next tracked voice (0x64..0x7D) whose tracked_voices slot is free, from the mg bank
   (sound below 8) or the sfx_com bank, at the sound volume option; returns the voice, -1 if 11 tries find
   none. */
int Sound_PlayTracked(int sound, int unused, float *position) {
    float diff[4];
    float direction[4];
    float origin[4];
    float offset[4];
    SoundJob *bank;
    int slot;
    int tries;

    if (sound < 8) {
        bank = &mg_bank_job;
    } else if (sound < 0x7F) {
        bank = &sfx_com_job;
    } else {
        return -1;
    }
    tries = 0;
    Sound_Delta(diff, position, &player_state.position.x);
    Sound_CosSin(player_state.rotation.y, origin, offset);
    direction[0] = origin[0];
    direction[1] = 0.0f;
    direction[2] = -offset[0];
    for (;;) {
        slot = Sound_NextVoice(5);
        tries++;
        if (tracked_voices[slot - 100][0] == 0) {
            break;
        }
        if (tries >= 0xB) {
            return -1;
        }
    }
    SoundDriver_NoteOn(slot, bank->result, 0, sound, 0x3C, game_options[2], 0, 0x3F);
    tracked_voices[slot - 100][0] = 1;
    tracked_voices[slot - 100][1] = (u8)game_options[2];
    return slot;
}

/* As Sound_PlayTracked from the sfx_com bank, at a volume that falls off with the distance from the player
   (silent at 2500) and panned by the side the point lies on. */
int Sound_PlayTrackedAt(int sound, int unused, float *position) {
    float diff[4];
    float direction[4];
    float origin[4];
    float offset[4];
    float distance;
    float falloff;
    int volume;
    int pan;
    int slot;
    int tries;

    if (sound == 0) {
        return -1;
    }
    if (sound >= 0x80) {
        return -1;
    }
    tries = 0;
    distance = Sound_Delta(diff, position, &player_state.position.x);
    Sound_CosSin(player_state.rotation.y, origin, offset);
    direction[0] = origin[0];
    direction[1] = 0.0f;
    direction[2] = -offset[0];
    falloff = 1.0f - distance * distance * 0.0004f * 0.0004f;
    volume = sqrt(falloff) * 64.0;
    if (volume < 0) {
        volume = 0;
    }
    volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
    if (volume >= 0x80) {
        volume = 0x7F;
    }
    pan = ((diff[0] * direction[0] + diff[1] * direction[1] + diff[2] * direction[2]) / distance + 1.0f) * 63.0f;
    for (;;) {
        slot = Sound_NextVoice(5);
        tries++;
        if (tracked_voices[slot - 100][0] == 0) {
            break;
        }
        if (tries >= 0xB) {
            return -1;
        }
    }
    SoundDriver_NoteOn(slot, sfx_com_job.result, 0, sound, 0x3C, volume, 0, (char) pan);
    tracked_voices[slot - 100][0] = TRUE;
    tracked_voices[slot - 100][1] = volume;
    return slot;
}

/* Turns the volume of every playing tracked voice to 0. */
void Sound_MuteTracked(void) {
    int i;

    for (i = 0; i < 0x1A; i++) {
        if (tracked_voices[i][0] > 0) {
            SoundDriver_SetNoteControl(0, i + 100, 0);
        }
    }
}

/* Gives every playing tracked voice its volume back. */
void Sound_UnmuteTracked(void) {
    s32 i;
    for (i = 0; i < 0x1A; i++) {
        if (tracked_voices[i][0] > 0)
            SoundDriver_SetNoteControl(0, i + 100, tracked_voices[i][1]);
    }
}

/* Stops a tracked voice and frees its slot. */
void Sound_StopTracked(s8 slot) {
    if (slot > 0) {
        tracked_voices[slot - 100][0] = 0;
        SoundDriver_NoteOff(slot);
    }
}

/* Stops a voice (nothing for 0 or less). */
void Sound_StopVoice(s8 slot) {
    if (slot > 0) {
        SoundDriver_NoteOff(slot);
    }
}

/* Plays a sound of the se_com bank on voice 1 unless it is 0xFF (none): the menu sounds. */
s32 Sound_PlayMenu(s32 sound) {
    if (sound != 0xFF) {
        SoundDriver_NoteOn(1, se_com_job.result, 0, (char)sound, 0x3C, sfx_volume[0], 0, 0x3F);
    }
    /* match: no return; an s32 function, so $2 stays live and the beq delay slot takes the second lui */
}

/* Plays a sound of the sw bank on voice 2. */
void Sound_PlaySw(int mode) {
    SoundDriver_NoteOn(2, sw_bank_job.result, 0, (char)mode, 0x3C, sfx_volume[0], 0, 0x3F);
}

/* Plays a map bank sound at a point (while map_sounds_enabled is set): the volume falls off with the distance from
   the player (silent at 2500), panned by side, with a random bend. */
int Sound_PlayMapAt(u8 sound, void *position) {
    float origin[4];
    float offset[4];
    float direction[4];
    float diff[4];
    float distance;
    float falloff;
    float level;
    int volume;
    int pan;
    int detune;

    if (map_sounds_enabled == 0) {
        return -1;
    }
    distance = Sound_Delta(diff, position, &player_state.position.x);
    Sound_CosSin(player_state.rotation.y, origin, offset);
    direction[0] = origin[0];
    direction[1] = 0.0f;
    direction[2] = -offset[0];
    falloff = 1.0f - distance * distance * 0.0004f * 0.0004f;
    level = (int)(sqrt(falloff) * 64.0);
    if (level < 0.0f) {
        level = 0.0f;
    }
    volume = level * (float) sfx_volume[0] * 0.015625f;
    if (volume >= 0x80) {
        volume = 0x7F;
    }
    pan = ((diff[0] * direction[0] + diff[1] * direction[1] + diff[2] * direction[2]) / distance + 1.0f) * 63.0f;
    detune = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
    SoundDriver_NoteOn(Sound_NextVoice(3), map_bank_job.result, 0, sound, 0x3C, volume, detune, (char) pan);
    return 1;
}

/* Plays a footstep sound (0x20..0x27) from the se_com bank (below 0x22, 0x24, 0x25) or the map bank, with a
   random bend; returns the voice, -1 for other sounds. */
int Sound_PlayFootstep(int sound) {
    SoundJob *bank;
    int slot;
    int bend;

    if (sound < 0x22 || sound == 0x24 || sound == 0x25) {
        bank = &se_com_job;
    } else if (sound < 0x28) {
        bank = &map_bank_job;
    } else {
        return -1;
    }
    slot = Sound_NextVoice(1);
    bend = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
    SoundDriver_NoteOn(slot, bank->result, 0, sound, 0x3C, sfx_volume[0], bend, 0x3F);
    return slot;
}

/* Plays a sound of the se_com bank on the next voice of group 1; returns the voice. */
s32 Sound_PlayCommon(s32 value) {
    s32 slot;

    slot = Sound_NextVoice(1);
    SoundDriver_NoteOn(slot, se_com_job.result, 0, (char)value, 0x3C, sfx_volume[0], 0, 0x3F);
    return slot;
}

/* Plays a sound of the map bank on the next voice of group 1; returns the voice. */
int Sound_PlayMap(int value) {
    int result;

    result = Sound_NextVoice(1);
    SoundDriver_NoteOn(result, map_bank_job.result, 0, (char)value, 0x3C, sfx_volume[0], 0, 0x3F);
    return result;
}

/* Plays a se_com sound at a point within range: the volume falls off linearly, panned by where the point
   lies from the player's facing; returns the voice. */
int Sound_PlayCommonAt(float range, int sound, float *position) {
    float origin[4];
    float offset[4];
    float diff[4];
    float distance;
    float angle;
    int volume;
    int pan_pos;
    int slot;

    distance = Sound_Delta(diff, position, &player_state.position.x);
    if (!(range < distance)) {
        angle = atan2f(position[0] - player_state.position.x, position[2] - player_state.position.z) - player_state.rotation.y;
        angle -= (int)(angle / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        Sound_CosSin(angle, origin, offset);
        volume = (range - distance) * 64.0f * (1.0f / range);
        if (!((float) volume < 0.0f)) {
            pan_pos = (offset[0] + 1.0f) * 63.0f;
            volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
            if (volume >= 128) {
                volume = 127;
            }
            slot = Sound_NextVoice(3);
            SoundDriver_NoteOn(slot, se_com_job.result, 0, sound, 0x3C, volume, 0, (char) pan_pos);
            return slot;
        }
    }
}

/* Plays a map bank sound at volume (scaled by the sound option) with a random bend; -1 while map_sounds_enabled is
   clear. */
int Sound_PlayMapVolume(u8 sound, int volume) {
    int bend;
    int slot;

    if (map_sounds_enabled == 0) {
        return -1;
    }
    if (volume < 0) {
        volume = 0;
    }
    volume = (int) ((float) volume * (float) sfx_volume[0] * 0.015625f);
    if (volume >= 128) {
        volume = 127;
    }
    bend = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
    slot = Sound_NextVoice(3);
    if (volume != 0) {
        SoundDriver_NoteOn(slot, map_bank_job.result, 0, sound, 0x3C, volume, bend, 0x3F);
    }
    return 1;
}

/* Plays a map bank sound at a point within range, the volume falling off by mode (0 and 6 linear, 1 and 7
   steep, 2 and 8 round), panned; a random bend for modes below 3. */
int Sound_PlayMapAtRange(u8 sound, u8 mode, float *position, float range) {
    float origin[4];
    float offset[4];
    float diff[4];
    float distance;
    float level;
    float angle;
    int volume;
    int detune;
    int pan;
    int slot;

    if (map_sounds_enabled == 0) {
        return -1;
    }
    distance = Sound_Delta(diff, position, &player_state.position.x);
    switch (mode) {
    case 0:
    case 6:
        volume = (range - distance) * 64.0f * (1.0f / range);
        break;
    case 1:
    case 7:
        level = range * range;
        level -= (range - distance) * (range - distance);
        volume = (int)((1.0 - sqrt(level) / range) * 64.0) * 1.5f;
        break;
    case 2:
    case 8:
        level = range * range;
        level -= distance * distance;
        volume = sqrt(level) / range * 64.0;
        break;
    }
    level = volume;
    if (level < 0.0f) {
        level = 0.0f;
    }
    volume = level * (float) sfx_volume[0] * 0.015625f;
    if (volume >= 0x80) {
        volume = 0x7F;
    }
    angle = atan2f(position[0] - player_state.position.x, position[2] - player_state.position.z) - player_state.rotation.y;
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    Sound_CosSin(angle, origin, offset);
    pan = (offset[0] + 1.0f) * 63.0f;
    if (mode < 3) {
        detune = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
    } else {
        detune = 0;
    }
    slot = Sound_NextVoice(3);
    if (volume != 0) {
        SoundDriver_NoteOn(slot, map_bank_job.result, 0, sound, 0x3C, volume, detune, (char) pan);
    }
    return 1;
}

/* Starts a tagged sound (channel kind 0x63, no position) of the map bank at volume; queued in tagged_requests
   while map_sounds_enabled is clear. */
int Sound_StartTagged(s8 sound, u8 voice, int volume) {
    SoundChannel *channel;
    SoundChannel *request;
    int i;
    s8 voice_id;

    volume = (int) ((float) volume * (float) sfx_volume[0] * 0.015625f);
    if (volume >= 0x80) {
        volume = 0x7F;
    }
    if (map_sounds_enabled == 0) {
        for (i = 0; i < 0x80; i++) {
            if (tagged_requests[i].voice == -1) {
                request = &tagged_requests[i];
                request->voice = 1;
                request->sound_id = sound;
                request->note_slot = voice;
                request->mode = 0x63;
                request->volume_level = volume;
                return 0;
            }
        }
    }
    for (i = 0; i < 0x80; i++) {
        if (sound_channels[i].sound_id < 0) {
            break;
        }
    }
    channel = &sound_channels[i];
    channel->sound_id = sound;
    channel->kind = 0x63;
    channel->volume_level = volume;
    voice_id = channel->note_slot = Sound_ClaimTaggedVoice();
    SoundDriver_NoteOn(voice_id, map_bank_job.result, 0, voice, 0x3C, volume, 0, 0x3F);
}

/* Starts a tagged sound at a point, heard within range (SoundChannel.volume); queued in tagged_requests while
   map_sounds_enabled is clear. Sound_UpdateTagged gives it a voice. */
s32 Sound_StartTaggedAt(s8 sound_id, u8 voice, u8 mode, f32 volume, Vector4Arg *position) {
    SoundChannel *request;
    SoundChannel *channel;
    int i;

    if (map_sounds_enabled == 0) {
        for (i = 0; i < 0x80; i++) {
            if (tagged_requests[i].voice == -1) {
                request = &tagged_requests[i];
                request->sound_id = sound_id;
                request->note_slot = voice;
                request->volume = volume;
                request->kind = mode;
                request->mode = request->voice = 1;
                request->origin.x = position->x;
                request->origin.y = position->y;
                request->origin.z = position->z;
                return 0;
            }
        }
    }
    for (i = 0; i < 0x80; i++) {
        if (sound_channels[i].sound_id < 0) {
            break;
        }
    }
    channel = &sound_channels[i];
    channel->sound_id = sound_id;
    channel->voice = voice;
    channel->note_slot = -1;
    channel->origin.x = position->x;
    channel->origin.y = position->y;
    channel->origin.z = position->z;
    channel->mode = mode;
    channel->kind = 1;
    channel->volume = volume;
    /* match: no return; "return channel" adds a $2 copy retail lacks (implicit-int style fall-off) */
}

/* As Sound_StartTaggedAt for a sound along the segment origin..target. */
s32 Sound_StartTaggedAlong(s8 sound_id, u8 voice, u8 mode, Vector4Arg *origin, Vector4Arg *target, f32 volume) {
    SoundChannel *request;
    SoundChannel *channel;
    int i;

    if (map_sounds_enabled == 0) {
        for (i = 0; i < 0x80; i++) {
            if (tagged_requests[i].voice == -1) {
                request = &tagged_requests[i];
                request->voice = 1;
                request->sound_id = sound_id;
                request->note_slot = voice;
                request->volume = volume;
                request->kind = mode;
                request->mode = 2;
                request->origin.x = origin->x;
                request->origin.y = origin->y;
                request->origin.z = origin->z;
                request->target.x = target->x;
                request->target.y = target->y;
                request->target.z = target->z;
                return 0;
            }
        }
    }
    for (i = 0; i < 0x80; i++) {
        if (sound_channels[i].sound_id < 0) {
            break;
        }
    }
    channel = &sound_channels[i];
    channel->sound_id = sound_id;
    channel->voice = voice;
    channel->note_slot = -1;
    channel->origin.x = origin->x;
    channel->origin.y = origin->y;
    channel->origin.z = origin->z;
    channel->target.x = target->x;
    channel->target.y = target->y;
    channel->target.z = target->z;
    channel->mode = mode;
    channel->kind = 2;
    channel->volume = volume;
    /* match: no return; "return channel" adds a $2 copy retail lacks (implicit-int style fall-off) */
}

/* Each frame, updates the voice of every positioned tagged sound: stops it out of range, else sets its
   volume (by its falloff mode) and pan, with a wandering bend, and gives it a voice when it has none. */
void Sound_UpdateTagged(void) {
    float diff[4];
    float origin[4];
    float offset[4];
    Vector4 nearest;
    float distance;
    float level;
    float angle;
    int volume;
    int pan;
    int i;
    s8 voice;

    for (i = 0; i < 0x80; i++, tagged_bend = 0) {
        if (sound_channels[i].sound_id >= 0 && sound_channels[i].kind != 0x63) {
            Sound_TaggedNearestPoint(i, &nearest);
            distance = Sound_Delta(diff, &nearest.x, &player_state.position.x);
            if (sound_channels[i].volume < distance) {
                if (sound_channels[i].note_slot > 0) {
                    SoundDriver_NoteOff(sound_channels[i].note_slot);
                    tagged_voice_flags[sound_channels[i].note_slot - 0x54] = sound_channels[i].note_slot = -1;
                }
            } else {
                if (positional_sounds_muted == 0) {
                    switch ((u8) sound_channels[i].mode) {
                    case 0:
                    case 6:
                        volume = (sound_channels[i].volume - distance) * 64.0f * (1.0f / sound_channels[i].volume);
                        break;
                    case 1:
                    case 7:
                        level = sound_channels[i].volume - distance;
                        level = sound_channels[i].volume * sound_channels[i].volume - level * level;
                        volume = (int)((1.0 - sqrt(level) / sound_channels[i].volume) * 64.0) * 1.5f;
                        break;
                    case 2:
                    case 8:
                        level = sound_channels[i].volume * sound_channels[i].volume - distance * distance;
                        volume = sqrt(level) / sound_channels[i].volume * 64.0;
                        break;
                    }
                    if (volume <= 0) {
                        if (sound_channels[i].note_slot > 0) {
                            SoundDriver_NoteOff(sound_channels[i].note_slot);
                            tagged_voice_flags[sound_channels[i].note_slot - 0x54] = sound_channels[i].note_slot = -1;
                        }
                        continue;
                    } else {
                        volume = (float) volume * (float) game_options[2] * 0.015625f;
                        if (volume >= 0x80) {
                            volume = 0x7F;
                        }
                    }
                } else {
                    volume = 0;
                }
                angle = atan2f(nearest.x - player_state.position.x, nearest.z - player_state.position.z) -
                        player_state.rotation.y;
                angle -= (int)(angle / 6.2831855f) * 6.2831855f;
                if (3.1415927f < angle) {
                    angle -= 6.2831855f;
                } else if (angle < -3.1415927f) {
                    angle += 6.2831855f;
                }
                Sound_CosSin(angle, origin, offset);
                pan = (offset[0] * 0.6f + 1.0f) * 63.0f;
                if ((u8) sound_channels[i].mode < 6 || (u8) sound_channels[i].mode > 8) {
                    if (tagged_bend_falling != 0) {
                        tagged_bend -= rand() % 60;
                    } else {
                        tagged_bend += rand() % 60;
                    }
                    if (tagged_bend_high < tagged_bend) {
                        tagged_bend_low = -(rand() % 0x555 + 0x555);
                        tagged_bend_falling = 1;
                    } else if (tagged_bend < tagged_bend_low) {
                        tagged_bend_high = rand() % 0x555 + 0x555;
                        tagged_bend_falling = 0;
                    }
                    SoundDriver_SetNoteControl(2, sound_channels[i].note_slot, tagged_bend);
                }
                if (sound_channels[i].note_slot < 0) {
                    voice = sound_channels[i].note_slot = Sound_ClaimTaggedVoice();
                    SoundDriver_NoteOn(voice, map_bank_job.result, 0, sound_channels[i].voice, 0x3C, volume, tagged_bend, (char) pan);
                    continue;
                }
                SoundDriver_SetNoteControl(0, sound_channels[i].note_slot, volume);
                SoundDriver_SetNoteControl(1, sound_channels[i].note_slot, pan);
            }
        }
    }
}

/* Writes where a tagged sound comes from nearest to the player: its point, or the nearest point of its
   segment. */
static int Sound_TaggedNearestPoint(s32 index, Vector4 *nearest) {
    Vector4Arg start;
    Vector4Arg end;
    char reserved[0x40]; /* unused, but retail reserves the 0x40 bytes */
    f32 seg_x;
    f32 seg_y;
    f32 seg_z;
    f32 to_start_x;
    f32 to_start_y;
    f32 to_start_z;
    f32 to_end_x;
    f32 to_end_y;
    f32 to_end_z;
    f32 back_x;
    f32 back_y;
    f32 back_z;
    f32 dot_start;
    f32 cos_start;
    f32 cos_end;
    f32 along;

    switch (sound_channels[index].kind) {
    case 1:
        nearest->x = sound_channels[index].origin.x;
        nearest->y = sound_channels[index].origin.y;
        nearest->z = sound_channels[index].origin.z;
        break;
    case 2:
        start = sound_channels[index].origin;
        // match: retail adds the 0x10 to the table base before the index
        end = (&sound_channels[0].target)[index * 4];
        to_start_x = player_state.position.x - start.x;
        seg_x = end.x - start.x;
        to_start_y = player_state.position.y - start.y;
        seg_y = end.y - start.y;
        back_x = start.x - end.x;
        back_y = start.y - end.y;
        back_z = start.z - end.z;
        to_end_x = player_state.position.x - end.x;
        to_start_z = player_state.position.z - start.z;
        seg_z = end.z - start.z;
        to_end_y = player_state.position.y - end.y;
        to_end_z = player_state.position.z - end.z;
        dot_start = to_start_x * seg_x + to_start_y * seg_y + to_start_z * seg_z;
        cos_start = dot_start / (sqrt(to_start_x * to_start_x + to_start_y * to_start_y + to_start_z * to_start_z) * sqrt(seg_x * seg_x + seg_y * seg_y + seg_z * seg_z));
        cos_end = (to_end_x * back_x + to_end_y * back_y + to_end_z * back_z) / (sqrt(to_end_x * to_end_x + to_end_y * to_end_y + to_end_z * to_end_z) * sqrt(back_x * back_x + back_y * back_y + back_z * back_z));
        if (cos_start > 0.0f && cos_start < 1.0f && cos_end > 0.0f && cos_end < 1.0f) {
            along = dot_start / (seg_x * seg_x + seg_y * seg_y + seg_z * seg_z);
            nearest->x = start.x + seg_x * along;
            nearest->y = start.y + seg_y * along;
            nearest->z = start.z + seg_z * along;
        } else if (cos_start <= 0.0f) {
            nearest->x = start.x;
            nearest->y = start.y;
            nearest->z = start.z;
        } else if (cos_end <= 0.0f) {
            nearest->x = end.x;
            nearest->y = end.y;
            nearest->z = end.z;
        }
        break;
    case 3:
        break;
    }
}

/* Finds the point on the segment start..end nearest the player (x/z only) and returns its distance; the
   floats are the player and the segment seen from each end. Nothing calls it. */
static f32 Sound_SegmentNearestPointXZ(Vector4 *start, Vector4 *end, Vector4 *nearest, f32 start_dx, f32 start_dz, f32 end_dx, f32 end_dz, f32 back_dx, f32 back_dz, f32 seg_dx, f32 seg_dz) {
    f32 dot_start;
    f32 dot_end;
    f32 cos_start;
    f32 cos_end;
    f32 along;

    dot_start = start_dx * seg_dx + start_dz * seg_dz;
    cos_start = dot_start / (sqrt(start_dx * start_dx + start_dz * start_dz) * sqrt(seg_dx * seg_dx + seg_dz * seg_dz));
    dot_end = end_dx * back_dx + end_dz * back_dz;
    cos_end = dot_end / (sqrt(end_dx * end_dx + end_dz * end_dz) * sqrt(back_dx * back_dx + back_dz * back_dz));
    if (cos_start > 0.0f && cos_start < 1.0f && cos_end > 0.0f && cos_end < 1.0f) {
        along = dot_start / (seg_dx * seg_dx + seg_dz * seg_dz);
        nearest->x = start->x + seg_dx * along;
        nearest->y = 0.0f;
        nearest->z = start->z + seg_dz * along;
    } else if (cos_start <= 0.0f) {
        nearest->x = start->x;
        nearest->y = start->y;
        nearest->z = start->z;
    } else if (cos_end <= 0.0f) {
        nearest->x = end->x;
        nearest->y = end->y;
        nearest->z = end->z;
    }
    return sqrt((player_state.position.x - nearest->x) * (player_state.position.x - nearest->x) + (player_state.position.z - nearest->z) * (player_state.position.z - nearest->z));
}

/* Starts the sounds queued in tagged_requests once map_sounds_enabled is set, freeing each request. */
static void Sound_StartQueued(void) {
    int i;
    SoundChannel *request;

    if (map_sounds_enabled != 0) {
        for (i = 0; i < 0x80; i++) {
            request = &tagged_requests[i];
            if (request->voice > 0) {
                switch (request->mode) {
                case 1:
                    request->voice = -1;
                    Sound_StartTaggedAt(request->sound_id, request->note_slot, request->kind, request->volume, &request->origin);
                    break;
                case 2:
                    request->voice = -1;
                    // match: target is indexed off the array; through request it is not strength-reduced
                    Sound_StartTaggedAlong(request->sound_id, request->note_slot, request->kind, &request->origin, &tagged_requests[i].target, request->volume);
                    break;
                case 0x63:
                    sound_channels[i].sound_id = request->sound_id;
                    sound_channels[i].kind = 0x63;
                    sound_channels[i].volume_level = request->volume_level;
                    request->voice = -1;
                    sound_channels[i].note_slot = Sound_ClaimTaggedVoice();
                    SoundDriver_NoteOn(sound_channels[i].note_slot, map_bank_job.result, 0, request->note_slot, 0x3C, request->volume_level, 0, 0x3F);
                    break;
                }
            }
        }
    }
}

/* Turns the volume of every playing unpositioned tagged sound (kind 0x63) to 0. */
void Sound_MuteTagged(void) {
    SoundChannel *entry = sound_channels;
    int n = 0x7F;

    do {
        n--;
        if (entry->sound_id >= 0 && entry->kind == 0x63) {
            SoundDriver_SetNoteControl(0, entry->note_slot, 0);
        }
        entry++;
    } while (n >= 0);
}

/* Gives every playing unpositioned tagged sound its volume back. */
void Sound_UnmuteTagged(void) {
    SoundChannel *channel = sound_channels;
    int n = 0x7F;
    int volume;

    do {
        n--;
        if (channel->sound_id >= 0 && channel->kind == 0x63) {
            volume = (int)((float)channel->volume_level * (float)game_options[2] * 0.015625f);
            SoundDriver_SetNoteControl(0, channel->note_slot, (volume >= 0x80) ? 0x7F : volume);
        }
        channel++;
    } while (n >= 0);
}

/* Stops the tagged sound with tag `tag` and frees its channel; -1 if there is none. */
int Sound_StopTagged(char sound) {
    int i;

    for (i = 0; i < 0x80; i++) {
        if (sound_channels[i].sound_id == sound) {
            break;
        }
    }
    if (i >= 0x80) {
        return -1;
    }
    if ((sound_channels + i)->note_slot > 0) {
        SoundDriver_NoteOff((sound_channels + i)->note_slot);
        (sound_channels + i)->sound_id = -1;
        tagged_voice_flags[(sound_channels + i)->note_slot - 0x54] = -1;
    } else {
        (sound_channels + i)->sound_id = -1;
    }
}

/* Silences every sound voice and marks all channels (and optionally all queued requests) free. */
void Sound_StopAll(int clear_queue) {
    int i;

    map_sounds_enabled = 0;
    for (i = 0x54; i < 0x64; i++) {
        SoundDriver_NoteOff(i);
    }
    for (i = 0x3A; i < 0x54; i++) {
        SoundDriver_NoteOff(i);
    }
    for (i = 0; i < 0x80; i++) {
        sound_channels[i].sound_id = -1;
        tagged_voice_flags[sound_channels[i].note_slot - 0x54] = -1;
    }
    if (clear_queue != 0) {
        for (i = 0; i < 0x80; i++) {
            tagged_requests[i].voice = -1;
        }
    }
}

/* Plays a sound of the se_com bank in note slot 0 (Sound_PlayMenu uses slot 1). Nothing calls it. */
static int Sound_PlayCommonFixedSlot(char sound) {
    SoundDriver_NoteOn(0, se_com_job.result, 0, sound, 0x3C, sfx_volume[0], 0, 0x3F);
    return 0;
}

/* BGM load step: opens the job's .bd file for the bgm number at a 64-byte aligned address and starts reading
   it; -1 while the file layer or the driver is busy. */
int Sound_BgmReadBody(LoadJob *job, int index) {
    char name[128];
    int handle;
    int load_addr;

    if (File_IsLocked() != 0) {
        bgm_load_can_yield = 1;
        return -1;
    }
    if (SoundDriver_StepUpload() == 0) {
        return -1;
    }
    load_addr = ((u32)(sound_bank_area + 0x3F) >> 6) << 6;
    job->buffers[0] = sound_heap_next = load_addr;
    sound_heap_next = load_addr + 0x80000;
    sprintf(name, job->name, index);
    handle = File_Open(name, TRUE, TRUE);
    job->file_handle = handle;
    bgm_bd_size = File_GetSize(handle);
    File_Lock();
    File_Read(job->file_handle, (void *)job->buffers[0], -1);
    bgm_load_step += 1;
    return 0;
}

/* BGM load step: once the driver is free, fades the playing BGM down by 3 a frame; done at 0. Should the
   job hold no voice or sequence (a negative result), it first loads one (SoundDriver_LoadVoiceStep,
   SoundDriver_LoadSequenceStep) and frees it again. */
int Sound_BgmFadeOut(IopLoadJob *job) {
    int *spec;
    int level;
    int voice;
    int result;

    if (SoundDriver_StepUpload() == 0) {
        return -1;
    }
    if (SoundDriver_IsBusy() != 0) {
        return -1;
    }
    voice = job->result;
    if (voice < 0) {
        spec = job->ctl;
        result = SoundDriver_LoadVoiceStep(job->load_index, spec[1], spec[4], spec[0], spec[3], 0, job->spu_dest);
        job->result = result;
        if (result >= 0) {
            SoundDriver_FreeVoice(result);
        }
        return -1;
    } else {
        if (job->sequence_index < 0) {
            spec = job->ctl;
            result = SoundDriver_LoadSequenceStep(job->load_index, voice, spec[2], spec[5]);
            job->sequence_index = result;
            if (result >= 0) {
                SoundDriver_FreeVoice(job->result);
                SoundDriver_FreeSequence(job->sequence_index);
            }
            return -1;
        }
        bgm_load_can_yield = 1;
        bgm_fade_level -= 3;
        if (bgm_fade_level < 0) {
            bgm_fade_level = 0;
        }
        level = bgm_fade_level;
        Sound_SetBgmFade((level < music_volume[0]) ? level : music_volume[0]);
        if (bgm_fade_level == 0) {
            bgm_load_step += 1;
            return 0;
        }
        return -1;
    }
}

/* BGM load step: once the read has finished, closes the file and frees the old BGM's bank and voice. */
int Sound_BgmCloseBody(LoadJob *job) {
    if (File_Sync(job->file_handle, 1) != 0) {
        return -1;
    }
    File_Close(job->file_handle);
    File_Unlock();
    job->file_handle = -1;
    SoundDriver_StopSequence(job->sequence_index);
    SoundDriver_FreeSequence(job->sequence_index);
    SoundDriver_FreeVoice(job->voice_index);
    bgm_load_can_yield = 1;
    bgm_load_step += 1;
    return 0;
}

/* BGM load step: opens the job's .hd file for the bgm number and starts reading it into the spec's second
   buffer; -1 while busy. */
int Sound_BgmReadHeader(LoadJob *job, int index) {
    char name[128];
    int file;
    int heap_top;
    int *buffers;

    if (File_IsLocked() != 0) {
        bgm_load_can_yield = 1;
        return -1;
    }
    if (SoundDriver_StepUpload() == 0) {
        return -1;
    }
    heap_top = sound_heap_next;
    bgm_header_buffer[0] = heap_top;
    sound_heap_next = ((u32)(heap_top + 0x2003F) >> 6) << 6;
    sprintf(name, job->header_format, index);
    file = File_Open(name, 1, 1);
    job->file_handle = file;
    bgm_hd_size = File_GetSize(file);
    File_Lock();
    buffers = job->buffers;
    File_Read(job->file_handle, (void *)buffers[1], -1);
    bgm_load_step += 1;
    return 0;
}

/* BGM load step: once the read has finished, closes the file. */
int Sound_BgmCloseHeader(LoadJob *job) {
    if (File_Sync(job->file_handle, 1) == 0) {
        File_Close(job->file_handle);
        File_Unlock();
        job->file_handle = -1;
        bgm_load_can_yield = 1;
        bgm_load_step += 1;
        return 0;
    }
    return -1;
}

/* BGM load step: opens the job's .sq file for the bgm number and starts reading it into the third buffer; -1
   while busy. */
int Sound_BgmReadSequence(LoadJob *job, int index) {
    char name[128];
    int handle;
    int heap_top;
    int *buffers;

    if (File_IsLocked() != 0) {
        return -1;
    }
    if (SoundDriver_StepUpload() == 0) {
        return -1;
    }
    heap_top = sound_heap_next;
    bgm_sequence_buffer[0] = heap_top;
    sound_heap_next = ((u32)(heap_top + 0x2003F) >> 6) << 6;
    sprintf(name, job->name_format, index);
    handle = File_Open(name, TRUE, TRUE);
    job->file_handle = handle;
    bgm_sq_size = File_GetSize(handle);
    File_Lock();
    buffers = job->buffers;
    File_Read(job->file_handle, (void *)buffers[2], -1);
    bgm_load_step += 1;
    return 0;
}

/* BGM load step: once the read has finished, closes the file. */
int Sound_BgmCloseSequence(LoadJob *job) {
    if (File_Sync(job->file_handle, 1) != 0) {
        return -1;
    }
    File_Close(job->file_handle);
    File_Unlock();
    job->file_handle = -1;
    bgm_load_step += 1;
    return 0;
}

/* BGM load step: loads the sequence (SoundDriver_LoadSequenceStep) and starts it playing; -1 until it is
   loaded. */
int Sound_BgmStartSequence(IopLoadJob *job) {
    int *spec;
    int result;

    spec = job->ctl;
    result = SoundDriver_LoadSequenceStep(job->load_index, job->result, spec[2], spec[5]);
    job->sequence_index = result;
    if (result >= 0) {
        SoundDriver_PlaySequence(result, 0);
        bgm_load_step += 1;
        return 0;
    }
    return -1;
}

/* BGM load step: raises the BGM fade level by 3 a frame (applied unless the bgm is 0x12) until it passes the
   music volume option. */
int Sound_BgmFadeIn(int unused, int mode) {
    if (SoundDriver_IsBusy() != 0) {
        return -1;
    }
    bgm_load_can_yield = 1;
    bgm_fade_level += 3;
    if (mode != 0x12) {
        Sound_SetBgmFade((bgm_fade_level < music_volume[0]) ? bgm_fade_level : music_volume[0]);
    }
    if (bgm_fade_level >= 0x100 || music_volume[0] < bgm_fade_level) {
        bgm_load_step += 1;
        return 1;
    }
    return -1;
}

/* BGM load step: frees the reverb slot of the last BGM and stores the reverb settings of BGM cue_index
   from rev_para.dat (none for 0x12). */
int Sound_BgmStartCue(void *context, int cue_index) {
    SoundCue *cue;

    if (cue_index == 0x12) {
        bgm_load_step += 1;
        return 0;
    }
    if (bgm_reverb_slot < 4U) {
        SoundDriver_FreeReverb((char)bgm_reverb_slot);
    }
    cue = (SoundCue *)(bgm_cues + cue_index * 0x10);
    bgm_reverb_slot = SoundDriver_AddReverb(cue->flags, cue->depth_l, cue->depth_r, cue->delay, cue->feedback, cue->unk0C, cue->unk0E, 0);
    bgm_load_step += 1;
    return 0;
}

/* BGM load step: unless the bgm is 0x12, waits for the driver and sends the BGM's reverb settings
   (SoundDriver_ApplyReverb), flagging the change (reverb_change_pending). */
int Sound_BgmApplyReverb(void *actor, int mode) {
    if (mode != 0x12) {
        if (SoundDriver_IsBusy() != 0) {
            return -1;
        }
        if (SoundDriver_StepUpload() == 0) {
            return -1;
        }
        if (bgm_reverb_slot < 4U) {
            SoundDriver_ApplyReverb((char) bgm_reverb_slot);
            reverb_change_pending = TRUE;
        }
    }
    bgm_load_step += 1;
    return 0;
}

/* Reads the file job->name formats with index into the start of the sound heap, a step a call, then frees
   the voice job->unk10 (SoundDriver_FreeVoice); 1 when done. Nothing calls it. */
static int Sound_ReadHeapFileStep(LoadJob *job, int index) {
    int heap_start;
    int file;

    if (SoundDriver_StepUpload() != 0) {
        switch (heap_file_read_step) {
        case 0:
            heap_start = ((u32)&sound_heap_base >> 6) << 6;
            sound_heap_next = heap_start;
            *job->buffers = heap_start;
            sound_heap_next = heap_start + 0xA0000;
            sprintf(sound_file_path, job->name, index);
            file = File_Open(sound_file_path, 1, 1);
            heap_file_handle = file;
            File_Read(file, (void *)*job->buffers, -1);
            heap_file_read_step += 1;
            break;
        case 1:
            if (File_Sync(heap_file_handle, 1) == 0) {
                File_Close(heap_file_handle);
                heap_file_read_step = 0;
                SoundDriver_FreeVoice(job->voice_index);
                return 1;
            }
            break;
        }
    }
    return -1;
}

/* Reads the file request->name formats with the unk70 of map slot 0's header into a new buffer of the sound heap
   (its name in map_bank_file_path), a step a call; 1 when done. Nothing calls it. */
s32 Sound_ReadMapBankFileStep(LoadRequest *request) {
    u32 buffer;

    if (SoundDriver_StepUpload() != 0) {
        switch (map_bank_file_read_step) {
        case 0:
            buffer = sound_heap_next;
            request->target->buffer = (void *)buffer;
            sound_heap_next = (((buffer + 0x2003F) >> 6) << 6);
            sprintf(map_bank_file_path, request->name, map_slot0_header[0]->file_index);
            map_bank_file_handle = File_Open(map_bank_file_path, TRUE, TRUE);
            File_Read(map_bank_file_handle, request->target->buffer, -1);
            map_bank_file_read_step += 1;
            break;
        case 1:
            if (File_Sync(map_bank_file_handle, 1) == 0) {
                File_Close(map_bank_file_handle);
                map_bank_file_read_step = 0;
                return 1;
            }
            break;
        }
    }
    return -1;
}

/* BGM load step: loads the BGM's voice (its .hd and .bd, SoundDriver_LoadVoiceStep) and keeps it in the
   job; moves on once it is loaded. */
int Sound_BgmLoadVoice(IopLoadJob *job) {
    int *ctl = job->ctl;
    int result;

    result = SoundDriver_LoadVoiceStep(job->load_index, ctl[1], ctl[4], ctl[0], ctl[3], 0, job->spu_dest);
    job->result = result;
    if (result >= 0) {
        bgm_load_step += 1;
    }
    return 0;
}

/* BGM load step: once the upload and the transfer to sound memory are done, frees the IOP copy of the
   voice's samples. */
int Sound_BgmFreeSampleBuffer(IopLoadJob *job) {
    if (SoundDriver_StepUpload() == 0) {
        return -1;
    }
    if (SoundDriver_IsTransferDone(0) == 0) {
        return -1;
    }
    SoundDriver_FreeSampleBuffer(job->result);
    FlushCache(0);
    bgm_load_step += 1;
    return 0;
}

/* Stops a BGM load: breaks any read in flight, closes its file and puts the step counter back to 0. */
static void Sound_CancelBgmLoad(LoadJob *job) {
    if (job->file_handle >= 0) {
        sceCdBreak();
        File_Close(job->file_handle);
        File_Unlock();
    }
    job->file_handle = -1;
    bgm_load_step = 0;
}

/* Runs the current step (bgm_load_steps) of loading BGM bgm, cancelling the load in progress when bgm is a new
   one; 0 once the last step is done, else 1. */
static int Sound_StepBgmLoad(LoadJob *job, s32 state) {
    if (loading_bgm != state) {
        loading_bgm = state;
        Sound_CancelBgmLoad(job);
    }
    if (bgm_load_steps[bgm_load_step](job, state) > 0) {
        bgm_load_can_yield = 1;
        loading_bgm = 0xFF;
        bgm_load_step = 0;
        return 0;
    }
    return 1;
}

/* Loads BGM bgm a step a call while pending; returns 0 once it is the loaded one. */
int Sound_LoadBgm(int bgm, int pending) {
    int done;

    bgm_load_can_yield = 1;
    if (pending != 0) {
        if ((loaded_bgm == bgm) && (bgm_load_running == 0)) {
            pending = 0;
        } else {
            bgm_load_can_yield = 0;
            done = Sound_StepBgmLoad(&bgm_load_job, bgm);
            bgm_load_running = 1;
            if (done == 0) {
                loaded_bgm = bgm;
                pending = 0;
                bgm_load_running = 0;
            }
        }
    }
    return pending;
}

/* Whether the BGM load may be set aside for another sound load: bgm_load_can_yield, which Sound_LoadBgm clears
   while it runs a step and the steps set again where they wait (the file layer busy, a fade). */
int Sound_BgmLoadCanYield(void) {
    return bgm_load_can_yield;
}

/* Load step (Sound_RunFileLoad): reads the file job->name into the job's first buffer, waiting for it. */
int Sound_ReadJobFile(LoadJob *job) {
    int file_handle;

    if (SoundDriver_StepUpload() == 0) {
        return -1;
    }
    file_handle = File_Open(job->name, 0, 1);
    File_Read(file_handle, (void *)*job->buffers, -1);
    File_Close(file_handle);
    return 1;
}

/* Reads snd/rev_para.dat into the two cue tables bgm_cues and map_bank_cues. */
static int Sound_LoadReverbParams(void) {
    int file;

    file = File_Open(rev_para_path, 0, 1);
    File_Read(file, bgm_cues, 0x400);
    File_Read(file, map_bank_cues, 0x800);
    File_Close(file);
    return 1;
}

/* Load step (Sound_RunFileLoad): reads the file request->name into the request's buffer, waiting for
   it. */
s32 Sound_ReadRequestFile(LoadRequest *request) {
    s32 file_handle;

    if (SoundDriver_StepUpload() == 0) {
        return -1;
    }
    file_handle = File_Open(request->name, 0, 1);
    File_Read(file_handle, request->target->buffer, -1);
    File_Close(file_handle);
    return 1;
}

/* Load step (Sound_RunFileLoad): as Sound_BgmLoadVoice. */
int Sound_FileLoadVoice(IopLoadJob *job) {
    int *ctl = job->ctl;
    int result;

    result = SoundDriver_LoadVoiceStep(job->load_index, ctl[1], ctl[4], ctl[0], ctl[3], 0, job->spu_dest);
    job->result = result;
    if (result >= 0) {
        bgm_load_step += 1;
    }
    return 0;
}

/* Load step (Sound_RunFileLoad): as Sound_BgmFreeSampleBuffer. */
int Sound_FileLoadFreeSampleBuffer(IopLoadJob *job) {
    if (SoundDriver_StepUpload() == 0) {
        return -1;
    }
    if (SoundDriver_IsTransferDone(0) == 0) {
        return -1;
    }
    SoundDriver_FreeSampleBuffer(job->result);
    FlushCache(0);
    bgm_load_step += 1;
    return 0;
}

/* Runs the current step of a four-step file load (sound_file_load_steps: two reads, the voice load, freeing the
   sample copy) and moves on when it succeeds; 0 after the fourth, else 1. Nothing calls it, and it calls
   the steps without their arguments. */
static s32 Sound_RunFileLoad(void) {
    s32 result;

    result = sound_file_load_steps[sound_file_load_step]();
    if (result >= 0) {
        sound_file_load_step += 1;
    }
    if ((u32)sound_file_load_step < 4U) {
        return 1;
    }
    sound_file_load_step = 0;
    return 0;
}

/* Plays a sound of bank while distance is within range: volume falls off linearly, panned by angle, with a
   random bend. */
void Sound_PlayLinear(u8 sound, int bank, float range, float distance, float angle) {
    float origin[4];
    float offset[4];
    int volume;
    int pan_pos;
    int pan;

    if (!(range < distance) && sound != 0xFF) {
        Sound_CosSin(angle, origin, offset);
        volume = (range - distance) * 64.0f * (1.0f / range);
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        SoundDriver_NoteOn(Sound_NextVoice(2), bank, 0, sound, 0x3C, volume, pan, (char) pan_pos);
    }
}

/* As Sound_PlayLinear with the steep falloff (loudest near, 1.5 times as loud at the point). */
void Sound_PlaySteep(u8 sound, int bank, float range, float distance, float angle) {
    float origin[4];
    float offset[4];
    int volume;
    int pan_pos;
    int pan;
    int slot;

    if (!(range < distance) && sound != 0xFF) {
        Sound_CosSin(angle, origin, offset);
        volume = (1.0 - sqrt(range * range - (range - distance) * (range - distance)) / range) * 64.0;
        volume *= 1.5f;
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        slot = Sound_NextVoice(2);
        SoundDriver_NoteOn(slot, bank, 0, sound, 0x3C, volume, pan, (char) pan_pos);
    }
}

/* As Sound_PlayLinear with the round falloff (the root of 1 - (distance / range) squared). */
void Sound_PlayRound(u8 sound, int bank, float range, float distance, float angle) {
    float origin[4];
    float offset[4];
    int volume;
    int pan_pos;
    int pan;
    int slot;

    if (!(range < distance) && sound != 0xFF) {
        Sound_CosSin(angle, origin, offset);
        volume = sqrt(range * range - distance * distance) / range * 64.0;
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        slot = Sound_NextVoice(2);
        SoundDriver_NoteOn(slot, bank, 0, sound, 0x3C, volume, pan, (char) pan_pos);
    }
}

/* As Sound_PlayLinear without the bend. */
void Sound_PlayLinearSteady(u8 sound, int bank, float range, float distance, float angle) {
    float origin[4];
    float offset[4];
    int volume;
    int pan_pos;
    int slot;

    if (!(range < distance) && sound != 0xFF) {
        Sound_CosSin(angle, origin, offset);
        volume = (range - distance) * 64.0f * (1.0f / range);
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        slot = Sound_NextVoice(2);
        SoundDriver_NoteOn(slot, bank, 0, sound, 0x3C, volume, 0, (char) pan_pos);
    }
}

/* As Sound_PlaySteep without the bend. */
void Sound_PlaySteepSteady(u8 sound, int bank, float range, float distance, float angle) {
    float origin[4];
    float offset[4];
    int volume;
    int pan_pos;
    int slot;

    if (!(range < distance) && sound != 0xFF) {
        Sound_CosSin(angle, origin, offset);
        volume = (1.0 - sqrt(range * range - (range - distance) * (range - distance)) / range) * 64.0;
        volume = (float) volume * 1.5f;
        volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        if (volume >= 128) {
            volume = 127;
        }
        slot = Sound_NextVoice(2);
        SoundDriver_NoteOn(slot, bank, 0, sound, 0x3C, volume, 0, (char) pan_pos);
    }
}

/* As Sound_PlayRound without the bend. */
void Sound_PlayRoundSteady(u8 sound, int bank, float range, float distance, float angle) {
    float origin[4];
    float offset[4];
    int volume;
    int pan_pos;
    int slot;

    if (!(range < distance) && sound != 0xFF) {
        Sound_CosSin(angle, origin, offset);
        volume = sqrt(range * range - distance * distance) / range * 64.0;
        volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        if (volume >= 128) {
            volume = 127;
        }
        slot = Sound_NextVoice(2);
        SoundDriver_NoteOn(slot, bank, 0, sound, 0x3C, volume, 0, (char) pan_pos);
    }
}

/* Sound_PlayLinear from the sfx_com bank. */
void Sound_PlaySfxLinear(u8 sound, float range, float distance, float angle) {
    float origin[4];
    float offset[4];
    int volume;
    int pan_pos;
    int pan;

    if (!(range < distance) && sound != 0xFF) {
        Sound_CosSin(angle, origin, offset);
        volume = (range - distance) * 64.0f * (1.0f / range);
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        SoundDriver_NoteOn(Sound_NextVoice(2), sfx_com_job.result, 0, sound, 0x3C, volume, pan, (char) pan_pos);
    }
}

/* Sound_PlaySteep from the sfx_com bank. */
void Sound_PlaySfxSteep(u8 sound, float range, float distance, float angle) {
    float origin[4];
    float offset[4];
    int volume;
    int pan_pos;
    int pan;
    float reach;

    if (!(range < distance) && sound != 0xFF) {
        Sound_CosSin(angle, origin, offset);
        reach = range * range - (range - distance) * (range - distance);
        volume = (1.0 - sqrt(reach) / range) * 64.0;
        volume *= 1.5f;
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        SoundDriver_NoteOn(Sound_NextVoice(2), sfx_com_job.result, 0, sound, 0x3C, volume, pan, (char) pan_pos);
    }
}

/* Sound_PlayRound from the sfx_com bank. */
void Sound_PlaySfxRound(u8 sound, float range, float distance, float angle) {
    float origin[4];
    float offset[4];
    int volume;
    int pan_pos;
    int pan;
    int slot;

    if (!(range < distance) && sound != 0xFF) {
        Sound_CosSin(angle, origin, offset);
        volume = sqrt(range * range - distance * distance) / range * 64.0;
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        slot = Sound_NextVoice(2);
        SoundDriver_NoteOn(slot, sfx_com_job.result, 0, sound, 0x3C, volume, pan, (char) pan_pos);
    }
}

/* Sound_PlayLinear from the map bank. */
void Sound_PlayMapLinear(u8 sound, float range, float distance, float angle) {
    float origin[4];
    float offset[4];
    int volume;
    int pan_pos;
    int pan;
    int slot;

    if (!(range < distance) && sound != 0xFF) {
        Sound_CosSin(angle, origin, offset);
        volume = (range - distance) * 64.0f * (1.0f / range);
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        slot = Sound_NextVoice(2);
        SoundDriver_NoteOn(slot, map_bank_job.result, 0, sound, 0x3C, volume, pan, (char) pan_pos);
    }
}

/* Sound_PlaySteep from the map bank. */
void Sound_PlayMapSteep(u8 sound, float range, float distance, float angle) {
    float origin[4];
    float offset[4];
    float falloff;
    int volume;
    int pan_pos;
    int pan;
    int slot;

    if (!(range < distance) && sound != 0xFF) {
        Sound_CosSin(angle, origin, offset);
        falloff = range * range - (range - distance) * (range - distance);
        volume = (1.0 - sqrt(falloff) / range) * 64.0;
        volume = (float) volume * 1.5f;
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        slot = Sound_NextVoice(2);
        SoundDriver_NoteOn(slot, map_bank_job.result, 0, sound, 0x3C, volume, pan, (char) pan_pos);
    }
}

/* Sound_PlayRound from the map bank. */
void Sound_PlayMapRound(u8 sound, float range, float distance, float angle) {
    float origin[4];
    float offset[4];
    int volume;
    int pan_pos;
    int pan;
    int slot;

    if (!(range < distance) && sound != 0xFF) {
        Sound_CosSin(angle, origin, offset);
        volume = sqrt(range * range - distance * distance) / range * 64.0;
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        volume = (float) volume * (float) sfx_volume[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        slot = Sound_NextVoice(2);
        SoundDriver_NoteOn(slot, map_bank_job.result, 0, sound, 0x3C, volume, pan, (char) pan_pos);
    }
}

/* Entity bank load step: loads the voice from the job's spec (SoundDriver_LoadVoiceStep) and keeps it in
   the job; moves on once it is loaded. */
int Sound_EntityBankLoadVoice(SoundJob *job) {
    SoundJobSpec *spec = job->spec;
    int result = SoundDriver_LoadVoiceStep(job->voice_index, (int)spec->buffer_b, spec->size_b, (int)spec->buffer_a, spec->size_a, 0, job->spu_dest);

    job->result = result;
    if (result >= 0) {
        entity_bank_load_step += 1;
    }
    return 0;
}

/* Entity bank load step: once the upload and the transfer to sound memory are done, frees the IOP copy of
   the voice's samples; 1 (done). */
int Sound_EntityBankFreeSampleBuffer(SoundJob *job) {
    if (SoundDriver_StepUpload() == 0) {
        return -1;
    }
    if (SoundDriver_IsTransferDone(0) == 0) {
        return -1;
    }
    SoundDriver_FreeSampleBuffer(job->result);
    FlushCache(0);
    entity_bank_load_step += 1;
    return 1;
}

/* Runs the current step of the entity bank load (entity_bank_load_steps); 0 when it reports done, else 1. */
static int Sound_RunEntityBankLoad(void *sequence, int index) {
    if (entity_bank_load_steps[entity_bank_load_step]() > 0) {
        entity_bank_load_step = 0;
        return 0;
    }
    return 1;
}

/* Loads the sound bank of the entity type file just read (args: its header, the two buffers and their
   sizes) into voice index + 6 (index: the entity pool slot), at 0x138810 + index * 0x11800 in sound
   memory, a step a call; 0 and the voice in *voice_out once done, else 1. */
int Sound_LoadEntityBankStep(int args_addr, int index, int voice_out) {
    SoundStartArgs *args = (SoundStartArgs *)args_addr;
    int pending = 1;

    sound_load_job.voice_index = index + 6;
    sound_load_job.spu_dest = index * 0x11800 + 0x138810;
    sound_load_spec.buffer_a = (void *)args->buffer_a;
    sound_load_spec.buffer_b = (void *)args->buffer_b;
    sound_load_spec.size_a = args->size_a;
    sound_load_spec.size_b = args->size_b;
    sound_load_job.spec = (SoundJobSpec *)&sound_load_spec;
    sound_load_job.result = index + 6;
    if (Sound_RunEntityBankLoad(&sound_load_job, index) == 0) {
        *(u8 *)voice_out = sound_load_job.result;
        pending = 0;
    }
    return pending;
}

/* Plays sound from bank with the falloff player kind of falloff_sound_players while distance is within range, panned by
   angle (EntityEvent_StartCueAlways calls it for the sounds of an entity's animation). Range, distance and angle pass through in
   $f12..$f14; match: declared without them, which keeps the registers retail uses. */
void Sound_PlayFalloff(int sound, int kind, int bank) {
    falloff_sound_players[kind & 0xFF](sound & 0xFF, bank);
}

/* Map bank load step: does nothing but move on. */
s32 Sound_MapBankNoOp(void) {
    map_bank_load_step += 1;
    return 0;
}

/* Map bank load step: waits for a pending reverb change, then loads the map's voice from the job's spec
   (SoundDriver_LoadVoiceStep); moves on once it is loaded. */
int Sound_MapBankLoadVoice(SoundJob *job) {
    SoundJobSpec *spec;
    int result;

    if (reverb_change_pending != 0) {
        if (SoundDriver_IsBusy() != 0) {
            return -1;
        }
        reverb_change_pending = 0;
    }
    spec = job->spec;
    result = SoundDriver_LoadVoiceStep(job->voice_index, (int)spec->buffer_b, spec->size_b, (int)spec->buffer_a, spec->size_a, 0, job->spu_dest);
    job->result = result;
    if (result >= 0) {
        map_bank_load_step += 1;
    }
    return 0;
}

/* Map bank load step: once the upload and the transfer to sound memory are done, frees the IOP copy of
   the voice's samples. */
int Sound_MapBankFreeSampleBuffer(SoundJob *job) {
    if (SoundDriver_StepUpload() == 0) {
        return -1;
    }
    if (SoundDriver_IsTransferDone(0) == 0) {
        return -1;
    }
    SoundDriver_FreeSampleBuffer(job->result);
    FlushCache(0);
    map_bank_load_step += 1;
    return 0;
}

/* Map bank load step: frees the reverb slot of the last map and stores the map's reverb settings (the
   entry map_reverb_cue_indices names in map_bank_cues) for core 1. */
int Sound_MapBankAddReverb(void) {
    SoundScriptState state;
    SoundCue *cue;

    state = map_reverb_cue_indices;
    if (map_reverb_slot < 4U) {
        SoundDriver_FreeReverb((char)map_reverb_slot);
    }
    cue = (SoundCue *)(map_bank_cues + state.cue_index * 0x10);
    map_reverb_slot = SoundDriver_AddReverb(cue->flags, cue->depth_l, cue->depth_r, cue->delay, cue->feedback, cue->unk0C, cue->unk0E, 1);
    map_bank_load_step += 1;
    return 0;
}

/* Map bank load step: once the upload is done, sends the map's reverb settings (SoundDriver_ApplyReverb)
   and sets map_sounds_enabled (map sounds may start); 1 (done). */
int Sound_MapBankApplyReverb(void) {
    if (SoundDriver_StepUpload() == 0) {
        return 0;
    }
    if (map_reverb_slot < 4U) {
        SoundDriver_ApplyReverb((char) map_reverb_slot);
        reverb_change_pending = TRUE;
    }
    map_sounds_enabled = TRUE;
    map_bank_load_step += 1;
    return 1;
}

/* Map bank load step: the first time after Sound_Init sets map_reverb_set and moves on to the reverb steps;
   after that sets map_sounds_enabled (map sounds may start) and ends the load (1), so the reverb stays. */
int Sound_MapBankFinishUnlessFirst(void) {
    map_bank_load_step += 1;
    D_004468B4 = 0;
    if (map_reverb_set != 0) {
        map_sounds_enabled = TRUE;
        return 1;
    }
    map_reverb_set = TRUE;
    return 0;
}

/* Runs the current step of the map bank load (map_bank_load_steps); 0 when it reports done, else 1. */
static int Sound_RunMapBankLoad(void *sequence, int kind) {
    if (map_bank_load_steps[map_bank_load_step]() > 0) {
        map_bank_load_step = 0;
        return 0;
    }
    return 1;
}

/* Loads the sound bank of a map slot (MapSlot.sound_bank) a step a call while pending; returns 0 once it is done. */
int Sound_LoadMapBankStep(int slot_addr, int pending) {
    MapSlot *slot = (MapSlot *)slot_addr;
    SoundBankInfo *bank;

    if (pending == 0) {
        return 0;
    }
    map_bank_load_job.spu_dest = 0x106810;
    map_bank_load_job.voice_index = 3;
    bank = (SoundBankInfo *)slot->sound_bank;
    map_bank_load_job.spec = (SoundJobSpec *)&map_bank_spec;
    map_bank_spec.size_a = bank->size_a;
    map_bank_spec.buffer_a = bank->buffer_a;
    map_bank_spec.buffer_b = bank->buffer_b;
    map_bank_spec.size_b = bank->size_b;
    if (Sound_RunMapBankLoad(&map_bank_load_job, slot->tag) == 0) {
        pending = 0;
    }
    return pending;
}

/* Loads the sound bank of a map slot (MapSlot.sound_bank), waiting until it is done. */
int Sound_LoadMapBank(MapSlot *slot) {
    SoundBankInfo *bank;

    map_bank_job.voice_index = 3;
    map_bank_job.spu_dest = 0x106810;
    bank = (SoundBankInfo *)slot->sound_bank;
    map_bank_job.spec = (SoundJobSpec *)&map_bank_files;
    map_bank_files.size_a = bank->size_a;
    map_bank_files.buffer_a = bank->buffer_a;
    map_bank_files.buffer_b = bank->buffer_b;
    map_bank_files.size_b = bank->size_b;
    while (Sound_RunMapBankLoad(&map_bank_job, 1) > 0) {
    }
    return 0;
}

/* Loads the sw bank file_id (sw%03d with 0x96 added) into the sound heap and hands it to the driver, waiting
   until it is done. */
void Sound_LoadSwBank(int file_id) {
    char path_a[128];
    char path_b[128];
    int file_number;
    int buffer_base;
    int buffer_limit;

    file_number = file_id + 0x96;
    sprintf(path_a, sw_bank_job.name_a, file_number);
    sprintf(path_b, sw_bank_job.name_b, file_number);
    buffer_base = ((u32)sound_heap_base >> 6) << 6;
    sw_bank.base = buffer_base;
    buffer_limit = buffer_base + 0x20000;
    sw_bank.limit = buffer_limit;
    sw_bank.used = 0;
    sound_heap_next = buffer_limit + 0x8000;
    while (Sound_StepUpload() == 0) {
    }
    sw_bank_job.result = Sound_LoadFilePair(path_a, path_b, sw_bank_job.spec, sw_bank_job.voice_index, sw_bank_job.spu_dest);
    while (Sound_StepUpload() == 0) {
    }
    while (SoundDriver_IsTransferDone(0) == 0) {
    }
    SoundDriver_FreeSampleBuffer(sw_bank_job.result);
    FlushCache(0);
}

/* Loads the mg bank file_number (mg%03d) into the sound heap and hands it to the driver, waiting until it is
   done. */
void Sound_LoadMgBank(int file_number) {
    char path_a[128];
    char path_b[128];
    int heap_start;
    int buffer_end;

    sprintf(path_a, mg_bank_job.name_a, file_number);
    sprintf(path_b, mg_bank_job.name_b, file_number);
    heap_start = ((u32)sound_heap_base >> 6) << 6;
    mg_bank.base = heap_start;
    buffer_end = heap_start + 0x10000;
    mg_bank.limit = buffer_end;
    mg_bank.used = 0;
    sound_heap_next = buffer_end + 0x8000;
    while (Sound_StepUpload() == 0);
    mg_bank_job.result = Sound_LoadFilePair(path_a, path_b, mg_bank_job.spec, mg_bank_job.voice_index, mg_bank_job.spu_dest);
    while (Sound_StepUpload() == 0);
    while (SoundDriver_IsTransferDone(0) == 0);
    SoundDriver_FreeSampleBuffer(mg_bank_job.result);
    FlushCache(0);
}

/* Reads two files into the spec's buffers and hands them to the driver (SoundDriver_LoadVoiceAt); 0 if either file
   will not open. */
static int Sound_LoadFilePair(char *name_a, char *name_b, SoundJobSpec *spec, int first_arg, int second_arg) {
    int handle;

    handle = File_Open(name_a, 0, 1);
    if (handle < 0) {
        File_Close(handle);
        return 0;
    }
    File_Read(handle, spec->buffer_a, spec->size_a);
    File_Close(handle);
    handle = File_Open(name_b, 0, 1);
    if (handle < 0) {
        File_Close(handle);
        return 0;
    }
    File_Read(handle, spec->buffer_b, spec->size_b);
    File_Close(handle);
    return SoundDriver_LoadVoiceAt(first_arg, (int)spec->buffer_b, spec->size_b, (int)spec->buffer_a, spec->size_a, 0, second_arg);
}

/* Keeps the driver's upload going (SoundDriver_StepUpload); 1 once it is done. */
static int Sound_StepUpload(void) {
    return SoundDriver_StepUpload();
}

/* Loads the two files named in a job and hands them to the driver, waiting until it is done. */
void Sound_LoadJob(SoundJob *slot) {
    while (Sound_StepUpload() == 0);
    slot->result = Sound_LoadFilePair(slot->name_a, slot->name_b, slot->spec, slot->voice_index, slot->spu_dest);
    while (Sound_StepUpload() == 0);
    while (SoundDriver_IsTransferDone(0) == 0);
    SoundDriver_FreeSampleBuffer(slot->result);
    FlushCache(0);
}

/* Sets the reverb return volume of core 1 (SPU2 parameters EVOLL and EVOLR, 0xB81 and 0xC81 through
   command 0xD0) from the player's map record (byte 0x4D) when it differs from the one sent last
   (map_reverb_volume), and only while no sound load is waiting. */
static int Sound_UpdateMapReverbVolume(void) {
    MapSlot *slot;
    int cached;
    int wanted;

    if (sound_load_queue[3] == 0 && sound_load_queue[6] == 0 && sound_load_queue[1] == 0) {
        slot = &map_slots[current_map_slot];
        cached = map_reverb_volume;
        if ((u8)slot->records[(player_state.map_index << 7) + 0x4D] != cached) {
            if (cached >= 0x40) {
                map_reverb_volume = 0x3F;
            } else if (cached < 0) {
                map_reverb_volume = 0;
            }
            wanted = (u8)slot->records[(player_state.map_index << 7) + 0x4D];
            map_reverb_volume = wanted;
            SoundDriver_Remote(1, 0xD0, 0xB81, (s8)wanted << 8);
            return SoundDriver_Remote(1, 0xD0, 0xC81, map_reverb_volume << 8);
        }
    }
}

/* Lets Sound_UpdateTagged give positioned tagged sounds their volume again. */
void Sound_UnmutePositional(void) {
    positional_sounds_muted = 0;
}

/* Makes Sound_UpdateTagged keep positioned tagged sounds at volume 0. */
void Sound_MutePositional(void) {
    positional_sounds_muted = 1;
}

/* Frees the IOP copy of a voice's samples (SoundDriver_FreeSampleBuffer); returns 0. Nothing calls it. */
int Sound_FreeSampleBuffer(int voice_index) {
    SoundDriver_FreeSampleBuffer(voice_index);
    return 0;
}
