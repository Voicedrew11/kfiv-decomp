#include "kf_script.h"
#include "kf_map.h"

extern MapDrawStateOverlays D_0038A1B0;

/* Set MapDrawState.unk68. */
void func_00214B70(ScriptObjectCommand *command) {
    D_0038A1B0.unk68 = command->object_id.value;
    command->active = 0;
}
