#include "kf_collision.h"
#include "kf_map.h"
#include "kf_effect.h"
#include "kf_map_object.h"

extern u8 D_0034B696[];
extern MapSlot D_0038CD20[];

void *func_00116160(int, int);

static inline void hide_record(int map, int record_id) {
    MapRecordBounds *record = func_00116160(map, record_id);

    if (record != NULL) {
        record->unk3C |= 0x40000000;
    }
}

static inline void show_record(int map, int record_id) {
    MapRecordBounds *record = func_00116160(map, record_id);

    if (record != NULL) {
        record->unk3C &= ~0x40000000;
    }
}

/* Shows the map records that go with the stage flag; at 2 also moves object 0x66 and spawns effect 0x1DF. */
void func_00119B20(int map) {
    MapObject *object;
    f32 position[4];
    f32 rotation[4];

    if (D_0034B696[0] != 2) {
        hide_record(map, 0x42);
        hide_record(map, 0x43);
        hide_record(map, 0x44);
        show_record(map, 0x3F);
        show_record(map, 0x41);
    } else {
        show_record(map, 0x42);
        show_record(map, 0x43);
        show_record(map, 0x44);
        hide_record(map, 0x3F);
        hide_record(map, 0x41);
        object = KF_MapObject_GetById(map, 0x66);
        if (object != NULL) {
            object->model->position.x = -5887.0f;
            object->model->position.y = -790.0f;
            object->model->position.z = -1157.0f;
        }
        position[0] = -5887.0f;
        position[1] = -910.0f;
        position[2] = -1157.0f;
        position[3] = 0.0f;
        rotation[1] = rotation[0] = 3.14f;
        rotation[2] = 0.0f;
        rotation[3] = 0.0f;
        KF_Effect_StartSpawner(0x1DF, position, rotation, &D_0038CD20[map]);
    }
    object = KF_MapObject_GetById(map, 0x67);
    if (object != NULL) {
        object->model->unk150 = object->model->unk154 = object->model->unk158 = 255.0f;
    }
}
