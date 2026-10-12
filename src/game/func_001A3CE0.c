#include "common.h"
#include "kf_effect.h"
#include "kf_gs.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010AEB8(void *model);

/* Sets the GS state for an effect pass, then draws the effect's model. */
void func_001A3CE0(EffectActor *effect) {
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(GS_REG_ALPHA_1, 0x42);
    func_00109A80(GS_REG_TEST_1, 0x33001);
    func_00109AD0();
    func_0010AEB8(effect);
}
