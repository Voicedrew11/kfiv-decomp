#include "common.h"

char *func_0010CAB8(char *, u32);
int func_001A1C58(int, void *, void *, void *);

void func_0018D600(char **arg0, int arg1, u32 arg2, int arg3) {
    char *temp_v0;

    temp_v0 = func_0010CAB8(*arg0, arg2);
    if (temp_v0 != 0) {
        func_001A1C58(arg1, temp_v0, arg3, arg0);
    }
}
