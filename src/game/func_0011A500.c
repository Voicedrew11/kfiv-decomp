#include "kf_game.h"

extern ViewerEntry D_0038D6B0[];
extern int D_00447064;

void func_0011A598(void);
void func_0011A5E0(void);
int func_0011A600(void);
void func_0019CCB0(void);
void func_0019CD10(void);
void func_0019DC00(void);
void func_0019E8B0(void);
void func_0019E920(void);
void func_001A15B8(void);
void func_001A15C0(void);
int func_001CEBE0(void);

/* Loads the picked viewer, runs it until it is left and frees what it loaded. */
void func_0011A500(void) {
    if (func_0011A600() != 0) {
        func_0019E8B0();
        func_001A15C0();
        func_0019DC00();
        func_0019CCB0();
        func_001CEBE0();
        D_0038D6B0[D_00447064].run();
        func_0019E920();
        func_001A15B8();
        func_0019CD10();
        func_0011A5E0();
        func_0011A598();
    }
}
