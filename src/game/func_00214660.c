#include "kf_effect.h"

void func_0019D980(char, u8, u8, f32, Vector4Arg);

void func_00214660(SpawnRequest *request) {
    Vector4Arg position = {request->pos_x, request->pos_y, request->pos_z, 1.0f};

    func_0019D980(request->kind, request->unk18, request->unk10, request->unk20, position);
    request->pending = 0;
}
