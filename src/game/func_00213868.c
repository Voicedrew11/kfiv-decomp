#include "kf_script.h"
#include "kf_model.h"

extern char D_0038CD20[];
extern int D_00446770;

int func_001A1C58(int, void *, void *, void *);

void func_00213868(ScriptPointCommand *command) {
    VuVector4 out_a = (VuVector4){ command->from_x.value, command->from_y.value, command->from_z.value, 0.0f };
    VuVector4 out_b = (VuVector4){ command->to_x.value, command->to_y.value, command->to_z.value, 0.0f };

    func_001A1C58(command->object_id.value, &out_a, &out_b, D_0038CD20 + D_00446770 * 0x60);
    command->active = 0;
}
