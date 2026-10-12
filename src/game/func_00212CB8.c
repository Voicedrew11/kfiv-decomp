#include "kf_model.h"
#include "kf_script.h"

int func_0011E1F0(int, int);
void func_0011E7B0(void *, int, int);
void func_0011E810(void *, int);
extern int D_00446770;

/* Starts the target map object on the two arguments of the command, then deactivates it. */
void func_00212CB8(ScriptCallCommand *command) {
    MapObject *object;

    object = (MapObject *)func_0011E1F0(D_00446770, command->target_id.value);
    if (object != NULL) {
        func_0011E7B0(object, command->first.value, command->second.value);
        func_0011E810(object, 1);
    }
    command->active = 0;
}
