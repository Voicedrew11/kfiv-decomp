#include "kf_script.h"
#include "kf_collision.h"
#include "kf_model.h"

MapProp *func_00116160(int, int);
void func_00115550(MapRecordBounds *, f32 *);
extern int D_00446770;

/* Move the target prop by the x/y/z of the command every frame for `frames` frames. */
void func_00213FF8(ScriptMoveCommand *command) {
    MapProp *prop = func_00116160(D_00446770, command->target_id.value);
    VuVector4 offset = (VuVector4){ command->x.value, command->y.value, command->z.value, 0.0f };

    if (prop != NULL && command->frames.value > 0) {
        func_00115550((MapRecordBounds *)prop, &offset.x);
        if (--command->frames.value < 0) {
            command->active = 0;
        }
    } else {
        command->active = 0;
    }
}
