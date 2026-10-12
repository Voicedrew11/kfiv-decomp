#include "kf_entity.h"

extern u8 D_00349690[];
extern u8 D_0034B6D0[];

int func_0011E1F0(int, int);
void func_0011E758(MapObject *, int);
void func_0011E7B0(void *, int, int);
void func_0011E810(void *, int);
void *func_001267E8(int, int);
int func_001C8C80(int, int);

/* Sets map objects 0xA, 0xB and 0x22 and entity 0x17 up from the stage flags. */
void func_00119590(int map) {
    MapObject *object;
    Entity *entity;

    if (D_00349690[0x2C5] != 0) {
        func_001C8C80(map, 0x46);
    }
    if (D_0034B6D0[0x5C6] == 1 && D_00349690[0x2C5] == 0) {
        object = (MapObject *)func_0011E1F0(map, 0xA);
        if (object != NULL) {
            func_0011E758(object, 1);
        }
        object = (MapObject *)func_0011E1F0(map, 0xB);
        if (object != NULL) {
            func_0011E758(object, 1);
        }
        ((int (*)(int, int))func_001C8C80)(map, 0x46); // match: called as returning a value, so the next test uses $3
        if (D_0034B6D0[0x5C0] == 0) {
            entity = func_001267E8(map, 0x17);
            if (entity != NULL) {
                entity->flags |= 2;
                entity->flags |= 0x80;
            }
        }
    } else {
        object = (MapObject *)func_0011E1F0(map, 0xA);
        if (object != NULL) {
            func_0011E758(object, 0);
            object->unk10 |= 0x20000;
        }
        object = (MapObject *)func_0011E1F0(map, 0xB);
        if (object != NULL) {
            func_0011E758(object, 0);
            object->unk10 |= 0x20000;
        }
        object = (MapObject *)func_0011E1F0(map, 0x22);
        if (object != NULL) {
            func_0011E758(object, 1);
            func_0011E7B0(object, 1, 0);
            func_0011E810(object, 1);
        }
    }
}
