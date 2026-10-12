#include "kf_map.h"
#include "kf_file.h"

extern MapSlot D_0038CD20[];
extern MapLoadState D_0038D610;
extern MapSlotMemory D_004321F0[2];
extern char D_0043E710[];
extern int D_00446770;
extern u32 D_00446F1C;
extern u32 D_00446F20;
extern char *D_00446F50;
extern char *D_00446F54;

int func_00102230(void);
void func_00102218(void);
int func_00102900(int);
int sprintf(char *, char *, ...);

/* Map load step: once the drive is free, opens the requested map file and picks the memory it loads into. */
void func_001182A0(void) {
    char name[0x100];
    MapSlotMemory *memory = (D_00446770 == 0) ? &D_004321F0[1] : &D_004321F0[0];
    MapSlot *other = (D_00446770 == 0) ? &D_0038CD20[1] : &D_0038CD20[0];
    MapSlot *current = &D_0038CD20[D_00446770];
    u32 limit;

    if (func_00102230() == 0) {
        sprintf(name, D_0043E710, D_0038D610.request->map);
        do {
            D_0038D610.file_handle = func_00102238(name, 1, 1);
        } while (D_0038D610.file_handle < 0);
        func_00102218();
        other->unk04 = (func_00102900(D_0038D610.file_handle) + 0x3FFFF) / 0x40000 * 0x40000;
        limit = D_00446F20;
        if (limit < (u32)(other->unk04 + current->unk04)) {
            other->tag = D_0038D610.request->map;
            for (;;) {
            }
        }
        if (D_00446770 != 0) {
            memory->unk1C = D_00446F50;
            memory->unk20 = D_00446F54;
            memory->unk14 = D_00446F50;
            memory->unk18 = D_00446F54;
        } else {
            memory->unk14 = memory->unk1C = D_00446F50 + limit - other->unk04;
            memory->unk18 = memory->unk20 = D_00446F54 + D_00446F1C / 2;
        }
        memset(memory->unk1C, 0, other->unk04);
        memset(memory->unk20, 0, D_00446F1C / 2);
        D_0038D610.step++;
    }
}
