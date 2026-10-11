#include "common.h"

int FlushCache(int);
int sceSifSetDma(int *, int);
extern char *D_004471D4;
extern int D_005BF860[];
extern void *D_005BF870[];

typedef struct {
    int w00;
    int w04;
    int w08;
    int w0C;
    int w10;
    int w14;
    int w18;
} Req;

int func_001CBC30(int arg0, int arg1, u32 arg2) {
    Req *d;
    int temp_a1;
    int temp_v0;

    ((Req *)D_005BF870)->w04 = arg1;
    D_005BF870[0] = (void *)arg0;
    ((Req *)D_005BF870)->w18 = 1;
    ((Req *)D_005BF870)->w08 = arg2;
    if (arg2 >= 0x4001U) {
        ((Req *)D_005BF870)->w10 = ((Req *)D_005BF870)->w0C = 0x4000;
    } else {
        ((Req *)D_005BF870)->w0C = arg2;
        ((Req *)D_005BF870)->w10 = arg2;
        if (arg2 & 0xF) {
            do {
                ((Req *)D_005BF870)->w0C = ((Req *)D_005BF870)->w0C + 1;
            } while (((Req *)D_005BF870)->w0C & 0xF);
        }
    }
    d = (Req *)D_005BF860;
    temp_a1 = *(int *)(D_004471D4 + 0x4C8);
    d->w00 = ((Req *)D_005BF870)->w04;
    d->w04 = temp_a1;
    d->w08 = ((Req *)D_005BF870)->w0C;
    d->w0C = 0;
    FlushCache(0);
    temp_v0 = sceSifSetDma((int *)d, 1);
    ((Req *)D_005BF870)->w14 = temp_v0;
    return (temp_v0 != 0) ? 0 : -1;
}
