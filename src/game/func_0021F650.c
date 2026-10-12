#include "kf_movie.h"
#include <eeregs.h>

/* shared with the main thread */
extern volatile s32 D_00437824[]; /* handler enabled */
extern volatile s32 D_00437828[]; /* vblanks counted while enabled */
extern s32 D_0043782C[];          /* a picture was shown completely: drop it from the ring */
extern volatile s32 D_00437830[]; /* field that starts, from GS CSR */
extern volatile s32 D_00437834[]; /* nonzero: path 3 still busy */
extern MovieOutBuffer D_00437838;
extern s32 D_00437A4C[];          /* vblanks with no picture ready */
extern MovieDisplay D_004378D0;

int sceGsSyncPath(int, int);
MovieOutTag *func_0021E668(MovieOutBuffer *);
void func_0021ED10(MovieDisplay *, s32);
void func_0021F618(u32);

/* Vblank handler of the movie player: sends the waiting picture's packets for the field that starts. */
int func_0021F650(int cause) {
    MovieOutTag *tag;
    s32 unused[4]; // match: retail keeps 0x10 bytes of stack it never touches

    D_00437830[0] = (*GS_CSR >> 13) & 1;
    if (D_00437824[0] != 0) {
        D_00437828[0]++;
        D_00437834[0] = sceGsSyncPath(1, 0);
        if (D_00437834[0] == 0) {
            tag = func_0021E668(&D_00437838);
            if (tag == NULL) {
                D_00437A4C[0]++;
                __asm__ volatile("sync");
                __asm__ volatile("ei");
                return 0;
            }
            if (D_00437830[0] == 0 && tag->status == MOVIE_OUT_STATUS_FULL) {
                func_0021ED10(&D_004378D0, 1);
                func_0021F618((u32)tag->field_packets[0]);
                tag->status = 1;
            } else if (D_00437830[0] != 0 && tag->status == 1) {
                func_0021ED10(&D_004378D0, 0);
                func_0021F618((u32)tag->field_packets[1]);
                tag->status = 0;
                D_0043782C[0] = 1;
            }
        }
    }
    __asm__ volatile("sync");
    __asm__ volatile("ei");
    return 0;
}
