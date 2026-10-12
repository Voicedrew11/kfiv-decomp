#include "kf_script.h"

double sqrt(double);
int func_00211D90(void *, double);  // match: callers pass a double although the definition takes float (as in retail)

void func_00215F50(ScriptDistance2DCommand *command) {
    float delta_x;
    float delta_y;
    float distance_sq;
    float distance;

    delta_x = command->a_x.value - command->b_x.value;
    delta_y = command->a_y.value - command->b_y.value;
    distance_sq = delta_x * delta_x + delta_y * delta_y;
    distance = sqrt(distance_sq);
    func_00211D90(&command->result, distance);
    command->active = 0;
}
