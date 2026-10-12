#include "kf_sound.h"

extern SoundState *D_004471D4;

/* Queues a 10-byte command for the note in slot `slot` in the buffer the IOP reads:
   kind 0 and 1 send `value` as they are (1 forces 0x3F unless the voice allows a volume),
   kind 2 sends it as two 7-bit halves. -1: no driver or bad slot, -2: the buffer is full. */
int func_001CE9A0(int kind, u32 slot, int value) {
    SoundState *state = D_004471D4;
    int *queue_size;
    int queued;
    int offset;
    u8 *out;
    SoundKey *key;
    SoundVoice *voice;

    if (state->active == 0) {
        return -1;
    }
    if (slot >= 0x7E) {
        return -1;
    }
    queue_size = (int *)(state->dma_dest | 0x20000000);
    queued = *queue_size;
    if ((u32)(queued + 0xA) >= 0x3FC) {
        return -2;
    }
    offset = queued + 4;
    out = (u8 *)(((int)queue_size + offset) | 0x20000000);
    key = (SoundKey *)state->unk240[slot];
    voice = &state->voices[key->voice];
    *out++ = 0xFF;
    *out++ = voice->unk18;
    *out++ = 7;
    switch (kind) {
    case 0:
        *out++ = 0xFD;
        *out++ = 0;
        *out++ = key->unk01;
        *out++ = key->unk02;
        *out++ = slot;
        out[0] = value;
        out[1] = 0;
        break;
    case 1:
        // match: the u16 at 0x4D4; the header has six chars there
        if (*(u16 *)&D_004471D4->unk4D0[4] == 0 || voice->unk1C == 0) {
            value = 0x3F;
        }
        *out++ = 0xFD;
        *out++ = 1;
        *out++ = key->unk01;
        *out++ = key->unk02;
        *out++ = slot;
        out[0] = value;
        out[1] = 0;
        break;
    case 2:
        *out++ = 0xFD;
        *out++ = kind;
        *out++ = key->unk01;
        *out++ = key->unk02;
        *out++ = slot;
        out[0] = value & 0x7F;
        out[1] = ((value >> 7) + 0x40) & 0x7F;
        break;
    }
    *queue_size += 0xA;
    return 0;
}
