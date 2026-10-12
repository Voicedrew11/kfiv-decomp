#include "common.h"

/* Surface attribute: the first value of (0-7, 0xFF, 0xFF), the third of (0xFF, 0xFF, 0xFE-0xFF), 0 otherwise. */
u32 func_00112560(u32 first, u32 second, u32 third) {
    if (first < 8 && second == 0xFF && third == 0xFF) {
        return first;
    }
    if (first == 0xFF && second == 0xFF && third >= 0xFE) {
        return third;
    }
    return 0;
}
