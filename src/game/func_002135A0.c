#include "common.h"

typedef struct {
    int unk0;
    char unk4[4];
    u32 unk8;
    char unkC[4];
    int unk10;
} Arg;

extern u8 item_counts[];
extern u8 D_0041A6F8[];

void Menu_AddKeyItem(u32, u32, int); // match: three arguments are passed; the definition takes the first alone

void func_002135A0(Arg *arg0) {
    u32 idx;
    int add;
    int v;
    int t;
    u8 *e;
    u8 *p;

    add = arg0->unk10;
    idx = arg0->unk8;
    if (idx < 0x17D) {
        p = &item_counts[idx];
        v = *p + add;
        if (v >= 0x64) {
            *p = 0x63;
        } else if (v < 0) {
            *p = 0;
        } else {
            *p = v;
        }
        if ((int)idx < 0x96) {
            t = idx << 5;
            e = D_0041A6F8 + t;
            if (*(u16 *)(e + 0xE) >> 1 & 1) {
                if (item_counts[idx] >= 2) {
                    item_counts[idx] = 1;
                }
            }
            if (D_0041A6F8[t] == 3) {
                Menu_AddKeyItem(idx, idx, t);
            }
        } else {
            if (item_counts[idx] >= 2) {
                item_counts[idx] = 1;
            }
        }
    }
    arg0->unk0 = 0;
}
