#include "common.h"

void func_001B0A58(void);
void func_001B0A60(void);
void func_001B0A68(void);
void func_001B0A70(void);
int func_001B0A78(void);

/* Debug menu loop: runs the menu until it returns zero. */
int func_001B0A00(void) {
    func_001B0A58();
    while (TRUE) {
        func_001B0A60();
        func_001B0A68();
        if (func_001B0A78() == 0) {
            return 0;
        }
        func_001B0A70();
        if (func_001B0A78() == 0) {
            return 0;
        }
    }
}
