#include "kf_sound.h"

int func_001CBAE0();
int func_001CBC30(int, int, u32);
int func_001CBD20(void);
int func_001CD6E0(u32);
int FlushCache(int);
int sceSifAllocIopHeap(int);
int sceSifFreeIopHeap(int);
extern SoundState *D_004471D4;

/* Loads a voice from memory: header data to its two IOP blocks, sample data to sound memory.
   Returns the voice, -2 when none is free, -1 on failure. */
int func_001CC400(int voice_id, int header_data, int header_size, int sample_data, int sample_size, int unk1B) {
    SoundVoice *voice = D_004471D4->voices;
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
        voice->unk0C = D_004471D4->unk4C0;
    }
    if ((u32)voice->unk0C >= D_004471D4->unk4C0) {
        D_004471D4->unk4C0 = voice->unk0C + voice->unk14;
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
