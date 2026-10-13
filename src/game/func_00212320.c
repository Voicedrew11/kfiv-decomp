#include "kf_script.h"
#include "kf_splash.h"

/* Each frame, start or wait on a splash screen with the given timing, then clear the command. */
void func_00212320(ScriptSplashCommand *command) {
    if (command->started == 0) {
        if (Splash_Begin(command->splash_id.value, command->hold_frames.value, command->skippable.value,
                            command->fade_in_frames.value, command->fade_out_frames.value) != 0) {
            command->started = 1;
        }
    } else if (Splash_GetState() == 0) {
        command->active = 0;
    }
}
