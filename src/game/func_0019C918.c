#include "kf_player.h"

extern PlayerState D_00413F80;
extern s8 D_00349680[];
extern int D_003EF618[];
extern int D_003EF6A8[];
extern s32 D_00502170[][2];
float func_0019C640(float *, float *, float *);
void func_0019C598(float, float *, float *);
int func_0019C688(int);
int func_001CE690(u32, int, char, char, char, char, s16, char);

/* Starts a sound from one of two banks in a free D_00502170 slot at full volume. */
int func_0019C918(int sound, int unused, float *position) {
    float diff[4];
    float direction[4];
    float origin[4];
    float offset[4];
    int *bank;
    int slot;
    int tries;

    if (sound < 8) {
        bank = D_003EF6A8;
    } else if (sound < 0x7F) {
        bank = D_003EF618;
    } else {
        return -1;
    }
    tries = 0;
    func_0019C640(diff, position, &D_00413F80.position.x);
    func_0019C598(D_00413F80.rotation.y, origin, offset);
    direction[0] = origin[0];
    direction[1] = 0.0f;
    direction[2] = -offset[0];
    for (;;) {
        slot = func_0019C688(5);
        tries++;
        if (D_00502170[slot - 100][0] == 0) {
            break;
        }
        if (tries >= 0xB) {
            return -1;
        }
    }
    func_001CE690(slot, bank[4], 0, sound, 0x3C, D_00349680[2], 0, 0x3F);
    D_00502170[slot - 100][0] = 1;
    D_00502170[slot - 100][1] = (u8)D_00349680[2];
    return slot;
}
