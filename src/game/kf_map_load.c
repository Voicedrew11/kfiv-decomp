#include "kf_map.h"
#include "kf_file.h"
#include "kf_player.h"
#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_gold.h"
#include "kf_linked_point.h"
#include "kf_map_object.h"
#include "kf_effect.h"
#include "kf_map_scroll.h"

extern void (*map_load_steps[])(void);
void func_00101618(void);
int func_001005E8(int);
int func_00102230(void);
void func_00102228(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_0010B000(void);
void MapLoad_FinishAndEnter(void);
void func_00123288(void);
void Sound_Update(void);
void func_001B5778(void);
void Display_SwapBuffers(int);
void Map_MakeCurrent(int);
void Map_UnlinkSlotRecords(int);
void func_001A15F8(void);
void MapItem_UnloadMap(int);
void func_00102218(void);
int func_00102900(int);
int sprintf(char *, char *, ...);
void Map_ParseFile(int);
void Map_BuildModels(int);
void Map_OffsetRecords(int, f32 *);
void Map_PlaceRecordModels(int);
void func_001A15F0(void);
void MapItem_InitMap(int);
void MapItem_MoveMap(int, f32 *);
void func_001034F8(int);
void MapLoad_Release(void);
void MapSetup_RunForMap(int);
void func_00123158(int);
void MapItem_RollFixedItems(int);
extern MapLoadState map_load_state;
extern MapLoadRequest map_load_request;
extern int D_00446794;
extern MapSlot map_slots[];
extern PlayerState player_state;
extern int D_00446770;
extern MapSlotMemory D_004321F0[2];
extern char D_0043E710[];
extern u32 D_00446F1C;
extern int D_00446F20;
extern char *D_00446F50;
extern char *D_00446F54;

/* Resets the background map load and the pending request. */
void MapLoad_Reset(void) {
    memset(&map_load_state, 0, sizeof(MapLoadState));
    map_load_state.file_handle = -1;
    map_load_state.step = 0;
    map_load_state.request = NULL;
    D_00446794 = 0;
    map_load_request.x = 0.0f;
    map_load_request.y = 0.0f;
    map_load_request.z = 0.0f;
    map_load_request.map = 0xFFFF;
}

/* Starts, queues and steps the background load of the map next to where the player stands. */
void MapLoad_Update(void) {
    MapSlot *current = &map_slots[D_00446770];
    MapSlot *other = (D_00446770 == 0) ? &map_slots[1] : &map_slots[0];
    MapRecordInfo *record;
    MapLoadRequest *request;

    if (D_00446794 != 0) {
        MapLoad_FinishAndEnter();
        return;
    }
    if (player_state.map_index != 0xFFFF) {
        record = (MapRecordInfo *)current->records + player_state.map_index;
    } else {
        record = NULL;
    }
    if (record != NULL && record->unk3A != 0xFF) {
        request = (MapLoadRequest *)&current->origins[record->unk3A];
        if (current->tag != request->map) {
            if (other->unk00 == 0 || other->tag != request->map) {
                if (map_load_state.request == NULL || map_load_state.request->map != request->map) {
                    if (map_load_state.step == 0) {
                        map_load_state.request = request;
                    } else {
                        map_load_state.next_request = request;
                    }
                }
            }
        }
    }
    if (map_load_state.next_request != NULL && map_load_state.step != 4) {
        map_load_state.step = 2;
        map_load_state.request = map_load_state.next_request;
        map_load_state.next_request = NULL;
        if (map_load_state.file_handle != -1) {
            func_001024B0(map_load_state.file_handle);
            if (func_00102230() != 0) {
                func_00102228();
            }
            map_load_state.file_handle = -1;
        }
    }
    if ((map_load_state.flags.raw & 6) == 4) {
        if (map_load_state.step == 0 || map_load_state.step == 3 || map_load_state.step == 5) {
            map_load_state.flags.f.bit1 = TRUE;
            map_load_state.flags.f.bit2 = FALSE;
        }
    }
    if (!((s32)(map_load_state.flags.raw >> 1) & 1)) {
        if (record != NULL && record->unk39 != 0) {
            if ((u32)(map_load_state.step - 1) < 5) {
                func_001B5778();
                while (map_load_state.step != 5) {
                    func_001005E8(0);
                    func_00109800();
                    Display_SwapBuffers(0);
                    func_001096E8();
                    func_001095E0();
                    func_0010B000();
                    map_load_steps[map_load_state.step]();
                    func_00123288();
                    func_00101618();
                    Sound_Update();
                }
                map_load_state.step = 6;
            }
        }
        map_load_steps[map_load_state.step]();
    }
}

/* Finishes the running map load at once, makes the loaded map current, puts the player at its start
   and frees the old map. */
void MapLoad_FinishAndEnter(void) {
    while (map_load_state.step != 5) {
        func_001005E8(0);
        func_00109800();
        Display_SwapBuffers(0);
        func_001096E8();
        func_001095E0();
        func_0010B000();
        map_load_steps[map_load_state.step]();
    }
    map_load_state.step = 6;
    do {
        map_load_steps[map_load_state.step]();
    } while (map_load_state.step != 0);
    Map_MakeCurrent(D_00446770 == 0);
    player_state.position.x = ((MapSlotUnk08 *)map_slots[D_00446770].header)->unk60;
    player_state.position.y = ((MapSlotUnk08 *)map_slots[D_00446770].header)->unk64;
    player_state.position.z = ((MapSlotUnk08 *)map_slots[D_00446770].header)->unk68;
    player_state.rotation.y = ((MapSlotUnk08 *)map_slots[D_00446770].header)->unk6C;
    map_load_request.map = 0xFFFF;
    D_00446794 = 0;
    player_state.unk1AC = player_state.unk1B0 = player_state.unk1B4 = 0.0f;
    player_state.unk1B8 = player_state.unk1BC = 0.0f;
    map_load_request.x = map_load_request.z = map_load_request.y = 0.0f;
    PLAYER_STATUS_BITS(&player_state)->bit34 = FALSE;
    map_slots[D_00446770 == 0].unk00 = 0;
    Map_UnlinkSlotRecords(D_00446770 == 0);
    Effect_FreeForMap(D_00446770 == 0);
    MapObject_UnloadMap(D_00446770 == 0);
    Entity_FreeAllInGroup(D_00446770 == 0);
    Entity_ClearMapSlot(D_00446770 == 0);
    Gold_UnlinkMap(D_00446770 == 0);
    MapItem_UnloadMap(D_00446770 == 0);
    LinkedPoint_ReleaseByKind(D_00446770 == 0);
    func_001A15F8();
}

/* Map load step 0: once a load is asked for, frees what the other map slot holds and goes on to wait two
   frames. */
void MapLoad_StartRequest(void) {
    int other_map;
    MapSlot *other;

    other_map = D_00446770 == 0;
    other = D_00446770 == 0 ? &map_slots[1] : &map_slots[0];
    if (map_load_state.request != NULL) {
        if (other->unk00 != 0) {
            other->unk00 = 0;
            Map_UnlinkSlotRecords(other_map);
            Effect_FreeForMap(other_map);
            MapObject_UnloadMap(other_map);
            Entity_FreeAllInGroup(other_map);
            Entity_ClearMapSlot(other_map);
            Gold_UnlinkMap(other_map);
            MapItem_UnloadMap(other_map);
            LinkedPoint_ReleaseByKind(other_map);
            func_001A15F8();
        }
        map_load_state.timer = 2;
        map_load_state.step++;
    }
}

/* Map load step: waits for the timer to run out. */
void MapLoad_WaitTimer(void) {
    if (--map_load_state.timer <= 0) {
        map_load_state.step++;
    }
}

/* Map load step: once the drive is free, opens the requested map file and picks the memory it loads into. */
void MapLoad_OpenFile(void) {
    char name[0x100];
    MapSlotMemory *memory = (D_00446770 == 0) ? &D_004321F0[1] : &D_004321F0[0];
    MapSlot *other = (D_00446770 == 0) ? &map_slots[1] : &map_slots[0];
    MapSlot *current = &map_slots[D_00446770];
    u32 limit;

    if (func_00102230() == 0) {
        sprintf(name, D_0043E710, map_load_state.request->map);
        do {
            map_load_state.file_handle = func_00102238(name, 1, 1);
        } while (map_load_state.file_handle < 0);
        func_00102218();
        other->unk04 = (func_00102900(map_load_state.file_handle) + 0x3FFFF) / 0x40000 * 0x40000;
        limit = D_00446F20;
        if (limit < (u32)(other->unk04 + current->unk04)) {
            other->tag = map_load_state.request->map;
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
        map_load_state.step++;
    }
}

/* Map load step: reads the next 0x80000 bytes of the map file, closing it at the end. */
void MapLoad_ReadChunk(void) {
    MapSlotMemory *memory = &D_004321F0[0];

    if (D_00446770 == 0) {
        memory = &D_004321F0[1];
    }
    if (func_001025C0(map_load_state.file_handle, memory->unk14, 0x80000) > 0) {
        memory->unk14 += 0x80000;
        map_load_state.step++;
    } else {
        func_001024B0(map_load_state.file_handle);
        func_00102228();
        map_load_state.file_handle = -1;
        memory->unk14 = memory->unk1C;
        map_load_state.step += 2;
    }
}

/* Map load step: once the read in flight is done, goes back to read the next piece. */
void MapLoad_WaitForChunk(void) {
    if (func_00102790(map_load_state.file_handle, 1) == 0) {
        map_load_state.step--;
    }
}

/* Map load step 5: empty; the load rests here until MapLoad_Update or MapLoad_FinishAndEnter moves it on. */
void func_00118538(void) {
}

/* Map load step: sets up the loaded map in the other slot, shifted so that it joins the current one. */
void MapLoad_SetUpMap(void) {
    int other_slot = D_00446770 == 0;
    f32 offset[4];

    offset[0] = -map_load_state.request->x;
    offset[1] = -map_load_state.request->y;
    offset[2] = -map_load_state.request->z;
    offset[3] = 0.0f;
    if (D_00446794 == 0) {
        map_slots[other_slot].tag = map_load_state.request->map;
    } else {
        map_slots[other_slot].tag = map_load_request.map;
    }
    Map_ParseFile(other_slot);
    Map_BuildModels(other_slot);
    Map_OffsetRecords(other_slot, offset);
    Map_PlaceRecordModels(other_slot);
    MapScroll_ClearPairs(other_slot);
    func_001A15F0();
    MapObject_InitMap(other_slot);
    MapObject_OffsetAll(other_slot, offset);
    MapItem_InitMap(other_slot);
    MapItem_MoveMap(other_slot, offset);
    Gold_InitMap(other_slot);
    Gold_MoveMap(other_slot, offset);
    map_load_state.step++;
}

/* Map load step: nothing to wait for, goes on. */
void MapLoad_SkipStep(void) {
    map_load_state.step++;
}

/* Map load step: brings the other slot to life and ends the load. */
void MapLoad_ActivateMap(void) {
    MapSlot *slot = &map_slots[0];
    int other_slot;
    f32 offset[4];

    if (D_00446770 == 0) {
        slot = &map_slots[1];
    }
    other_slot = D_00446770 == 0;
    offset[0] = -map_load_state.request->x;
    offset[1] = -map_load_state.request->y;
    offset[2] = -map_load_state.request->z;
    offset[3] = 0.0f;
    slot->unk00 = 1;
    MapObject_StartMap(other_slot);
    Entity_InitFromMapSpawns(other_slot);
    Entity_OffsetAllInSlot(other_slot, offset);
    func_00123158(other_slot);
    MapItem_RollFixedItems(other_slot);
    Entity_ClearFlag10InSlot(other_slot);
    MapScroll_CarryPairs(D_00446770, other_slot);
    MapSetup_RunForMap(other_slot);
    func_001034F8(slot->tag);
    map_load_state.position.x = map_load_state.request->x;
    map_load_state.position.y = map_load_state.request->y;
    map_load_state.position.z = map_load_state.request->z;
    map_load_state.position.w = 0.0f;
    map_load_state.step = 0;
    map_load_state.request = NULL;
    MapLoad_Release();
}

/* Asks the background load to stop at its next safe step (MapLoad_Update holds it there). */
void MapLoad_RequestHold(void) {
    if (!(map_load_state.flags.raw & 6)) {
        map_load_state.flags.raw |= 4;
    }
}

/* Lets a held or hold-requested background load go on. */
void MapLoad_Release(void) {
    map_load_state.flags.f.bit1 = FALSE;
    map_load_state.flags.f.bit2 = FALSE;
}

/* Whether the background load is held off. */
int MapLoad_IsHeld(void) {
    // match: bit 1 of the flags taken out with shifts; the bit-field reads it another way
    return (int)(((s64)map_load_state.flags.raw << 31) >> 32) & 1;
}
