#include "common.h"

void func_00216788(void *, void *);
extern s32 D_0044717C;
extern s32 D_00447184;
extern u8 D_0057DB40[];

void func_001C08A0(void *dst, s32 index) {
    s32 table_index;

    if (index < D_0044717C) {
        table_index = D_00447184 + index;
        func_00216788(dst, &D_0057DB40[(table_index % 64) * 16]);
    }
}
