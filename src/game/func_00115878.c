#include "common.h"

typedef struct Node {
    char pad0[0x38];
    u64 f38;
    char pad40[0x1C];
    struct Node *f5C;
} Node;

extern Node *D_00446788;
extern Node *D_0044678C;

void func_00115878(int arg0) {
    Node *prev;
    Node *cur;

    prev = 0;
    cur = D_00446788;
    while (cur != 0) {
        if (((u8)(cur->f38 >> 27) & 1) == arg0) {
            if (prev != 0) {
                prev->f5C = cur->f5C;
            } else {
                D_00446788 = cur->f5C;
            }
        } else {
            prev = cur;
        }
        cur = cur->f5C;
    }
    if (prev == 0) {
        D_00446788 = 0;
    }

    prev = 0;
    cur = D_0044678C;
    while (cur != 0) {
        if (((u8)(cur->f38 >> 27) & 1) == arg0) {
            if (prev != 0) {
                prev->f5C = cur->f5C;
            } else {
                D_0044678C = cur->f5C;
            }
        } else {
            prev = cur;
        }
        cur = cur->f5C;
    }
    if (prev == 0) {
        D_0044678C = 0;
    }
}
