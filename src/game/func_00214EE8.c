#include "kf_script.h"

void sceVu0RotMatrixY(float, int *, int *);
void sceVu0ApplyMatrix(float *, int *, float *);
void KF_Vector_Scale(void *, void *, float);
void func_00216C48(void *, void *, void *);
int func_00211D90(void *, double);  // match: retail converts the float argument to double at the call site (fptodp), as the definition takes float
extern int D_00431AF0[];
extern float D_00413F80[];

/* Rotates the vector at D_00413F80+0x1AC about Y, scales it, and stores the three components as ints. */
void func_00214EE8(ScriptPointCommand *command) {
    int matrix[16];
    float vec[4];
    float out[4];

    vec[0] = D_00413F80[0x1B0 / 4];
    vec[1] = D_00413F80[0x1AC / 4];
    vec[2] = D_00413F80[0x1B4 / 4];
    vec[3] = 0.0f;
    sceVu0RotMatrixY(D_00413F80[0x14 / 4], matrix, D_00431AF0);
    sceVu0ApplyMatrix(vec, matrix, vec);
    KF_Vector_Scale(vec, vec, (float)command->object_id.value);
    func_00216C48(out, D_00413F80, vec);
    func_00211D90(&command->from_x, out[0]);
    func_00211D90(&command->from_y, out[1]);
    func_00211D90(&command->from_z, out[2]);
    command->active = 0;
}
