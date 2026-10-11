#include "common.h"

void func_001C2F30(void *, int, int, float);
void func_001C31E8(void *, int, int, float);
void func_001C3910(void *, int, int, float);
void func_001C3A18(void *, int, int, float);
void func_001C2E48(void *, int, int, float);

typedef struct {
    char *obj;
    u8 *cmd;
} Ev;

void func_001C2128(Ev *arg0, int arg1, int arg2, float arg3) {
    *(int *)0x70000010 = 0;
    switch (*arg0->cmd) {
    case 1:
        func_001C2F30(arg0, arg1, arg2, arg3);
        break;
    case 2:
        func_001C31E8(arg0, arg1, arg2, arg3);
        break;
    case 17:
        func_001C3910(arg0, arg1, arg2, arg3);
        break;
    case 18:
        func_001C3A18(arg0, arg1, arg2, arg3);
        break;
    default:
        if (*(int *)(arg0->obj + 0x110) != 0) {
            func_001C2E48(arg0, arg1, arg2, arg3);
        }
        break;
    }
}
