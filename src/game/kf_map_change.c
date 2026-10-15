#include "kf_map.h"
#include "kf_player.h"
#include "kf_screen.h"
#include "kf_game.h"
#include "kf_splash.h"
#include "kf_gs.h"
#include "kf_packet.h"
#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_gold.h"
#include "kf_linked_point.h"
#include "kf_map_object.h"
#include "kf_effect.h"
#include "kf_file.h"
#include "kf_map_scroll.h"
#include "kf_sound.h"

extern void (*screen_fade_steps[])(void);
extern void (*screen_fade_draw_steps[])(void);
int func_00103A18(int, int *, int *);
void func_00101618(void);
void func_001020F8(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_0010B000(void);
void MapChange_CaptureFrame(void);
void func_001B5460(void);
void Display_SwapBuffers(int);
int func_001005E8(int);
void func_0010A040(int);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A100(void *);
void func_0010A4E8(int, int, int, int, int);
void Map_DrawRecords(int);
int func_00102230(void);
void MapLoad_Update(void);
int func_00123108(void);
void func_00123288(void);
void Sound_Update(void);
void Map_UnlinkSlotRecords(int);
void MapLoad_Reset(void);
void func_001A15F8(void);
void func_001BC128(void);
void MapItem_UnloadMap(int);
void func_00102218(void);
void func_00102228(void);
int func_00102900(int);
int sprintf(char *, char *, ...);
float cosf(float);
float sinf(float);
void func_001034F8(int);
void func_00106178(int, int, int, int);
void Map_ParseFile(int);
void Map_BuildModels(int);
void Map_PlaceRecordModels(int);
void MapSetup_RunForMap(int);
void func_00123158(int);
void Sound_StopAll(int);
void func_001A15F0(void);
void MapItem_InitMap(int);
void MapItem_RollFixedItems(int);
void Script_Init(int);
int Script_Start(int, int);
void Vector_Copy(void *, void *);
extern int D_00446770;
extern MapSlot map_slots[2];
extern ScreenFade screen_fade;
extern PlayerState player_state;
extern GameStatus game_status;
extern MapLoadState map_load_state;
extern MapProp *map_slot_props[2][0xFF];
extern MapDrawRecord *D_00446778[2];
extern MapDrawRecord *D_00446780[2];
extern AreaNode *D_00446788;
extern AreaNode *D_0044678C;
extern MapSlotMemory D_004321F0[2];
extern char D_0043E720[];
extern u32 D_00446F1C;
extern char *D_00446F50;
extern char *D_00446F54;
extern SoundLoadQueue sound_load_queue;

/* Starts the change to map new_value, the player to stand at the map's start, unless a change is running
   or it is the current map. */
void MapChange_StartToMap(int new_value) {
    MapSlot *slot;

    slot = &map_slots[D_00446770];
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
    MapSlot *slot = &map_slots[D_00446770];

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
   five below the wand was placed (func_00103A18). FALSE when a change runs or it was not placed. */
int MapChange_StartPassage(int entry) {
    int target[2];
    int map;

    if (screen_fade.step != 0) {
        return FALSE;
    }
    if (func_00103A18(entry - 5, &target[0], &target[1]) == 0) {
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
        func_001020F8();
        Splash_Abort();
        do {
            func_001B5460();
            Display_SwapBuffers(0);
            func_001096E8();
            func_001095E0();
            func_0010B000();
            screen_fade_draw_steps[screen_fade.draw_step]();
            screen_fade_steps[screen_fade.step]();
            func_00101618();
            ((u8 (*)(void))func_00109800)(); // match: called as returning a value, so the loop test uses $3
        } while (screen_fade.step != 0);
        game_status.bit2 = FALSE;
        game_status.save_requested = FALSE;
    }
}

/* Draws one frame of the game and copies the picture to GS block 0x2BC0, where
   MapChange_FadeOutCapture fades it out. */
void MapChange_CaptureFrame(void) {
    Display_SwapBuffers(0);
    func_001096E8();
    func_001095E0();
    func_0010B000();
    func_001005E8(0);
    func_0010A040(0x2BC0);
    func_00109800();
}

/* Map change drawing step 0: draws the captured picture in ten strips, darker each frame, until the
   timer runs out. */
void MapChange_FadeOutCapture(void) {
    ScreenQuad quad;
    int remaining = 9;
    int total;

    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(GS_REG_ALPHA_1, 0x88);
    func_00109AD0();
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
        func_0010A100(&quad);
        quad.first_x += 0x400;
        quad.second_x += 0x400;
        quad.first_u += 0x400;
        quad.second_u += 0x400;
    } while (remaining >= 0);
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109AD0();
    if (--screen_fade.timer <= 0) {
        screen_fade.draw_step++;
    }
}

/* Map change drawing step: keeps the screen black. */
void MapChange_DrawBlack(void) {
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109AD0();
    func_0010A4E8(0, 0, 0, 0x80, 0x88);
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109AD0();
}

/* Map change drawing step 2: draws the map records under a black cover that thickens over 30 frames, and
   counts the timer down. Nothing sets this step. */
void MapChange_FadeMapToBlack(void) {
    Map_DrawRecords(0);
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(GS_REG_ALPHA_1, 0x88);
    func_00109AD0();
    {
        int total = 30; /* two separate 30s, as retail keeps both */
        int span = 30;
        func_0010A4E8(0, 0, 0, ((total - screen_fade.timer) << 7) / span, 0x88);
    }
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109AD0();
    screen_fade.timer--;
}

/* Map change step 0: empty, no change is running. */
void func_00118D78(void) {
}

/* Map change step: lets the background load and everything else in flight settle. */
void MapChange_WaitForIdle(void) {
    MapLoad_Update();
    func_00123288();
    Sound_Update();
    if (map_load_state.step == 0 || map_load_state.step == 5) {
        if (func_00123108() == 0 && sound_load_queue.unk0C == 0 && sound_load_queue.unk04 == 0 && sound_load_queue.unk18 == 0 &&
            func_00102230() == 0) {
            screen_fade.step++;
        }
    }
}

/* Map change step: frees everything both map slots hold and clears the map tables. */
void MapChange_UnloadAll(void) {
    int slot;

    for (slot = 0; slot < 2; slot++) {
        if (map_slots[slot].unk00 != 0) {
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
    func_001BC128();
    Effect_FreePlayerMagic();
    D_00446770 = 0;
    memset(map_slots, 0, sizeof(map_slots));
    memset(D_00446778, 0, sizeof(D_00446778));
    memset(D_00446780, 0, sizeof(D_00446780));
    memset(map_slot_props, 0, sizeof(map_slot_props));
    D_00446788 = NULL;
    D_0044678C = NULL;
    screen_fade.step++;
}

/* Map change step: opens the new map file and starts reading all of it into the memory of slot 0. */
void MapChange_OpenFile(void) {
    char name[0x100];
    MapSlot *slot = &map_slots[0];
    MapSlotMemory *memory = &D_004321F0[0];
    int size;

    sprintf(name, D_0043E720, screen_fade.map);
    screen_fade.file_handle = func_00102238(name, 1, 1);
    if (screen_fade.file_handle < 0) {
        for (;;) {
        }
    }
    func_00102218();
    size = (func_00102900(screen_fade.file_handle) + 0x3FFFF) / 0x40000 * 0x40000;
    memory->unk14 = memory->unk1C = D_00446F50;
    memory->unk18 = memory->unk20 = D_00446F54;
    slot->unk04 = size;
    memset(memory->unk1C, 0, slot->unk04);
    memset(memory->unk20, 0, D_00446F1C / 2);
    func_001025C0(screen_fade.file_handle, memory->unk14, -1);
    screen_fade.step++;
}

/* Map change step: once the whole file is read, closes it and goes on. */
void MapChange_WaitForRead(void) {
    int next_step;

    if (func_00102790(screen_fade.file_handle, 1) == 0) {
        func_001024B0(screen_fade.file_handle);
        func_00102228();
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
        player_state.position.x = header->unk60;
        player_state.position.y = header->unk64;
        player_state.position.z = header->unk68;
        player_state.rotation.y = header->unk6C;
        break;
    }
    player_state.map_index = 0xFFFF;
    player_state.unk250 = 0xFF;
    player_state.unk208 = 0;
    player_state.unk204 = 0;
    player_state.unk20C = 0;
    map_slots[0].unk00 = 1;
    MapObject_StartMap(0);
    Entity_InitFromMapSpawns(0);
    func_00123158(0);
    MapItem_RollFixedItems(0);
    Entity_ClearFlag10InSlot(0);
    MapSetup_RunForMap(0);
    func_001034F8(map_slots[0].tag);
    Script_Init(map_slots[0].unk5C);
    Script_Start(0x7FFF, 0);
    sound_load_queue.unk00 = ((MapSlotUnk08 *)map_slots[0].header)->unk70;
    sound_load_queue.unk04 = 1;
    Sound_StopAll(1);
    sound_load_queue.unk14 = &map_slots[0];
    func_00106178(0, 0, 0, 30);
    screen_fade.step = 0;
}

/* Map change step 6: ends the change once the timer has run out (MapChange_SetUpMap ends it itself). */
void MapChange_EndWhenTimerDone(void) {
    if (screen_fade.timer <= 0) {
        screen_fade.step = 0;
    }
}
