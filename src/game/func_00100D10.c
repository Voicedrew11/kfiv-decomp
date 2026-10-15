#include "kf_game.h"
#include "kf_map.h"
#include "kf_player.h"

extern GameStatus D_0038CC50;
extern MapSlot D_0038CD20[];
extern PlayerState D_00413F80;
extern f32 D_00431B30[4][4];
extern f32 D_00431C80[4];
extern CameraState D_004325A0;
extern int D_00446770;

MapRecordInfo *func_00115CD0(CameraState *, f32, int);
int rand(void);
float Angle_Wrap(float);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(float *, int *, float *);
void Vector_Copy(void *, void *);

/* Finds the map record the camera is in and works out this frame's two camera shake offsets. */
void func_00100D10(void) {
    f32 matrix[4][4];
    MapRecordInfo *record;
    f32 angle;

    if (D_0038CC50.unk03 != 0) {
        record = func_00115CD0(&D_004325A0, 100.0f, D_00446770);
        if (record != NULL) {
            D_00413F80.map_index = (u32)((char *)record - D_0038CD20[D_00446770].records) >> 14; // match: the byte offset of 0x80-byte records shifted by 14; 7 would give the record number
            D_00413F80.unk250 = record->unk38;
        }
    }
    if (D_004325A0.unk74 != 0 && D_004325A0.unk70 > 0.0f) {
        angle = (f32)(int)((f32)rand() * 360.0f / 2147483648.0f) * 0.017453292f;
        angle = Angle_Wrap(angle + D_004325A0.unk18);
        D_004325A0.unk50[0] = D_004325A0.unk70;
        D_004325A0.unk50[1] = 0.0f;
        D_004325A0.unk50[2] = 0.0f;
        D_004325A0.unk50[3] = 1.0f;
        sceVu0CopyMatrix(matrix, D_00431B30);
        sceVu0RotMatrixZ(matrix[0], matrix[0], angle);
        sceVu0RotMatrixX(matrix[0], matrix[0], D_004325A0.unk10);
        sceVu0RotMatrixY(matrix[0], matrix[0], D_004325A0.unk14);
        sceVu0ApplyMatrix(D_004325A0.unk50, (int *)matrix, D_004325A0.unk50);
        if (D_004325A0.unk74 > 0) {
            D_004325A0.unk74--;
        }
    } else {
        Vector_Copy(D_004325A0.unk50, D_00431C80);
    }
    if (D_004325A0.unk7C != 0 && D_004325A0.unk78 > 0.0f) {
        angle = (f32)(int)((f32)rand() * 360.0f / 2147483648.0f) * 0.017453292f;
        angle = Angle_Wrap(angle + D_004325A0.unk18);
        D_004325A0.unk60[0] = D_004325A0.unk78;
        D_004325A0.unk60[1] = 0.0f;
        D_004325A0.unk60[2] = 0.0f;
        D_004325A0.unk60[3] = 1.0f;
        sceVu0CopyMatrix(matrix, D_00431B30);
        sceVu0RotMatrixZ(matrix[0], matrix[0], angle);
        sceVu0RotMatrixX(matrix[0], matrix[0], D_004325A0.unk10);
        sceVu0RotMatrixY(matrix[0], matrix[0], D_004325A0.unk14);
        sceVu0ApplyMatrix(D_004325A0.unk60, (int *)matrix, D_004325A0.unk60);
        if (D_004325A0.unk7C > 0) {
            D_004325A0.unk7C--;
        }
    } else {
        Vector_Copy(D_004325A0.unk60, D_00431C80);
    }
}
