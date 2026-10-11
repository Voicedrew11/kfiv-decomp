#include "common.h"

typedef struct {
    u8 unk0;
    u8 unk1;
    u16 unk2;
} Ent4;

extern char D_0034B690[0x40];
extern Ent4 D_00367928[5];

int func_00103A18(int arg0, int *arg1, int *arg2) {
    Ent4 *p;
    int i;

    if (arg0 == 0) {
        return 0;
    }
    p = D_00367928;
    for (i = 0; i < 5; i++, p++) {
        if (p->unk2 == arg0) {
            u8 t;

            *arg2 = p->unk1;
            t = p->unk0;
            *arg1 = t;
            if ((u32)(t - 0x11) < 3) {
                switch ((u8)D_0034B690[6]) {
                case 0:
                    *arg1 = 0x11;
                    break;
                case 1:
                    *arg1 = 0x12;
                    break;
                case 2:
                    *arg1 = 0x13;
                    break;
                }
            }
            return 1;
        }
    }
    return 0;
}
