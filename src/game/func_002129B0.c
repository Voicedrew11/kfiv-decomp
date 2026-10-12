#include "kf_model.h"
#include "kf_script.h"

int func_0011E1F0(int, int);
void func_0011E140(void *);
extern int D_00446770;

/* Runs func_0011E140 on the map object, then finishes. */
void func_002129B0(ScriptObjectCommand *command) {
    MapObject *object;

    object = (MapObject *)func_0011E1F0(D_00446770, command->object_id.value);
    if (object != NULL) {
        func_0011E140(object);
    }
    command->active = 0;
}
