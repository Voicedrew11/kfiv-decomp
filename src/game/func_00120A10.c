#include "kf_model.h"

s16 func_0019A928(void *, Vector4 *, int, f32, int, int, int);
int rand(void);

/* Start handler of the light kind: makes a light of the colour in its source at
   a point of the model, and for the flickering sorts (0, 1) rolls the two
   flicker periods and where in them it starts. */
void func_00120A10(MapObject *object) {
    s16 *work = (s16 *)object->work;
    Vector4 colour;
    MapObjectUnk08Words *source = MAP_OBJECT_WORDS(object);
    void *point;
    int sort;
    MapObjectUnk08Words *params;

    colour.x = source->unk44;
    colour.y = source->unk46;
    colour.z = source->unk48;
    colour.w = 1.0f;
    point = (void *)object->model->get_point(object->model, source->unk52);
    params = MAP_OBJECT_WORDS(object);
    work[0] = func_0019A928(point, &colour, 0, params->unk4A, MAP_OBJECT_BYTES(object)->unk00 & 1, params->unk4C != 0 ? params->unk4E : -1, 1);
    sort = MAP_OBJECT_WORDS(object)->unk50;
    if (sort >= 0) {
        if (sort <= 1) {
            work[2] = (int)(rand() * 6.0f * 4.656613e-10f) + 0x14;
            work[1] = work[2] * (f32)rand() * 4.656613e-10f;
            work[4] = (int)(rand() * 6.0f * 4.656613e-10f) + 5;
            work[3] = work[4] * (f32)rand() * 4.656613e-10f;
        }
    }
    MAP_OBJECT_BYTES(object)->step = 1;
}
