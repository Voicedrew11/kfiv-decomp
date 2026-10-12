#include "kf_script.h"

int func_0011E1F0(int, int);
void func_0011E810(void *, int);
extern int D_00446770;

/* Clear bit 21 of a map object of the current map. */
void func_002133C0(ScriptObjectCommand *command) {
    void *object;

    object = (void *)func_0011E1F0(D_00446770, command->object_id.value);
    if (object != NULL) {
        func_0011E810(object, 0);
    }
    command->active = 0;
}
