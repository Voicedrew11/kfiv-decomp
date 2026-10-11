#include "common.h"

extern u8 D_00349690[];
void func_0011E0C8(void *);
void func_0011E140(void *);
void *func_0011E1F0(int, int);

void func_0011A370(int arg0) {
    void *temp;

    if (D_00349690[0x1C0A] != 0) {
        D_00349690[0x1ADB] = 1;
        temp = func_0011E1F0(arg0, 0x75);
        if (temp != 0) {
            func_0011E140(temp);
        }
        temp = func_0011E1F0(arg0, 0x76);
        if (temp != 0) {
            func_0011E0C8(temp);
        }
    }
}
