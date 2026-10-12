#include "common.h"

void func_0021B9E0(void);

/* The idle thread of the movie player: yields forever (the SDK sample's defMain). */
void func_0021BDC0(void *unused) {
    while (TRUE) {
        func_0021B9E0();
    }
}
