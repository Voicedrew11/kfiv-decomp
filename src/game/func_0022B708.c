#include "common.h"
#include <libdev_internal.h>

int func_0022B6B8(void);

// libdev: sceDevVif0GetRegs in libdev_internal.h (name inferred). Takes a snapshot of VIF0's registers;
// fails unless VIF0 is idle or stopped.
int func_0022B708(LibdevVif0Registers *registers) {
    u32 cycle = LIBDEV_VIF_REG32(VIF0_STAT, VIF_CYCLE);

    if ((u32)func_0022B6B8() >= 2) {
        return FALSE;
    }
    registers->row[0] = LIBDEV_VIF_REG32(VIF0_STAT, VIF_ROW + 0x00);
    registers->row[1] = LIBDEV_VIF_REG32(VIF0_STAT, VIF_ROW + 0x10);
    registers->row[2] = LIBDEV_VIF_REG32(VIF0_STAT, VIF_ROW + 0x20);
    registers->row[3] = LIBDEV_VIF_REG32(VIF0_STAT, VIF_ROW + 0x30);
    registers->col[0] = LIBDEV_VIF_REG32(VIF0_STAT, VIF_COL + 0x00);
    registers->col[1] = LIBDEV_VIF_REG32(VIF0_STAT, VIF_COL + 0x10);
    registers->col[2] = LIBDEV_VIF_REG32(VIF0_STAT, VIF_COL + 0x20);
    registers->col[3] = LIBDEV_VIF_REG32(VIF0_STAT, VIF_COL + 0x30);
    registers->mask = LIBDEV_VIF_REG32(VIF0_STAT, VIF_MASK);
    registers->code = LIBDEV_VIF_REG32(VIF0_STAT, VIF_CODE);
    registers->stat = LIBDEV_REG32(VIF0_STAT);
    registers->itop = LIBDEV_VIF_REG32(VIF0_STAT, VIF_ITOP);
    registers->itops = LIBDEV_VIF_REG32(VIF0_STAT, VIF_ITOPS);
    registers->mark = LIBDEV_VIF_REG32(VIF0_STAT, VIF_MARK);
    registers->num = LIBDEV_VIF_REG32(VIF0_STAT, VIF_NUM);
    registers->error = LIBDEV_REG32(VIF0_ERR);
    registers->cycle_length = cycle;
    registers->write_length = cycle >> 8;
    registers->mode = LIBDEV_VIF_REG32(VIF0_STAT, VIF_MODE);
    return TRUE;
}
