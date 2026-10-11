#include "common.h"

extern void *memset(void *, int, u32);
extern int FlushCache(int);
extern int func_0020D9A8(void);
extern int sceGsExecLoadImage(int *, int *);
extern int sceGsSetDefLoadImage(int *, int, int, int, int, int, int, int);
extern int sceGsSyncPath(int, int);

void func_0019BDA0(int arg0, int arg1) {
    int img[24];
    int buf[0x4000];
    int i;

    memset(buf, 0, 0x10000);
    for (i = arg0; i < arg1; i += 0x100) {
        sceGsSetDefLoadImage(img, (s16)i, 2, 0, 0, 0, 0x80, 0x80);
        FlushCache(0);
        sceGsExecLoadImage(img, buf);
        sceGsSyncPath(0, 0);
    }
    func_0020D9A8();
}
