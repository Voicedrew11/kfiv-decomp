#include "common.h"

int sceSdRemote(int, ...);

/* Sets the volume of the sound stream's input, left and right (changeInputVolume). */
void func_0021CCC8(int volume) {
    sceSdRemote(1, 0x8010, 0xF80, volume);
    sceSdRemote(1, 0x8010, 0x1080, volume);
}
