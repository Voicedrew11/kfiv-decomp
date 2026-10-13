#include "kf_map_object.h"
#include "kf_script.h"

int func_00211CF8(ScriptArg *, int);
extern int D_00446770;

/* Reads the four parameters of a kind 9 map object's source into the arguments. */
void func_00212F70(ScriptObjectValuesCommand *command) {
    MapObject *object;

    object = KF_MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL && object->model != NULL && object->unk04->kind == 9) {
        func_00211CF8(&command->values[0], MAP_OBJECT_WORDS(object)->unk44);
        func_00211CF8(&command->values[1], MAP_OBJECT_WORDS(object)->unk46);
        func_00211CF8(&command->values[2], MAP_OBJECT_WORDS(object)->unk48);
        func_00211CF8(&command->values[3], MAP_OBJECT_WORDS(object)->unk4A);
    }
    command->active = 0;
}
