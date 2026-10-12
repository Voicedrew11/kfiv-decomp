#include "kf_sound.h"

extern SoundState *D_004471D4;

/* Starts a note in slot `slot` on a voice: queues, in the buffer the IOP reads, an optional program
   change, the pan (0x3F unless the voice allows one), the bend as two 7-bit halves, the volume and the
   note itself. Returns the slot; -1: no driver or bad slot, -2: the buffer is full. */
int func_001CE690(u32 slot, int voice_index, char channel, char program, char note, char volume, s16 bend, char pan) {
    SoundState *state = D_004471D4;
    int *queue_size;
    int queued;
    int offset;
    int length;
    u8 *out;
    SoundKey *key;
    SoundVoice *voice;

    if (state->active == 0) {
        return -1;
    }
    queue_size = (int *)(state->dma_dest | 0x20000000);
    length = 0x19;
    if (program != -1) {
        length = 0x1B;
    }
    queued = *queue_size;
    if ((u32)(queued + length) >= 0x3FC) {
        return -2;
    }
    offset = queued + 4;
    out = (u8 *)(((int)queue_size + offset) | 0x20000000);
    voice = &state->voices[voice_index];
    if (slot >= 0x7E) {
        return -1;
    }
    key = (SoundKey *)state->unk240[slot & 0x7F];
    key->unk03 = 1;
    key->voice = voice_index;
    key->unk02 = note;
    key->unk01 = channel;
    *out++ = 0xFF;
    *out++ = voice->unk18;
    *out++ = length - 3;
    if (program != -1) {
        *out++ = channel - 0x40;
        *out++ = program;
    }
    // match: the u16 at 0x4D4; the header has six chars there
    if (*(u16 *)&D_004471D4->unk4D0[4] == 0 || voice->unk1C == 0) {
        pan = 0x3F;
    }
    *out++ = 0xF9;
    *out++ = 1;
    *out++ = channel;
    *out++ = pan;
    *out++ = 0;
    *out++ = 0xF9;
    *out++ = 2;
    *out++ = channel;
    *out++ = bend & 0x7F;
    *out++ = ((bend >> 7) + 0x40) & 0x7F;
    *out++ = 0xF9;
    *out++ = 0;
    *out++ = channel;
    *out++ = volume;
    *out++ = 0;
    *out++ = 0xFD;
    *out++ = 0x10;
    *out++ = channel;
    *out++ = note;
    *out++ = slot;
    out[0] = 0x7F;
    out[1] = 0;
    *queue_size += length;
    return slot;
}
