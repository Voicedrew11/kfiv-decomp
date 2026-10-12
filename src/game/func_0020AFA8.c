#include "kf_gs.h"

void sceGsSetDefDBuff(void *, s16, s16, s16, s16, s16, s16);
void sceVu0CopyMatrix(void *, void *);
void func_0020B5F8(f32 (*)[4], f32, f32, f32);

extern f32 D_00431AF0[16];
extern f32 D_00432320[16];
extern f32 D_004323A0[16];
extern f32 D_00432560[4][4];
extern GsDoubleBuffer D_00432D20;
extern GsDoubleBuffer D_00432F50;
extern GsRegisterPair D_00433180;
extern GsRegisterPair D_00433190;
extern int D_00446F78;
extern int D_00446F7C;
extern int D_00446F80;

/* Sets up the game's two double buffers (the second one draws through GS context 2), the view
   matrices and the projection. */
void func_0020AFA8(void) {
    sceGsSetDefDBuff(&D_00432D20, 0, 0x280, 0x1C0, 3, 0x3A, 1);
    D_00432D20.disp[0].dispfb.fbp = 0;
    D_00432D20.disp[1].dispfb.fbp = 0x8C;
    D_00432D20.draw0.frame.fbp = 0x8C;
    D_00432D20.draw1.frame.fbp = 0;
    D_00432D20.draw0.zbuf.zbp = 0x118;
    D_00432D20.draw1.zbuf.zbp = 0x118;
    sceVu0CopyMatrix(D_00432320, D_00431AF0);
    sceVu0CopyMatrix(D_004323A0, D_00431AF0);
    func_0020B5F8(D_00432560, 512.0f, 1.0f, 65536.0f);
    D_00446F78 = 0;
    D_00446F80 = 0;
    D_00432D20.clear1.rgbaq = D_00432D20.clear0.rgbaq = 0x100000000L;
    sceGsSetDefDBuff(&D_00432F50, 0, 0x280, 0x1C0, 3, 0x3A, 1);
    D_00432F50.disp[0].dispfb.fbp = 0;
    D_00432F50.disp[1].dispfb.fbp = 0x8C;
    D_00432F50.draw0.frame.fbp = 0x8C;
    D_00432F50.draw1.frame.fbp = 0;
    D_00432F50.draw0.zbuf.zbp = 0x118;
    D_00432F50.draw1.zbuf.zbp = 0x118;
    D_00432F50.draw0.frame_addr = 0x4D;
    D_00432F50.draw0.zbuf_addr = 0x4F;
    D_00432F50.draw0.xyoffset_addr = 0x19;
    D_00432F50.draw0.scissor_addr = 0x41;
    D_00432F50.draw0.test_addr = 0x48;
    D_00432F50.clear0.test_a_addr = 0x48;
    D_00432F50.clear0.prim |= 0x200;
    D_00432F50.clear0.test_b_addr = 0x48;
    D_00432F50.draw1.frame_addr = 0x4D;
    D_00432F50.draw1.zbuf_addr = 0x4F;
    D_00432F50.draw1.xyoffset_addr = 0x19;
    D_00432F50.draw1.scissor_addr = 0x41;
    D_00432F50.draw1.test_addr = 0x48;
    D_00432F50.clear1.test_a_addr = 0x48;
    D_00432F50.clear1.prim |= 0x200;
    D_00432F50.clear1.test_b_addr = 0x48;
    D_00446F7C = 0;
    D_00433180.first = *(u64 *)&D_00432D20.disp[0].dispfb;
    D_00433180.second = *(u64 *)&D_00432D20.disp[1].dispfb;
    D_00433190.first = D_00432D20.disp[0].display;
    D_00433190.second = D_00432D20.disp[1].display;
    // match: the four stores below come out in retail order only when written in this one
    D_00432F50.draw1.xyoffset.ofy = 0x7200;
    D_00432F50.draw0.xyoffset.ofy = 0x7200;
    D_00432D20.draw1.xyoffset.ofy = 0x7200;
    D_00432D20.draw0.xyoffset.ofy = 0x7200;
}
