#include "kf_game.h"

extern PadState D_00413DA8;
extern PadState D_00413DB8[2];
extern s8 D_00349682[];
extern u8 D_00446949;
extern u8 D_0044694A;
extern u8 D_0044694B;
extern u8 D_0044694C;
extern u8 D_0044694D;
extern u8 D_0044694E;

int func_002179F8(int, int);
int func_00217970(int, int, int);
void func_00217008(void);
void func_001B5590(int);
void func_00217908(int, int, int);

/* Once a frame: reads the controller and runs the rumble that func_001B57A0 asked for. */
void func_001B5460(void) {
    int type = func_002179F8(0, 0);
    int scaled;

    if (type != 1 && type != 0xFF) {
        func_00217970(0, 0, 3);
    }
    func_00217008();
    func_001B5590(0);
    D_00413DA8 = D_00413DB8[0];
    if (D_00349682[4] != 0) {
        if (D_00446949 != 0 && D_0044694E == 0) {
            if (D_0044694B != 0) {
                if (D_0044694D != 0) {
                    scaled = D_0044694A * D_0044694B / D_0044694C;
                    if (scaled == 0) {
                        scaled = 1;
                    }
                    func_00217908(0, 0, (u8)scaled);
                } else {
                    func_00217908(0, 0, D_0044694A);
                }
                D_0044694B--;
            } else {
                func_00217908(0, 0, 0);
                D_00446949 = 0;
            }
        }
    } else if (D_00446949 != 0) {
        func_00217908(0, 0, 0);
        D_00446949 = 0;
    }
}
