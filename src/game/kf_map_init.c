#include "kf_game.h"
#include "kf_file.h"
#include "kf_gold.h"
#include "kf_map.h"
#include "kf_map_object.h"
#include "kf_map_scroll.h"
#include "kf_entity_world.h"
#include "kf_sound.h"
#include "kf_memory.h"
#include "kf_screen.h"

int File_GetSize(int);
int sprintf(char *, char *, ...);
void Map_ParseFile(int);
void Map_BuildModels(int);
int Sound_LoadMapBank(MapSlot *);
void Map_PlaceRecordModels(int);
void func_001A15F0(void);
void MapItem_InitMap(int);
void Entity_AddGroupToWaiting(int);
void MapItem_RollFixedItems(int);
void MapSetup_RunForMap(int);
void Game_MarkMapVisited(int);
void Script_Init(int);
int Script_Start(int, int);
extern char map_object_defs[];
extern GameDefinition item_name_defs[380];
extern u8 item_common_defs[];
extern u8 weapon_defs[];
extern char worn_defs[];
extern char obj_prm_path[];
extern char chr_prm_path[];
extern char item_nam_prm_path[];
extern char item_cmn_prm_path[];
extern char item_wpn_prm_path[];
extern char item_pct_prm_path[];
extern char *entity_params;
extern GameStatus game_status;
extern CameraState camera_state;
extern MapProp *map_slot_props[2][0xFF];
extern int current_map_slot;
extern MapDrawRecord *record_draw_lists[2];
extern MapDrawRecord *late_record_draw_lists[2];
extern AreaNode *record_boxes;
extern AreaNode *backdrop_record_boxes;
extern MapSlot map_slots[];
extern MapDrawState map_draw_state;
extern SoundLoadQueue sound_load_queue;
extern MapSlotMemory map_slot_memory[2];
extern char map_file_name_format[];
extern u32 map_work_area_size;
extern int map_file_area_size;
extern char *map_file_area;
extern char *map_work_area;

/* Reads the six parameter files (param/obj.prm, chr.prm, item_nam.prm, item_cmn.prm, item_wpn.prm and
   item_pct.prm), then turns each item's weight from kilograms into pounds. */
void Map_LoadParamFiles(void) {
    GameDefinition *definition;
    int handle;
    int i;

    handle = File_Open(obj_prm_path, 0, 1);
    File_Read(handle, map_object_defs, -1);
    File_Close(handle);
    handle = File_Open(chr_prm_path, 0, 1);
    File_Read(handle, entity_params, -1);
    File_Close(handle);
    handle = File_Open(item_nam_prm_path, 0, 1);
    File_Read(handle, item_name_defs, -1);
    File_Close(handle);
    handle = File_Open(item_cmn_prm_path, 0, 1);
    File_Read(handle, item_common_defs, -1);
    File_Close(handle);
    handle = File_Open(item_wpn_prm_path, 0, 1);
    File_Read(handle, weapon_defs, -1);
    File_Close(handle);
    handle = File_Open(item_pct_prm_path, 0, 1);
    File_Read(handle, worn_defs, -1);
    File_Close(handle);
    definition = item_name_defs;
    for (i = 379; i >= 0; i--) {
        if (definition->unk33 != 0) {
            definition->unk3E = (f32)definition->unk33 * 2.2046; /* kilograms to pounds; the double is D_0043E568 */
        } else {
            definition->unk3E = 0;
        }
        definition++;
    }
}

/* Resets the game status, the map draw state and the camera. */
void Map_ResetGameState(void) {
    memset(&game_status, 0, 0x48);
    game_status.unk07 = 1;
    game_status.unk00 = 1;
    game_status.unk01 = 1;
    memset(&map_draw_state, 0, sizeof(MapDrawState));
    map_draw_state.unk60 |= (s64)0x8000 << 31;
    map_draw_state.unk60 |= (s64)0x8000 << 27;
    map_draw_state.unk60 |= (s64)0x8000 << 30;
    map_draw_state.unk88 = 0x40;
    map_draw_state.unk68 = 1;
    map_draw_state.unk6C = 0;
    map_draw_state.unk70 = 0;
    map_draw_state.unk74 = 0;
    map_draw_state.unk78 = 0;
    map_draw_state.unk80 = 0x44;
    memset(&camera_state, 0, sizeof(CameraState));
    camera_state.unk48 = 512.0f;
}

/* Empties the map slots, the map draw lists, the records-by-id table and the two lists of record boxes. */
void Map_ClearSlots(void) {
    current_map_slot = 0;
    memset(map_slots, 0, 2 * sizeof(MapSlot));
    memset(record_draw_lists, 0, sizeof(record_draw_lists));
    memset(late_record_draw_lists, 0, sizeof(late_record_draw_lists));
    memset(map_slot_props, 0, sizeof(map_slot_props));
    record_boxes = NULL;
    backdrop_record_boxes = NULL;
}

/* Loads a map into slot 0 in one go, sets everything in it up and makes it the current map. */
void Map_LoadAtOnce(int map) {
    char name[0x100];
    MapSlot *slot = &map_slots[0];
    MapSlotMemory *memory = &map_slot_memory[0];
    int handle;
    int size;
    MapFogColor *fog;

    ((int (*)(void *, int))Memory_ClearWords)(slot, 0x18); // match: the callee returns a value here, which keeps $2 busy
    memory->unk14 = memory->unk1C = map_file_area;
    memory->unk18 = memory->unk20 = map_work_area;
    Memory_ClearWords(memory->unk1C, (u32)map_file_area_size / 4);
    Memory_ClearWords(memory->unk20, map_work_area_size / 4);
    sprintf(name, map_file_name_format, map);
    handle = File_Open(name, 0, 1);
    size = File_GetSize(handle);
    File_Read(handle, memory->unk14, -1);
    File_Close(handle);
    slot->tag = map;
    slot->unk04 = (size + 0x3FFFF) / 0x40000 * 0x40000;
    Map_ParseFile(0);
    Map_BuildModels(0);
    Sound_LoadMapBank(slot);
    Map_PlaceRecordModels(0);
    MapScroll_ClearPairs(0);
    func_001A15F0();
    MapObject_InitMap(0);
    MapItem_InitMap(0);
    Gold_InitMap(0);
    map_slots[0].unk00 = 1;
    current_map_slot = 0;
    map_draw_state.unkA0 = *slot->unk0C;
    map_draw_state.unkC0 = *(MapSlotUnk10Words *)slot->unk10;
    fog = &map_draw_state.unk280;
    *fog = ((MapSlotUnk08 *)map_slots[0].header)->unk50;
    map_draw_state.unk9A = 0;
    map_draw_state.unk9B = 0;
    map_draw_state.unk9C = 0;
    MapObject_StartMap(0);
    Entity_InitFromMapSpawns(0);
    Entity_AddGroupToWaiting(0);
    MapItem_RollFixedItems(0);
    Entity_ClearFlag10InSlot(0);
    MapSetup_RunForMap(0);
    Game_MarkMapVisited(map_slots[0].tag);
    Script_Init(map_slots[0].unk5C);
    Script_Start(0x7FFF, 0);
    sound_load_queue.unk00 = ((MapSlotUnk08 *)map_slots[0].header)->unk70;
    sound_load_queue.unk04 = 1;
}
