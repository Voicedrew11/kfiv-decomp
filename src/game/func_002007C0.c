#include "common.h"

typedef struct {
    char pad0[0x34];
    char *f34;
    int pad38;
    char *f3C;
    int pad40;
    char *f44;
    int pad48[2];
} Entry;

typedef struct {
    int unk0;
    int unk4;
    u32 count;
    int unkC;
    Entry entries[1];
} Header;

void func_002007C0(Header *arg0) {
    u32 i;
    Entry *e;

    e = arg0->entries;
    for (i = 0; i < arg0->count; i++) {
        if (e->f34 != 0) {
            e->f34 = (char *)arg0 + (int)e->f34;
        }
        if (e->f3C != 0) {
            e->f3C = (char *)arg0 + (int)e->f3C;
        }
        if (e->f44 != 0) {
            e->f44 = (char *)arg0 + (int)e->f44;
        }
        e++;
    }
}
