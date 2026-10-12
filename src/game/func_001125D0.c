#include "common.h"

/* Surface attribute: the third value when the triple is (0xFF, 0xFF, 0xFE-0xFF), 1 otherwise. */
u32 func_001125D0(u32 first, u32 second, u32 third) {
    if (first == 0xFF && second == 0xFF && third >= 0xFE) {
        return third;
    }
    return 1;
}
