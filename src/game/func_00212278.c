#include "kf_script.h"
#include "kf_sound.h"

extern SoundLoadQueue D_0038A130;

/* Asks for the sound load `value` unless it is the current one, then finishes. */
void func_00212278(ScriptValueCommand *command) {
    if (D_0038A130.unk00 != command->value.value) {
        D_0038A130.unk00 = command->value.value;
        D_0038A130.unk04 = 1;
    }
    command->active = 0;
}
