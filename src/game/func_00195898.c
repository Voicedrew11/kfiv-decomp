#include "common.h"

void func_00195898(char *arg0, u8 arg1) {
    u8 temp_2;

    temp_2 = arg0[0xCC];
    arg0[0xCC] = arg1;
    arg0[0xCE] = temp_2;
}
