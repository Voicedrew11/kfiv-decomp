/* The sound loads waiting to run, one at a time (sound/sound.h's SoundLoadQueue). */
#include "sound/sound.h"

/* 0x38A130 the queue; its bgm_load_pending field is also read as int bgm_load_pending[3] by ui/splash.c */
SoundLoadQueue sound_load_queue = { 0 };
asm(".globl bgm_load_pending\nbgm_load_pending = sound_load_queue + 4\n");
