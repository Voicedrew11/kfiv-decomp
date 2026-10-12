#include "kf_sound.h"

int func_001CBAE0();
int func_001CBC30(int, int, u32);
int func_001CBD20(void);
int func_001CE408(int);
int FlushCache(int);
int sceSifAllocIopHeap(int);
extern SoundState *D_004471D4;

/* Loads a voice's data into the first free bank: allocates IOP memory for it, sends the data there
   (waiting until the transfer is done) and hands bank and voice to the driver.
   Returns the bank's index, -2 when all four are taken, -1 on any other failure. */
int func_001CE170(int bank_id, int voice_index, int data, int size) {
    SoundState *state = D_004471D4;
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
