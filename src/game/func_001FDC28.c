#include "common.h"
#include "kf_gs.h"

extern GsPackedWord D_00446D68;

int func_001FDC28(GsPackedWord word) {
    D_00446D68.f.lo16 = word.f.lo16;
    D_00446D68.f.mid16 = word.f.mid16;
    D_00446D68.f.mid24 = word.f.mid24;
    D_00446D68.f.hi8 = word.f.hi8;
    return 1;
}
