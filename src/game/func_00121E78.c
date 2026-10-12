#include "kf_model.h"
#include "kf_game.h"

extern GameStatus D_0038CC50;
extern void *D_00420D50[];
extern float D_00413F80[];

void func_001CFA70(int, int, int, int, int, int, ...);
void func_0011E3A0(char *);
void func_0011F148(char *);
void func_00216B48(void *, void *, void *);
void func_00216BC0(void *, void *, void *);
void func_00216778(void *, void *);
void sceVu0InversMatrix(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
float sqrtf(float);
void KF_Vector_Scale(float *, float *, float);

/* Update handler, step 1: used by the player it only shows a message; once fired
   it puts the part at a point 220 below the player and works out the steps that
   carry it back (6.67 a frame). */
void func_00121E78(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    ModelPart *part = &object->model->parts[work->part];
    f32 matrix[16];
    f32 unused[32];
    Vector4 origin;
    Vector4 target;
    f32 distance;

    if (work->timer > 0) {
        work->timer--;
    }
    if (D_0038CC50.bit1 && D_00420D50[0] == object) {
        if (work->timer > 0) {
            return;
        }
        func_001CFA70(0, 0, 0, 0, 0x2D, (s16)object->unk04->unk68);
        work->timer = 0x2D;
    } else if (work->fired != 0) {
        work->fired = 0;
        object->unk10 |= 0x20000;
        MAP_OBJECT_BYTES(object)->step = 3;
        func_0011E3A0((char *)object);
        func_0011F148((char *)object);
        func_00216778(&origin, part->unk40);
        origin.w = 1.0f;
        func_00216778(&target, D_00413F80);
        target.w = 1.0f;
        target.y -= 220.0f;
        func_00216B48((int)matrix, (int *)object->model->unk40, (int)part);
        sceVu0InversMatrix(matrix, matrix);
        func_00216BC0(&target, matrix, &target);
        KF_Vector_Subtract(work, &origin, &target);
        distance = sqrtf(work->step.x * work->step.x + work->step.y * work->step.y + work->step.z * work->step.z);
        work->frames = distance / 6.6666665f;
        func_00216778(part->unk40, &target);
        KF_Vector_Scale(&work->step.x, &work->step.x, 1.0f / work->frames);
        D_0038CC50.unk30 |= 1;
    }
}
