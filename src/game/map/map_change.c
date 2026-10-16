#include "map/map.h"
#include "player/player.h"
#include "gfx/screen.h"
#include "core/game.h"
#include "ui/splash.h"
#include "gfx/gs.h"
#include "gfx/packet.h"
#include "entity/entity.h"
#include "entity/entity_world.h"
#include "item/gold.h"
#include "model/linked_point.h"
#include "map_object/map_object.h"
#include "effect/effect.h"
#include "core/file.h"
#include "map/map_scroll.h"
#include "sound/sound.h"

extern void (*screen_fade_steps[])(void);
extern void (*screen_fade_draw_steps[])(void);
int Game_FindTriggerOfItem(int, int *, int *);
void GameLoop_RunSoundLoads(void);
void GameLoop_CancelMenuRequests(void);
void Packet_SwapBuffers(void);
void Packet_SendChain(void);
void Packet_WaitForSend(void);
void DrawList_Clear(void);
void MapChange_CaptureFrame(void);
void Input_Update(void);
void Display_SwapBuffers(int);
int GameLoop_DrawFrame(int);
void GsDraw_CopyFrame(int);
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int, s64);
void Packet_EndGsRegisters(void);
void GsDraw_QueueTexturedSprite(void *);
void GsDraw_FillScreen(int, int, int, int, int);
void Map_DrawRecords(int);
int File_IsLocked(void);
void MapLoad_Update(void);
int EntityPool_IsLoading(void);
void Entity_UpdatePool(void);
void Sound_Update(void);
void Map_UnlinkSlotRecords(int);
void MapLoad_Reset(void);
void func_001A15F8(void);
void Player_EndSwingHits(void);
void MapItem_UnloadMap(int);
void File_Lock(void);
void File_Unlock(void);
int File_GetSize(int);
int sprintf(char *, char *, ...);
float cosf(float);
float sinf(float);
void Game_MarkMapVisited(int);
void Game_StartFadeIn(int, int, int, int);
void Map_ParseFile(int);
void Map_BuildModels(int);
void Map_PlaceRecordModels(int);
void MapSetup_RunForMap(int);
void Entity_AddGroupToWaiting(int);
void Sound_StopAll(int);
void func_001A15F0(void);
void MapItem_InitMap(int);
void MapItem_RollFixedItems(int);
void Script_Init(int);
int Script_Start(int, int);
void Vector_Copy(void *, void *);
extern int current_map_slot;
extern MapSlot map_slots[2];
extern ScreenFade screen_fade;
extern PlayerState player_state;
extern GameStatus game_status;
extern MapLoadState map_load_state;
extern MapProp *map_slot_props[2][0xFF];
extern MapDrawRecord *record_draw_lists[2];
extern MapDrawRecord *late_record_draw_lists[2];
extern AreaNode *record_boxes;
extern AreaNode *backdrop_record_boxes;
extern MapSlotMemory map_slot_memory[2];
extern char map_change_file_name_format[];
extern u32 map_work_area_size;
extern char *map_file_area;
extern char *map_work_area;
extern SoundLoadQueue sound_load_queue;

/* Starts the change to map new_value, the player to stand at the map's start, unless a change is running
   or it is the current map. */
void MapChange_StartToMap(int new_value) {
    MapSlot *slot;

    slot = &map_slots[current_map_slot];
    if (screen_fade.step == FALSE && slot->tag != new_value) {
        screen_fade.step = TRUE;
        screen_fade.timer = 0x1E;
        screen_fade.map = new_value;
        screen_fade.draw_step = 0;
        screen_fade.kind = 0;
    }
}

/* Moves the player to position in the current map, or starts the change to another map. */
void MapChange_StartToPosition(int map, Vector4 *position) {
    MapSlot *slot = &map_slots[current_map_slot];

    if (screen_fade.step == 0) {
        if (slot->tag == map) {
            if (position != NULL) {
                player_state.position.x = position->x;
                player_state.position.y = position->y;
                player_state.position.z = position->z;
            }
        } else {
            screen_fade.map = map;
            screen_fade.position.x = position->x;
            screen_fade.position.y = position->y;
            screen_fade.position.z = position->z;
            screen_fade.timer = 30;
            screen_fade.step = 1;
            screen_fade.kind = 1;
            screen_fade.draw_step = 0;
            screen_fade.position.w = 0.0f;
        }
    }
}

/* Starts the passage a wand used on a Symbol of Passage gives: the change to the map object where the item
   five below the wand was placed (Game_FindTriggerOfItem). FALSE when a change runs or it was not placed. */
int MapChange_StartPassage(int entry) {
    int target[2];
    int map;

    if (screen_fade.step != 0) {
        return FALSE;
    }
    if (Game_FindTriggerOfItem(entry - 5, &target[0], &target[1]) == 0) {
        return FALSE;
    }
    map = target[0];
    screen_fade.step = 1;
    screen_fade.unk14 = target[1];
    screen_fade.timer = 30;
    screen_fade.map = map;
    screen_fade.kind = 2;
    screen_fade.draw_step = 0;
    return TRUE;
}

