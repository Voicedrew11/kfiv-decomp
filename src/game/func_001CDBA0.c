#include "kf_sound.h"

int func_001CBAE0();
extern SoundState *D_004471D4;

void func_001CDBA0(int voice_index, u8 value) {
    func_001CBAE0(1, 0x1050, D_004471D4->voices[voice_index].unk18, value);
}
