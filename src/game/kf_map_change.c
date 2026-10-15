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
void Packet_SwapBuffers(void);
void Packet_SendChain(void);
void Packet_WaitForSend(void);
void DrawList_Clear(void);
void func_00118A50(void);
void func_001B5460(void);
void Display_SwapBuffers(int);
int func_001005E8(int);
void GsDraw_CopyFrame(int);
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int, s64);
void Packet_EndGsRegisters(void);
void GsDraw_QueueTexturedSprite(void *);
void GsDraw_FillScreen(int, int, int, int, int);
void func_00114198(int);
int File_IsLocked(void);
void func_00117CC8(void);
int EntityPool_IsLoading(void);
void Entity_UpdatePool(void);
void Sound_Update(void);
void func_00115878(int);
void func_00117C60(void);
void func_001A15F8(void);
void func_001BC128(void);
void func_001C8938(int);
void File_Lock(void);
void File_Unlock(void);
int File_GetSize(int);
int sprintf(char *, char *, ...);
float cosf(float);
float sinf(float);
void func_001034F8(int);
void func_00106178(int, int, int, int);
void func_00114840(int);
void func_00114AA8(int);
void func_001155F8(int);
void func_00119360(int);
void Entity_AddGroupToWaiting(int);
void Sound_StopAll(int);
void func_001A15F0(void);
void func_001C82C0(int);
void func_001C87D8(int);
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

/* Start a transition to new_value unless one is running or it is already current. */
void func_001187F0(int new_value) {
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
void func_00118848(int map, Vector4 *position) {
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

/* Starts the map change that entry (5 and up) of the table func_00103A18 reads leads to. */
int func_001188E0(int entry) {
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
void func_00118958(void) {
    if (screen_fade.step != 0) {
        func_00118A50();
        func_001020F8();
        Splash_Abort();
        do {
            func_001B5460();
            Display_SwapBuffers(0);
            Packet_SendChain();
            Packet_SwapBuffers();
            DrawList_Clear();
            screen_fade_draw_steps[screen_fade.draw_step]();
            screen_fade_steps[screen_fade.step]();
            func_00101618();
            ((u8 (*)(void))Packet_WaitForSend)(); // match: called as returning a value, so the loop test uses $3
        } while (screen_fade.step != 0);
        game_status.bit2 = FALSE;
        game_status.save_requested = FALSE;
    }
}

/* Draws one frame with nothing in it. */
void func_00118A50(void) {
    Display_SwapBuffers(0);
    Packet_SendChain();
    Packet_SwapBuffers();
    DrawList_Clear();
    func_001005E8(0);
    GsDraw_CopyFrame(0x2BC0);
    Packet_WaitForSend();
}

/* Map change drawing step: darkens the screen in ten strips, more each frame, until the timer runs out. */
void func_00118A98(void) {
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
void func_00118C28(void) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
    Packet_EndGsRegisters();
    GsDraw_FillScreen(0, 0, 0, 0x80, 0x88);
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
    Packet_EndGsRegisters();
}

/* Draws one frame of a 30-frame fade and counts the timer down. */
void func_00118CA8(void) {
    func_00114198(0);
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

void func_00118D78(void) {
}

/* Map change step: lets the background load and everything else in flight settle. */
void func_00118D80(void) {
    func_00117CC8();
    Entity_UpdatePool();
    Sound_Update();
    if (map_load_state.step == 0 || map_load_state.step == 5) {
        if (EntityPool_IsLoading() == 0 && sound_load_queue.unk0C == 0 && sound_load_queue.unk04 == 0 && sound_load_queue.unk18 == 0 &&
            File_IsLocked() == 0) {
            screen_fade.step++;
        }
    }
}

/* Map change step: frees everything both map slots hold and clears the map tables. */
void func_00118E20(void) {
    int slot;

    for (slot = 0; slot < 2; slot++) {
        if (map_slots[slot].unk00 != 0) {
            func_00115878(slot);
            Effect_FreeForMap(slot);
            MapObject_UnloadMap(slot);
            Entity_FreeAllInGroup(slot);
            Entity_ClearMapSlot(slot);
            Gold_UnlinkMap(slot);
            func_001C8938(slot);
            LinkedPoint_ReleaseByKind(slot);
        }
    }
    func_00117C60();
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
void func_00118F40(void) {
    char name[0x100];
    MapSlot *slot = &map_slots[0];
    MapSlotMemory *memory = &D_004321F0[0];
    int size;

    sprintf(name, D_0043E720, screen_fade.map);
    screen_fade.file_handle = File_Open(name, 1, 1);
    if (screen_fade.file_handle < 0) {
        for (;;) {
        }
    }
    File_Lock();
    size = (File_GetSize(screen_fade.file_handle) + 0x3FFFF) / 0x40000 * 0x40000;
    memory->unk14 = memory->unk1C = D_00446F50;
    memory->unk18 = memory->unk20 = D_00446F54;
    slot->unk04 = size;
    memset(memory->unk1C, 0, slot->unk04);
    memset(memory->unk20, 0, D_00446F1C / 2);
    File_Read(screen_fade.file_handle, memory->unk14, -1);
    screen_fade.step++;
}

/* Map change step: once the whole file is read, closes it and goes on. */
void func_00119068(void) {
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
void func_001190C8(void) {
    MapSlot *slot = &map_slots[0];
    MapObject *object;
    MapSlotUnk08 *header;
    f32 sine;
    f32 cosine;

    slot->tag = screen_fade.map;
    func_00114840(0);
    func_00114AA8(0);
    func_001155F8(0);
    MapScroll_ClearPairs(0);
    func_001A15F0();
    MapObject_InitMap(0);
    func_001C82C0(0);
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
    Entity_AddGroupToWaiting(0);
    func_001C87D8(0);
    Entity_ClearFlag10InSlot(0);
    func_00119360(0);
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

/* Map change step: ends the change once the timer has run out. */
void func_00119340(void) {
    if (screen_fade.timer <= 0) {
        screen_fade.step = 0;
    }
}
