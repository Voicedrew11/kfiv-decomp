#include "common.h"
#include "kf_effect.h"

void func_0019D980(char, u8, u8, f32, Vector4Arg);

void func_002145F0(SpawnRequestWide *request) {
    Vector4Arg position = {request->pos_x, request->pos_y, request->pos_z, 1.0f};

    func_0019D980(request->kind, request->unk10, 0, request->unk18, position);
    request->pending = 0;
}
