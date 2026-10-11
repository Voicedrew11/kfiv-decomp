#include "common.h"

typedef struct Node { struct Node *next; int pad; } Node;
typedef struct { s64 a[2]; char *p; int q; s64 b[7]; } Big;
extern Node D_005CE900[];
extern Big D_005CE8B0;
extern Big *D_00446C90;
extern Node *D_004472B0;

void func_001D45D0(void) {
    Big *src = D_00446C90;
    u32 i;
    D_004472B0 = D_005CE900;
    for (i = 0; i < 15; i++) {
        D_005CE900[i].next = &D_005CE900[i + 1];
    }
    D_005CE900[15].next = 0;
    if (src != 0) {
        D_005CE8B0 = *src;
        D_00446C90 = &D_005CE8B0;
        D_005CE8B0.q = 0;
        if (D_005CE8B0.p != 0) {
            D_005CE8B0.p[0x24] = -1;
        }
    }
}
