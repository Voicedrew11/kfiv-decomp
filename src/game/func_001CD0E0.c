#include "kf_sound.h"

int func_001CBAE0();
int func_001CBC30(int, int, u32);
int func_001CBD20(void);
int func_001CD6E0(u32);
int FlushCache(int);
int sceSifAllocIopHeap(int);
int sceSifFreeIopHeap(int);
int sceSifDmaStat(int);
int sceSifSetDma(int *, int);
extern SoundState *D_004471D4;
extern SoundDmaRequest D_005BF860;

/* Loads a voice from memory like func_001CC888, into a voice and a place in sound memory the
   caller chooses. Returns the voice, or -1 on failure. */
int func_001CD0E0(int voice_index, int header_data, int header_size, int sample_data, int sample_size, int unk1B,
                  int dest) {
    SoundVoice *voice = &D_004471D4->voices[voice_index];
    u32 heap_size = 0x100000;
    int block;
    int i;
    int remaining;
    int heap;
    int sent;
    int write_addr;

    voice->unk1B = unk1B;
    voice->unk18 = voice_index;
    while (header_size & 0xF) {
        header_size++;
    }
    voice->unk10 = header_size;
    if (voice->unk00 != 0) {
        sceSifFreeIopHeap(voice->unk00);
    }
    if (voice->unk04 != 0) {
        sceSifFreeIopHeap(voice->unk04);
    }
    voice->unk00 = sceSifAllocIopHeap(voice->unk10);
    voice->unk04 = sceSifAllocIopHeap(voice->unk10);
    if (voice->unk00 == 0 || voice->unk04 == 0 || (u32)(voice->unk00 + voice->unk10) > 0x1FFFFF ||
        (u32)(voice->unk04 + voice->unk10) > 0x1FFFFF) {
        func_001CD6E0(voice_index);
        return -1;
    }
    func_001CBC30(voice->unk00, header_data, voice->unk10);
    while (func_001CBD20() == 0) {
    }
    func_001CBAE0(1, 0x1070, voice->unk04, voice->unk00, voice->unk10);
    while (sample_size & 0xF) {
        sample_size++;
    }
    remaining = sample_size;
    voice->unk14 = remaining;
    voice->unk0C = dest;
    write_addr = dest;
    do {
        heap_size >>= 1;
    } while ((heap = sceSifAllocIopHeap(heap_size)) == 0);
    if (sample_size > 0) {
        do {
            if (heap_size < (u32)remaining) {
                D_005BF860.size = heap_size;
            } else {
                D_005BF860.size = remaining;
            }
            D_005BF860.src = sample_data;
            D_005BF860.dest = heap;
            D_005BF860.mode = 0;
            func_001CBAE0(1, 0x80, voice->unk1A, 1);
            i = sceSifSetDma((int *)&D_005BF860, 1); // match: the loop counter doubles as the DMA id
            if (i == 0) {
                sceSifFreeIopHeap(heap);
                func_001CD6E0(voice_index);
                return -1;
            }
            while (sceSifDmaStat(i) >= 0) {
            }
            sent = func_001CBAE0(1, 0x90, voice->unk1A, 0, heap, write_addr, D_005BF860.size);
            if (sent != D_005BF860.size) {
                sceSifFreeIopHeap(heap);
                func_001CD6E0(voice_index);
                return -1;
            }
            sample_data += sent;
            write_addr += sent;
            remaining -= sent;
        } while (remaining > 0);
    }
    func_001CBAE0(1, 0x80, voice->unk1A, 1);
    sceSifFreeIopHeap(heap);
    if (voice->unk1C != 0) {
        block = voice->unk00;
    } else {
        block = voice->unk04;
    }
    if (func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, block, voice->unk1B) != 0) {
        func_001CD6E0(voice_index);
        return -1;
    }
    FlushCache(0);
    return voice_index;
}
