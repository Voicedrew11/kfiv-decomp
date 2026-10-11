#include "common.h"

typedef struct KfNode {
    int f00;
    u8 *f04;
    struct KfNode *f08;
    char pad[0x14];
    int f20;
    struct KfNode *f24;
    char pad28[0x14];
} KfNode;

int func_001CA5B0(int, int);
extern int D_00446770;
extern void *D_004469D4;
extern KfNode *D_004469D8;
extern int D_00447198;
extern int D_004471A4;
extern KfNode D_0057E240[][64];

int func_001C9B58(int arg0, int arg1) {
    int n;
    KfNode *p;

    n = func_001CA5B0(D_00446770, arg0);
    if (D_004469D8 != 0 || n < 0) {
        return 0;
    }
    p = &D_0057E240[D_00446770][n];
    if (D_004469D4 != p && p->f20 == 0 && p->f24 == 0) {
        return 0;
    }
    D_004469D8 = p;
    D_00447198 = arg1 == 0;
    D_004471A4 = 0;
    return 1;
}
