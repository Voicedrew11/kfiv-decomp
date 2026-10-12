#include "common.h"

/* Surface attribute: the second value when the triple is (0-7, 0-1, 0xFF), 0xFFFFFFFF otherwise. */
u32 func_00112618(u32 first, u32 second, int third) {
    if (first < 8 && second < 2 && third == 0xFF) {
        return second;
    }
    return 0xFFFFFFFF;
}
