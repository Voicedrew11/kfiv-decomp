#include "kf_map.h"
#include "kf_file.h"
#include "kf_sound.h"

extern MapSlot D_0038CD20[];
extern MapDrawState D_0038A1B0;
extern SoundLoadQueue D_0038A130;
extern MapSlotMemory D_004321F0[2];
extern char D_0043E570[];
extern int D_00446770;
extern u32 D_00446F1C;
extern u32 D_00446F20;
extern char *D_00446F50;
extern char *D_00446F54;

void func_0020B2E8(void *, u32);
int func_00102900(int);
int sprintf(char *, char *, ...);
void func_00114840(int);
void func_00114AA8(int);
int func_001A10D8(MapSlot *);
void func_001155F8(int);
void func_00116F20(int);
void func_001A15F0(void);
void func_0011C708(int);
void func_001C82C0(int);
void func_001CA920(int);
void func_0011CC50(int);
void func_00124F00(int);
void func_00123158(int);
void func_001C87D8(int);
void func_00125188(int);
void func_00119360(int);
void func_001034F8(int);
void func_00210930(int);
int func_002109C8(int, int);

/* Loads a map into slot 0 in one go, sets everything in it up and makes it the current map. */
void func_00112948(int map) {
    char name[0x100];
    MapSlot *slot = &D_0038CD20[0];
    MapSlotMemory *memory = &D_004321F0[0];
    int handle;
    int size;
    MapFogColor *fog;

    ((int (*)(void *, int))func_0020B2E8)(slot, 0x18); // match: the callee returns a value here, which keeps $2 busy
    memory->unk14 = memory->unk1C = D_00446F50;
    memory->unk18 = memory->unk20 = D_00446F54;
    func_0020B2E8(memory->unk1C, D_00446F20 / 4);
    func_0020B2E8(memory->unk20, D_00446F1C / 4);
    sprintf(name, D_0043E570, map);
    handle = func_00102238(name, 0, 1);
    size = func_00102900(handle);
    func_001025C0(handle, memory->unk14, -1);
    func_001024B0(handle);
    slot->tag = map;
    slot->unk04 = (size + 0x3FFFF) / 0x40000 * 0x40000;
    func_00114840(0);
    func_00114AA8(0);
    func_001A10D8(slot);
    func_001155F8(0);
    func_00116F20(0);
    func_001A15F0();
    func_0011C708(0);
    func_001C82C0(0);
    func_001CA920(0);
    D_0038CD20[0].unk00 = 1;
    D_00446770 = 0;
    D_0038A1B0.unkA0 = *slot->unk0C;
    D_0038A1B0.unkC0 = *(MapSlotUnk10Words *)slot->unk10;
    fog = &D_0038A1B0.unk280;
    *fog = ((MapSlotUnk08 *)D_0038CD20[0].header)->unk50;
    D_0038A1B0.unk9A = 0;
    D_0038A1B0.unk9B = 0;
    D_0038A1B0.unk9C = 0;
    func_0011CC50(0);
    func_00124F00(0);
    func_00123158(0);
    func_001C87D8(0);
    func_00125188(0);
    func_00119360(0);
    func_001034F8(D_0038CD20[0].tag);
    func_00210930(D_0038CD20[0].unk5C);
    func_002109C8(0x7FFF, 0);
    D_0038A130.unk00 = ((MapSlotUnk08 *)D_0038CD20[0].header)->unk70;
    D_0038A130.unk04 = 1;
}
