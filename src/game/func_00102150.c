#include "common.h"

int sceCdInit(int);
int sceCdMmode(int);

/* Starts the disc drive in DVD mode. */
void func_00102150(void) {
    while (sceCdInit(0) == 0) {
    }
    sceCdMmode(2);
}
