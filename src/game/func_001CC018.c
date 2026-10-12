#include "kf_sound.h"

int func_001CBAE0();
extern SoundState *D_004471D4;

/* Switches every loaded voice and bank to its first data block (unk00). */
void func_001CC018(void) {
    SoundVoice *voice;
    SoundBank *bank;
    int i;

    if (D_004471D4->active != 0) {
        func_001CBAE0(1, 0x50);
    }
    *(u16 *)&D_004471D4->unk4D0[4] = 1; // match: the u16 at 0x4D4; the header has six chars there
    func_001CBAE0(1, 0xC0, 1);
    voice = D_004471D4->voices;
    for (i = 0; i < 16; i++) {
        voice->unk1C = 1;
        if (voice->unk00 == 0) {
            continue;
        }
        if (voice->unk0C == 0) {
            continue;
        }
        if (voice->unk04 == 0) {
            continue;
        }
        func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, voice->unk00, voice->unk1B);
        voice++; // match: retail only steps to the next voice after a reload
    }
    bank = D_004471D4->banks;
    for (i = 0; i < 4; i++) {
        if (bank->iop_heap == 0) {
            continue;
        }
        if (bank->voice == -1) {
            continue;
        }
        voice = &D_004471D4->voices[bank->voice];
        func_001CBAE0(1, 0x2070, bank->unk0C, voice->unk0C, voice->unk00, voice->unk1B, 1);
        bank++; // match: as above
    }
    if (D_004471D4->active != 0) {
        func_001CBAE0(1, 0x40);
    }
}
