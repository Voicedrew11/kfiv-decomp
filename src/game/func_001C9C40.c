#include "kf_model.h"
#include "kf_game.h"
#include "kf_player.h"

void func_001B9478(void);
void func_00216778(void *, void *);
float func_0019B838(float);
float sinf(float);
float cosf(float);
float atan2f(float, float);
double sqrt(double);
void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
extern GameStatus D_0038CC50;
extern PlayerState D_00413F80;
extern EventRunner D_00420D00;
extern ItemNode *D_004469D8;
extern f32 D_0044719C;
extern int D_004471A4;
extern f32 D_004471A8;
extern int D_004471AC;
extern int D_004471B0;
extern f32 D_005B60C0[4];
extern f32 D_005B60D0[4];
extern f32 D_005B60E0[4];
extern f32 D_005B60F0[4];

/* Pickup step 0: remembers where the item lies and aims it at a point in front of the player's eyes, five units
   a frame when it is near and in fifteen frames when it is not. */
void func_001C9C40(void) {
    f32 delta[4];
    f32 distance;

    if (D_0038CC50.unk30 != 0) {
        return;
    }
    D_0038CC50.unk30 = 0x87F;
    func_001B9478();
    func_00216778(D_005B60C0, &D_004469D8->model->position);
    func_00216778(D_005B60D0, &D_004469D8->model->rotation);
    D_005B60D0[1] = func_0019B838(D_005B60D0[1]);
    D_005B60E0[0] = D_00420D00.unk00[0] + sinf(D_00413F80.rotation.y) * 150.0f;
    D_005B60E0[2] = D_00420D00.unk00[2] + cosf(D_00413F80.rotation.y) * 150.0f;
    D_005B60E0[1] = D_004469D8->model->position.y - 100.0f;
    if (D_005B60E0[1] < D_00420D00.unk00[1]) {
        D_005B60E0[1] = D_00420D00.unk00[1];
    }
    D_005B60E0[3] = 0.0f;
    D_004471A8 = atan2f(D_00420D00.unk00[1] - D_005B60E0[1], 150.0f);
    KF_Vector_Subtract(delta, D_005B60E0, D_005B60C0);
    distance = sqrt(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
    if (distance < 75.0f) {
        KF_Vector_Scale(D_005B60F0, delta, 1.0f / distance);
        KF_Vector_Scale(D_005B60F0, D_005B60F0, 5.0f);
        D_004471AC = distance / 5.0f;
    } else {
        KF_Vector_Scale(D_005B60F0, delta, 0.06666667f);
        D_004471AC = 15;
    }
    D_0044719C = 1.0f;
    D_004471B0 = D_004471AC;
    D_004471A4++;
}
