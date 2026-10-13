#include "kf_map_object.h"
#include "kf_model.h"

void Vector_Subtract(void *, void *, void *);
float sinf(float);
float cosf(float);
void func_00216C48(void *, void *, void *);
void func_00216778(void *, void *);

/* Update handler of a closing door-like object: runs the animation (kind 6 moves
   with its second part); at its end the object is closed again. */
void func_0011F7E0(MapObject *object) {
    MapObjectWork *work = MAP_OBJECT_WORK(object);
    Vector4 delta;
    Vector4 offset;
    f32 sine;
    f32 cosine;

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->param != 0) {
        ((int (*)(char *))MapObject_StepAnim)((char *)object); // match: it returns a value
        if (object->unk04->kind == 6) {
            Vector_Subtract(&delta, object->model->parts[1].unk40, work);
            sine = sinf(-object->model->rotation.y);
            cosine = cosf(-object->model->rotation.y);
            offset.y = delta.y;
            offset.x = delta.x * cosine - delta.z * sine;
            offset.z = delta.x * sine + delta.z * cosine;
            func_00216C48(&object->model->position, &object->model->position, &offset);
            func_00216778(work, object->model->parts[1].unk40);
        }
    } else {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        func_00216778(&object->model->position, object->unk08);
    }
}
