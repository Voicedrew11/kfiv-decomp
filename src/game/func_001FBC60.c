#include "common.h"
#include "kf_state.h"

extern s16 D_005CF420[];

/* Sets the first status byte of both slot records to 1 and clears D_005CF420[0]; the caller passes the selected card, which goes unused. */
void func_001FBC60(s32 index) {
    SLOT_RECORD_BYTES(&D_00428B80[0])->first = 1;
    D_005CF420[0] = 0;
    SLOT_RECORD_BYTES(&D_00428B80[1])->first = 1;
}
