#include "kf_script.h"

double sqrt(double);
int func_00211D90(void *, double);  // match: retail converts the float argument to double at the call site (fptodp), as the definition takes float

void func_00215FE0(ScriptDistanceCommand *command) {
    float delta_x;
    float delta_y;
    float delta_z;
    float distance_sq;
    float distance;

    delta_x = command->a_x.value - command->b_x.value;
    delta_y = command->a_y.value - command->b_y.value;
    delta_z = command->a_z.value - command->b_z.value;
    distance_sq = delta_x * delta_x + delta_y * delta_y + delta_z * delta_z;
    distance = sqrt(distance_sq);
    func_00211D90(&command->result, distance);
    command->active = 0;
}
