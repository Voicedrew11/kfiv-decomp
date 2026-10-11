#include "common.h"

extern u8 D_0034B098[];
void *func_0011E1F0(int, int);
void func_0011E140(void *);

typedef struct {
    char *f0;
    u8 *f4;
    char *f8;
} Rec;

void func_0011A2B8(int arg0) {
    void *temp;

    if (D_0034B098[0] == 0) {
        temp = func_0011E1F0(arg0, 0x77);
        if (temp != 0) {
            func_0011E140(temp);
            if (((Rec *)temp)->f0 != 0) {
                if (*((Rec *)temp)->f4 == 9) {
                    char *s = ((Rec *)temp)->f8;
                    *(s16 *)(s + 0x44) = 0xFF;
                    *(s16 *)(s + 0x46) = 0x80;
                    *(s16 *)(s + 0x48) = 0x18;
                    *(s16 *)(s + 0x4A) = 0;
                }
            }
        }
        temp = func_0011E1F0(arg0, 0x76);
        if (temp != 0) {
            func_0011E140(temp);
        }
    }
}
