#include "common.h"

/* Surface attribute: the first value when the triple is (0-7, 0xFF, 0xFF), 6 otherwise. */
u32 func_001125A8(u32 first, u32 second, int third) {
    if (first < 8 && second == 0xFF && third == 0xFF) {
        return first;
    }
    return 6;
}
