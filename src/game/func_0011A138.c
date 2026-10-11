#include "kf_model.h"

extern u8 D_00349690[];
void func_0011E0C8(MapObject *);
void func_0011E140(MapObject *);
MapObject *func_0011E1F0(int, int);
void func_001C8C80(int, int);

/* Parks or reveals the pair of scene objects 0x66/0x67 depending on the stage flags. */
void func_0011A138(int map) {
    MapObject *object;

    if (D_00349690[0xA85] != 0) {
        if (D_00349690[0xA86] == 0) {
            func_001C8C80(map, 0xC9);
            object = func_0011E1F0(map, 0x67);
            if (object != NULL) {
                func_0011E0C8(object);
                object->model->position.x = -19150.0f;
                object->model->position.y = -2030.0f;
                object->model->position.z = 3450.0f;
            }
        }
    } else {
        object = func_0011E1F0(map, 0x66);
        if (object != NULL) {
            func_0011E140(object);
        }
        object = func_0011E1F0(map, 0x67);
        if (object != NULL) {
            func_0011E140(object);
        }
    }
}
