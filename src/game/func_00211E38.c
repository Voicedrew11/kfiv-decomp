#include "kf_script.h"

/* Waits: counts the argument down once a frame and finishes when it reaches zero. */
void func_00211E38(ScriptValueCommand *command) {
    command->value.value--;
    if (command->value.value <= 0) {
        command->active = 0;
    }
}
