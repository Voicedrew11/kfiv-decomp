#include "kf_collision.h"
#include "kf_entity.h"
#include "kf_map.h"

extern MapDrawState D_0038A1B0;
extern u8 D_004141D0[];
extern f32 D_004323E0[16];
extern f32 D_00431AF0[16];
extern int D_00446770;

void Vu0_LoadMatrix(void *);
void Vu0_ProjectToScreen(int *, void *);
int func_00116C08(int, int, int, int);
void Vu0_ProjectToScreen3(int *, int *, int *, f32 *, f32 *, f32 *);
void sceVu0MulMatrix(void *, void *, void *);

/* Tests the corners of a map record's box against the screen: 1 when it has to be drawn whole (or the test is
   off), -1 when all eight corners are outside, 0 otherwise. The model is not used. */
int func_00116CB0(MapDrawRecord *draw_record, ModelInstance *model) {
    MapRecord *record = (MapRecord *)draw_record;
    MapRecordBounds *bounds = (MapRecordBounds *)draw_record;
    f32 matrix[16];
    f32 corner0[4];
    f32 corner1[4];
    f32 corner2[4];
    int screen0[4];
    int screen1[4];
    int screen2[4];
    int outside;

    if (!MAP_DRAW_FLAG_BITS(&D_0038A1B0)->bit46) {
        return 1;
    }
    if (bounds->unk3C & 0x40) {
        return 1;
    }
    if (bounds->unk30 == 0xFFFF) {
        return 1;
    }
    if (func_00116C08(D_00446770, D_004141D0[0], (u8)(record->unk38 >> 27) & 1, (u8)record->unk38) == 0) {
        return 1;
    }
    sceVu0MulMatrix(matrix, D_004323E0, D_00431AF0);
    Vu0_LoadMatrix(matrix);
    corner2[3] = 1.0f;
    corner1[3] = 1.0f;
    corner0[3] = 1.0f;
    corner0[0] = bounds->unk40[0];
    corner0[1] = bounds->unk40[1];
    corner0[2] = bounds->unk40[2];
    corner1[0] = bounds->unk40[0];
    corner1[1] = bounds->unk40[1];
    corner1[2] = bounds->unk50[2];
    corner2[0] = bounds->unk40[0];
    corner2[1] = bounds->unk50[1];
    corner2[2] = bounds->unk40[2];
    Vu0_ProjectToScreen3(screen0, screen1, screen2, corner0, corner1, corner2);
    outside = (u32)screen0[2] > 0xFFFF0;
    outside += (u32)screen1[2] > 0xFFFF0;
    outside += (u32)screen2[2] > 0xFFFF0;
    corner0[0] = bounds->unk40[0];
    corner0[1] = bounds->unk50[1];
    corner0[2] = bounds->unk50[2];
    corner1[0] = bounds->unk50[0];
    corner1[1] = bounds->unk40[1];
    corner1[2] = bounds->unk40[2];
    corner2[0] = bounds->unk50[0];
    corner2[1] = bounds->unk40[1];
    corner2[2] = bounds->unk50[2];
    Vu0_ProjectToScreen3(screen0, screen1, screen2, corner0, corner1, corner2);
    outside += (u32)screen0[2] > 0xFFFF0;
    outside += (u32)screen1[2] > 0xFFFF0;
    outside += (u32)screen2[2] > 0xFFFF0;
    corner0[0] = bounds->unk50[0];
    corner0[1] = bounds->unk50[1];
    corner0[2] = bounds->unk40[2];
    Vu0_ProjectToScreen(screen0, corner0);
    outside += (u32)screen0[2] > 0xFFFF0;
    corner0[0] = bounds->unk50[0];
    corner0[1] = bounds->unk50[1];
    corner0[2] = bounds->unk50[2];
    Vu0_ProjectToScreen(screen0, corner0);
    outside += (u32)screen0[2] > 0xFFFF0;
    if (outside == 0) {
        return 1;
    }
    if (outside == 8) {
        return -1;
    }
    return 0;
}
