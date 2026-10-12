#include "kf_script.h"
#include "kf_model.h"

void func_0019DAA8(char, u8, u8, VuVector4 *, VuVector4 *, float);

void func_00214700(ScriptTwoPointCommand *command) {
    VuVector4 from = (VuVector4){ command->from_x.value, command->from_y.value, command->from_z.value, 1.0f };
    VuVector4 to = (VuVector4){ command->to_x.value, command->to_y.value, command->to_z.value, 1.0f };

    func_0019DAA8(command->unk04.value, command->unk0C.value, 0, &from, &to, command->unk14.value);
    command->active = 0;
}
