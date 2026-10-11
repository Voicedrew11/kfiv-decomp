#include "common.h"

extern char D_003F1750[];

int func_001A1C58();

typedef struct {
    char pad[0x38];
    int a38;
    s16 a3c;
} Ent;

int func_001A1E28(int a0, int a1, int a2, int a3, u16 a4, int a5) {
    int temp_v0;
    Ent *temp_v1;

    temp_v0 = func_001A1C58();
    if (temp_v0 != -1) {
        temp_v1 = (Ent *)(D_003F1750 + temp_v0 * 0x50);
        temp_v1->a3c = a4;
        temp_v1->a38 = a5;
    }
    return temp_v0;
}
