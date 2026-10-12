#include "kf_script.h"

void func_0019C7F0(int);
extern u8 D_00349681[];

/* Sets the volume to `value` 64ths (0 to 64) of the configured one, then finishes. */
void func_002122A0(ScriptValueCommand *command) {
    u32 value;

    value = command->value.value;
    if (value < 0x41) {
        func_0019C7F0((int)(D_00349681[0] * value) / 64);
    }
    command->active = 0;
}
