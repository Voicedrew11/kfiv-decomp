#include "common.h"
#include <libdev_internal.h>

int func_0022B9E8(void);

// libdev: sceDevVif1GetRegs in libdev_internal.h (name inferred). Takes a snapshot of VIF1's registers;
// fails unless VIF1 is idle or stopped.
int func_0022BA38(LibdevVif1Registers *registers) {
    u32 cycle = LIBDEV_VIF_REG32(VIF1_STAT, VIF_CYCLE);

    if ((u32)func_0022B9E8() >= 2) {
        return FALSE;
    }
    registers->row[0] = LIBDEV_VIF_REG32(VIF1_STAT, VIF_ROW + 0x00);
    registers->row[1] = LIBDEV_VIF_REG32(VIF1_STAT, VIF_ROW + 0x10);
    registers->row[2] = LIBDEV_VIF_REG32(VIF1_STAT, VIF_ROW + 0x20);
    registers->row[3] = LIBDEV_VIF_REG32(VIF1_STAT, VIF_ROW + 0x30);
    registers->col[0] = LIBDEV_VIF_REG32(VIF1_STAT, VIF_COL + 0x00);
    registers->col[1] = LIBDEV_VIF_REG32(VIF1_STAT, VIF_COL + 0x10);
    registers->col[2] = LIBDEV_VIF_REG32(VIF1_STAT, VIF_COL + 0x20);
    registers->col[3] = LIBDEV_VIF_REG32(VIF1_STAT, VIF_COL + 0x30);
    registers->mask = LIBDEV_VIF_REG32(VIF1_STAT, VIF_MASK);
    registers->code = LIBDEV_VIF_REG32(VIF1_STAT, VIF_CODE);
    registers->stat = LIBDEV_REG32(VIF1_STAT);
    registers->itop = LIBDEV_VIF_REG32(VIF1_STAT, VIF_ITOP);
    registers->itops = LIBDEV_VIF_REG32(VIF1_STAT, VIF_ITOPS);
    registers->base = LIBDEV_VIF_REG32(VIF1_STAT, VIF_BASE);
    registers->offset = LIBDEV_VIF_REG32(VIF1_STAT, VIF_OFST);
    registers->top = LIBDEV_VIF_REG32(VIF1_STAT, VIF_TOP);
    registers->tops = LIBDEV_VIF_REG32(VIF1_STAT, VIF_TOPS);
    registers->mark = LIBDEV_VIF_REG32(VIF1_STAT, VIF_MARK);
    registers->num = LIBDEV_VIF_REG32(VIF1_STAT, VIF_NUM);
    registers->error = LIBDEV_REG32(VIF1_ERR);
    registers->cycle_length = cycle;
    registers->write_length = cycle >> 8;
    registers->mode = LIBDEV_VIF_REG32(VIF1_STAT, VIF_MODE);
    return TRUE;
}
