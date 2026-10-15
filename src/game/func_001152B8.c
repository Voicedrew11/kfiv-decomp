#include "kf_gold.h"
#include "kf_map.h"
#include "kf_map_object.h"
#include "kf_map_scroll.h"
#include "kf_entity_world.h"
#include "kf_linked_point.h"
#include "kf_sound.h"
#include "kf_effect.h"

extern MapSlot D_0038CD20[];
extern MapLoadState D_0038D610;
extern SoundLoadQueue D_0038A130;
extern int D_00446770;

void func_001B93B8(f32 *);
void func_001D1210(f32 *);
void func_001154D8(int, int);
void func_001155F8(int);
void func_001C8990(int, f32 *);
void Script_Init(int);
int Script_Start(int, int);
void func_001BB590(void);
void Sound_StopAll(int);

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
        func_001B93B8(offset);
        func_001D1210(offset);
        func_001154D8(0, (int)offset);
        func_001154D8(1, (int)offset);
        func_001155F8(0);
        func_001155F8(1);
        MapObject_OffsetAll(0, offset);
        MapObject_OffsetAll(1, offset);
        Entity_OffsetAllInSlot(0, offset);
        Entity_OffsetAllInSlot(1, offset);
        Effect_OffsetAll(offset);
        LinkedPoint_TransformFollows(offset);
        func_001C8990(0, offset);
        func_001C8990(1, offset);
        Gold_MoveMap(0, offset);
        Gold_MoveMap(1, offset);
        D_0038D610.position.x = -D_0038D610.position.x;
        D_0038D610.position.y = -D_0038D610.position.y;
        D_0038D610.position.z = -D_0038D610.position.z;
        Script_Init(D_0038CD20[map].unk5C);
        Script_Start(0x7FFF, 0);
        loading = queue->unk10;
        if (loading != slot && queue->unk14 != slot) {
            func_001BB590();
            Sound_StopAll(1);
            queue->unk14 = slot;
        } else if (loading == slot && D_0038A130.unk14 != slot) {
            D_0038A130.unk14 = NULL;
        }
        D_0038A130.unk00 = ((MapSlotUnk08 *)D_0038CD20[map].header)->unk70;
        D_0038A130.unk04 = 1;
        MapScroll_CarryPairs(map == 0, map);
    }
}
