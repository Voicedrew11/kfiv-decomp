#include "common.h"
#include "kf_sound.h"
#include "kf_map.h"

extern SoundJob D_003EF648;
extern SoundBankSpec D_003EF5A0;

int func_001A1000(void *, int);

/* Loads the sound bank of a map slot, waiting until it is done. */
int func_001A10D8(MapSlot *slot) {
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
