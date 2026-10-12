#include "kf_map.h"

extern void *D_00414188[];

/* Update handler of the kind of func_00121660 (a platform): it rises while the
   player stands on it, after a wait of unk4E frames, and sinks back when he does
   not. unk46 and unk48 are the speeds up and down, or accelerations when unk4A
   and unk4C are set; unk44 is how far it rises. */
void func_00121680(MapObject *object) {
    MapTriggerShotKind121660 *shot = (MapTriggerShotKind121660 *)object->work;
    MapObjectUnk08Words *source;
    ModelInstance *model;
    f32 top;

    shot->unk0C = shot->unk08;
    shot->unk08 = D_00414188[0] == object;
    if (shot->unk08 != shot->unk0C) {
        shot->unk04 = MAP_OBJECT_WORDS(object)->unk4E;
    }
    switch (shot->unk08) {
        case 0:
            source = MAP_OBJECT_WORDS(object);
            if (source->unk4C == 0) {
                ((MapTriggerShotKind121660 *)object->work)->unk00 = source->unk48;
            } else {
                ((MapTriggerShotKind121660 *)object->work)->unk00 -= source->unk48;
            }
            model = object->model;
            model->position.y -= shot->unk00;
            if (model->position.y < object->unk08->position.y) {
                model->position.y = object->unk08->position.y;
            }
            break;
        case 1:
            if (shot->unk04 > 0) {
                shot->unk04--;
                return;
            }
            source = MAP_OBJECT_WORDS(object);
            if (source->unk4A == 0) {
                ((MapTriggerShotKind121660 *)object->work)->unk00 = source->unk46;
            } else {
                ((MapTriggerShotKind121660 *)object->work)->unk00 += source->unk46;
            }
            model = object->model;
            model->position.y += shot->unk00;
            top = object->unk08->position.y + source->unk44;
            if (top < model->position.y) {
                model->position.y = top;
            }
            break;
    }
}
