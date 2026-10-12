#include "kf_model.h"
#include "kf_script.h"

int func_0011E1F0(int, int);
void func_0011E810(void *, int);
extern int D_00446770;

/* Sets bit 21 of the map object if it is waiting, then finishes. */
void func_00213380(ScriptObjectCommand *command) {
    MapObject *object;

    object = (MapObject *)func_0011E1F0(D_00446770, command->object_id.value);
    if (object != NULL) {
        func_0011E810(object, 1);
    }
    command->active = 0;
}
