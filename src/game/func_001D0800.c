#include "kf_game.h"
#include "kf_player.h"

void func_00216778(void *, void *);
void func_00216BC0(void *, void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);

extern GameStatus D_0038CC50;
extern PlayerState D_00413F80;
extern EventRunner D_00420D00;
extern CameraState D_004325A0;
extern f32 D_00431AF0[16];

/* Keep last frame's view ray, then rebuild it: from the player's eyes (or the camera, in a cutscene) along the
   direction looked in. */
void func_001D0800(void) {
    f32 rotation[16];
    f32 pitch;
    f32 heading;

    func_00216778(D_00420D00.unk20, D_00420D00.unk00);
    func_00216778(D_00420D00.unk30, D_00420D00.unk10);
    D_00420D00.unk44 = D_00420D00.unk40;
    if (D_0038CC50.unk03 == 0) {
        func_00216778(D_00420D00.unk00, &D_00413F80);
        D_00420D00.unk00[1] = D_00420D00.unk00[1] - 220.0f + D_00413F80.unk1F0 + D_004325A0.unk4C;
        pitch = D_00413F80.rotation.x - D_00413F80.unk238;
        if (pitch > 1.012291f) {
            pitch = 1.012291f;
        } else if (pitch < -1.012291f) {
            pitch = -1.012291f;
        }
        heading = D_00413F80.rotation.y;
    } else {
        func_00216778(D_00420D00.unk00, &D_004325A0);
        pitch = D_004325A0.unk10;
        heading = D_004325A0.unk14;
    }
    D_00420D00.unk10[2] = 1.0f;
    D_00420D00.unk10[0] = 0.0f;
    D_00420D00.unk10[1] = 0.0f;
    D_00420D00.unk10[3] = 0.0f;
    sceVu0RotMatrixX(rotation, D_00431AF0, pitch);
    func_00216BC0(D_00420D00.unk10, rotation, D_00420D00.unk10);
    sceVu0RotMatrixY(rotation, D_00431AF0, heading);
    func_00216BC0(D_00420D00.unk10, rotation, D_00420D00.unk10);
}