/* Runs a pending map change to the end, drawing its own frames meanwhile. */
void MapChange_RunToEnd(void) {
    if (screen_fade.step != 0) {
        MapChange_CaptureFrame();
        GameLoop_CancelMenuRequests();
        Splash_Abort();
        do {
            Input_Update();
            Display_SwapBuffers(0);
            Packet_SendChain();
            Packet_SwapBuffers();
            DrawList_Clear();
            screen_fade_draw_steps[screen_fade.draw_step]();
            screen_fade_steps[screen_fade.step]();
            GameLoop_RunSoundLoads();
            ((u8 (*)(void))Packet_WaitForSend)(); // match: called as returning a value, so the loop test uses $3
        } while (screen_fade.step != 0);
        game_status.bit2 = FALSE;
        game_status.save_requested = FALSE;
    }
}

/* Draws one frame of the game and copies the picture to GS block 0x2BC0, where
   MapChange_FadeOutCapture fades it out. */
void MapChange_CaptureFrame(void) {
    Display_SwapBuffers(0);
    Packet_SendChain();
    Packet_SwapBuffers();
    DrawList_Clear();
    GameLoop_DrawFrame(0);
    GsDraw_CopyFrame(0x2BC0);
    Packet_WaitForSend();
}

/* Map change drawing step 0: draws the captured picture in ten strips, darker each frame, until the
   timer runs out. */
void MapChange_FadeOutCapture(void) {
    ScreenQuad quad;
    int remaining = 9;
    int total;

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x88);
    Packet_EndGsRegisters();
    total = 30;
    quad.tag = 0x26802ABC0L;
    quad.unk10 = 0x156;
    quad.first_x = 0x6BF8;
    quad.first_y = 0x71F8;
    quad.second_x = 0x7000;
    quad.second_y = 0x8E00;
    quad.unk08 = 0;
    quad.second_z = quad.first_z = 0xFFFFFFFF;
    quad.second_u = 0x400;
    quad.second_v = 0x1C00;
    quad.first_u = 0;
    quad.first_v = 0;
    quad.red = quad.green = quad.blue = (screen_fade.timer << 7) / total;
    quad.alpha = 0x80;
    do {
        remaining--;
        GsDraw_QueueTexturedSprite(&quad);
        quad.first_x += 0x400;
        quad.second_x += 0x400;
        quad.first_u += 0x400;
        quad.second_u += 0x400;
    } while (remaining >= 0);
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
    Packet_EndGsRegisters();
    if (--screen_fade.timer <= 0) {
        screen_fade.draw_step++;
    }
}

/* Map change drawing step: keeps the screen black. */
void MapChange_DrawBlack(void) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
    Packet_EndGsRegisters();
    GsDraw_FillScreen(0, 0, 0, 0x80, 0x88);
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
    Packet_EndGsRegisters();
}

/* Map change drawing step 2: draws the map records under a black cover that thickens over 30 frames, and
   counts the timer down. Nothing sets this step. */
void MapChange_FadeMapToBlack(void) {
    Map_DrawRecords(0);
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x88);
    Packet_EndGsRegisters();
    {
        int total = 30; /* two separate 30s, as retail keeps both */
        int span = 30;
        GsDraw_FillScreen(0, 0, 0, ((total - screen_fade.timer) << 7) / span, 0x88);
    }
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
    Packet_EndGsRegisters();
    screen_fade.timer--;
}

/* Map change step 0: empty, no change is running. */
void MapChange_Idle(void) {
}

/* Map change step: lets the background load and everything else in flight settle. */
void MapChange_WaitForIdle(void) {
    MapLoad_Update();
    Entity_UpdatePool();
    Sound_Update();
    if (map_load_state.step == 0 || map_load_state.step == 5) {
        if (EntityPool_IsLoading() == 0 && sound_load_queue.map_bank_load_pending == 0 && sound_load_queue.bgm_load_pending == 0 && sound_load_queue.entity_bank_load_pending == 0 &&
            File_IsLocked() == 0) {
            screen_fade.step++;
        }
    }
}

