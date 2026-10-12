#include "kf_packet.h"

extern u32 D_00446A4C;
extern SpritePacket *D_00447248;
int func_001E5210(u8);
void func_00203F28(int, int, int);

/* Draws every kind 2 sprite of the D_00447248 table, each with its own unk42 setting. */
void func_001D76D0(void) {
    u32 i;

    for (i = 0; i < D_00446A4C; i++) {
        if (D_00447248[i].unk3E == 2) {
            func_001E5210(D_00447248[i].unk42);
            func_00203F28((int)&D_00447248[i], 2, 0);
            func_001E5210(0);
        }
    }
}
