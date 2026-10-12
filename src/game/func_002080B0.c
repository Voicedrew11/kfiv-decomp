#include "common.h"

extern char D_005F9700[0x400];
extern char *D_00447350;
extern int D_00447354;

void *memset(void *, int, u32);

/* Copies the next line of the text at D_00447350 to D_005F9700, without its CR LF. */
char *func_002080B0(void) {
    char *dest;
    char *source;
    char current;
    char previous;

    memset(D_005F9700, 0, sizeof(D_005F9700));
    if (D_00447354 <= 0) {
        return NULL;
    }
    source = D_00447350;
    dest = D_005F9700;
    current = 0;
    for (;;) {
        previous = current;
        current = *source;
        D_00447354--;
        if (previous == '\r' && current == '\n') {
            dest[-1] = 0;
            break;
        }
        if (D_00447354 <= 0) {
            *dest = 0;
            break;
        }
        *dest = *source;
        source++;
        dest++;
    }
    D_00447350 = source + 1;
    return D_005F9700;
}

