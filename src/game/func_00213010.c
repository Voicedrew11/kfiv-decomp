#include "kf_map_object.h"
#include "kf_script.h"

extern int D_00446770;

/* Sets the four parameters of a kind 9 map object's source. */
void func_00213010(ScriptObjectShortsCommand *command) {
    MapObject *object;
    MapObjectUnk08Words *source;

    object = MapObject_GetById(D_00446770, command->target_id.value);
    if (object != NULL && object->model != NULL && object->unk04->kind == 9) {
        source = MAP_OBJECT_WORDS(object);
        source->unk44 = command->values[0].value;
        source->unk46 = command->values[1].value;
        source->unk48 = command->values[2].value;
        source->unk4A = command->values[3].value;
    }
    command->active = 0;
}
