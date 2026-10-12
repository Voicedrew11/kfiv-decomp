#include "kf_model.h"
#include "kf_player.h"

extern ModelInstance *D_00446978;
extern PlayerState D_00413F80;

void func_00108AC8(void *);

void func_001B7CC8(void) {
    if (MODEL_CHANNEL(D_00446978)->value != 0) {
        func_00108AC8(D_00446978);
        if (MODEL_CHANNEL(D_00446978)->value == 0) {
            func_00108AC8(D_00446978);
        }
    } else if (MODEL_CHANNEL(D_00446978)->param_shifted > 0) {
        func_00108AC8(D_00446978);
    } else {
        D_00413F80.unk258 = 0;
        D_00413F80.unk298 = 0;
    }
}
