#include "kf_map_object.h"
#include "kf_model.h"

extern u8 D_00349690[];
int func_001C8C80(int, int);

/* Parks or reveals the pair of scene objects 0x66/0x67 depending on the stage flags. */
void func_0011A138(int map) {
    MapObject *object;

    if (D_00349690[0xA85] != 0) {
        if (D_00349690[0xA86] == 0) {
            func_001C8C80(map, 0xC9);
            object = KF_MapObject_GetById(map, 0x67);
            if (object != NULL) {
                KF_MapObject_Activate(object);
                object->model->position.x = -19150.0f;
                object->model->position.y = -2030.0f;
                object->model->position.z = 3450.0f;
            }
        }
    } else {
        object = KF_MapObject_GetById(map, 0x66);
        if (object != NULL) {
            KF_MapObject_Deactivate(object);
        }
        object = KF_MapObject_GetById(map, 0x67);
        if (object != NULL) {
            KF_MapObject_Deactivate(object);
        }
    }
}
