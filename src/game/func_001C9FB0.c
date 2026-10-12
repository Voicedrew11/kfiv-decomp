#include "kf_model.h"
#include "kf_player.h"

float func_0019B838(float);
int func_001BC148(int);
void func_00216778(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void func_001CFA70(int, int, int, int, int, int, ...);
extern PlayerState D_00413F80;
extern u16 D_00413DAA[];
extern ItemNode *D_004469D8;
extern int D_00447198;
extern f32 D_0044719C;
extern int D_004471A4;
extern int D_004471AC;
extern int D_004471B0;
extern f32 D_005B60E0[4];
extern f32 D_005B60F0[4];

/* Pickup step 2: the item turns in front of the player until it is taken (0x4000) or put back (0x2000). */
void func_001C9FB0(void) {
    f32 delta[4];

    D_0044719C = 0.0f;
    D_004469D8->model->rotation.y = func_0019B838(D_004469D8->model->rotation.y + 0.08377580f);
    if (D_00413DAA[0] & 0x4000) {
        if (!func_001BC148((s16)D_004469D8->item)) {
            func_00216778(D_005B60E0, &D_00413F80);
            D_005B60E0[1] -= 115.0f;
            KF_Vector_Subtract(delta, D_005B60E0, &D_004469D8->model->position);
            KF_Vector_Scale(D_005B60F0, delta, 0.06666667f);
            D_004471B0 = 15;
            D_004471A4++;
            return;
        }
        func_001CFA70(0, 0, 0, 0, 0x2D, 0x1E);
    } else if (!(D_00413DAA[0] & 0x2000) || D_00447198 != 0) {
        return;
    }
    KF_Vector_Scale(D_005B60F0, D_005B60F0, -1.0f);
    D_004471B0 = D_004471AC;
    D_004471A4 += 2;
}
