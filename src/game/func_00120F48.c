#include "kf_model.h"
#include "kf_game.h"

extern GameStatus D_0038CC50;
extern void *D_00420D50[];
extern float D_00413F80[];
extern f32 D_00431AF0[16];

void func_001CFA70(int, int, int, int, int, int, ...);
void func_0011E3A0(char *);
void func_0011F148(char *);
void func_00216BC0(void *, void *, void *);
void func_00216778(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
float sqrtf(float);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void KF_Vector_Scale(float *, float *, float);

/* Update handler of a sliding kind, step 1: used by the player it only shows a
   message; once fired (func_00121620) it puts the part at a point 115 below the
   player and works out the steps that carry it back (6.67 a frame). */
void func_00120F48(MapObject *object) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    ModelPart *part = &object->model->parts[work->part];
    f32 matrix[16];
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
        MAP_OBJECT_BYTES(object)->step = 2;
        func_0011E3A0((char *)object);
        func_0011F148((char *)object);
        origin.w = 1.0f;
        origin.x = origin.y = origin.z = 0.0f;
        func_00216BC0(&origin, part, &origin);
        func_00216BC0(&origin, object->model->unk40, &origin);
        func_00216778(&target, D_00413F80);
        target.y -= 115.0f;
        KF_Vector_Subtract(work, &origin, &target);
        sceVu0RotMatrixX(matrix, D_00431AF0, object->model->rotation.x);
        sceVu0RotMatrixY(matrix, D_00431AF0, -object->model->rotation.y);
        func_00216BC0(work, matrix, work);
        distance = sqrtf(work->step.x * work->step.x + work->step.y * work->step.y + work->step.z * work->step.z);
        work->frames = distance / 6.6666665f;
        KF_Vector_Subtract(part->unk40, part->unk40, work);
        KF_Vector_Scale(&work->step.x, &work->step.x, 1.0f / work->frames);
        D_0038CC50.unk30 |= 1;
    }
}
