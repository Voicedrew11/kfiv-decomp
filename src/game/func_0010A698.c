#include "kf_gs.h"
#include "common.h"

void func_00109A50(void);
void func_00109A80(int, u64);
void func_00109AD0(void);
void func_0010A338(void *);

typedef struct {
    int pad0[4];
    int unk10;
    char unk14[4];
    u8 pad18[8];
    u16 unk20;
    u16 unk22;
    int unk24;
    u8 pad28[8];
    u16 unk30;
    u16 unk32;
    int unk34;
} Ctx;

void func_0010A698(void) {
    Ctx s;

    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x32001);
    func_00109AD0();
    s.unk10 = 6;
    s.unk14[0] = 0;
    s.unk14[1] = 0;
    s.unk14[2] = 0;
    s.unk14[3] = 0;
    s.unk20 = 0x6C00;
    s.unk22 = 0x7200;
    s.unk24 = 0;
    s.unk30 = 0x9400;
    s.unk32 = 0x8E00;
    s.unk34 = 0;
    func_0010A338(&s);
}
