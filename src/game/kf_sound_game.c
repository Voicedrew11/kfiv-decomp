/* The game's sound layer over the driver (kf_sound.c): voices, volumes, sounds played at a
   point and panned by where it lies from the player, tagged (looping) sounds, BGM and bank loading. */
#include "kf_load.h"
#include "common.h"
#include "kf_sound.h"
#include "kf_player.h"
#include "kf_model.h"
#include "kf_file.h"
#include "kf_map.h"
#include "kf_effect.h"
#include "kf_sound_game.h"

int func_001CBAC0(void);
int func_00102230(void);
int func_001CBD20(void);
int func_00102790(int, int);
int func_001CD6B8(int);

extern LoadJob D_003EF5B8;
extern SoundJob D_003EF5E8;
extern SoundJob D_003EF618;
extern s32 D_00502170[][2];
extern SoundFileBuffer D_003EF500;
extern SoundFileBuffer D_003EF520;
extern SoundChannel D_003EF710[];
extern SoundChannel D_00500170[];
extern char D_00500158[];
extern char D_002496BF[];
extern int D_004468BC;
extern float D_004468C0;
extern int D_004468C4;
extern s8 D_004470E0;
extern char D_004470E1;
extern char D_004470E2;
extern s8 D_004470E3;
extern u8 D_004470E4;
extern u8 D_004470E5;
extern s8 D_004470E6;
extern u8 D_004470E7;
void *memset(void *, int, u32);
int func_001CB778(void);
void func_001CBE60(void);
int func_001CE690(u32 slot, int voice_index, char channel, char program, char note, char volume, s16 bend, char pan);
int func_001CE8B0(u32 key_index);
extern u8 D_00349680[];
void func_001CBED8(void);
void func_001CC018(void);
int func_001CEBE0(void);
extern f32 D_003EF6E0[4];
extern u8 D_003EF6F0[][2];
extern u8 D_00446880[8];
int func_001CBAE0(int, ...);
int func_001CE468(u32 bank_index, int value);
int func_001CE620(int, s16);
int func_001CDBC0(u32, s16);
extern SoundJob D_003EF648;
extern SoundJob D_003EF678;
extern SoundJob D_003EF6A8;
extern u8 D_00349681[];
extern int D_00446860;
extern void *D_003EF700[];
extern PlayerState D_00413F80;
extern u8 D_00349682[];
double sqrt(double);
int func_001CE9A0(int kind, u32 slot, int value);
int rand(void);
float atan2f(float, float);
extern int D_00446888;
extern int D_0044688C;
extern int D_00446890;
extern char D_00446894;
extern int D_00446858;
extern int D_0044685C;
extern int D_00446F74;
extern int D_004470E8;
extern int D_004470F4;
void func_00102218(void);
int func_00102900(int);
int sprintf(char *, char *, ...);
int func_001CD020(int word0, int word1, int word2, int word3, int word4, int word5, int word6);
int func_001CD6E0(u32);
int func_001CDF90(int bank_id, int voice_index, int data, u32 size);
int func_001CE408(int);
void func_00102228(void);
extern int func_001CE510(u32);
extern int D_003EF544[];
extern int D_004470EC;
int func_00102238(char *name, int nowait, int pack);
int func_001025C0(int file, void *buf, int size);
extern int D_003EF548[];
extern int D_004470F0;
extern u8 D_00446896;
extern char D_004FF558[];
int func_001CEF48(u32);
int func_001CEE78(int, s16, s16, s16, s16, s16, s16, int);
void func_001CEFA0(u32);
extern int D_004468A4;
extern int D_004470D4;
extern char D_004FF3C0[];
int func_001024B0(int file);
extern s32 D_004468A8;
extern s32 D_004470DC;
extern char D_004FF440[];
extern MapHeader *D_0038CD28[];
int func_001CD7E0(int);
int FlushCache(int);
int sceCdBreak(void);
extern int (*const D_0043ED50[])(LoadJob *, s32);
extern char D_0043EE18[];
extern char D_004FF958[];
extern s32 D_004468AC;
extern s32 (*const D_0043EDA8[])(void);
extern int D_00446864;
extern int (*D_0043ED88[])(void);
extern SoundJob D_004FF4D8;
extern SoundStartSpec D_004FF4C0;
/* the falloff players Sound_PlayFalloff picks from (the Sfx and Map ones take no bank) */
extern int (*D_0043EE30[])(int, int); // match: an int result, though the players are void
extern int D_00446868;
extern SoundScriptState D_0043EE78;
extern u8 D_00446895;
extern int D_004468B4;
extern int (*D_0043ED90[])(void);
extern SoundJob D_004FF528;
extern SoundBankSpec D_004FF510;
extern SoundBankSpec D_003EF5A0;
extern SoundFileBuffer D_003EF560;
extern SoundFileBuffer D_003EF580;
int func_001CD0E0(int, int, int, int, int, int, int);
extern int D_0038A130[0x28 / 4];
extern MapSlot D_0038CD20[];
extern int D_00446770;

static void func_0019C4A0(void);
static void Sound_CosSin(f32 angle, f32 *cos_out, f32 *sin_out);
static float Sound_Delta(f32 *out, f32 *point_a, f32 *point_b);
static int Sound_NextVoice(int slot);
static s8 Sound_ClaimTaggedVoice(void);
static void func_0019C748(void);
static void func_0019C7C0(int value);
static void Sound_SetBgmFade(s32 level);
static int Sound_TaggedNearestPoint(s32 index, Vector4 *nearest);
static f32 func_0019E478(Vector4 *start, Vector4 *end, Vector4 *nearest, f32 start_dx, f32 start_dz, f32 end_dx, f32 end_dz, f32 back_dx, f32 back_dz, f32 seg_dx, f32 seg_dz);
static void Sound_StartQueued(void);
static int func_0019EB70(char sound);
static int func_0019F298(LoadJob *job, int index);
static void Sound_CancelBgmLoad(LoadJob *job);
static int Sound_StepBgmLoad(LoadJob *job, s32 state);
static int Sound_LoadReverbParams(void);
static s32 func_0019F8B0(void);
static int func_001A0BD0(void *sequence, int index);
static int func_001A1000(void *sequence, int kind);
static int Sound_LoadFilePair(char *name_a, char *name_b, SoundJobSpec *spec, int first_arg, int second_arg);
static int func_001A1438(void);
static int func_001A14D8(void);

/* Starts the sound driver and resets the sound state: loads the se_com and sfx_com banks into the heap,
   reads rev_para.dat, and frees every channel, queued request and claimed voice. */
void Sound_Init(void) {
    int i;
    u32 heap_start;

    D_004468BC = 0x40;
    D_004468C0 = 0.013f;
    D_004468C4 = 1;
    D_004470E2 = 0;
    D_004470E3 = 0;
    D_004470E1 = 0;
    D_004470E4 = 0xFF;
    D_004470E5 = 0;
    D_004470E6 = 0;
    D_004470E0 = 0;
    D_004470E7 = 0xFF;
    memset(D_00502170, 0, 0xD0);
    func_001CB778();
    func_001CBAE0(1, 0x100, 0xA, 0x800);
    func_001CBE60();
    D_003EF500.used = 0;
    D_003EF500.base = heap_start = ((u32)D_002496BF >> 6) << 6;
    D_003EF500.limit = heap_start + 0xA0000;
    Sound_LoadJob(&D_003EF5E8);
    D_003EF520.base = D_003EF500.base;
    D_003EF520.limit = D_003EF500.limit;
    Sound_LoadJob(&D_003EF618);
    for (i = 0; i < 0x80; i++) {
        D_003EF710[i].unk3A = -1;
    }
    Sound_LoadReverbParams();
    for (i = 0; i < 0x80; i++) {
        D_00500170[i].unk38 = -1;
    }
    for (i = 0; i < 0x10; i++) {
        D_00500158[i] = -1;
    }
    D_003EF5B8.unk10 = 0;
    D_003EF5B8.unk14 = 0;
}

