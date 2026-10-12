#include "kf_entity.h"
#include "kf_map.h"

extern s32 D_00446770;
extern PlayerData D_00413F80;
extern MapSlot D_0038CD20[];

int rand(void);
void *func_00116160(int, int);
int func_002109C8(int, int);

void func_0016DFD8(Entity *entity) {
    MapSlot *slot = &D_0038CD20[D_00446770];
    MapRecord *record = &((MapRecord *)slot->records)[D_00413F80.unk252];
    s32 remaining;
    s32 chance;
    s32 i;
    s32 slots_left;
    s32 roll_range;

    if (record->unk4E != 0 && entity->group_id == D_00446770) {
        func_002109C8(record->unk4E + 10000, 0);
    }
    slots_left = 0x1D;
    i = 0;
    remaining = 2;
    roll_range = 100;
    chance = 200;
    for (; i < 0x1E; i++) {
        if (rand() % roll_range < chance / slots_left) {
            record = func_00116160(entity->group_id, i + 0x6B);
            if (record != NULL && record->unk4E != 0 && entity->group_id == D_00446770) {
                chance -= 100;
                remaining--;
                func_002109C8(record->unk4E + 10000, 0);
            }
        }
        if (remaining <= 0) {
            break;
        }
        slots_left--;
        if (slots_left <= 0) {
            break;
        }
    }
}
