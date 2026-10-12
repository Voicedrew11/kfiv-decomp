#include "kf_sound.h"

int func_001CBAE0();
int FlushCache(int);
int sceSifDmaStat(int);
int sceSifSetDma(int *, int);
extern SoundState *D_004471D4;
extern SoundDmaRequest D_005BF860;
extern SoundUpload D_005BF870;

/* Keeps the upload func_001CBC30 started going: once the piece in flight has arrived it is
   written to sound memory and the next one is sent. Returns 1 when nothing is left to send. */
int func_001CBD20(void) {
    SoundDmaRequest *dma;
    int iop_buffer;
    int dma_id;

    if (D_005BF870.active == 0) {
        return 1;
    }
    if (sceSifDmaStat(D_005BF870.dma_id) >= 0) {
        return 0;
    }
    func_001CBAE0(1, 0xB0, D_005BF870.dest, D_005BF870.piece_size);
    D_005BF870.remaining -= D_005BF870.piece_size;
    if (D_005BF870.remaining <= 0) {
        D_005BF870.active = 0;
        return 1;
    }
    D_005BF870.src += 0x4000;
    D_005BF870.dest += 0x4000;
    if (D_005BF870.remaining > 0x4000) {
        D_005BF870.piece_size = D_005BF870.padded_size = 0x4000;
    } else {
        D_005BF870.padded_size = D_005BF870.remaining;
        D_005BF870.piece_size = D_005BF870.remaining;
        if (D_005BF870.remaining & 0xF) {
            do {
                D_005BF870.padded_size++;
            } while (D_005BF870.padded_size & 0xF);
        }
    }
    dma = &D_005BF860;
    iop_buffer = D_004471D4->unk4C8;
    dma->src = D_005BF870.src;
    dma->dest = iop_buffer;
    dma->size = D_005BF870.padded_size;
    dma->mode = 0;
    FlushCache(0);
    dma_id = sceSifSetDma((int *)dma, 1);
    D_005BF870.dma_id = dma_id;
    return 0;
}
