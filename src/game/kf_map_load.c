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
void GameLoop_RunSoundLoads(void);
int GameLoop_DrawFrame(int);
int File_IsLocked(void);
void File_Unlock(void);
void Packet_SwapBuffers(void);
void Packet_SendChain(void);
void Packet_WaitForSend(void);
void DrawList_Clear(void);
void MapLoad_FinishAndEnter(void);
void Entity_UpdatePool(void);
void Sound_Update(void);
void Input_StopRumble(void);
void Display_SwapBuffers(int);
void Map_MakeCurrent(int);
void Map_UnlinkSlotRecords(int);
void func_001A15F8(void);
void MapItem_UnloadMap(int);
void File_Lock(void);
int File_GetSize(int);
int sprintf(char *, char *, ...);
void Map_ParseFile(int);
void Map_BuildModels(int);
void Map_OffsetRecords(int, f32 *);
void Map_PlaceRecordModels(int);
void func_001A15F0(void);
void MapItem_InitMap(int);
void MapItem_MoveMap(int, f32 *);
void Game_MarkMapVisited(int);
void MapLoad_Release(void);
void MapSetup_RunForMap(int);
void Entity_AddGroupToWaiting(int);
void MapItem_RollFixedItems(int);
extern MapLoadState map_load_state;
extern MapLoadRequest map_load_request;
extern int map_load_committed;
extern MapSlot map_slots[];
extern PlayerState player_state;
extern int current_map_slot;
extern MapSlotMemory map_slot_memory[2];
extern char map_load_file_name_format[];
extern u32 map_work_area_size;
extern int map_file_area_size;
extern char *map_file_area;
extern char *map_work_area;

/* Resets the background map load and the pending request. */
void MapLoad_Reset(void) {
    memset(&map_load_state, 0, sizeof(MapLoadState));
    map_load_state.file_handle = -1;
    map_load_state.step = 0;
    map_load_state.request = NULL;
    map_load_committed = 0;
    map_load_request.x = 0.0f;
    map_load_request.y = 0.0f;
    map_load_request.z = 0.0f;
    map_load_request.map = 0xFFFF;
}

