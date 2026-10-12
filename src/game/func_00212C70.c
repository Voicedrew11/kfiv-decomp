#include "kf_model.h"
#include "kf_script.h"

int func_0011E1F0(int, int);
void func_0011E758(MapObject *, int);
extern int D_00446770;

/* Runs func_0011E758 on the map object with the first argument, then finishes. */
void func_00212C70(ScriptCallCommand *command) {
    MapObject *object;

    object = (MapObject *)func_0011E1F0(D_00446770, command->target_id.value);
    if (object != NULL) {
        func_0011E758(object, command->first.value);
    }
    command->active = 0;
}
