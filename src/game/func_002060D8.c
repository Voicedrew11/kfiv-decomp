#include "common.h"

extern float D_00446C04;
extern char D_00446C08;
typedef struct {
    s64 w[12];
} Blk;

typedef struct {
    char pad[0x20];
    u8 b0, b1, b2, b3;
} Out;

extern Blk D_005DDAC0[];

int *func_002060D8(Blk *arg0, int arg1) {
    D_005DDAC0[0] = *arg0;

    if (((char *)arg0)[0x3F] == arg1) {
        if (D_00446C08 == 0) {
            D_00446C04 = D_00446C04 + 0.8f;
            if (D_00446C04 > 128.0f) {
                D_00446C04 = 128.0f;
                D_00446C08 = 1;
            }
        } else {
            D_00446C04 = D_00446C04 - 0.8f;
            if (D_00446C04 < 80.0f) {
                D_00446C04 = 80.0f;
                D_00446C08 = 0;
            }
        }
        ((Out *)D_005DDAC0)->b0 = 0xFF;
        ((Out *)D_005DDAC0)->b3 = (char)D_00446C04;
        ((Out *)D_005DDAC0)->b2 = 0x78;
        ((Out *)D_005DDAC0)->b1 = 0xFF;
    }
    return (int *)D_005DDAC0;
}