/* Starts and at once stops sound 0 of the sfx_com bank on each tracked voice (0x64..0x7D). Nothing calls it. */
static void func_0019C4A0(void) {
    s32 i;

    for (i = 0x64; i < 0x7E; i++) {
        func_001CE690(i, D_003EF618.result, 0, 0, 0x3C, D_00349680[2], 0, 0x3F);
        func_001CE8B0(i);
    }
}

/* Applies option 0 (D_00349680[0]) to the driver: func_001CC018 when it is set, func_001CBED8 when not. */
void func_0019C520(void) {
    if (D_00349680[0] != 0) {
        func_001CC018();
        return;
    }
    func_001CBED8();
}

/* Once a frame: starts the queued tagged sounds; then, unless a cue change is pending (D_004470E2), updates
   the tagged channels, the map's effect level and the driver; otherwise waits for the driver to finish it. */
void Sound_Update(void) {
    Sound_StartQueued();
    if (D_004470E2 == 0) {
        Sound_UpdateTagged();
        func_001A14D8();
        func_001CEBE0();
        return;
    }
    if (func_001CBAC0() == 0) {
        D_004470E2 = 0;
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
        "la $8, D_003EF6E0\n"
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

/* Steps the cursor of voice group `group` round its range (D_003EF6F0) and returns the next voice. */
static int Sound_NextVoice(int slot) {
    s8 index = slot;
    u8 low = D_003EF6F0[index][0];
    s8 high = D_003EF6F0[index][1];
    s8 offset = D_00446880[index] - low + 1;
    int next = offset % (high - (s8)low + 1) + low;

    D_00446880[index] = next;
    return (s8)next;
}

/* Claims the first free of the 16 voices 0x54..0x63 for a tagged sound and returns it. */
static s8 Sound_ClaimTaggedVoice(void) {
    s8 *flag;
    s32 i;

    flag = D_00500158;
    for (i = 0; i < 16; i++, flag++) {
        if (*flag < 0) {
            *flag = 1;
            return i + 0x54;
        }
    }
}

/* Sends five fixed settings to the driver (func_001CBAE0 commands 0x100 and 0xD0). Nothing calls it. */
static void func_0019C748(void) {
    func_001CBAE0(1, 0x100, 0xA, 0x800);
    func_001CBAE0(1, 0xD0, 0x800, 0xFC0);
    func_001CBAE0(1, 0xD0, 0x801, 0xFCC);
    func_001CBAE0(1, 0xD0, 0xD81, 0x7FFF);
    func_001CBAE0(1, 0xD0, 0xE81, 0x7FFF);
}

/* func_001CE468 on the BGM bank (D_003EF5B8.unk14) with value. Nothing calls it. */
static void func_0019C7C0(int value) {
    func_001CE468(D_003EF5B8.unk14, value);
}

/* Sets the BGM bank's volume (func_001CE620) to level, at most 0x7F: the fades of the BGM load steps. */
static void Sound_SetBgmFade(s32 level) {
    func_001CE620(D_003EF5B8.unk14, (s16)(level < 0x80 ? level : 0x7F));
}

/* Sets the BGM voice's volume (func_001CDBC0) to level, kept within 0..0x7F: the music volume option. */
void Sound_SetBgmVolume(s32 level) {
    if (level < 0) {
        level = 0;
    }
    if (level > 0x7F) {
        level = 0x7F;
    }
    func_001CDBC0(D_003EF5B8.unk10, level);
}

/* Sets one volume (func_001CDBC0) on the voices of the five sound banks and on voices 6 to 9. */
void Sound_SetBankVolumes(int level) {
    s16 short_level = level;

    func_001CDBC0(D_003EF5E8.result, short_level);
    func_001CDBC0(D_003EF6A8.result, short_level);
    func_001CDBC0(D_003EF678.result, short_level);
    func_001CDBC0(D_003EF648.result, short_level);
    func_001CDBC0(D_003EF618.result, short_level);
    func_001CDBC0(6, short_level);
    func_001CDBC0(7, short_level);
    func_001CDBC0(8, short_level);
    func_001CDBC0(9, short_level);
}

/* Sets the BGM fade level (D_00446860) to the music volume option (D_00349681). */
void Sound_SyncBgmLevel(void) {
    D_00446860 = D_00349681[0];
}

/* Starts a tracked sound with the player of `kind` in D_003EF700 (0 Sound_PlayTracked, 1
   Sound_PlayTrackedAt). */
char Sound_PlayTrackedKind(int index, int first, int second, int third, int fourth) {
    int (*handler)(int, int, int, char);

    handler = D_003EF700[index];
    return handler(first, second, third, (char)fourth);
}

/* Starts a sound on the next tracked voice (0x64..0x7D) whose D_00502170 slot is free, from the mg bank
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
        bank = &D_003EF6A8;
    } else if (sound < 0x7F) {
        bank = &D_003EF618;
    } else {
        return -1;
    }
    tries = 0;
    Sound_Delta(diff, position, &D_00413F80.position.x);
    Sound_CosSin(D_00413F80.rotation.y, origin, offset);
    direction[0] = origin[0];
    direction[1] = 0.0f;
    direction[2] = -offset[0];
    for (;;) {
        slot = Sound_NextVoice(5);
        tries++;
        if (D_00502170[slot - 100][0] == 0) {
            break;
        }
        if (tries >= 0xB) {
            return -1;
        }
    }
    func_001CE690(slot, bank->result, 0, sound, 0x3C, D_00349680[2], 0, 0x3F);
    D_00502170[slot - 100][0] = 1;
    D_00502170[slot - 100][1] = (u8)D_00349680[2];
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
    distance = Sound_Delta(diff, position, &D_00413F80.position.x);
    Sound_CosSin(D_00413F80.rotation.y, origin, offset);
    direction[0] = origin[0];
    direction[1] = 0.0f;
    direction[2] = -offset[0];
    falloff = 1.0f - distance * distance * 0.0004f * 0.0004f;
    volume = sqrt(falloff) * 64.0;
    if (volume < 0) {
        volume = 0;
    }
    volume = (float) volume * (float) D_00349682[0] * 0.015625f;
    if (volume >= 0x80) {
        volume = 0x7F;
    }
    pan = ((diff[0] * direction[0] + diff[1] * direction[1] + diff[2] * direction[2]) / distance + 1.0f) * 63.0f;
    for (;;) {
        slot = Sound_NextVoice(5);
        tries++;
        if (D_00502170[slot - 100][0] == 0) {
            break;
        }
        if (tries >= 0xB) {
            return -1;
        }
    }
    func_001CE690(slot, D_003EF618.result, 0, sound, 0x3C, volume, 0, (char) pan);
    D_00502170[slot - 100][0] = TRUE;
    D_00502170[slot - 100][1] = volume;
    return slot;
}

/* Turns the volume of every playing tracked voice to 0. */
void Sound_MuteTracked(void) {
    int i;

    for (i = 0; i < 0x1A; i++) {
        if (D_00502170[i][0] > 0) {
            func_001CE9A0(0, i + 100, 0);
        }
    }
}

/* Gives every playing tracked voice its volume back. */
void Sound_UnmuteTracked(void) {
    s32 i;
    for (i = 0; i < 0x1A; i++) {
        if (D_00502170[i][0] > 0)
            func_001CE9A0(0, i + 100, D_00502170[i][1]);
    }
}

/* Stops a tracked voice and frees its slot. */
void Sound_StopTracked(s8 slot) {
    if (slot > 0) {
        D_00502170[slot - 100][0] = 0;
        func_001CE8B0(slot);
    }
}

/* Stops a voice (nothing for 0 or less). */
void Sound_StopVoice(s8 slot) {
    if (slot > 0) {
        func_001CE8B0(slot);
    }
}

/* Plays a sound of the se_com bank on voice 1 unless it is 0xFF (none): the menu sounds. */
s32 Sound_PlayMenu(s32 sound) {
    if (sound != 0xFF) {
        func_001CE690(1, D_003EF5E8.result, 0, (char)sound, 0x3C, D_00349682[0], 0, 0x3F);
    }
    /* match: no return; an s32 function, so $2 stays live and the beq delay slot takes the second lui */
}

/* Plays a sound of the sw bank on voice 2. */
void Sound_PlaySw(int mode) {
    func_001CE690(2, D_003EF678.result, 0, (char)mode, 0x3C, D_00349682[0], 0, 0x3F);
}

/* Plays a map bank sound at a point (while D_004470E1 is set): the volume falls off with the distance from
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

    if (D_004470E1 == 0) {
        return -1;
    }
    distance = Sound_Delta(diff, position, &D_00413F80.position.x);
    Sound_CosSin(D_00413F80.rotation.y, origin, offset);
    direction[0] = origin[0];
    direction[1] = 0.0f;
    direction[2] = -offset[0];
    falloff = 1.0f - distance * distance * 0.0004f * 0.0004f;
    level = (int)(sqrt(falloff) * 64.0);
    if (level < 0.0f) {
        level = 0.0f;
    }
    volume = level * (float) D_00349682[0] * 0.015625f;
    if (volume >= 0x80) {
        volume = 0x7F;
    }
    pan = ((diff[0] * direction[0] + diff[1] * direction[1] + diff[2] * direction[2]) / distance + 1.0f) * 63.0f;
    detune = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
    func_001CE690(Sound_NextVoice(3), D_003EF648.result, 0, sound, 0x3C, volume, detune, (char) pan);
    return 1;
}

/* Plays a footstep sound (0x20..0x27) from the se_com bank (below 0x22, 0x24, 0x25) or the map bank, with a
   random bend; returns the voice, -1 for other sounds. */
int Sound_PlayFootstep(int sound) {
    SoundJob *bank;
    int slot;
    int bend;

    if (sound < 0x22 || sound == 0x24 || sound == 0x25) {
        bank = &D_003EF5E8;
    } else if (sound < 0x28) {
        bank = &D_003EF648;
    } else {
        return -1;
    }
    slot = Sound_NextVoice(1);
    bend = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
    func_001CE690(slot, bank->result, 0, sound, 0x3C, D_00349682[0], bend, 0x3F);
    return slot;
}

/* Plays a sound of the se_com bank on the next voice of group 1; returns the voice. */
s32 Sound_PlayCommon(s32 value) {
    s32 slot;

    slot = Sound_NextVoice(1);
    func_001CE690(slot, D_003EF5E8.result, 0, (char)value, 0x3C, D_00349682[0], 0, 0x3F);
    return slot;
}

/* Plays a sound of the map bank on the next voice of group 1; returns the voice. */
int Sound_PlayMap(int value) {
    int result;

    result = Sound_NextVoice(1);
    func_001CE690(result, D_003EF648.result, 0, (char)value, 0x3C, D_00349682[0], 0, 0x3F);
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

    distance = Sound_Delta(diff, position, &D_00413F80.position.x);
    if (!(range < distance)) {
        angle = atan2f(position[0] - D_00413F80.position.x, position[2] - D_00413F80.position.z) - D_00413F80.rotation.y;
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
            volume = (float) volume * (float) D_00349682[0] * 0.015625f;
            if (volume >= 128) {
                volume = 127;
            }
            slot = Sound_NextVoice(3);
            func_001CE690(slot, D_003EF5E8.result, 0, sound, 0x3C, volume, 0, (char) pan_pos);
            return slot;
        }
    }
}

/* Plays a map bank sound at volume (scaled by the sound option) with a random bend; -1 while D_004470E1 is
   clear. */
int Sound_PlayMapVolume(u8 sound, int volume) {
    int bend;
    int slot;

    if (D_004470E1 == 0) {
        return -1;
    }
    if (volume < 0) {
        volume = 0;
    }
    volume = (int) ((float) volume * (float) D_00349682[0] * 0.015625f);
    if (volume >= 128) {
        volume = 127;
    }
    bend = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
    slot = Sound_NextVoice(3);
    if (volume != 0) {
        func_001CE690(slot, D_003EF648.result, 0, sound, 0x3C, volume, bend, 0x3F);
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

    if (D_004470E1 == 0) {
        return -1;
    }
    distance = Sound_Delta(diff, position, &D_00413F80.position.x);
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
    volume = level * (float) D_00349682[0] * 0.015625f;
    if (volume >= 0x80) {
        volume = 0x7F;
    }
    angle = atan2f(position[0] - D_00413F80.position.x, position[2] - D_00413F80.position.z) - D_00413F80.rotation.y;
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
        func_001CE690(slot, D_003EF648.result, 0, sound, 0x3C, volume, detune, (char) pan);
    }
    return 1;
}

/* Starts a tagged sound (channel kind 0x63, no position) of the map bank at volume; queued in D_00500170
   while D_004470E1 is clear. */
int Sound_StartTagged(s8 sound, u8 voice, int volume) {
    SoundChannel *channel;
    SoundChannel *request;
    int i;
    s8 voice_id;

    volume = (int) ((float) volume * (float) D_00349682[0] * 0.015625f);
    if (volume >= 0x80) {
        volume = 0x7F;
    }
    if (D_004470E1 == 0) {
        for (i = 0; i < 0x80; i++) {
            if (D_00500170[i].unk38 == -1) {
                request = &D_00500170[i];
                request->unk38 = 1;
                request->unk3A = sound;
                request->unk3B = voice;
                request->unk39 = 0x63;
                request->unk30 = volume;
                return 0;
            }
        }
    }
    for (i = 0; i < 0x80; i++) {
        if (D_003EF710[i].unk3A < 0) {
            break;
        }
    }
    channel = &D_003EF710[i];
    channel->unk3A = sound;
    channel->unk3C = 0x63;
    channel->unk30 = volume;
    voice_id = channel->unk3B = Sound_ClaimTaggedVoice();
    func_001CE690(voice_id, D_003EF648.result, 0, voice, 0x3C, volume, 0, 0x3F);
}

/* Starts a tagged sound at a point, heard within range (SoundChannel.volume); queued in D_00500170 while
   D_004470E1 is clear. Sound_UpdateTagged gives it a voice. */
s32 Sound_StartTaggedAt(s8 sound_id, u8 voice, u8 mode, f32 volume, Vector4Arg *position) {
    SoundChannel *request;
    SoundChannel *channel;
    int i;

    if (D_004470E1 == 0) {
        for (i = 0; i < 0x80; i++) {
            if (D_00500170[i].unk38 == -1) {
                request = &D_00500170[i];
                request->unk3A = sound_id;
                request->unk3B = voice;
                request->volume = volume;
                request->unk3C = mode;
                request->unk39 = request->unk38 = 1;
                request->origin.x = position->x;
                request->origin.y = position->y;
                request->origin.z = position->z;
                return 0;
            }
        }
    }
    for (i = 0; i < 0x80; i++) {
        if (D_003EF710[i].unk3A < 0) {
            break;
        }
    }
    channel = &D_003EF710[i];
    channel->unk3A = sound_id;
    channel->unk38 = voice;
    channel->unk3B = -1;
    channel->origin.x = position->x;
    channel->origin.y = position->y;
    channel->origin.z = position->z;
    channel->unk39 = mode;
    channel->unk3C = 1;
    channel->volume = volume;
    /* match: no return; "return channel" adds a $2 copy retail lacks (implicit-int style fall-off) */
}

/* As Sound_StartTaggedAt for a sound along the segment origin..target. */
s32 Sound_StartTaggedAlong(s8 sound_id, u8 voice, u8 mode, Vector4Arg *origin, Vector4Arg *target, f32 volume) {
    SoundChannel *request;
    SoundChannel *channel;
    int i;

    if (D_004470E1 == 0) {
        for (i = 0; i < 0x80; i++) {
            if (D_00500170[i].unk38 == -1) {
                request = &D_00500170[i];
                request->unk38 = 1;
                request->unk3A = sound_id;
                request->unk3B = voice;
                request->volume = volume;
                request->unk3C = mode;
                request->unk39 = 2;
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
        if (D_003EF710[i].unk3A < 0) {
            break;
        }
    }
    channel = &D_003EF710[i];
    channel->unk3A = sound_id;
    channel->unk38 = voice;
    channel->unk3B = -1;
    channel->origin.x = origin->x;
    channel->origin.y = origin->y;
    channel->origin.z = origin->z;
    channel->target.x = target->x;
    channel->target.y = target->y;
    channel->target.z = target->z;
    channel->unk39 = mode;
    channel->unk3C = 2;
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

    for (i = 0; i < 0x80; i++, D_00446888 = 0) {
        if (D_003EF710[i].unk3A >= 0 && D_003EF710[i].unk3C != 0x63) {
            Sound_TaggedNearestPoint(i, &nearest);
            distance = Sound_Delta(diff, &nearest.x, &D_00413F80.position.x);
            if (D_003EF710[i].volume < distance) {
                if (D_003EF710[i].unk3B > 0) {
                    func_001CE8B0(D_003EF710[i].unk3B);
                    D_00500158[D_003EF710[i].unk3B - 0x54] = D_003EF710[i].unk3B = -1;
                }
            } else {
                if (D_004470E6 == 0) {
                    switch ((u8) D_003EF710[i].unk39) {
                    case 0:
                    case 6:
                        volume = (D_003EF710[i].volume - distance) * 64.0f * (1.0f / D_003EF710[i].volume);
                        break;
                    case 1:
                    case 7:
                        level = D_003EF710[i].volume - distance;
                        level = D_003EF710[i].volume * D_003EF710[i].volume - level * level;
                        volume = (int)((1.0 - sqrt(level) / D_003EF710[i].volume) * 64.0) * 1.5f;
                        break;
                    case 2:
                    case 8:
                        level = D_003EF710[i].volume * D_003EF710[i].volume - distance * distance;
                        volume = sqrt(level) / D_003EF710[i].volume * 64.0;
                        break;
                    }
                    if (volume <= 0) {
                        if (D_003EF710[i].unk3B > 0) {
                            func_001CE8B0(D_003EF710[i].unk3B);
                            D_00500158[D_003EF710[i].unk3B - 0x54] = D_003EF710[i].unk3B = -1;
                        }
                        continue;
                    } else {
                        volume = (float) volume * (float) D_00349680[2] * 0.015625f;
                        if (volume >= 0x80) {
                            volume = 0x7F;
                        }
                    }
                } else {
                    volume = 0;
                }
                angle = atan2f(nearest.x - D_00413F80.position.x, nearest.z - D_00413F80.position.z) -
                        D_00413F80.rotation.y;
                angle -= (int)(angle / 6.2831855f) * 6.2831855f;
                if (3.1415927f < angle) {
                    angle -= 6.2831855f;
                } else if (angle < -3.1415927f) {
                    angle += 6.2831855f;
                }
                Sound_CosSin(angle, origin, offset);
                pan = (offset[0] * 0.6f + 1.0f) * 63.0f;
                if ((u8) D_003EF710[i].unk39 < 6 || (u8) D_003EF710[i].unk39 > 8) {
                    if (D_00446894 != 0) {
                        D_00446888 -= rand() % 60;
                    } else {
                        D_00446888 += rand() % 60;
                    }
                    if (D_0044688C < D_00446888) {
                        D_00446890 = -(rand() % 0x555 + 0x555);
                        D_00446894 = 1;
                    } else if (D_00446888 < D_00446890) {
                        D_0044688C = rand() % 0x555 + 0x555;
                        D_00446894 = 0;
                    }
                    func_001CE9A0(2, D_003EF710[i].unk3B, D_00446888);
                }
                if (D_003EF710[i].unk3B < 0) {
                    voice = D_003EF710[i].unk3B = Sound_ClaimTaggedVoice();
                    func_001CE690(voice, D_003EF648.result, 0, D_003EF710[i].unk38, 0x3C, volume, D_00446888, (char) pan);
                    continue;
                }
                func_001CE9A0(0, D_003EF710[i].unk3B, volume);
                func_001CE9A0(1, D_003EF710[i].unk3B, pan);
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

    switch (D_003EF710[index].unk3C) {
    case 1:
        nearest->x = D_003EF710[index].origin.x;
        nearest->y = D_003EF710[index].origin.y;
        nearest->z = D_003EF710[index].origin.z;
        break;
    case 2:
        start = D_003EF710[index].origin;
        // match: retail adds the 0x10 to the table base before the index
        end = (&D_003EF710[0].target)[index * 4];
        to_start_x = D_00413F80.position.x - start.x;
        seg_x = end.x - start.x;
        to_start_y = D_00413F80.position.y - start.y;
        seg_y = end.y - start.y;
        back_x = start.x - end.x;
        back_y = start.y - end.y;
        back_z = start.z - end.z;
        to_end_x = D_00413F80.position.x - end.x;
        to_start_z = D_00413F80.position.z - start.z;
        seg_z = end.z - start.z;
        to_end_y = D_00413F80.position.y - end.y;
        to_end_z = D_00413F80.position.z - end.z;
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
static f32 func_0019E478(Vector4 *start, Vector4 *end, Vector4 *nearest, f32 start_dx, f32 start_dz, f32 end_dx, f32 end_dz, f32 back_dx, f32 back_dz, f32 seg_dx, f32 seg_dz) {
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
    return sqrt((D_00413F80.position.x - nearest->x) * (D_00413F80.position.x - nearest->x) + (D_00413F80.position.z - nearest->z) * (D_00413F80.position.z - nearest->z));
}

/* Starts the sounds queued in D_00500170 once D_004470E1 is set, freeing each request. */
static void Sound_StartQueued(void) {
    int i;
    SoundChannel *request;

    if (D_004470E1 != 0) {
        for (i = 0; i < 0x80; i++) {
            request = &D_00500170[i];
            if (request->unk38 > 0) {
                switch (request->unk39) {
                case 1:
                    request->unk38 = -1;
                    Sound_StartTaggedAt(request->unk3A, request->unk3B, request->unk3C, request->volume, &request->origin);
                    break;
                case 2:
                    request->unk38 = -1;
                    // match: target is indexed off the array; through request it is not strength-reduced
                    Sound_StartTaggedAlong(request->unk3A, request->unk3B, request->unk3C, &request->origin, &D_00500170[i].target, request->volume);
                    break;
                case 0x63:
                    D_003EF710[i].unk3A = request->unk3A;
                    D_003EF710[i].unk3C = 0x63;
                    D_003EF710[i].unk30 = request->unk30;
                    request->unk38 = -1;
                    D_003EF710[i].unk3B = Sound_ClaimTaggedVoice();
                    func_001CE690(D_003EF710[i].unk3B, D_003EF648.result, 0, request->unk3B, 0x3C, request->unk30, 0, 0x3F);
                    break;
                }
            }
        }
    }
}

/* Turns the volume of every playing unpositioned tagged sound (kind 0x63) to 0. */
void Sound_MuteTagged(void) {
    SoundChannel *entry = D_003EF710;
    int n = 0x7F;

    do {
        n--;
        if (entry->unk3A >= 0 && entry->unk3C == 0x63) {
            func_001CE9A0(0, entry->unk3B, 0);
        }
        entry++;
    } while (n >= 0);
}

/* Gives every playing unpositioned tagged sound its volume back. */
void Sound_UnmuteTagged(void) {
    SoundChannel *channel = D_003EF710;
    int n = 0x7F;
    int volume;

    do {
        n--;
        if (channel->unk3A >= 0 && channel->unk3C == 0x63) {
            volume = (int)((float)channel->unk30 * (float)D_00349680[2] * 0.015625f);
            func_001CE9A0(0, channel->unk3B, (volume >= 0x80) ? 0x7F : volume);
        }
        channel++;
    } while (n >= 0);
}

/* Stops the tagged sound with tag `tag` and frees its channel; -1 if there is none. */
int Sound_StopTagged(char sound) {
    int i;

    for (i = 0; i < 0x80; i++) {
        if (D_003EF710[i].unk3A == sound) {
            break;
        }
    }
    if (i >= 0x80) {
        return -1;
    }
    if ((D_003EF710 + i)->unk3B > 0) {
        func_001CE8B0((D_003EF710 + i)->unk3B);
        (D_003EF710 + i)->unk3A = -1;
        D_00500158[(D_003EF710 + i)->unk3B - 0x54] = -1;
    } else {
        (D_003EF710 + i)->unk3A = -1;
    }
}

/* Silences every sound voice and marks all channels (and optionally all queued requests) free. */
void Sound_StopAll(int clear_queue) {
    int i;

    D_004470E1 = 0;
    for (i = 0x54; i < 0x64; i++) {
        func_001CE8B0(i);
    }
    for (i = 0x3A; i < 0x54; i++) {
        func_001CE8B0(i);
    }
    for (i = 0; i < 0x80; i++) {
        D_003EF710[i].unk3A = -1;
        D_00500158[D_003EF710[i].unk3B - 0x54] = -1;
    }
    if (clear_queue != 0) {
        for (i = 0; i < 0x80; i++) {
            D_00500170[i].unk38 = -1;
        }
    }
}

/* Plays a sound of the se_com bank on voice 0. Nothing calls it. */
static int func_0019EB70(char sound) {
    func_001CE690(0, D_003EF5E8.result, 0, sound, 0x3C, D_00349682[0], 0, 0x3F);
    return 0;
}

/* BGM load step: opens the job's .bd file for the bgm number at a 64-byte aligned address and starts reading
   it; -1 while the file layer or the driver is busy. */
int Sound_BgmReadBody(LoadJob *job, int index) {
    char name[128];
    int handle;
    int load_addr;

    if (func_00102230() != 0) {
        D_0044685C = 1;
        return -1;
    }
    if (func_001CBD20() == 0) {
        return -1;
    }
    load_addr = ((u32)(D_00446F74 + 0x3F) >> 6) << 6;
    job->buffers[0] = D_004470F4 = load_addr;
    D_004470F4 = load_addr + 0x80000;
    sprintf(name, job->name, index);
    handle = func_00102238(name, TRUE, TRUE);
    job->file_handle = handle;
    D_004470E8 = func_00102900(handle);
    func_00102218();
    func_001025C0(job->file_handle, (void *)job->buffers[0], -1);
    D_00446858 += 1;
    return 0;
}

/* BGM load step: once the driver is free, gets the voice (func_001CD020) and the bank (func_001CDF90) of the
   BGM if they are missing, then fades the BGM down by 3 a frame; done at 0. */
int func_0019EC90(IopLoadJob *job) {
    int *spec;
    int level;
    int voice;
    int result;

    if (func_001CBD20() == 0) {
        return -1;
    }
    if (func_001CBAC0() != 0) {
        return -1;
    }
    voice = job->result;
    if (voice < 0) {
        spec = job->ctl;
        result = func_001CD020(job->unk18, spec[1], spec[4], spec[0], spec[3], 0, job->unk1C);
        job->result = result;
        if (result >= 0) {
            func_001CD6E0(result);
        }
        return -1;
    } else {
        if (job->unk14 < 0) {
            spec = job->ctl;
            result = func_001CDF90(job->unk18, voice, spec[2], spec[5]);
            job->unk14 = result;
            if (result >= 0) {
                func_001CD6E0(job->result);
                func_001CE408(job->unk14);
            }
            return -1;
        }
        D_0044685C = 1;
        D_00446860 -= 3;
        if (D_00446860 < 0) {
            D_00446860 = 0;
        }
        level = D_00446860;
        Sound_SetBgmFade((level < D_00349681[0]) ? level : D_00349681[0]);
        if (D_00446860 == 0) {
            D_00446858 += 1;
            return 0;
        }
        return -1;
    }
}

/* BGM load step: once the read has finished, closes the file and frees the old BGM's bank and voice. */
int Sound_BgmCloseBody(LoadJob *job) {
    if (func_00102790(job->file_handle, 1) != 0) {
        return -1;
    }
    func_001024B0(job->file_handle);
    func_00102228();
    job->file_handle = -1;
    func_001CE510(job->unk14);
    func_001CE408(job->unk14);
    func_001CD6E0(job->unk10);
    D_0044685C = 1;
    D_00446858 += 1;
    return 0;
}

/* BGM load step: opens the job's .hd file for the bgm number and starts reading it into the spec's second
   buffer; -1 while busy. */
int Sound_BgmReadHeader(LoadJob *job, int index) {
    char name[128];
    int file;
    int heap_top;
    int *buffers;

    if (func_00102230() != 0) {
        D_0044685C = 1;
        return -1;
    }
    if (func_001CBD20() == 0) {
        return -1;
    }
    heap_top = D_004470F4;
    D_003EF544[0] = heap_top;
    D_004470F4 = ((u32)(heap_top + 0x2003F) >> 6) << 6;
    sprintf(name, job->unk08, index);
    file = func_00102238(name, 1, 1);
    job->file_handle = file;
    D_004470EC = func_00102900(file);
    func_00102218();
    buffers = job->buffers;
    func_001025C0(job->file_handle, (void *)buffers[1], -1);
    D_00446858 += 1;
    return 0;
}

/* BGM load step: once the read has finished, closes the file. */
int Sound_BgmCloseHeader(LoadJob *job) {
    if (func_00102790(job->file_handle, 1) == 0) {
        func_001024B0(job->file_handle);
        func_00102228();
        job->file_handle = -1;
        D_0044685C = 1;
        D_00446858 += 1;
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

    if (func_00102230() != 0) {
        return -1;
    }
    if (func_001CBD20() == 0) {
        return -1;
    }
    heap_top = D_004470F4;
    D_003EF548[0] = heap_top;
    D_004470F4 = ((u32)(heap_top + 0x2003F) >> 6) << 6;
    sprintf(name, job->name_format, index);
    handle = func_00102238(name, TRUE, TRUE);
    job->file_handle = handle;
    D_004470F0 = func_00102900(handle);
    func_00102218();
    buffers = job->buffers;
    func_001025C0(job->file_handle, (void *)buffers[2], -1);
    D_00446858 += 1;
    return 0;
}

/* BGM load step: once the read has finished, closes the file. */
int Sound_BgmCloseSequence(LoadJob *job) {
    if (func_00102790(job->file_handle, 1) != 0) {
        return -1;
    }
    func_001024B0(job->file_handle);
    func_00102228();
    job->file_handle = -1;
    D_00446858 += 1;
    return 0;
}

/* BGM load step: hands the sequence to the driver (func_001CDF90) and calls func_001CE468 on the bank it
   gives; -1 until it does. */
int func_0019F090(IopLoadJob *job) {
    int *spec;
    int result;

    spec = job->ctl;
    result = func_001CDF90(job->unk18, job->result, spec[2], spec[5]);
    job->unk14 = result;
    if (result >= 0) {
        func_001CE468(result, 0);
        D_00446858 += 1;
        return 0;
    }
    return -1;
}

/* BGM load step: raises the BGM fade level by 3 a frame (applied unless the bgm is 0x12) until it passes the
   music volume option. */
int Sound_BgmFadeIn(int unused, int mode) {
    if (func_001CBAC0() != 0) {
        return -1;
    }
    D_0044685C = 1;
    D_00446860 += 3;
    if (mode != 0x12) {
        Sound_SetBgmFade((D_00446860 < D_00349681[0]) ? D_00446860 : D_00349681[0]);
    }
    if (D_00446860 >= 0x100 || D_00349681[0] < D_00446860) {
        D_00446858 += 1;
        return 1;
    }
    return -1;
}

/* BGM load step: ends the cue of the last BGM and starts cue `bgm` of the rev_para.dat table (none for
   0x12). */
int Sound_BgmStartCue(void *context, int cue_index) {
    SoundCue *cue;

    if (cue_index == 0x12) {
        D_00446858 += 1;
        return 0;
    }
    if (D_00446896 < 4U) {
        func_001CEF48((char)D_00446896);
    }
    cue = (SoundCue *)(D_004FF558 + cue_index * 0x10);
    D_00446896 = func_001CEE78(cue->flags, cue->unk04, cue->unk06, cue->unk08, cue->unk0A, cue->unk0C, cue->unk0E, 0);
    D_00446858 += 1;
    return 0;
}

/* BGM load step: unless the bgm is 0x12, waits for the driver and calls func_001CEFA0 on the cue, flagging
   the change (D_004470E2). */
int func_0019F220(void *actor, int mode) {
    if (mode != 0x12) {
        if (func_001CBAC0() != 0) {
            return -1;
        }
        if (func_001CBD20() == 0) {
            return -1;
        }
        if (D_00446896 < 4U) {
            func_001CEFA0((char) D_00446896);
            D_004470E2 = TRUE;
        }
    }
    D_00446858 += 1;
    return 0;
}

/* Reads the file job->name formats with index into the start of the sound heap, a step a call, then calls
   func_001CD6E0 on job->unk10; 1 when done. Nothing calls it. */
static int func_0019F298(LoadJob *job, int index) {
    int heap_start;
    int file;

    if (func_001CBD20() != 0) {
        switch (D_004468A4) {
        case 0:
            heap_start = ((u32)&D_002496BF >> 6) << 6;
            D_004470F4 = heap_start;
            *job->buffers = heap_start;
            D_004470F4 = heap_start + 0xA0000;
            sprintf(D_004FF3C0, job->name, index);
            file = func_00102238(D_004FF3C0, 1, 1);
            D_004470D4 = file;
            func_001025C0(file, (void *)*job->buffers, -1);
            D_004468A4 += 1;
            break;
        case 1:
            if (func_00102790(D_004470D4, 1) == 0) {
                func_001024B0(D_004470D4);
                D_004468A4 = 0;
                func_001CD6E0(job->unk10);
                return 1;
            }
            break;
        }
    }
    return -1;
}

/* Reads the file request->name formats with the current map's unk70 into a new buffer of the sound heap, a
   step a call; 1 when done. */
s32 func_0019F3A0(LoadRequest *request) {
    u32 buffer;

    if (func_001CBD20() != 0) {
        switch (D_004468A8) {
        case 0:
            buffer = D_004470F4;
            request->target->buffer = (void *)buffer;
            D_004470F4 = (((buffer + 0x2003F) >> 6) << 6);
            sprintf(D_004FF440, request->name, D_0038CD28[0]->unk70);
            D_004470DC = func_00102238(D_004FF440, TRUE, TRUE);
            func_001025C0(D_004470DC, request->target->buffer, -1);
            D_004468A8 += 1;
            break;
        case 1:
            if (func_00102790(D_004470DC, 1) == 0) {
                func_001024B0(D_004470DC);
                D_004468A8 = 0;
                return 1;
            }
            break;
        }
    }
    return -1;
}

/* Load step: hands the job's spec to the driver (func_001CD020) and keeps the result. */
int func_0019F498(IopLoadJob *job) {
    int *ctl = job->ctl;
    int result;

    result = func_001CD020(job->unk18, ctl[1], ctl[4], ctl[0], ctl[3], 0, job->unk1C);
    job->result = result;
    if (result >= 0) {
        D_00446858 += 1;
    }
    return 0;
}

/* Load step: once the driver is free, calls func_001CD7E0 on the job's result. */
int func_0019F4F8(IopLoadJob *job) {
    if (func_001CBD20() == 0) {
        return -1;
    }
    if (func_001CD6B8(0) == 0) {
        return -1;
    }
    func_001CD7E0(job->result);
    FlushCache(0);
    D_00446858 += 1;
    return 0;
}

/* Stops a BGM load: breaks any read in flight, closes its file and puts the step counter back to 0. */
static void Sound_CancelBgmLoad(LoadJob *job) {
    if (job->file_handle >= 0) {
        sceCdBreak();
        func_001024B0(job->file_handle);
        func_00102228();
    }
    job->file_handle = -1;
    D_00446858 = 0;
}

/* Runs the current step (D_0043ED50) of loading BGM bgm, cancelling the load in progress when bgm is a new
   one; 0 once the last step is done, else 1. */
static int Sound_StepBgmLoad(LoadJob *job, s32 state) {
    if (D_004470E7 != state) {
        D_004470E7 = state;
        Sound_CancelBgmLoad(job);
    }
    if (D_0043ED50[D_00446858](job, state) > 0) {
        D_0044685C = 1;
        D_004470E7 = 0xFF;
        D_00446858 = 0;
        return 0;
    }
    return 1;
}

/* Loads BGM bgm a step a call while pending; returns 0 once it is the loaded one. */
int Sound_LoadBgm(int bgm, int pending) {
    int done;

    D_0044685C = 1;
    if (pending != 0) {
        if ((D_004470E4 == bgm) && (D_004470E5 == 0)) {
            pending = 0;
        } else {
            D_0044685C = 0;
            done = Sound_StepBgmLoad(&D_003EF5B8, bgm);
            D_004470E5 = 1;
            if (done == 0) {
                D_004470E4 = bgm;
                pending = 0;
                D_004470E5 = 0;
            }
        }
    }
    return pending;
}

/* Returns D_0044685C, which the BGM load steps clear while they work. */
int func_0019F6B0(void) {
    return D_0044685C;
}

/* Load step: reads the file job->name into the job's first buffer, waiting for it. */
int func_0019F6B8(LoadJob *job) {
    int file_handle;

    if (func_001CBD20() == 0) {
        return -1;
    }
    file_handle = func_00102238(job->name, 0, 1);
    func_001025C0(file_handle, (void *)*job->buffers, -1);
    func_001024B0(file_handle);
    return 1;
}

/* Reads snd/rev_para.dat into the two cue tables D_004FF558 and D_004FF958. */
static int Sound_LoadReverbParams(void) {
    int file;

    file = func_00102238(D_0043EE18, 0, 1);
    func_001025C0(file, D_004FF558, 0x400);
    func_001025C0(file, D_004FF958, 0x800);
    func_001024B0(file);
    return 1;
}

/* Load step: reads the file request->name into the request's buffer, waiting for it. */
s32 func_0019F788(LoadRequest *request) {
    s32 file_handle;

    if (func_001CBD20() == 0) {
        return -1;
    }
    file_handle = func_00102238(request->name, 0, 1);
    func_001025C0(file_handle, request->target->buffer, -1);
    func_001024B0(file_handle);
    return 1;
}

/* Load step: as func_0019F498. */
int func_0019F7F0(IopLoadJob *job) {
    int *ctl = job->ctl;
    int result;

    result = func_001CD020(job->unk18, ctl[1], ctl[4], ctl[0], ctl[3], 0, job->unk1C);
    job->result = result;
    if (result >= 0) {
        D_00446858 += 1;
    }
    return 0;
}

/* Load step: as func_0019F4F8. */
int func_0019F850(IopLoadJob *job) {
    if (func_001CBD20() == 0) {
        return -1;
    }
    if (func_001CD6B8(0) == 0) {
        return -1;
    }
    func_001CD7E0(job->result);
    FlushCache(0);
    D_00446858 += 1;
    return 0;
}

/* Runs the current step of D_0043EDA8 and moves on when it succeeds; 0 after the fourth, else 1. */
static s32 func_0019F8B0(void) {
    s32 result;

    result = D_0043EDA8[D_004468AC]();
    if (result >= 0) {
        D_004468AC += 1;
    }
    if ((u32)D_004468AC < 4U) {
        return 1;
    }
    D_004468AC = 0;
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
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        func_001CE690(Sound_NextVoice(2), bank, 0, sound, 0x3C, volume, pan, (char) pan_pos);
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
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        slot = Sound_NextVoice(2);
        func_001CE690(slot, bank, 0, sound, 0x3C, volume, pan, (char) pan_pos);
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
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        slot = Sound_NextVoice(2);
        func_001CE690(slot, bank, 0, sound, 0x3C, volume, pan, (char) pan_pos);
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
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        slot = Sound_NextVoice(2);
        func_001CE690(slot, bank, 0, sound, 0x3C, volume, 0, (char) pan_pos);
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
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        if (volume >= 128) {
            volume = 127;
        }
        slot = Sound_NextVoice(2);
        func_001CE690(slot, bank, 0, sound, 0x3C, volume, 0, (char) pan_pos);
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
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        pan_pos = (offset[0] + 1.0f) * 63.0f;
        if (volume >= 128) {
            volume = 127;
        }
        slot = Sound_NextVoice(2);
        func_001CE690(slot, bank, 0, sound, 0x3C, volume, 0, (char) pan_pos);
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
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        func_001CE690(Sound_NextVoice(2), D_003EF618.result, 0, sound, 0x3C, volume, pan, (char) pan_pos);
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
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        func_001CE690(Sound_NextVoice(2), D_003EF618.result, 0, sound, 0x3C, volume, pan, (char) pan_pos);
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
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        slot = Sound_NextVoice(2);
        func_001CE690(slot, D_003EF618.result, 0, sound, 0x3C, volume, pan, (char) pan_pos);
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
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        slot = Sound_NextVoice(2);
        func_001CE690(slot, D_003EF648.result, 0, sound, 0x3C, volume, pan, (char) pan_pos);
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
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        slot = Sound_NextVoice(2);
        func_001CE690(slot, D_003EF648.result, 0, sound, 0x3C, volume, pan, (char) pan_pos);
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
        volume = (float) volume * (float) D_00349682[0] * 0.015625f;
        if (volume >= 128) {
            volume = 127;
        }
        pan = ((u64) rand() << 11) / 0x7FFFFFFF - 0x400;
        slot = Sound_NextVoice(2);
        func_001CE690(slot, D_003EF648.result, 0, sound, 0x3C, volume, pan, (char) pan_pos);
    }
}

/* Load step: hands the job's spec to the driver (func_001CD020) and keeps the result. */
int func_001A0B10(SoundJob *job) {
    SoundJobSpec *spec = job->spec;
    int result = func_001CD020(job->unk18, (int)spec->buffer_b, spec->size_b, (int)spec->buffer_a, spec->size_a, 0, job->unk1C);

    job->result = result;
    if (result >= 0) {
        D_00446864 += 1;
    }
    return 0;
}

/* Load step: once the driver is free, calls func_001CD7E0 on the job's result. */
int func_001A0B70(SoundJob *job) {
    if (func_001CBD20() == 0) {
        return -1;
    }
    if (func_001CD6B8(0) == 0) {
        return -1;
    }
    func_001CD7E0(job->result);
    FlushCache(0);
    D_00446864 += 1;
    return 1;
}

/* Runs the current step of D_0043ED88; 0 when it reports done, else 1. */
static int func_001A0BD0(void *sequence, int index) {
    if (D_0043ED88[D_00446864]() > 0) {
        D_00446864 = 0;
        return 0;
    }
    return 1;
}

/* Loads the bank of args into driver slot index + 6, a step a call; 0 and the slot in *voice_out once done,
   else 1. */
int func_001A0C18(int args_addr, int index, int voice_out) {
    SoundStartArgs *args = (SoundStartArgs *)args_addr;
    int pending = 1;

    D_004FF4D8.unk18 = index + 6;
    D_004FF4D8.unk1C = index * 0x11800 + 0x138810;
    D_004FF4C0.buffer_a = (void *)args->buffer_a;
    D_004FF4C0.buffer_b = (void *)args->buffer_b;
    D_004FF4C0.size_a = args->size_a;
    D_004FF4C0.size_b = args->size_b;
    D_004FF4D8.spec = (SoundJobSpec *)&D_004FF4C0;
    D_004FF4D8.result = index + 6;
    if (func_001A0BD0(&D_004FF4D8, index) == 0) {
        *(u8 *)voice_out = D_004FF4D8.result;
        pending = 0;
    }
    return pending;
}

/* Plays sound from bank with the falloff player kind of D_0043EE30 while distance is within range, panned by
   angle (EntityEvent_StartCueAlways calls it for the sounds of an entity's animation). Range, distance and angle pass through in
   $f12..$f14; match: declared without them, which keeps the registers retail uses. */
void Sound_PlayFalloff(int sound, int kind, int bank) {
    D_0043EE30[kind & 0xFF](sound & 0xFF, bank);
}

/* Map bank load step: moves on. */
s32 func_001A0D08(void) {
    D_00446868 += 1;
    return 0;
}

/* Map bank load step: waits for a pending cue change, then hands the job's spec to the driver
   (func_001CD020). */
int func_001A0D20(SoundJob *job) {
    SoundJobSpec *spec;
    int result;

    if (D_004470E2 != 0) {
        if (func_001CBAC0() != 0) {
            return -1;
        }
        D_004470E2 = 0;
    }
    spec = job->spec;
    result = func_001CD020(job->unk18, (int)spec->buffer_b, spec->size_b, (int)spec->buffer_a, spec->size_a, 0, job->unk1C);
    job->result = result;
    if (result >= 0) {
        D_00446868 += 1;
    }
    return 0;
}

/* Map bank load step: once the driver is free, calls func_001CD7E0 on the job's result. */
int func_001A0D98(SoundJob *job) {
    if (func_001CBD20() == 0) {
        return -1;
    }
    if (func_001CD6B8(0) == 0) {
        return -1;
    }
    func_001CD7E0(job->result);
    FlushCache(0);
    D_00446868 += 1;
    return 0;
}

/* Map bank load step: ends the last map cue and starts the one D_0043EE78 names from the second cue table. */
int func_001A0DF8(void) {
    SoundScriptState state;
    SoundCue *cue;

    state = D_0043EE78;
    if (D_00446895 < 4U) {
        func_001CEF48((char)D_00446895);
    }
    cue = (SoundCue *)(D_004FF958 + state.cue_index * 0x10);
    D_00446895 = func_001CEE78(cue->flags, cue->unk04, cue->unk06, cue->unk08, cue->unk0A, cue->unk0C, cue->unk0E, 1);
    D_00446868 += 1;
    return 0;
}

/* Map bank load step: once the driver is free, calls func_001CEFA0 on the cue and sets D_004470E1 (sounds
   may start). */
int func_001A0F60(void) {
    if (func_001CBD20() == 0) {
        return 0;
    }
    if (D_00446895 < 4U) {
        func_001CEFA0((char) D_00446895);
        D_004470E2 = TRUE;
    }
    D_004470E1 = TRUE;
    D_00446868 += 1;
    return 1;
}

/* Map bank load step: moves on; the first time through sets D_004470E0 and returns 0, after that sets
   D_004470E1 and returns 1. */
int func_001A0FC8(void) {
    D_00446868 += 1;
    D_004468B4 = 0;
    if (D_004470E0 != 0) {
        D_004470E1 = TRUE;
        return 1;
    }
    D_004470E0 = TRUE;
    return 0;
}

/* Runs the current step of the map bank load (D_0043ED90); 0 when it reports done, else 1. */
static int func_001A1000(void *sequence, int kind) {
    if (D_0043ED90[D_00446868]() > 0) {
        D_00446868 = 0;
        return 0;
    }
    return 1;
}

/* Loads the sound bank of a map slot (MapSlot.unk34) a step a call while pending; returns 0 once it is done. */
int Sound_LoadMapBankStep(int slot_addr, int pending) {
    MapSlot *slot = (MapSlot *)slot_addr;
    SoundBankInfo *bank;

    if (pending == 0) {
        return 0;
    }
    D_004FF528.unk1C = 0x106810;
    D_004FF528.unk18 = 3;
    bank = (SoundBankInfo *)slot->unk34;
    D_004FF528.spec = (SoundJobSpec *)&D_004FF510;
    D_004FF510.size_a = bank->size_a;
    D_004FF510.buffer_a = bank->buffer_a;
    D_004FF510.buffer_b = bank->buffer_b;
    D_004FF510.size_b = bank->size_b;
    if (func_001A1000(&D_004FF528, slot->tag) == 0) {
        pending = 0;
    }
    return pending;
}

/* Loads the sound bank of a map slot (MapSlot.unk34), waiting until it is done. */
int Sound_LoadMapBank(MapSlot *slot) {
    SoundBankInfo *bank;

    D_003EF648.unk18 = 3;
    D_003EF648.unk1C = 0x106810;
    bank = (SoundBankInfo *)slot->unk34;
    D_003EF648.spec = (SoundJobSpec *)&D_003EF5A0;
    D_003EF5A0.size_a = bank->size_a;
    D_003EF5A0.buffer_a = bank->buffer_a;
    D_003EF5A0.buffer_b = bank->buffer_b;
    D_003EF5A0.size_b = bank->size_b;
    while (func_001A1000(&D_003EF648, 1) > 0) {
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
    sprintf(path_a, D_003EF678.name_a, file_number);
    sprintf(path_b, D_003EF678.name_b, file_number);
    buffer_base = ((u32)D_002496BF >> 6) << 6;
    D_003EF560.base = buffer_base;
    buffer_limit = buffer_base + 0x20000;
    D_003EF560.limit = buffer_limit;
    D_003EF560.used = 0;
    D_004470F4 = buffer_limit + 0x8000;
    while (func_001A1438() == 0) {
    }
    D_003EF678.result = Sound_LoadFilePair(path_a, path_b, D_003EF678.spec, D_003EF678.unk18, D_003EF678.unk1C);
    while (func_001A1438() == 0) {
    }
    while (func_001CD6B8(0) == 0) {
    }
    func_001CD7E0(D_003EF678.result);
    FlushCache(0);
}

/* Loads the mg bank file_number (mg%03d) into the sound heap and hands it to the driver, waiting until it is
   done. */
void Sound_LoadMgBank(int file_number) {
    char path_a[128];
    char path_b[128];
    int heap_start;
    int buffer_end;

    sprintf(path_a, D_003EF6A8.name_a, file_number);
    sprintf(path_b, D_003EF6A8.name_b, file_number);
    heap_start = ((u32)D_002496BF >> 6) << 6;
    D_003EF580.base = heap_start;
    buffer_end = heap_start + 0x10000;
    D_003EF580.limit = buffer_end;
    D_003EF580.used = 0;
    D_004470F4 = buffer_end + 0x8000;
    while (func_001A1438() == 0);
    D_003EF6A8.result = Sound_LoadFilePair(path_a, path_b, D_003EF6A8.spec, D_003EF6A8.unk18, D_003EF6A8.unk1C);
    while (func_001A1438() == 0);
    while (func_001CD6B8(0) == 0);
    func_001CD7E0(D_003EF6A8.result);
    FlushCache(0);
}

/* Reads two files into the spec's buffers and hands them to the driver (func_001CD0E0); 0 if either file
   will not open. */
static int Sound_LoadFilePair(char *name_a, char *name_b, SoundJobSpec *spec, int first_arg, int second_arg) {
    int handle;

    handle = func_00102238(name_a, 0, 1);
    if (handle < 0) {
        func_001024B0(handle);
        return 0;
    }
    func_001025C0(handle, spec->buffer_a, spec->size_a);
    func_001024B0(handle);
    handle = func_00102238(name_b, 0, 1);
    if (handle < 0) {
        func_001024B0(handle);
        return 0;
    }
    func_001025C0(handle, spec->buffer_b, spec->size_b);
    func_001024B0(handle);
    return func_001CD0E0(first_arg, (int)spec->buffer_b, spec->size_b, (int)spec->buffer_a, spec->size_a, 0, second_arg);
}

/* func_001CBD20. */
static int func_001A1438(void) {
    return func_001CBD20();
}

/* Loads the two files named in a job and hands them to the driver, waiting until it is done. */
void Sound_LoadJob(SoundJob *slot) {
    while (func_001A1438() == 0);
    slot->result = Sound_LoadFilePair(slot->name_a, slot->name_b, slot->spec, slot->unk18, slot->unk1C);
    while (func_001A1438() == 0);
    while (func_001CD6B8(0) == 0);
    func_001CD7E0(slot->result);
    FlushCache(0);
}

/* Sends the current map's sound byte (record 0x4D) to the driver (func_001CBAE0 commands 0xB81, 0xC81) when
   it differs from the one sent last (D_004470E3). */
static int func_001A14D8(void) {
    MapSlot *slot;
    int cached;
    int wanted;

    if (D_0038A130[3] == 0 && D_0038A130[6] == 0 && D_0038A130[1] == 0) {
        slot = &D_0038CD20[D_00446770];
        cached = D_004470E3;
        if ((u8)slot->records[(D_00413F80.map_index << 7) + 0x4D] != cached) {
            if (cached >= 0x40) {
                D_004470E3 = 0x3F;
            } else if (cached < 0) {
                D_004470E3 = 0;
            }
            wanted = (u8)slot->records[(D_00413F80.map_index << 7) + 0x4D];
            D_004470E3 = wanted;
            func_001CBAE0(1, 0xD0, 0xB81, (s8)wanted << 8);
            return func_001CBAE0(1, 0xD0, 0xC81, D_004470E3 << 8);
        }
    }
}

/* Lets Sound_UpdateTagged give positioned tagged sounds their volume again. */
void Sound_UnmutePositional(void) {
    D_004470E6 = 0;
}

/* Makes Sound_UpdateTagged keep positioned tagged sounds at volume 0. */
void Sound_MutePositional(void) {
    D_004470E6 = 1;
}
