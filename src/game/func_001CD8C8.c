#include "kf_sound.h"

int func_001CBAE0();
int FlushCache(int);
extern SoundState *D_004471D4;

/* Packs sound memory: sorts the voices by where their sample data sits, moves each one down
   to close the gaps, then rebinds every voice and bank. Returns -1 on failure. */
int func_001CD8C8(void) {
    SoundVoice *sorted[16];
    char moved[16]; // match: cleared and never read
    SoundVoice *voice;
    SoundVoice *swap;
    SoundBank *bank;
    u32 next_addr;
    int block;
    int sent;
    int gap;
    int i;
    int start;
    int k;

    for (i = 15; i != -1; i--) {
        sorted[i] = &D_004471D4->voices[i];
        moved[i] = 0;
    }
    for (gap = 8; gap != 0; gap /= 2) {
        for (start = 0; start < gap; start++) {
            for (i = start + gap; i < 16; i += gap) {
                for (k = i - gap; k >= start; k -= gap) {
                    if ((u32)sorted[k]->unk0C > (u32)sorted[k + gap]->unk0C) {
                        swap = sorted[k];
                        sorted[k] = sorted[k + gap];
                        sorted[k + gap] = swap;
                        continue;
                    }
                    break;
                }
            }
        }
    }
    next_addr = D_004471D4->unk4BC;
    for (i = 0; i < 16; i++) {
        voice = sorted[i];
        if (voice->iop_heap == 0) {
            continue;
        }
        if (voice->unk0C == 0) {
            continue;
        }
        if (next_addr < (u32)voice->unk0C) {
            voice->unk0C = next_addr;
            func_001CBAE0(1, 0x80, voice->unk1A, 1);
            sent = func_001CBAE0(1, 0x90, voice->unk1A, 0, voice->iop_heap, voice->unk0C, voice->unk14);
            if (sent != voice->unk14) {
                return -1;
            }
            next_addr += sent;
            func_001CBAE0(1, 0x80, voice->unk1A, 1);
            if (voice->unk1C != 0) {
                block = voice->unk00;
            } else {
                block = voice->unk04;
            }
            if (func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, block, voice->unk1B) != 0) {
                return -1;
            }
        } else {
            next_addr = voice->unk0C + voice->unk14;
        }
    }
    D_004471D4->unk4C0 = next_addr;
    bank = D_004471D4->banks;
    for (i = 0; i < 4; i++, bank++) {
        if (bank->iop_heap == 0) {
            continue;
        }
        if (bank->voice == -1) {
            continue;
        }
        voice = &D_004471D4->voices[bank->voice];
        if (voice->unk1C != 0) {
            block = voice->unk00;
        } else {
            block = voice->unk04;
        }
        if (func_001CBAE0(1, 0x1010, bank->unk0C, voice->unk0C, block, voice->unk1B) != 0) {
            return -1;
        }
    }
    FlushCache(0);
    return 0;
}
