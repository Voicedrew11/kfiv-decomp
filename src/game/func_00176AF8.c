#include "common.h"

int func_00197078(void);
int func_0018F030(char *);
void func_00182240(void);
void func_00181E48(void);
void func_001958B8(char *, void (*)(void), int);

void func_00176AF8(char *arg0, int arg1) {
    char *s;

    s = arg0 + 0xE0;
    if (*(u16 *)(arg0 + 0xC6) == 0) {
        if (func_00197078() != 0 && *(u8 *)(arg0 + 0xA2) == 0) {
            if (func_0018F030(arg0) != 0) {
                func_001958B8(arg0, func_00182240, arg1);
            } else {
                func_001958B8(arg0, func_00181E48, arg1);
            }
        } else {
            func_001958B8(arg0, *(void (**)(void))(s + 0x18), arg1);
        }
    }
}
