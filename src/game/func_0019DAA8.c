#include "common.h"
#include "kf_model.h"
#include "kf_sound.h"

extern char D_004470E1;
extern SoundChannel D_00500170[];
extern SoundChannel D_003EF710[];

/* Queues a moving sound in a free D_00500170 slot, or (with the driver busy) claims a free channel. */
s32 func_0019DAA8(s8 sound_id, u8 voice, u8 mode, Vector4Arg *origin, Vector4Arg *target, f32 volume) {
    SoundChannel *request;
    SoundChannel *channel;
    int i;

    if (D_004470E1 == 0) {
        for (i = 0; i < 0x80; i++) {
            if (D_00500170[i].unk38 == -1) {
                request = &D_00500170[i];
                request->unk38 = 1;
                request->unk3A = sound_id;
                request->unk3B = voice;
                request->volume = volume;
                request->unk3C = mode;
                request->unk39 = 2;
                request->origin.x = origin->x;
                request->origin.y = origin->y;
                request->origin.z = origin->z;
                request->target.x = target->x;
                request->target.y = target->y;
                request->target.z = target->z;
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
    channel->unk3A = sound_id;
    channel->unk38 = voice;
    channel->unk3B = -1;
    channel->origin.x = origin->x;
    channel->origin.y = origin->y;
    channel->origin.z = origin->z;
    channel->target.x = target->x;
    channel->target.y = target->y;
    channel->target.z = target->z;
    channel->unk39 = mode;
    channel->unk3C = 2;
    channel->volume = volume;
    /* match: no return; "return channel" adds a $2 copy retail lacks (implicit-int style fall-off) */
}
