#include "common.h"

/* Whether a map object of this kind is one of the ordinary ones. */
int func_0011C678(int kind) {
    switch (kind) {
        case 0:
        case 2:
        case 4:
        case 8:
        case 9:
        case 0x62:
        case 0x63:
            return FALSE;
    }
    return TRUE;
}