/* Starts, queues and steps the background load of the map next to where the player stands. */
void MapLoad_Update(void) {
    MapSlot *current = &map_slots[current_map_slot];
    MapSlot *other = (current_map_slot == 0) ? &map_slots[1] : &map_slots[0];
    MapRecordInfo *record;
    MapLoadRequest *request;

    if (map_load_committed != 0) {
        MapLoad_FinishAndEnter();
        return;
    }
    if (player_state.map_index != 0xFFFF) {
        record = (MapRecordInfo *)current->records + player_state.map_index;
    } else {
        record = NULL;
    }
    if (record != NULL && record->neighbour_origin_index != 0xFF) {
        request = (MapLoadRequest *)&current->origins[record->neighbour_origin_index];
        if (current->tag != request->map) {
            if (other->loaded == 0 || other->tag != request->map) {
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
            File_Close(map_load_state.file_handle);
            if (File_IsLocked() != 0) {
                File_Unlock();
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
        if (record != NULL && record->needs_neighbour_map != 0) {
            if ((u32)(map_load_state.step - 1) < 5) {
                Input_StopRumble();
                while (map_load_state.step != 5) {
                    GameLoop_DrawFrame(0);
                    Packet_WaitForSend();
                    Display_SwapBuffers(0);
                    Packet_SendChain();
                    Packet_SwapBuffers();
                    DrawList_Clear();
                    map_load_steps[map_load_state.step]();
                    Entity_UpdatePool();
                    GameLoop_RunSoundLoads();
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
        GameLoop_DrawFrame(0);
        Packet_WaitForSend();
        Display_SwapBuffers(0);
        Packet_SendChain();
        Packet_SwapBuffers();
        DrawList_Clear();
        map_load_steps[map_load_state.step]();
    }
    map_load_state.step = 6;
    do {
        map_load_steps[map_load_state.step]();
    } while (map_load_state.step != 0);
    Map_MakeCurrent(current_map_slot == 0);
    player_state.position.x = ((MapSlotUnk08 *)map_slots[current_map_slot].header)->player_start_x;
    player_state.position.y = ((MapSlotUnk08 *)map_slots[current_map_slot].header)->player_start_y;
    player_state.position.z = ((MapSlotUnk08 *)map_slots[current_map_slot].header)->player_start_z;
    player_state.rotation.y = ((MapSlotUnk08 *)map_slots[current_map_slot].header)->player_start_heading;
    map_load_request.map = 0xFFFF;
    map_load_committed = 0;
    player_state.fall_speed = player_state.strafe_speed = player_state.walk_speed = 0.0f;
    player_state.look_speed = player_state.turn_speed = 0.0f;
    map_load_request.x = map_load_request.z = map_load_request.y = 0.0f;
    PLAYER_STATUS_BITS(&player_state)->bit34 = FALSE;
    map_slots[current_map_slot == 0].loaded = 0;
    Map_UnlinkSlotRecords(current_map_slot == 0);
    Effect_FreeForMap(current_map_slot == 0);
    MapObject_UnloadMap(current_map_slot == 0);
    Entity_FreeAllInGroup(current_map_slot == 0);
    Entity_ClearMapSlot(current_map_slot == 0);
    Gold_UnlinkMap(current_map_slot == 0);
    MapItem_UnloadMap(current_map_slot == 0);
    LinkedPoint_ReleaseByKind(current_map_slot == 0);
    func_001A15F8();
}

/* Map load step 0: once a load is asked for, frees what the other map slot holds and goes on to wait two
   frames. */
void MapLoad_StartRequest(void) {
    int other_map;
    MapSlot *other;

    other_map = current_map_slot == 0;
    other = current_map_slot == 0 ? &map_slots[1] : &map_slots[0];
    if (map_load_state.request != NULL) {
        if (other->loaded != 0) {
            other->loaded = 0;
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
    MapSlotMemory *memory = (current_map_slot == 0) ? &map_slot_memory[1] : &map_slot_memory[0];
    MapSlot *other = (current_map_slot == 0) ? &map_slots[1] : &map_slots[0];
    MapSlot *current = &map_slots[current_map_slot];
    u32 limit;

    if (File_IsLocked() == 0) {
        sprintf(name, map_load_file_name_format, map_load_state.request->map);
        do {
            map_load_state.file_handle = File_Open(name, 1, 1);
        } while (map_load_state.file_handle < 0);
        File_Lock();
        other->rounded_file_size = (File_GetSize(map_load_state.file_handle) + 0x3FFFF) / 0x40000 * 0x40000;
        limit = map_file_area_size;
        if (limit < (u32)(other->rounded_file_size + current->rounded_file_size)) {
            other->tag = map_load_state.request->map;
            for (;;) {
            }
        }
        if (current_map_slot != 0) {
            memory->file_base = map_file_area;
            memory->work_base = map_work_area;
            memory->file_read_cursor = map_file_area;
            memory->work_cursor = map_work_area;
        } else {
            memory->file_read_cursor = memory->file_base = map_file_area + limit - other->rounded_file_size;
            memory->work_cursor = memory->work_base = map_work_area + map_work_area_size / 2;
        }
        memset(memory->file_base, 0, other->rounded_file_size);
        memset(memory->work_base, 0, map_work_area_size / 2);
        map_load_state.step++;
    }
}

/* Map load step: reads the next 0x80000 bytes of the map file, closing it at the end. */
void MapLoad_ReadChunk(void) {
    MapSlotMemory *memory = &map_slot_memory[0];

    if (current_map_slot == 0) {
        memory = &map_slot_memory[1];
    }
    if (File_Read(map_load_state.file_handle, memory->file_read_cursor, 0x80000) > 0) {
        memory->file_read_cursor += 0x80000;
        map_load_state.step++;
    } else {
        File_Close(map_load_state.file_handle);
        File_Unlock();
        map_load_state.file_handle = -1;
        memory->file_read_cursor = memory->file_base;
        map_load_state.step += 2;
    }
}

/* Map load step: once the read in flight is done, goes back to read the next piece. */
void MapLoad_WaitForChunk(void) {
    if (File_Sync(map_load_state.file_handle, 1) == 0) {
        map_load_state.step--;
    }
}

/* Map load step 5: empty; the load rests here until MapLoad_Update or MapLoad_FinishAndEnter moves it on. */
void MapLoad_WaitForSetUp(void) {
}

/* Map load step: sets up the loaded map in the other slot, shifted so that it joins the current one. */
void MapLoad_SetUpMap(void) {
    int other_slot = current_map_slot == 0;
    f32 offset[4];

    offset[0] = -map_load_state.request->x;
    offset[1] = -map_load_state.request->y;
    offset[2] = -map_load_state.request->z;
    offset[3] = 0.0f;
    if (map_load_committed == 0) {
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

    if (current_map_slot == 0) {
        slot = &map_slots[1];
    }
    other_slot = current_map_slot == 0;
    offset[0] = -map_load_state.request->x;
    offset[1] = -map_load_state.request->y;
    offset[2] = -map_load_state.request->z;
    offset[3] = 0.0f;
    slot->loaded = 1;
    MapObject_StartMap(other_slot);
    Entity_InitFromMapSpawns(other_slot);
    Entity_OffsetAllInSlot(other_slot, offset);
    Entity_AddGroupToWaiting(other_slot);
    MapItem_RollFixedItems(other_slot);
    Entity_ClearFlag10InSlot(other_slot);
    MapScroll_CarryPairs(current_map_slot, other_slot);
    MapSetup_RunForMap(other_slot);
    Game_MarkMapVisited(slot->tag);
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
