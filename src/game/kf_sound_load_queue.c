/* The sound loads waiting to run, one at a time (kf_sound.h's SoundLoadQueue). */
#include "kf_sound.h"

/* 0x38A130 the queue; its unk04 is also read as int D_0038A134[3] by kf_splash.c */
SoundLoadQueue sound_load_queue = { 0 };
asm(".globl D_0038A134\nD_0038A134 = sound_load_queue + 4\n");
