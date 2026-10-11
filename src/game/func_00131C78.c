#include "common.h"

extern float D_00444EE8;
extern int D_00413F80[];
void func_00132428(void);
float func_001957B8(float, float);
s16 func_00190F90(float, char *, int);
void func_001958B8(char *, void (*)(void), int);

typedef struct A {
    struct A *p0;
    char pad[0x64];
    float f68;
    char pad2[0x68];
    float fD4;
    u64 b0 : 9;
    u64 b9 : 1;
    u64 b10 : 1;
    u64 b11 : 3;
    u64 b14 : 50;
} A;

void func_00131C78(char *arg0, int arg1) {
    char *temp_s1;
    float t;

    temp_s1 = arg0 + 0xE0;
    ((A *)arg0)->b11 = 1;
    ((A *)arg0)->b9 = 1;
    t = func_001957B8((*(A **)arg0)->fD4, ((A *)arg0)->f68);
    if (!(0.0f <= t ? D_00444EE8 < t : D_00444EE8 < -t)) {
        temp_s1[0x7B] = (char)(temp_s1[0x7B] == 0);
        func_001958B8(arg0, func_00132428, arg1);
        return;
    }
    *(int **)(arg0 + 0x5C) = D_00413F80;
    *(s16 *)(arg0 + 0xC8) = func_00190F90(50.0f, arg0, 1);
    func_001958B8(arg0, *(void (**)(void))(temp_s1 + 0x18), arg1);
}
