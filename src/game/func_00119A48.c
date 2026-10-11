#include "common.h"

extern u8 D_0034B696[];
extern char D_0038CD20[];

typedef struct {
    char pad0[0xA0];
    float fA0;
    float fA4;
    float fA8;
    float fAC;
    s64 lB0;
} Fog;

typedef struct {
    char pad0[0x480];
    float m[3][4];
    char pad4B0[0x10];
    float v[3][4];
    float f4F0;
    float f4F4;
    float f4F8;
} Light;

void func_00119A48(int arg0) {
    char *e = D_0038CD20 + arg0 * 0x60;
    Fog *fog;
    Light *l;

    if (D_0034B696[0] == 0) {
        (*(Fog **)(e + 0xC))->fA0 = (*(Fog **)(e + 0xC))->fA4 = 0.0f;
        (*(Fog **)(e + 0xC))->fA8 = -28.333328f;
        (*(Fog **)(e + 0xC))->fAC = 113333.336f;
        (*(Fog **)(e + 0xC))->lB0 = 0x8C8C8C;
    } else {
        fog = *(Fog **)(e + 0xC);
        l = *(Light **)(e + 0x10);
        fog->fA0 = fog->fA4 = 0.0f;
        fog->fA8 = -318.75f;
        fog->fAC = 1.1475e+06f;
        fog->lB0 = 0;
        l->f4F0 = 0.6627451f;
        l->f4F4 = 0.3372549f;
        l->m[0][0] = 0.0f;
        l->m[1][0] = 0.0f;
        l->m[2][0] = 0.0f;
        l->v[0][0] = 0.0f;
        l->v[0][1] = 0.0f;
        l->v[0][2] = 0.0f;
        l->m[0][1] = 0.0f;
        l->m[1][1] = 0.0f;
        l->m[2][1] = 0.0f;
        l->v[1][0] = 0.0f;
        l->v[1][1] = 0.0f;
        l->v[1][2] = 0.0f;
        l->m[0][2] = 0.0f;
        l->m[1][2] = 0.0f;
        l->m[2][2] = 0.0f;
        l->v[2][0] = 0.0f;
        l->v[2][1] = 0.0f;
        l->v[2][2] = 0.0f;
        l->f4F8 = 0.0f;
    }
}
