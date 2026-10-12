#include "common.h"

void func_001FBE48(void);
extern s32 D_00446D38;
extern s32 D_00446D3C;

void func_001FBDE8(s32 value) {
    D_00446D3C = value;
    D_00446D38 = value;
    func_001FBE48();
}
