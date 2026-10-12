#include "kf_gs.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);

void func_00109F80(s64 frame, s64 scissor, s64 xy_offset) {
    func_00109A50();
    func_00109A80(GS_REG_FRAME_1, frame);
    func_00109A80(GS_REG_FRAME_2, frame);
    func_00109A80(GS_REG_SCISSOR_1, scissor);
    func_00109A80(GS_REG_SCISSOR_2, scissor);
    func_00109A80(GS_REG_XYOFFSET_1, xy_offset);
    func_00109A80(GS_REG_XYOFFSET_2, xy_offset);
    func_00109AD0();
}
