#include "common.h"

int puts(char *);
extern char D_0043E4A0[];
extern int D_00446748;
extern u32 D_00447038;
extern int D_0044703C;

typedef struct { int w0, w1, w2, w3; } Blk;
extern void *D_00447018[2];
extern Blk *D_00447020[2];

void func_00109650(int arg0, u32 arg1) {
    void **p18;
    Blk **p20;

    p18 = &D_00447018[D_00446748];
    p20 = &D_00447020[D_00446748];
    if (D_0044703C == 0) {
        if (arg1 >= D_00447038) {
            puts(D_0043E4A0);
            while (1) {
            }
        } else {
            if (*p18 != 0) {
                Blk *b = *p20;
                b->w0 = 0x20000000;
                b->w1 = arg0;
                b->w3 = b->w2 = 0;
            } else {
                *p18 = (void *) arg0;
            }
            *p20 = (Blk *) arg1;
        }
    }
}
