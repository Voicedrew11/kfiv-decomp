#include "common.h"

int func_001D62C8(s8);
int func_00209608(int);

int func_001F91F8(void) {
    int s0 = 0;

    do {
        switch (func_00209608(s0)) {
        case 1:
            return 0;
        case 2:
            if (func_001D62C8(1) != 0) {
                return 1;
            }
            s0 = 2;
            break;
        case 3:
            func_001D62C8(2);
            s0 = 2;
            break;
        case 4:
            func_001D62C8(3);
            s0 = 2;
            break;
        }
    } while (s0 != -1);
}
