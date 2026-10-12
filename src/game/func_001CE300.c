#include "kf_sound.h"

int func_001CBAE0();
int FlushCache(int);
extern SoundState *D_004471D4;

/* Takes the first free bank for a voice's data in IOP memory and hands both to the driver.
   Returns the bank's index, -2 when all four are taken, -1 on any other failure. */
int func_001CE300(int bank_id, int voice_index, int iop_heap, int size) {
    SoundState *state = D_004471D4;
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
