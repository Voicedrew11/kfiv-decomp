#include "kf_gs.h"
#include "kf_menu.h"

extern s32 D_00446F38;
extern void **D_005CE840[];

s32 func_001CFC58(s32);
int func_001FE350(int, int);
s32 func_00207320(int, s64);

/* Looks up the table entry for index in the current node, and if it is set, sets TEST_1 and submits it. */
int func_001E44D0(s32 index) {
    DataNode *node;
    s32 table;
    s32 slot;
    s32 entry;
    GsPackedWord word;

    node = (DataNode *)*D_005CE840[0];
    table = func_001CFC58(D_00446F38);
    slot = node->unk0C + 1;
    entry = ((s32 *)table)[index + slot];
    word.raw = 0x000A87507E9080C0;
    func_001FDB88(word);
    if (entry != 0) {
        func_00207320(GS_REG_TEST_1, 0x30000);
        return func_001FE350(table + entry, 0x1B);
    }
}
