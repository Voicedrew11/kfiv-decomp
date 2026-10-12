#include "common.h"
#include "kf_sound.h"
#include "kf_map.h"

extern SoundJob D_004FF528;
extern SoundBankSpec D_004FF510;

int func_001A1000(void *, int);

/* Runs the step 1 sound load, the sound bank of a map slot; returns 0 once it is done. */
int func_001A1048(int slot_addr, int pending) {
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
