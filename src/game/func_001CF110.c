#include "common.h"
#include "kf_sound.h"

/* SDK: loads an IOP heap file, returns below zero until the IOP is ready */
int sceSifLoadIopHeap(char *name, void *addr);
extern SoundState *D_004471D4;

int func_001CF110(char *name, void *addr) {
    if (D_004471D4->load_addr == NULL) {
        D_004471D4->load_addr = addr;
    }
    while (sceSifLoadIopHeap(name, addr) < 0) {
    }
    return 0;
}
