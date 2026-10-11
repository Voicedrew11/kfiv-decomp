#include "common.h"

typedef struct {
    u64 a : 16;
    u64 b : 16;
    u64 c : 24;
    u64 d : 8;
} Packed;

extern Packed D_00446D60;

int func_001FDB88(Packed arg0) {
    D_00446D60.a = arg0.a;
    D_00446D60.b = arg0.b;
    D_00446D60.c = arg0.c;
    D_00446D60.d = arg0.d;
    return 1;
}
