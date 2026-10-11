#include "kf_ent.h"

float func_00194F80(int, int);

void func_001409D0(Ent *arg0) {
    Ent *s0 = D_004467E0;
    while (s0 != 0) {
        if (arg0->unk0C == s0->unk0C) {
            if (*(int *)&s0->flags & 1) {
                long x = s0->flags;
                if (((int)(x >> 1) & 1) && !((int)(x >> 2) & 3) && s0 != arg0) {
                    EntE0 *t = &s0->unkE0;
                    if (t->unk7A == 0 &&
                        func_00194F80((int)arg0->model + 0xC0, (int)s0->model + 0xC0) <
                            (float)*(short *)(arg0->unk0C + 0x438)) {
                        t->unk7A = 1;
                    }
                }
            }
        }
        s0 = s0->next;
    }
}
