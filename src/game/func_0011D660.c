#include "common.h"

typedef struct {
    char pad0[4];
    u8 *type;           /* 0x4 */
    char pad8[0x10 - 8];
    int flags;                     /* 0x10 */
} Obj;

extern void (*D_003BE330[])(Obj *, int);

void func_0011D660(Obj *arg0, int arg1) {
    if (arg0 != 0) {
        if (!(arg0->flags & 0x100000)) {
            D_003BE330[*arg0->type](arg0, arg1);
        }
    }
}
