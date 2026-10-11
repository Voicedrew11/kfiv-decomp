#include "common.h"

int func_0019F5A8(int *, int);
extern int D_003EF5B8[];
extern int D_0044685C;
extern u8 D_004470E4;
extern u8 D_004470E5;

int func_0019F628(int arg0, int arg1) {
    int var_s0;
    int ret;

    var_s0 = arg1;
    D_0044685C = 1;
    if (var_s0 != 0) {
        if ((D_004470E4 == arg0) && (D_004470E5 == 0)) {
            var_s0 = 0;
        } else {
            D_0044685C = 0;
            ret = func_0019F5A8(D_003EF5B8, arg0);
            D_004470E5 = 1;
            if (ret == 0) {
                D_004470E4 = arg0;
                var_s0 = 0;
                D_004470E5 = 0;
            }
        }
    }
    return var_s0;
}
