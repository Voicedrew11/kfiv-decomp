#include "kf_entity.h"

extern u32 D_00446ED4;
extern float D_00413F80[];

void KF_Vector_Subtract(void *, void *, void *);
float sqrtf(float);
float func_0019B838(float);
void *func_001B1390();
void func_001B4F30(u8, void *, void *, void *, void *);

/* Update handler of the shooting kind: every so many frames (or all the time
   when its model is animated), while it is on and the player is in range, it
   launches something from a point of the model, turned by two angles of its source. */
void func_00121968(MapObject *object) {
    Vector4 aim;
    f32 distance;
    void *point;
    EntityUnk1B1390 *shot;

    if (!(object->model->unk100 & 1)) {
        if (D_00446ED4 % MAP_OBJECT_WORDS(object)->unk46 != 0) {
            return;
        }
    }
    if (!(object->unk10 & 0x20000)) {
        return;
    }
    KF_Vector_Subtract(&aim, &object->model->position, D_00413F80);
    distance = sqrtf(aim.x * aim.x + aim.y * aim.y + aim.z * aim.z);
    if (MAP_OBJECT_WORDS(object)->unk4E < distance) {
        return;
    }
    point = (void *)object->model->get_point(object->model, MAP_OBJECT_WORDS(object)->unk48);
    aim.x = func_0019B838(object->model->rotation.x + MAP_OBJECT_WORDS(object)->unk4A * 0.017453292f);
    aim.y = func_0019B838(object->model->rotation.y + MAP_OBJECT_WORDS(object)->unk4C * 0.017453292f + 3.1415927f);
    aim.z = object->model->rotation.z;
    aim.w = 0.0f;
    shot = func_001B1390();
    shot->unk20 = 0;
    shot->unk00 = 1;
    shot->unk22 = 0;
    // match: the id is passed as 16 bits
    ((void (*)(u16, void *, void *, void *, void *))func_001B4F30)(MAP_OBJECT_WORDS(object)->unk44, shot, point, &aim, object);
}
