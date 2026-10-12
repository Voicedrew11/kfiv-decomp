#include "common.h"
#include "kf_sound.h"

extern u8 D_00349682[];
extern char D_004470E1;
extern int D_003EF648[];
extern SoundChannel D_00500170[];
extern SoundChannel D_003EF710[];

s8 func_0019C700(void);
int func_001CE690(u32, int, char, char, char, char, s16, char);

/* Queues a sound in a free D_00500170 slot, or (with the driver busy) claims a free channel and starts it. */
int func_0019D7D0(s8 sound, u8 voice, int volume) {
    SoundChannel *channel;
    SoundChannel *request;
    int i;
    s8 voice_id;

    volume = (int) ((float) volume * (float) D_00349682[0] * 0.015625f);
    if (volume >= 0x80) {
        volume = 0x7F;
    }
    if (D_004470E1 == 0) {
        for (i = 0; i < 0x80; i++) {
            if (D_00500170[i].unk38 == -1) {
                request = &D_00500170[i];
                request->unk38 = 1;
                request->unk3A = sound;
                request->unk3B = voice;
                request->unk39 = 0x63;
                request->unk30 = volume;
                return 0;
            }
        }
    }
    for (i = 0; i < 0x80; i++) {
        if (D_003EF710[i].unk3A < 0) {
            break;
        }
    }
    channel = &D_003EF710[i];
    channel->unk3A = sound;
    channel->unk3C = 0x63;
    channel->unk30 = volume;
    voice_id = channel->unk3B = func_0019C700();
    func_001CE690(voice_id, D_003EF648[4], 0, voice, 0x3C, volume, 0, 0x3F);
}
