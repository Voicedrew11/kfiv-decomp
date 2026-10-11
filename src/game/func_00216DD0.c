#include "common.h"

int scePadRead(int, int, void *);

typedef struct {
    u8 data[0x24];
} PadData;

typedef struct {
    char pad0[0x100];
    PadData data;
    char pad124[0x58];
    u8 state;
} Pad;

int func_00216DD0(Pad *p, int port, int slot) {
    PadData buf;
    int old;

    old = p->state;
    if (scePadRead(port, slot, &buf) == 0) {
        return 0;
    }
    if (buf.data[0] == 0) {
        p->state = buf.data[1] >> 4;
        if (old != 0 && p->state != old) {
            return -1;
        }
        p->data = buf;
        *(u16 *)&p->data.data[2] = ~*(u16 *)&p->data.data[2];
    }
    return 1;
}
