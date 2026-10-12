#include "kf_state.h"

s32 func_001FB960(s32 index) {
    return SLOT_RECORD_BYTES(&D_00428B80[index])->second != 0;
}
