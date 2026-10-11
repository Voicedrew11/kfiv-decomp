#include "common.h"

extern char D_004681A0[];

typedef struct {
    char pad0[0x128];
    void *next[2];
    char pad1[0x78];
    void (*f1A8)(void *);
    void (*f1AC)(void *);
    void (*f1B0)(void *);
} Obj;

void func_0010B140(u32 arg0, u32 arg1) {
    int i;
    Obj *s;
    Obj **slot;

    if (arg0 < 5U && arg1 < 2U) {
        for (i = 999; i >= 0; i--) {
            slot = (Obj **)(D_004681A0 + (i * 4 + arg1 * 0xFA0 + arg0 * 0x1F40));
            s = *slot;
            if (s != 0) {
                do {
                    if (s->f1AC != 0) {
                        s->f1AC(s);
                    }
                    s->f1A8(s);
                    if (s->f1B0 != 0) {
                        s->f1B0(s);
                    }
                    s = s->next[arg1];
                } while (s != 0);
            }
        }
    }
}
