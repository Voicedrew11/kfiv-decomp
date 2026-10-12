#include "kf_sound.h"

int func_001CBAE0();
int func_001CD6E0(u32);
int func_001CF070(const char *);
int func_001CF110(char *, void *);
int FlushCache(int);
int sceSifAllocIopHeap(int);
int sceSifFreeIopHeap(int);
extern SoundState *D_004471D4;

/* Loads a voice from files: the header file to its two IOP blocks and, if one is named, the
   sample file. Returns the voice, -2 when none is free, -1 on failure. */
int func_001CC1A8(int voice_id, char *header_name, char *sample_name, int unk1B) {
    SoundVoice *voice = D_004471D4->voices;
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
        voice->unk0C = D_004471D4->unk4C0;
    }
    if ((u32)voice->unk0C >= D_004471D4->unk4C0) {
        D_004471D4->unk4C0 = voice->unk0C + voice->unk14;
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
