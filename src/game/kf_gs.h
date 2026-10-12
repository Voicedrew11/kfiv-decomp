/* GS registers, as passed to func_00109A80 (set a GS register). */
#ifndef KF_GS_H
#define KF_GS_H

#include "common.h"

#define GS_REG_TEX0_1       0x06
#define GS_REG_TEX1_1       0x14
#define GS_REG_SCANMSK      0x22
#define GS_REG_TEXFLUSH     0x3F
#define GS_REG_ALPHA_1      0x42
#define GS_REG_ALPHA_2      0x43
#define GS_REG_TEST_1       0x47
#define GS_REG_TEST_2       0x48
#define GS_REG_ZBUF_1       0x4E
#define GS_REG_ZBUF_2       0x4F
#define GS_REG_FINISH       0x61

/* the ALPHA registers (GS_REG_ALPHA_1/2) as a value to write */
typedef union {
    s64 raw;
    struct {
        u64 a_source : 2;           /* 0 */
        u64 b_source : 2;           /* 2 */
        u64 c_source : 2;           /* 4 */
        u64 d_source : 2;           /* 6 */
        u64 pad8 : 24;
        u64 fix : 8;                /* 32 */
        u64 pad40 : 24;
    } f;
} GsAlphaReg;

#endif
