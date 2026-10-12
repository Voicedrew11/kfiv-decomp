#include "kf_sound.h"

int func_001CBAE0();
int FlushCache(int);
int sceSifAllocIopHeap(int);
extern SoundState *D_004471D4;

/* Sets up a free voice from data that is already on the IOP: its first header block and its
   sample block. Returns the voice, -2 when none is free, -1 on failure. */
int func_001CD3D0(int voice_id, int header_block, int header_size, int sample_block, int sample_size, int unk1B) {
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
        voice->unk0C = D_004471D4->unk4C0;
    }
    if ((u32)voice->unk0C >= D_004471D4->unk4C0) {
        D_004471D4->unk4C0 = voice->unk0C + voice->unk14;
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
