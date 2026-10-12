#include "kf_map.h"
#include "kf_sound.h"

extern MapSlot D_0038CD20[];
extern MapLoadState D_0038D610;
extern SoundLoadQueue D_0038A130;
extern int D_00446770;

void func_001B93B8(char *);
void func_001D1210(void *);
void func_001154D8(int, int);
void func_001155F8(int);
void func_0011D0D0(int, int);
void func_00125620(int, f32 *);
void func_001A2F20(f32 *);
void func_0019B150(void *);
void func_001C8990(int, f32 *);
void func_001CACF8(int, f32 *);
void func_00210930(int);
int func_002109C8(int, int);
void func_001BB590(void);
void func_0019EA98(int);
void func_00117AA8(int, int);

/* Makes the other map slot the current one: moves everything by the load's offset and switches
   the sound over. */
void func_001152B8(int map) {
    MapSlot *slot;
    SoundLoadQueue *queue;
    f32 *offset;
    MapSlot *loading;

    if (D_00446770 != map) {
        offset = &D_0038D610.position.x;
        slot = &D_0038CD20[map];
        queue = &D_0038A130;
        D_00446770 = map;
        func_001B93B8((char *)offset);
        func_001D1210(offset);
        func_001154D8(0, (int)offset);
        func_001154D8(1, (int)offset);
        func_001155F8(0);
        func_001155F8(1);
        func_0011D0D0(0, (int)offset);
        func_0011D0D0(1, (int)offset);
        func_00125620(0, offset);
        func_00125620(1, offset);
        func_001A2F20(offset);
        func_0019B150(offset);
        func_001C8990(0, offset);
        func_001C8990(1, offset);
        func_001CACF8(0, offset);
        func_001CACF8(1, offset);
        D_0038D610.position.x = -D_0038D610.position.x;
        D_0038D610.position.y = -D_0038D610.position.y;
        D_0038D610.position.z = -D_0038D610.position.z;
        func_00210930(D_0038CD20[map].unk5C);
        func_002109C8(0x7FFF, 0);
        loading = queue->unk10;
        if (loading != slot && queue->unk14 != slot) {
            func_001BB590();
            func_0019EA98(1);
            queue->unk14 = slot;
        } else if (loading == slot && D_0038A130.unk14 != slot) {
            D_0038A130.unk14 = NULL;
        }
        D_0038A130.unk00 = ((MapSlotUnk08 *)D_0038CD20[map].header)->unk70;
        D_0038A130.unk04 = 1;
        func_00117AA8(map == 0, map);
    }
}
