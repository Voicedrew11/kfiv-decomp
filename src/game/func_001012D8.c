#include "kf_game.h"
#include "kf_map.h"

extern MapDrawStateMirror D_0038A1B0;
extern f32 D_00431AF0[16];      /* the unit matrix */
extern f32 D_004322A0[16];      /* view to screen */
extern f32 D_00432320[16];      /* world to view */
extern f32 D_00432360[16];      /* the camera's rotation, inverted */
extern f32 D_004323E0[16];      /* world to screen */
extern CameraState D_004325A0;

void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0InversMatrix(void *, void *);
void sceVu0MulMatrix(void *, void *, void *);
void sceVu0CopyMatrix(void *, void *);
float Angle_Wrap(float);

/* Builds the camera matrices: the projection, world to view, and world to screen for the given mode. */
void func_001012D8(int projection, int mode) {
    f32 local[16];
    f32 other[16];
    f32 translation[4];
    f32 angle;

    switch (projection) {
    case 0:
        sceVu0ViewScreenMatrix(D_004322A0, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 1.0f, 1048560.0f, 1.0f, 65535.0f);
        break;
    case 1:
        sceVu0ViewScreenMatrix(D_004322A0, 512.0f, 0.4f, 0.5714286f, 2048.0f, 2048.0f, 1.0f, 1048560.0f, 1.0f,
                               65535.0f);
        break;
    }
    translation[0] = -D_004325A0.position[0];
    translation[1] = -D_004325A0.position[1];
    translation[2] = -D_004325A0.position[2];
    translation[3] = 0.0f;
    sceVu0TransMatrix(D_00432320, D_00431AF0, translation);
    sceVu0RotMatrixY(D_00432320, D_00432320, -D_004325A0.unk14);
    sceVu0RotMatrixX(D_00432320, D_00432320, -D_004325A0.unk10);
    sceVu0RotMatrixY(D_00432360, D_00431AF0, Angle_Wrap(3.1415927f - D_004325A0.unk14));
    sceVu0RotMatrixX(D_00432360, D_00432360, Angle_Wrap(-D_004325A0.unk10));
    sceVu0RotMatrixZ(D_00432360, D_00432360, D_004325A0.unk18);
    sceVu0InversMatrix(D_00432360, D_00432360);
    switch (mode) {
    case 0:
        sceVu0MulMatrix(D_004323E0, D_004322A0, D_00432320);
        break;
    case 1:
        translation[1] = D_0038A1B0.mirror_height;
        translation[0] = translation[2] = translation[3] = 0.0f;
        sceVu0TransMatrix(local, D_00431AF0, translation);
        sceVu0CopyMatrix(other, D_00431AF0);
        other[5] = -1.0f;
        sceVu0MulMatrix(local, local, other);
        translation[1] = -D_0038A1B0.mirror_height;
        sceVu0TransMatrix(other, D_00431AF0, translation);
        sceVu0MulMatrix(local, local, other);
        sceVu0MulMatrix(D_004323E0, D_004322A0, D_00432320);
        sceVu0MulMatrix(D_004323E0, D_004323E0, local);
        break;
    case 2:
        translation[1] = 220.0f;
        translation[0] = translation[2] = translation[3] = angle = 0.0f;
        sceVu0TransMatrix(local, D_00431AF0, translation);
        sceVu0RotMatrixY(local, local, angle);
        sceVu0RotMatrixX(local, local, angle);
        sceVu0MulMatrix(D_004323E0, D_004322A0, local);
        break;
    }
}
