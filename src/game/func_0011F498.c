#include "kf_map_object.h"
#include "kf_model.h"

void Vector_Subtract(void *, void *, void *);
float sinf(float);
float cosf(float);
float sqrtf(float);
void func_00216C48(void *, void *, void *);
void func_00216778(void *, void *);

/* Update handler of an opening door-like object: runs the animation (kind 6
   moves with its second part; kind 2 clears unk60 bit 0 once both pairs of its
   points are more than 225 apart); at its end the object is open. */
void func_0011F498(MapObject *object) {
    MapObjectWork *work = MAP_OBJECT_WORK(object);
    Vector4 delta;
    Vector4 offset;
    f32 sine;
    f32 cosine;
    f32 *point_a;
    f32 *point_b;
    f32 *point_c;
    f32 *point_d;
    f32 gap_ad;
    f32 gap_bc;
    f32 delta_x;
    f32 delta_z;
    u8 kind;

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->param != 0) {
        ((int (*)(char *))MapObject_StepAnim)((char *)object); // match: it returns a value
        kind = object->unk04->kind;
        if (kind == 6) {
            Vector_Subtract(&delta, object->model->parts[1].unk40, work);
            sine = sinf(-object->model->rotation.y);
            cosine = cosf(-object->model->rotation.y);
            offset.y = delta.y;
            offset.x = delta.x * cosine - delta.z * sine;
            offset.z = delta.x * sine + delta.z * cosine;
            func_00216C48(&object->model->position, &object->model->position, &offset);
            func_00216778(work, object->model->parts[1].unk40);
        } else if (kind == 2) {
            if (object->unk60 & 1) {
                object->model->unk1B4(object->model);
                point_a = (f32 *)object->model->get_point(object->model, 2);
                point_b = (f32 *)object->model->get_point(object->model, 3);
                point_c = (f32 *)object->model->get_point(object->model, 4);
                point_d = (f32 *)object->model->get_point(object->model, 5);
                delta_x = point_a[0] - point_d[0];
                delta_z = point_a[2] - point_d[2];
                gap_ad = sqrtf(delta_x * delta_x + delta_z * delta_z);
                delta_x = point_b[0] - point_c[0];
                delta_z = point_b[2] - point_c[2];
                gap_bc = sqrtf(delta_x * delta_x + delta_z * delta_z);
                if (gap_ad > 225.0f && gap_bc > 225.0f) {
                    MAP_OBJECT_UNK60_BITS(object)->bit0 = FALSE;
                }
            }
        }
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}
