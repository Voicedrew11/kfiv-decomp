#include "common.h"
#include "kf_movie.h"

extern MovieThreadState D_00437850;
extern IntcDmacState D_00437938;
extern s32 D_00437838[];
extern MovieBuffers D_004379F0;

/* the two stubs take an argument the retail caller passes and they ignore */
void func_0021C130();
void func_0021E570();
s32 TerminateThread(s32);
s32 DeleteThread(s32);
s32 DisableDmac(s32);
s32 RemoveDmacHandler(s32, s32);
s32 DisableIntc(s32);
s32 RemoveIntcHandler(s32, s32);
void func_0021DF88(void *);
s32 func_0021C2D8(MovieBuffers *);
s32 func_0021FCE8(void *);

void func_0021BD18(void) {
    func_0021C130(D_00437850.unk38);
    func_0021E570(D_00437838);
    TerminateThread(D_00437850.unk40);
    DeleteThread(D_00437850.unk40);
    TerminateThread(D_00437850.unk44);
    DeleteThread(D_00437850.unk44);
    DisableDmac(2);
    RemoveDmacHandler(2, D_00437938.unkB0);
    DisableIntc(2);
    RemoveIntcHandler(2, D_00437938.unkB4);
    func_0021DF88(&D_00437938);
    func_0021C2D8(&D_004379F0);
    func_0021FCE8(&D_00437850.unk4C);
}
