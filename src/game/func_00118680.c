#include "kf_map.h"
#include "kf_entity_world.h"
#include "kf_map_object.h"
#include "kf_map_scroll.h"

extern MapSlot D_0038CD20[];
extern MapLoadState D_0038D610;
extern int D_00446770;

void func_001034F8(int);
void func_001187B0(void);
void func_00119360(int);
void func_00123158(int);
void func_001C87D8(int);

/* Map load step: brings the other slot to life and ends the load. */
void func_00118680(void) {
    MapSlot *slot = &D_0038CD20[0];
    int other_slot;
    f32 offset[4];

    if (D_00446770 == 0) {
        slot = &D_0038CD20[1];
    }
    other_slot = D_00446770 == 0;
    offset[0] = -D_0038D610.request->x;
    offset[1] = -D_0038D610.request->y;
    offset[2] = -D_0038D610.request->z;
    offset[3] = 0.0f;
    slot->unk00 = 1;
    MapObject_StartMap(other_slot);
    Entity_InitFromMapSpawns(other_slot);
    Entity_OffsetAllInSlot(other_slot, offset);
    func_00123158(other_slot);
    func_001C87D8(other_slot);
    Entity_ClearFlag10InSlot(other_slot);
    MapScroll_CarryPairs(D_00446770, other_slot);
    func_00119360(other_slot);
    func_001034F8(slot->tag);
    D_0038D610.position.x = D_0038D610.request->x;
    D_0038D610.position.y = D_0038D610.request->y;
    D_0038D610.position.z = D_0038D610.request->z;
    D_0038D610.position.w = 0.0f;
    D_0038D610.step = 0;
    D_0038D610.request = NULL;
    func_001187B0();
}
