#include "kf_model.h"

extern ModelInstance *D_00446978;
extern u8 D_004141D8[];

void func_00108AC8(void *);
void func_001A35D0(void);

void func_001B7D38(void) {
    func_00108AC8(D_00446978);
    if (MODEL_CHANNEL(D_00446978)->value == 0) {
        func_001A35D0();
        D_004141D8[0] = 0;
    }
}
