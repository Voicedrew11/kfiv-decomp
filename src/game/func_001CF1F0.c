#include "kf_sound.h"

int func_001CBAE0();
extern SoundState *D_004471D4;

/* Sends slot voice_index's two words with param; the reply is re-packed into *status (some fields inverted). */
void func_001CF1F0(int voice_index, int param, SoundVoiceStatus *status) {
    SoundVoice *slot;
    u32 reply, inverted;

    slot = &D_004471D4->voices[voice_index];
    reply = func_001CBAE0(1, 0x110, slot->unk00, slot->unk10, param);
    inverted = ~reply;
    status->bit31 = reply >> 31;
    status->bits24_30 = inverted >> 24;
    status->bits20_23 = inverted >> 20;
    status->bits16_19 = reply >> 16;
    status->bits13_15 = reply >> 13;
    status->bits6_12 = inverted >> 6;
    status->bit5 = reply >> 5;
    status->bits0_4 = inverted;
}
