#include "kf_map_object.h"
#include "kf_model.h"
#include "kf_game.h"
#include "kf_message.h"

extern GameStatus D_0038CC50;
extern void *D_00420D50[];
extern float D_00413F80[];

void func_0011F148(char *);
void Matrix_Multiply(void *, void *, void *);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Copy(void *, void *);
void sceVu0InversMatrix(void *, void *);
void Vector_Subtract(void *, void *, void *);
float sqrtf(float);
void Vector_Scale(float *, float *, float);

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
        Message_ShowNotice(0, 0, 0, 0, 0x2D, (s16)object->unk04->unk68);
        work->timer = 0x2D;
    } else if (work->fired != 0) {
        work->fired = 0;
        object->unk10 |= 0x20000;
        MAP_OBJECT_BYTES(object)->step = 3;
        MapObject_StepAnim(object);
        func_0011F148((char *)object);
        Vector_Copy(&origin, part->unk40);
        origin.w = 1.0f;
        Vector_Copy(&target, D_00413F80);
        target.w = 1.0f;
        target.y -= 220.0f;
        Matrix_Multiply(matrix, object->model->unk40, part);
        sceVu0InversMatrix(matrix, matrix);
        Vector_ApplyMatrix(&target, matrix, &target);
        Vector_Subtract(work, &origin, &target);
        distance = sqrtf(work->step.x * work->step.x + work->step.y * work->step.y + work->step.z * work->step.z);
        work->frames = distance / 6.6666665f;
        Vector_Copy(part->unk40, &target);
        Vector_Scale(&work->step.x, &work->step.x, 1.0f / work->frames);
        D_0038CC50.unk30 |= 1;
    }
}
