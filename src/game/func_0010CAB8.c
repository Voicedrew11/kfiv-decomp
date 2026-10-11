#include "common.h"

char *func_0010CAB8(char *arg0, u32 arg1) {
    if (arg0 != NULL) {
        if (arg1 < 0x20U) {
            return *(char **)(arg0 + 0x124) + (arg1 * 0x10);
        }
        return arg0 + 0xC0;
    }
    return NULL;
}
