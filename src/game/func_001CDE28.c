#include "kf_sound.h"

int func_001CBAE0();
int func_001CF070(const char *);
int func_001CF110(char *, void *);
int FlushCache(int);
int sceSifAllocIopHeap(int);
extern SoundState *D_004471D4;

/* Loads a bank file to the IOP and binds it to a voice.
   Returns the bank, -2 when none is free, -1 on failure. */
int func_001CDE28(int bank_id, int voice_index, char *name) {
    SoundBank *bank = D_004471D4->banks;
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
    voice = &D_004471D4->voices[voice_index];
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
