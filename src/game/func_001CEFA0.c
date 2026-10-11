#include "common.h"

int func_001CBAE0();
extern char *D_004471D4;

typedef struct {
    char b[24];
} Rec;

typedef struct {
    char pad[0x440];
    Rec tab[4];
    Rec cur;
} G;

void func_001CEFA0(u32 arg0) {
    if (arg0 < 5) {
        ((G *)D_004471D4)->cur = ((G *)D_004471D4)->tab[arg0];
        func_001CBAE0(1, 0x40070, &((G *)D_004471D4)->cur);
    }
}
