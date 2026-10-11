#include "common.h"

extern void *D_004469D4;

typedef struct KfNode {
    int f00;
    u8 *f04;
    struct KfNode *f08;
    char pad[0x14];
    int f20;
    struct KfNode *f24;
} KfNode;

int func_001C9B00(KfNode *arg0) {
    KfNode *temp;
    KfNode *cur;
    int var_v0;

    cur = D_004469D4;
    if (arg0 == 0) {
        return 0;
    }
    if ((cur != arg0) && (arg0->f20 == 0) && (arg0->f24 == 0)) {
        return 0;
    }
    temp = arg0->f08;
    var_v0 = 1;
    if (temp != 0) {
        var_v0 = *temp->f04 != 4;
    }
    return var_v0;
}
