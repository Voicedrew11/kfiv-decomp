#include "kf_player.h"
#include "kf_game.h"
#include "kf_map.h"

extern PlayerState D_00413F80;
extern MapSlot D_0038CD20[];
extern int D_00446770;

MapRecordInfo *func_00115CD0(CameraState *, f32, int);

/* Finds the map record the player stands in and notes its index. */
void func_001BB908(void) {
    PlayerState *player = &D_00413F80;
    MapRecordInfo *record;

    record = func_00115CD0((CameraState *)player, 100.0f, D_00446770);
    if (record != NULL) {
        // match: retail shifts by 14, not by 7 (the records are 0x80 bytes each)
        player->map_index = (u32)((char *)record - D_0038CD20[D_00446770].records) >> 14;
        player->unk250 = record->unk38;
    }
}
