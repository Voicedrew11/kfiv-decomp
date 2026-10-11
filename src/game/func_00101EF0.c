#include "common.h"

extern int D_00446F78;
extern int D_00446F7C;
void func_001095E0(void);
void func_0010B000(void);
void func_001095D0(void);
void func_0010A008(void);
void func_0010A4E8(int, int, int, int, int);
void func_001095D8(void);
void func_001096E8(void);
void func_00109800(void);
void func_0020B780(int);

/* Runs two passes of the frame setup, syncing after the first. */
void func_00101EF0(void) {
    int pass;

    for (pass = 0; pass < 2; pass++) {
        func_001095E0();
        func_0010B000();
        func_001095D0();
        D_00446F78 = pass;
        D_00446F7C = pass;
        func_0010A008();
        func_0010A4E8(0, 0, 0, 0x80, 0x44);
        func_001095D8();
        if (pass == 0) {
            D_00446F78 = 1;
            D_00446F7C = 1;
            func_001096E8();
            func_00109800();
            func_0020B780(0);
        }
    }
}
