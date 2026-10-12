#include "common.h"

int sceSdRemote(int, ...);

/* Sets the master volume of both SPU2 cores, left and right (changeMasterVolume). */
void func_0021CC60(int volume) {
    int core;

    for (core = 0; core < 2; core++) {
        sceSdRemote(1, 0x8010, core | 0x980, volume);
        sceSdRemote(1, 0x8010, core | 0xA80, volume);
    }
}
