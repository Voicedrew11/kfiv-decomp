#include "kf_sound.h"

int func_001CBAE0();
extern SoundState *D_004471D4;

int func_001CDD20(int voice_index) {
    SoundVoice *voice = &D_004471D4->voices[voice_index];
    SoundBank *bank = D_004471D4->banks;
    int result = 0;
    int i;

    voice->unk1C = 1;
    if (D_004471D4->active != 0) {
        func_001CBAE0(1, 0x50);
    }
    if (func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, voice->unk00, voice->unk1B) != 0) {
        result = -1;
    }
    for (i = 4; --i != -1; bank++) {
        if (bank->voice == voice_index) {
            if (func_001CBAE0(1, 0x2070, bank->unk0C, voice->unk0C, voice->unk00, voice->unk1B, 1) != 0) {
                result = -1;
                break;
            }
        }
    }
    if (D_004471D4->active != 0) {
        func_001CBAE0(1, 0x40);
    }
    return result;
}
