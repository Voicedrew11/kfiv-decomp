#include "kf_entity.h"

int func_00195E48(void *, int, void *, void *);
void func_0019D490(s32, s32, f32 *, f32);

void func_0016D9C0(Entity *entity) {
    f32 position[4];
    f32 direction[4];
    s32 i;

    position[0] = -17300.0f;
    position[1] = 2930.0f;
    position[2] = 1800.0f;
    position[3] = 0.0f;
    direction[0] = 0.0f;
    direction[1] = 0.0f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    for (i = -17300; i <= -16100; i += 400) {
        position[0] = i;
        func_00195E48(entity, 0xCA, position, direction);
    }
    position[0] = -17300.0f;
    position[1] = 2930.0f;
    position[2] = 4200.0f;
    position[3] = 0.0f;
    direction[0] = 0.0f;
    direction[1] = 3.1415927f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    for (i = -17300; i <= -16100; i += 400) {
        position[0] = i;
        func_00195E48(entity, 0xCA, position, direction);
    }
    position[0] = -17700.0f;
    position[1] = 2830.0f;
    position[2] = 2000.0f;
    position[3] = 0.0f;
    direction[0] = 0.0f;
    direction[1] = 1.5707964f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    for (i = 2000; i <= 4000; i += 400) {
        position[2] = i;
        func_00195E48(entity, 0xCA, position, direction);
    }
    position[0] = -15700.0f;
    position[1] = 2830.0f;
    position[2] = 3200.0f;
    position[3] = 0.0f;
    direction[0] = 0.0f;
    direction[1] = -1.5707964f;
    direction[2] = 0.0f;
    direction[3] = 0.0f;
    for (i = 3200; i <= 4000; i += 400) {
        position[2] = i;
        func_00195E48(entity, 0xCA, position, direction);
    }
    position[0] = -15700.0f;
    position[1] = 2830.0f;
    position[2] = 2000.0f;
    position[3] = 0.0f;
    func_00195E48(entity, 0xCA, position, direction);
    position[0] = -15700.0f;
    position[1] = 2830.0f;
    position[2] = 3600.0f;
    position[3] = 0.0f;
    func_0019D490(0x42, 0, position, 2000.0f);
    position[0] = -16900.0f;
    position[1] = 2930.0f;
    position[2] = 4200.0f;
    position[3] = 0.0f;
    func_0019D490(0x42, 0, position, 2000.0f);
    position[0] = -17700.0f;
    position[1] = 2830.0f;
    position[2] = 3200.0f;
    position[3] = 0.0f;
    func_0019D490(0x42, 0, position, 2000.0f);
    position[0] = -17300.0f;
    position[1] = 2930.0f;
    position[2] = 2200.0f;
    position[3] = 0.0f;
    func_0019D490(0x42, 0, position, 2000.0f);
    position[0] = -15700.0f;
    position[1] = 2830.0f;
    position[2] = 2000.0f;
    position[3] = 0.0f;
    func_0019D490(0x42, 0, position, 2000.0f);
}
