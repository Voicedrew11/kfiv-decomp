#include "kf_gs.h"
#include "kf_packet.h"

s32 func_00207320(int, s64);

/* Sets the blend mode of ALPHA_1 to (source - destination) * source alpha + destination and hands the packet back. */
SpritePacket *func_002062A0(SpritePacket *packet) {
    GsAlphaReg reg; // match: never seeded, so the bits outside the five fields are whatever the register held

    reg.f.a_source = 0;
    reg.f.b_source = 2;
    reg.f.c_source = 0;
    reg.f.d_source = 1;
    reg.f.fix = 0;
    func_00207320(GS_REG_ALPHA_1, *(s64 *)&reg); // match: read through a pointer; reg.raw keeps the value in another register
    return packet;
}
