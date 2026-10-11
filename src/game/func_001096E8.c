#include "kf_gs.h"

void FlushCache(int);
void func_00109A50(void);
int func_00109A80(int, unsigned long);
void func_00109AD0(void);
extern int D_00446748;
extern void *D_00447018[2];
typedef struct { int w0, w1, w2, w3; } Blk;
extern Blk *D_00447020[2];
extern int D_00447028;
extern int D_0044703C;

#define HW_10009000 (*(volatile unsigned int *)0x10009000)
#define HW_10009010 (*(volatile unsigned int *)0x10009010)
#define HW_10009020 (*(volatile unsigned int *)0x10009020)
#define HW_10009030 (*(volatile unsigned int *)0x10009030)
#define HW_12001000 (*(long *)0x12001000)

void func_001096E8(void) {
    void **temp_s0;
    Blk *temp_v0;

    temp_s0 = &D_00447018[D_00446748];
    if (*temp_s0 != 0) {
        if (D_0044703C == 0) {
            D_00447028 = 1;
            func_00109A50();
            func_00109A80(GS_REG_FINISH, 0xFFFFFFFFUL);
            func_00109AD0();
            temp_v0 = D_00447020[D_00446748];
            temp_v0->w0 = 0x70000000;
            temp_v0->w1 = 0;
            temp_v0->w2 = 0;
            temp_v0->w3 = 0;
            FlushCache(0);
            do {
            } while (HW_10009000 & 0x100);
            HW_12001000 |= 2;
            FlushCache(0);
            HW_10009020 = 0;
            HW_10009010 = 0;
            HW_10009030 = (unsigned int)*temp_s0;
            HW_10009000 = 0x105;
        }
    }
}
