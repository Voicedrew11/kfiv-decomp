#include "kf_map.h"
#include "kf_game.h"

MapRecordInfo *func_00115CD0(CameraState *, f32, int);
void func_0010A698(void);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void KF_Vector_Scale(float *, float *, float);
void func_0020C300(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void func_0019AB60(ModelInstance *);
void func_0010AEB8(void *);
void func_0010DB40(ModelInstance *);
void func_00109E18(void);
extern MapDrawState D_0038A1B0;
extern MapSlot D_0038CD20[];
extern f32 D_00431AF0[16];
extern ItemNode *D_004469D8;
extern f32 D_0044719C;

/* an angle brought back into -pi..pi (for angles above -pi) */
static inline f32 wrap_angle(f32 angle) {
    angle += 3.1415927f;
    return angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
}

/* Draws the item being picked up, over everything else: each part's own offset and turn are scaled by
   D_0044719C, so that the model folds flat while it is held up. Nothing is drawn when hidden is set. */
void func_001C97E8(int hidden) {
    f32 scale[4];
    f32 offset[4];
    f32 turn[4];
    ItemNode *node = D_004469D8;
    ModelPart *part;
    s32 count;
    s32 i;

    if (node == NULL || hidden != 0) {
        return;
    }
    node->map_record = func_00115CD0((CameraState *)&node->model->position, 0.0f, node->flags & 1);
    if (node->map_record == NULL) {
        return;
    }
    i = 0;
    func_0010A698();
    func_00109A50();
    func_00109A80(0x47, 0x5000D);
    func_00109AD0();
    KF_Vector_Scale(scale, &node->model->unkE0, 1.0f);
    func_0020C300(node->model, &node->model->position, &node->model->rotation, scale);
    count = node->model->part_count;
    part = node->model->parts;
    for (; i < count; i++, part++) {
        KF_Vector_Scale(offset, part->unk40, D_0044719C);
        KF_Vector_Scale(turn, &part->unk50, D_0044719C);
        sceVu0CopyMatrix(part, D_00431AF0);
        PART_MATRIX(part)[0][0] = part->unk60[0];
        PART_MATRIX(part)[1][1] = part->unk60[1];
        PART_MATRIX(part)[2][2] = part->unk60[2];
        sceVu0RotMatrixX((f32 *)part, (f32 *)part, wrap_angle(turn[0]));
        sceVu0RotMatrixZ((f32 *)part, (f32 *)part, wrap_angle(turn[2]));
        sceVu0RotMatrixY((f32 *)part, (f32 *)part, wrap_angle(turn[1]));
        sceVu0TransMatrix((f32 *)part, (f32 *)part, offset);
    }
    node->model->unk148 = (s32)&D_0038A1B0.unkA90[node->flags & 1][D_0038CD20[node->flags & 1].header->unk71];
    node->model->unk14C = &D_0038A1B0.unk290[node->flags & 1][((MapRecordInfo *)node->map_record)->unk34];
    func_0019AB60(node->model);
    func_0010AEB8(node->model);
    func_0010DB40(node->model);
    func_00109E18();
}
