#include "common.h"

int func_001005E8(int);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_0010A040(int);
void func_0010B000(void);
void Display_SwapBuffers(int);

/* Draws one frame with nothing in it. */
void func_00118A50(void) {
    Display_SwapBuffers(0);
    func_001096E8();
    func_001095E0();
    func_0010B000();
    func_001005E8(0);
    func_0010A040(0x2BC0);
    func_00109800();
}
