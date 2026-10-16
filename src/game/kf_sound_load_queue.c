/* The sound loads waiting to run, one at a time (kf_sound.h's SoundLoadQueue). */
#include "kf_sound.h"

/* 0x38A130 the queue; its unk04 is also read as int bgm_load_pending[3] by kf_splash.c */
SoundLoadQueue sound_load_queue = { 0 };
asm(".globl bgm_load_pending\nbgm_load_pending = sound_load_queue + 4\n");
