#include "common.h"
#include "kf_effect.h"

void func_0019D490(s32, s32, f32 *, f32);

void func_00214580(SpawnRequestWide *request) {
    Vector4Arg position = {request->pos_x, request->pos_y, request->pos_z, 1.0f};

    func_0019D490(request->kind, request->unk10, (f32 *)&position, request->unk18);
    request->pending = 0;
}
