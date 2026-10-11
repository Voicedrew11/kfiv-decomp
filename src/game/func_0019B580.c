#include "common.h"

typedef struct Env {
    s64 d[16];
} Env;

typedef struct Pkt {
    int tag[4];
    Env env;
} Pkt;

extern int D_00446F78;
extern Env D_00432D80;
extern Env D_00432E70;
int FlushCache(int);
int sceGsPutDrawEnv(void *);

void func_0019B580(void) {
    Pkt pkt;

    pkt.tag[0] = 0x8008;
    pkt.tag[1] = 0x10000000;
    pkt.tag[2] = 0xE;
    pkt.tag[3] = 0;
    pkt.env = *(D_00446F78 != 0 ? &D_00432D80 : &D_00432E70);
    FlushCache(0);
    sceGsPutDrawEnv(&pkt);
}
