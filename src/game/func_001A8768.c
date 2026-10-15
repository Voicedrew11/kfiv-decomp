#include "common.h"
#include "kf_effect.h"

extern float D_00413F80[];
extern f32 D_00431AF0[16];

s16 func_001B0208(float, void *, void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(void *, void *, void *);
void Vector_Add(void *, void *, void *);
void Vector_Copy(void *, void *);

/* Per-frame update: aims the model along D_00413F80 and sets its turn angle. */
void func_001A8768(EffectActor *effect, EffectSlot *spawn_slot) {
    AngleAimMotion *motion;
    EffectDef *def;
    float *angles;
    f32 rotation_matrix[16];
    Vector4 offset;

    EffectStart_TurnModel(effect, spawn_slot);
    motion = (AngleAimMotion *)&effect->motion;
    EffectStart_PlaceModel(effect, spawn_slot);
    def = effect->def;
    if (def->unkE8 != 0.0f) {
        angles = D_00413F80;
        offset.x = def->offset_base[0];
        offset.y = def->offset_base[1];
        offset.z = def->offset_base[2];
        offset.w = 0.0f;
        sceVu0RotMatrixX(rotation_matrix, D_00431AF0, angles[4]);
        sceVu0RotMatrixY(rotation_matrix, rotation_matrix, angles[5]);
        sceVu0ApplyMatrix(&offset, rotation_matrix, &offset);
        Vector_Add(&effect->model->position, angles, &offset);
        Vector_Copy(&effect->model->rotation, &angles[4]);
        effect->model->position.y -= 220.0f;
    }
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->angle = func_001B0208(30.0f, effect, motion, motion->unk10);
    motion->unk20 = 0;
    motion->unk28 = 0;
    motion->unk26 = 0;
}
