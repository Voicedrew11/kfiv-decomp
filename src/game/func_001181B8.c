#include "kf_entity.h"
#include "kf_linked_point.h"
#include "kf_gold.h"
#include "kf_linked_point.h"
#include "kf_entity_world.h"
#include "kf_linked_point.h"

void func_00115878(int);
void func_001A3040(int);
void func_0011CE80(int);
void func_001C8938(int);

void func_001A15F8(void);
extern char D_0038CD20[];
extern int D_0038D610[];
extern int D_00446770;

/* Runs the pending per-slot refresh for the slot opposite the current one, then steps the counter. */
void func_001181B8(void) {
    int other_slot;
    u8 *slot_entry;

    other_slot = D_00446770 == 0;
    slot_entry = (u8 *)(D_00446770 == 0 ? D_0038CD20 + 0x60 : D_0038CD20);
    if (D_0038D610[5] != 0) {
        if (*slot_entry != 0) {
            *slot_entry = 0;
            func_00115878(other_slot);
            func_001A3040(other_slot);
            func_0011CE80(other_slot);
            KF_Entity_FreeAllInGroup(other_slot);
            KF_Entity_ClearMapSlot(other_slot);
            KF_Gold_UnlinkMap(other_slot);
            func_001C8938(other_slot);
            KF_LinkedPoint_ReleaseByKind(other_slot);
            func_001A15F8();
        }
        D_0038D610[10] = 2;
        D_0038D610[4]++;
    }
}
