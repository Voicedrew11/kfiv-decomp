#include "kf_map.h"
#include "kf_file.h"
#include "kf_screen.h"

extern MapSlot D_0038CD20[];
extern ScreenFade D_0038D680;
extern MapSlotMemory D_004321F0[2];
extern char D_0043E720[];
extern u32 D_00446F1C;
extern char *D_00446F50;
extern char *D_00446F54;

void func_00102218(void);
int func_00102900(int);
int sprintf(char *, char *, ...);

/* Map change step: opens the new map file and starts reading all of it into the memory of slot 0. */
void func_00118F40(void) {
    char name[0x100];
    MapSlot *slot = &D_0038CD20[0];
    MapSlotMemory *memory = &D_004321F0[0];
    int size;

    sprintf(name, D_0043E720, D_0038D680.map);
    D_0038D680.file_handle = func_00102238(name, 1, 1);
    if (D_0038D680.file_handle < 0) {
        for (;;) {
        }
    }
    func_00102218();
    size = (func_00102900(D_0038D680.file_handle) + 0x3FFFF) / 0x40000 * 0x40000;
    memory->unk14 = memory->unk1C = D_00446F50;
    memory->unk18 = memory->unk20 = D_00446F54;
    slot->unk04 = size;
    memset(memory->unk1C, 0, slot->unk04);
    memset(memory->unk20, 0, D_00446F1C / 2);
    func_001025C0(D_0038D680.file_handle, memory->unk14, -1);
    D_0038D680.step++;
}