/* Map change step: frees everything both map slots hold and clears the map tables. */
void MapChange_UnloadAll(void) {
    int slot;

    for (slot = 0; slot < 2; slot++) {
        if (map_slots[slot].loaded != 0) {
            Map_UnlinkSlotRecords(slot);
            Effect_FreeForMap(slot);
            MapObject_UnloadMap(slot);
            Entity_FreeAllInGroup(slot);
            Entity_ClearMapSlot(slot);
            Gold_UnlinkMap(slot);
            MapItem_UnloadMap(slot);
            LinkedPoint_ReleaseByKind(slot);
        }
    }
    MapLoad_Reset();
    func_001A15F8();
    Entity_ResetAllLists();
    Player_EndSwingHits();
    Effect_FreePlayerMagic();
    current_map_slot = 0;
    memset(map_slots, 0, sizeof(map_slots));
    memset(record_draw_lists, 0, sizeof(record_draw_lists));
    memset(late_record_draw_lists, 0, sizeof(late_record_draw_lists));
    memset(map_slot_props, 0, sizeof(map_slot_props));
    record_boxes = NULL;
    backdrop_record_boxes = NULL;
    screen_fade.step++;
}

/* Map change step: opens the new map file and starts reading all of it into the memory of slot 0. */
void MapChange_OpenFile(void) {
    char name[0x100];
    MapSlot *slot = &map_slots[0];
    MapSlotMemory *memory = &map_slot_memory[0];
    int size;

    sprintf(name, map_change_file_name_format, screen_fade.map);
    screen_fade.file_handle = File_Open(name, 1, 1);
    if (screen_fade.file_handle < 0) {
        for (;;) {
        }
    }
    File_Lock();
    size = (File_GetSize(screen_fade.file_handle) + 0x3FFFF) / 0x40000 * 0x40000;
    memory->file_read_cursor = memory->file_base = map_file_area;
    memory->work_cursor = memory->work_base = map_work_area;
    slot->rounded_file_size = size;
    memset(memory->file_base, 0, slot->rounded_file_size);
    memset(memory->work_base, 0, map_work_area_size / 2);
    File_Read(screen_fade.file_handle, memory->file_read_cursor, -1);
    screen_fade.step++;
}

/* Map change step: once the whole file is read, closes it and goes on. */
void MapChange_WaitForRead(void) {
    int next_step;

    if (File_Sync(screen_fade.file_handle, 1) == 0) {
        File_Close(screen_fade.file_handle);
        File_Unlock();
        next_step = screen_fade.step + 1;
        screen_fade.file_handle = -1;
        screen_fade.step = next_step;
    }
}

/* Map change step: sets the loaded map up in slot 0, puts the player in it and ends the change. */
void MapChange_SetUpMap(void) {
    MapSlot *slot = &map_slots[0];
    MapObject *object;
    MapSlotUnk08 *header;
    f32 sine;
    f32 cosine;

    slot->tag = screen_fade.map;
    Map_ParseFile(0);
    Map_BuildModels(0);
    Map_PlaceRecordModels(0);
    MapScroll_ClearPairs(0);
    func_001A15F0();
    MapObject_InitMap(0);
    MapItem_InitMap(0);
    Gold_InitMap(0);
    switch (screen_fade.kind) {
    case 1:
        Vector_Copy(&player_state, &screen_fade);
        break;
    case 2:
        object = MapObject_GetById(0, screen_fade.unk14);
        if (object != NULL) {
            sine = sinf(-object->model->rotation.y);
            cosine = cosf(-object->model->rotation.y);
            Vector_Copy(&player_state, &object->model->position);
            player_state.position.x += cosine * 0.0f - sine * -250.0f;
            player_state.position.z += sine * 0.0f + cosine * -250.0f;
            player_state.rotation.y = object->model->rotation.y;
            break;
        }
    case 0:
        header = slot->header;
        player_state.position.x = header->player_start_x;
        player_state.position.y = header->player_start_y;
        player_state.position.z = header->player_start_z;
        player_state.rotation.y = header->player_start_heading;
        break;
    }
    player_state.map_index = 0xFFFF;
    player_state.record_flag_block = 0xFF;
    player_state.floor_map_object = 0;
    player_state.stepped_on_entity = 0;
    player_state.touched_map_object = 0;
    map_slots[0].loaded = 1;
    MapObject_StartMap(0);
    Entity_InitFromMapSpawns(0);
    Entity_AddGroupToWaiting(0);
    MapItem_RollFixedItems(0);
    Entity_ClearFlag10InSlot(0);
    MapSetup_RunForMap(0);
    Game_MarkMapVisited(map_slots[0].tag);
    Script_Init(map_slots[0].script);
    Script_Start(0x7FFF, 0);
    sound_load_queue.bgm_id = ((MapSlotUnk08 *)map_slots[0].header)->sound_bank_file_index;
    sound_load_queue.bgm_load_pending = 1;
    Sound_StopAll(1);
    sound_load_queue.next_map_slot = &map_slots[0];
    Game_StartFadeIn(0, 0, 0, 30);
    screen_fade.step = 0;
}

/* Map change step 6: ends the change once the timer has run out (MapChange_SetUpMap ends it itself). */
void MapChange_EndWhenTimerDone(void) {
    if (screen_fade.timer <= 0) {
        screen_fade.step = 0;
    }
}
