#include "kf_sound.h"

int func_001CBAE0();
extern SoundState *D_004471D4;

/* Send voice's two words with value, and status with its bit fields re-packed (some inverted). */
void func_001CF2E8(int voice_index, int value, u32 status) {
    SoundVoice *voice;
    u32 packed;

    voice = &D_004471D4->voices[voice_index];
    packed = (status >> 31) << 31;
    packed |= ((((status >> 24) & 0x7F) ^ 0x7F) << 24);
    packed |= ((((status >> 20) & 0xF) ^ 0xF) << 20);
    packed |= status & 0xF0000;
    packed |= status & 0xE000;
    packed |= ((((status >> 6) & 0x7F) ^ 0x7F) << 6);
    packed |= status & 0x20;
    packed |= (status & 0x1F) ^ 0x1F;
    func_001CBAE0(1, 0x120, voice->unk00, voice->unk10, value, packed);
}
