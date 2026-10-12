#include "kf_script.h"

float atan2f(float, float);
int func_00211D90(void *, double);  // match: callers pass a double although the definition takes float (as in retail)

void func_00214FF0(ScriptAngleCommand *command) {
    func_00211D90(&command->angle, atan2f(command->x.value, command->y.value));
    command->active = 0;
}
