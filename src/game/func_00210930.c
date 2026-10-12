#include "common.h"
#include "kf_script.h"

void *memset(void *, int, u32);
void func_002108F8(void *, int, int);
extern int D_004473E4;
extern int D_004473E8;
extern char D_01DFAFB0[];
extern ScriptSlot D_01DFB3B0[];

void func_00210930(int new_value) {
    ScriptSlot *slot;
    u8 *cursor;
    u8 *end;

    D_004473E8 = new_value;
    D_004473E4 = 0;
    memset(D_01DFAFB0, 0, 0x400);
    slot = D_01DFB3B0;
    cursor = slot->commands;
    end = (u8 *)slot + 0x12720;
    do {
        slot->unk04 = D_004473E8;
        func_002108F8(cursor, 0x29, 0);
        slot->active = 0;
        slot++;
        cursor += sizeof(ScriptSlot);
    } while ((s32)slot < (s32)end);
}
