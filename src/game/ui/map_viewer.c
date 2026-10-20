/* The map viewer the five map items open (items 1 to 5, in viewer_entries order): the Caretaker's, Zastari's,
   Earth Folk's and Widda's Maps show pictures with the player's marker, the King's Map a free camera over
   the map records. The names are the items' own (param/item_nam.prm). */
#include "common.h"
#include "collision/collision.h"
#include "ui/map_viewer.h"
#include "entity/entity.h"
#include "core/game.h"
#include "gfx/gs.h"
#include "map/map.h"
#include "gfx/packet.h"
#include "player/player.h"
#include "gfx/texture.h"
#include "input/pad.h"
#include "ui/message.h"
#include "core/display.h"
#include "core/file.h"
#include "enemy/enemy.h"
#include "gfx/draw_list.h"
#include "gfx/gs_draw.h"
#include "input/input.h"
#include <libvu0.h>
#include <math.h>
#include "math/angle.h"
#include "math/vu0.h"
#include "model/model.h"
#include "model/model_matrix.h"
#include "save/save_state.h"
#include "sound/sound.h"
#include "sound/sound_game.h"
#include <stdio.h>

/* Picture pixels per 1000 world units on each map picture of the viewer. */
#define CARETAKERS_MAP_X_SCALE 24.615385f
#define CARETAKERS_MAP_Z_SCALE 24.88889f
#define ZASTARI_PICTURE0_X_SCALE 18.626263f
#define ZASTARI_PICTURE0_Z_SCALE 18.666666f
#define ZASTARI_PICTURE1_X_SCALE 11.243243f
#define ZASTARI_PICTURE1_Z_SCALE 11.48718f
#define ZASTARI_PICTURE2_X_SCALE 11.186992f
#define EARTH_FOLKS_MAP_X_SCALE 22.857143f
#define WIDDAS_MAP_X_SCALE 21.333334f
#define WIDDAS_MAP_Z_SCALE 20.363636f


/* The Caretaker's Map pictures: fixed spots for each map, or a spot from the player's position. */
#define CARETAKERS_MAP_PICTURE_FIXED_SPOTS    0
#define CARETAKERS_MAP_PICTURE_PLAYER_SPOTS   1

static void MapViewer_RestorePacketBuffers(void);
static int MapViewer_Load(void);
static void MapViewer_DrawKingsMap(void);

/* the map viewer: the picture data read into the spare area, the viewer picked, and its animation */
static char *viewer_data;                   /* 0x44705C the spare area the viewer's textures are read into */
static int viewer_data_size;                /* 0x447060 */
static int picked_viewer;                   /* 0x447064 index into viewer_entries */
static int viewer_picture;                  /* 0x447068 which of the viewer's pictures is shown */
static int pulse_frame;                     /* 0x44706C frame of the 60-frame pulse of the player marker */
static int square_pulse_frame;              /* 0x447070 frame of the pulse of the fixed square */
static Vector4 bounds_min;                  /* 0x472060 the lowest corner of every map record */
static Vector4 bounds_max;                  /* 0x472070 the highest corner */
static ModelInstance pointer_model;         /* 0x472080 the pointer model, viewer 4 only */

/* Clears the map viewer at the start of a game. */
void MapViewer_Init(void) {
    MapViewer_Reset();
}

/* Asks for the map viewer of a map item (items 1 to 5, the viewer's number); the King's Map (5) only where
   it can be used. The game loop runs it (MapViewer_Run). */
void MapViewer_OpenForItem(int viewer_number) {
    u32 viewer_index;
    u8 *row;

    if (!game_status.bit8 && ((viewer_index = viewer_number - 1) < 5U)) {
        if (viewer_number == 5) {
            row = (u8 *)map_slots + current_map_slot * 0x60;
            if (MapViewer_IsKingsMapUsable(row[1]) == 0) {
                Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_UNABLE_TO_USE_HERE);
                return;
            }
        }
        picked_viewer = viewer_index;
        pulse_frame = 0;
        square_pulse_frame = 0;
        game_status.bit8 = 1;
    }
}

/* Loads the picked viewer, runs it until it is left and frees what it loaded. */
void MapViewer_Run(void) {
    if (MapViewer_Load() != 0) {
        Sound_MuteTagged();
        Sound_MutePositional();
        Sound_UpdateTagged();
        Sound_MuteTracked();
        SoundDriver_FlushCommands();
        viewer_entries[picked_viewer].run();
        Sound_UnmuteTagged();
        Sound_UnmutePositional();
        Sound_UnmuteTracked();
        MapViewer_RestorePacketBuffers();
        MapViewer_Reset();
    }
}

/* Forgets the viewer picked, its picture and pulses, and what it loaded. */
void MapViewer_Reset(void) {
    picked_viewer = 0;
    viewer_picture = 0;
    game_status.bit8 = 0;
    viewer_data = 0;
    viewer_data_size = 0;
    pulse_frame = 0;
    square_pulse_frame = 0;
    memset(&pointer_model, 0, sizeof(pointer_model));
}

/* Gives the packet buffers back the room MapViewer_Load took for the viewer's files. */
static void MapViewer_RestorePacketBuffers(void) {
    Packet_WaitForSend();
    Packet_LayOutBuffers(0);
}

static inline char *align16(char *address) {
    if ((int)address & 0xF) {
        address -= ((int)address & 0xF) - 0x10;
    }
    return address;
}

/* Loads the picked viewer's textures; for viewer 4 also the pointer model, put where the player is, and
   the box that holds every ordinary map record. FALSE when a file is missing. */
static int MapViewer_Load(void) {
    char name[0x100];
    char *model_data;
    char *buffer;
    int file;
    int size;
    int i;
    int j;
    MapRecordBounds *record;
    int count;

    sprintf(name, viewer_texture_file_name_format, picked_viewer);
    file = File_Open(name, 0, 1);
    if (file < 0) {
        return FALSE;
    }
    Packet_WaitForSend();
    Packet_LayOutBuffers(2);
    viewer_data = NULL;
    viewer_data_size = 0;
    Packet_GetSpareArea((int *)&viewer_data, &viewer_data_size);
    memset(viewer_data, 0, viewer_data_size);
    size = File_Read(file, viewer_data, -1);
    File_Close(file);
    TextureTable_SetBasePointers((TextureTableHeader *)viewer_data, 0x2BC0, 0x3FE0);
    if (picked_viewer != 4) {
        return TRUE;
    }
    model_data = viewer_data + size;
    model_data = align16(model_data);
    file = File_Open(viewer_pointer_model_path, 0, 1);
    if (file < 0) {
        return FALSE;
    }
    size = File_Read(file, model_data, -1);
    File_Close(file);
    buffer = model_data + size;
    buffer = align16(buffer);
    Model_SetUpPosed(&pointer_model, &model_data, &buffer);
    Model_SetPrimBits(&pointer_model, 0x260, 0x240);
    Vector_Copy(&pointer_model.position, &player_state);
    pointer_model.position.y -= 100.0f;
    pointer_model.rotation.x = 0.0f;
    pointer_model.rotation.y = player_state.rotation.y;
    pointer_model.rotation.z = 0.0f;
    pointer_model.scale_x = pointer_model.scale_y = pointer_model.scale_z = pointer_model.rotation.w = 4.0f;
    pointer_model.scale_w = 1.0f;
    // match: it returns a value
    ((int (*)(ModelInstance *, Vector4 *, Vector4 *, f32 *))ModelMatrix_Build)(&pointer_model, &pointer_model.position, &pointer_model.rotation, &pointer_model.scale_x);
    for (i = 0; i < pointer_model.part_count; i++) {
        // match: the callee returns a value here, which keeps $2 busy
        ((int (*)(ModelInstance *, ModelPart *, ModelPart *))ModelMatrix_BuildPartLocal)(&pointer_model, &pointer_model.parts[i], &pointer_model.parts[i]);
    }
    for (i = 0; i < pointer_model.part_count; i++) {
        Model_MultiplyChildMatrices(&pointer_model.parts[i]);
    }
    bounds_min.z = bounds_min.y = bounds_min.x = float_largest_value[0];
    bounds_max.z = bounds_max.y = bounds_max.x = float_lowest_value[0];
    for (i = 0; i < 2; i++) {
        if (map_slots[i].loaded != 0 && MapViewer_IsKingsMapUsable(map_slots[i].tag) != 0) {
            count = ((MapSlotUnk08 *)map_slots[i].header)->record_count;
            record = (MapRecordBounds *)map_slots[i].records;
            for (j = 0; j < count; j++, record++) {
                if (bounds_min.x > record->box_lowest[0]) {
                    bounds_min.x = record->box_lowest[0];
                }
                if (bounds_min.y > record->box_lowest[1]) {
                    bounds_min.y = record->box_lowest[1];
                }
                if (bounds_min.z > record->box_lowest[2]) {
                    bounds_min.z = record->box_lowest[2];
                }
                if (bounds_max.x < record->box_highest[0]) {
                    bounds_max.x = record->box_highest[0];
                }
                if (bounds_max.y < record->box_highest[1]) {
                    bounds_max.y = record->box_highest[1];
                }
                if (bounds_max.z < record->box_highest[2]) {
                    bounds_max.z = record->box_highest[2];
                }
            }
        }
    }
    return TRUE;
}

/* Frame loop of the picture maps (items 1 to 4): up and down page through the pictures, 0x2000 leaves. */
void MapViewer_RunPictureMap(void) {
    int next_picture;
    ViewerEntry *viewer;

    viewer = &viewer_entries[picked_viewer];
    do {
        Display_SwapBuffers(0);
        Packet_SendChain();
        Packet_SwapBuffers();
        DrawList_Clear();
        Packet_OnFrameStart();
        MapViewer_DrawPictureMap();
        Packet_OnFrameEnd();
        Input_Update();
        if ((pad_state.pressed & PAD_UP) && (viewer_picture > 0)) {
            viewer_picture -= 1;
        } else if (pad_state.pressed & PAD_DOWN) {
            next_picture = viewer_picture + 1;
            if (next_picture < viewer->count) {
                viewer_picture = next_picture;
            }
        }
        // match: it returns a value here, which keeps $2 busy
        ((u8 (*)(void))Packet_WaitForSend)();
    } while ((pad_state.pressed & PAD_CIRCLE) == 0);
}

/* Draws the picked viewer's picture and the player's marker on it: a turned, pulsing arrow where the
   position is known, a pulsing square at a fixed spot otherwise; then the picture arrows when the viewer
   has more than one picture. */
void MapViewer_DrawPictureMap(void) {
    ViewerCornerTable corner_us;
    ViewerCornerTable corner_vs;
    int corner_xs[4];
    int corner_ys[4];
    int x;
    int y;
    ViewerEntry *entry = &viewer_entries[picked_viewer];
    int found;
    int alpha;
    int i;
    f32 cosine;
    f32 sine;
    u32 *packet_start;
    u32 *packet;

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
    Packet_EndGsRegisters();
    TextureTable_QueueOne((TextureTableHeader *)viewer_data, viewer_picture);
    GsDraw_QueueTexFlush();
    GsDraw_QueueFlush();
    GsDraw_QueueTexturedSprite(&viewer_picture_rect);
    found = entry->locate_player_fn(&x, &y);
    if (found == 1) {
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
        Packet_EndGsRegisters();
        TextureTable_QueueOne((TextureTableHeader *)viewer_data, entry->count);
        GsDraw_QueueTexFlush();
        GsDraw_QueueFlush();
        corner_us = viewer_marker_corner_us;
        corner_vs = viewer_marker_corner_vs;
        if (pulse_frame < 45) {
            alpha = sinf(pulse_frame * PI / 45.0f) * 128.0f;
        } else {
            alpha = 0;
        }
        pulse_frame = (pulse_frame + 1) % 60;
        cosine = cosf(player_state.rotation.y);
        sine = sinf(player_state.rotation.y);
        corner_xs[0] = x + (int)((cosine * -32.0f - sine * -44.0f) * 0.5f);
        corner_ys[0] = y + (int)((sine * -32.0f + cosine * -44.0f) * 0.5f);
        corner_xs[1] = x + (int)((cosine * 32.0f - sine * -44.0f) * 0.5f);
        corner_ys[1] = y + (int)((sine * 32.0f + cosine * -44.0f) * 0.5f);
        corner_xs[2] = x + (int)((cosine * -32.0f - sine * 20.0f) * 0.5f);
        corner_ys[2] = y + (int)((sine * -32.0f + cosine * 20.0f) * 0.5f);
        corner_xs[3] = x + (int)((cosine * 32.0f - sine * 20.0f) * 0.5f);
        corner_ys[3] = y + (int)((sine * 32.0f + cosine * 20.0f) * 0.5f);
        packet_start = packet = Packet_Alloc(0x13);
        *packet++ = 0x10000011;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0x6C0FC000;
        *(u64 *)packet = 0xE0AA400000008001L;
        packet += 2;
        *(u64 *)packet = 0x513513513513EEL;
        packet += 2;
        *(u64 *)packet = 0x4007FC0599306BC0L;
        packet += 2;
        *packet++ = 6;
        *packet++ = 0;
        *(u64 *)packet = 0x60;
        packet += 2;
        *packet++ = 0x14;
        *packet++ = 0;
        for (i = 0; i < 4; i++) {
            *packet++ = corner_us.values[i];
            *packet++ = corner_vs.values[i];
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0x80;
            *packet++ = 0x80;
            *packet++ = 0x80;
            *packet++ = alpha;
            *packet++ = (corner_xs[i] + 0x6C0) << 4;
            *packet++ = (corner_ys[i] + 0x720) << 4;
            *packet++ = 0xFFFFFFFF;
            *packet++ = 1;
        }
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0x15000276;
        Packet_AppendToChain(packet_start, packet);
    }
    if (found == -1) {
        if (square_pulse_frame < 45) {
            viewer_marker_rect.alpha = sinf(square_pulse_frame * PI / 45.0f) * 128.0f;
        } else {
            viewer_marker_rect.alpha = 0;
        }
        square_pulse_frame = (square_pulse_frame + 1) % 60;
        viewer_marker_rect.first_x = (x + 0x690) << 4;
        viewer_marker_rect.first_y = (y + 0x6F0) << 4;
        viewer_marker_rect.second_x = (x + 0x6F0) << 4;
        viewer_marker_rect.second_y = (y + 0x750) << 4;
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ALPHA_1, 0x48);
        Packet_EndGsRegisters();
        GsDraw_QueueTexturedSprite(&viewer_marker_rect);
    }
    if (entry->count >= 2) {
        TextureTable_QueueOne((TextureTableHeader *)viewer_data, entry->count * 2 | 1);
        GsDraw_QueueTexFlush();
        GsDraw_QueueFlush();
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
        Packet_EndGsRegisters();
        GsDraw_QueueTexturedSprite(&viewer_arrow_rect_1);
        TextureTable_QueueOne((TextureTableHeader *)viewer_data, entry->count * 2 + 2);
        GsDraw_QueueTexFlush();
        GsDraw_QueueFlush();
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ALPHA_1, 0x48);
        Packet_EndGsRegisters();
        GsDraw_QueueTexturedSprite(&viewer_arrow_rect_2);
        TextureTable_QueueOne((TextureTableHeader *)viewer_data, entry->count + 1 + viewer_picture);
        GsDraw_QueueTexFlush();
        GsDraw_QueueFlush();
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ALPHA_1, 0x48);
        Packet_EndGsRegisters();
        GsDraw_QueueTexturedSprite(&viewer_picture_label_rect);
    }
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
    Packet_EndGsRegisters();
    Game_ApplyBrightness(game_options[10]);
}

/* Where the player's marker goes on picture viewer_picture of the Caretaker's Map: -1 for a fixed spot on the
   first picture, TRUE for a spot worked out from the player's position, FALSE for none. */
int MapViewer_LocateOnCaretakersMap(int *screen_x, int *screen_y) {
    switch (viewer_picture) {
    case CARETAKERS_MAP_PICTURE_FIXED_SPOTS:
        switch (map_slots[current_map_slot].tag) {
        case MAP_TAG_VILLAGE_RUINS:
            if (player_state.map_index != 0xFFFF
                && ((MapRecord *)map_slots[current_map_slot].records)[player_state.map_index].prop_id == 0xC8) {
                *screen_x = 0x96;
                *screen_y = 0xDB;
            } else {
                *screen_x = 0xB6;
                *screen_y = 0x69;
            }
            return -1;
        case MAP_TAG_MINERS_CEMETERY:
            if (player_state.map_index != 0xFFFF
                && ((MapRecord *)map_slots[current_map_slot].records)[player_state.map_index].prop_id == 0xC8) {
                *screen_x = 0x96;
                *screen_y = 0xDB;
            } else {
                *screen_x = 0x64;
                *screen_y = 0x151;
            }
            return -1;
        case MAP_TAG_EAST_VALLEY:
            *screen_x = 0x214;
            *screen_y = 0x12E;
            return -1;
        case MAP_TAG_UPPER_MINES:
        case MAP_TAG_LOWER_MINES:
            *screen_x = 0x214;
            *screen_y = 0x94;
            return -1;
        case MAP_TAG_PALACE_OF_OATH_INTERIOR:
            *screen_x = 0x169;
            *screen_y = 0x5C;
            return -1;
        case MAP_TAG_PALACE_OF_OATH_EXTERIOR:
            *screen_x = 0x14A;
            *screen_y = 0xFF;
            return -1;
        default:
            return FALSE;
        }
    case CARETAKERS_MAP_PICTURE_PLAYER_SPOTS:
        if (player_state.position.y > -700.0f) {
            switch (map_slots[current_map_slot].tag) {
            case MAP_TAG_UNDERGROUND_GRAVEYARD:
            case MAP_TAG_SHOP:
            case MAP_TAG_ICON_OF_HEALING:
                *screen_x = (player_state.position.x - 0.0f) * CARETAKERS_MAP_X_SCALE / 1000.0f + 148.0f;
                *screen_y = 273.0f - (player_state.position.z - 0.0f) * CARETAKERS_MAP_Z_SCALE / 1000.0f;
                return TRUE;
            }
        } else {
            switch (map_slots[current_map_slot].tag) {
            case MAP_TAG_UNDERGROUND_GRAVEYARD:
                if (player_state.position.x > 4800.0f && player_state.position.z > 8000.0f) {
                    return FALSE;
                }
            case MAP_TAG_SHOP:
                *screen_x = (player_state.position.x - 3600.0f) * CARETAKERS_MAP_X_SCALE / 1000.0f + 490.0f;
                *screen_y = 143.0f - (player_state.position.z - 5200.0f) * CARETAKERS_MAP_Z_SCALE / 1000.0f;
                return TRUE;
            case MAP_TAG_ICON_OF_HEALING: // match: named although it does what the default does; it shapes the compare tree
            default:
                return FALSE;
            }
        }
        break;
    }
    return FALSE;
}

/* Where the player's marker goes on picture viewer_picture (0 to 2) of Zastari's Map: TRUE with a spot worked
   out from the player's position when the current map is on that picture, FALSE when it is not. */
int MapViewer_LocateOnZastarisMap(int *screen_x, int *screen_y) {
    switch (viewer_picture) {
    case 0:
        switch (map_slots[current_map_slot].tag) {
        case MAP_TAG_ANCIENT_CITY_L1_CENTRAL_TOWER:
        case MAP_TAG_ANCIENT_CITY_L1_WEST:
            *screen_x = (player_state.position.x - 0.0f) * ZASTARI_PICTURE0_X_SCALE / 1000.0f + 321.0f;
            *screen_y = 280.0f - (player_state.position.z - 0.0f) * ZASTARI_PICTURE0_Z_SCALE / 1000.0f;
            return TRUE;
        case MAP_TAG_ANCIENT_CITY_L1_AND_2_EAST:
            *screen_x = (player_state.position.x - -10500.0f) * ZASTARI_PICTURE0_X_SCALE / 1000.0f + 321.0f;
            *screen_y = 280.0f - (player_state.position.z - 500.0f) * ZASTARI_PICTURE0_Z_SCALE / 1000.0f;
            return TRUE;
        case MAP_TAG_TREASURY:
            *screen_x = (player_state.position.x - -9700.0f) * ZASTARI_PICTURE0_X_SCALE / 1000.0f + 321.0f;
            *screen_y = 280.0f - (player_state.position.z - -8900.0f) * ZASTARI_PICTURE0_Z_SCALE / 1000.0f;
            return TRUE;
        }
        break;
    case 1:
        switch (map_slots[current_map_slot].tag) {
        case MAP_TAG_NORTH_CLIFFS:
            *screen_x = (player_state.position.x - 0.0f) * ZASTARI_PICTURE1_X_SCALE / 1000.0f + 348.0f;
            *screen_y = 263.0f - (player_state.position.z - -12100.0f) * ZASTARI_PICTURE1_Z_SCALE / 1000.0f;
            return TRUE;
        case MAP_TAG_ANCIENT_CITY_L2_CENTRAL_TOWER:
        case MAP_TAG_ANCIENT_CITY_L2_SOUTH:
        case MAP_TAG_ANCIENT_CITY_L2_WEST:
        case MAP_TAG_ANCIENT_CITY_L2_SORCERERS_AREA:
        case MAP_TAG_ANCIENT_CITY_L2_NORTH:
            *screen_x = (player_state.position.x - 0.0f) * ZASTARI_PICTURE1_X_SCALE / 1000.0f + 348.0f;
            *screen_y = 263.0f - (player_state.position.z - 0.0f) * ZASTARI_PICTURE1_Z_SCALE / 1000.0f;
            return TRUE;
        case MAP_TAG_ANCIENT_CITY_L1_AND_2_EAST:
            *screen_x = (player_state.position.x - -10500.0f) * ZASTARI_PICTURE1_X_SCALE / 1000.0f + 348.0f;
            *screen_y = 263.0f - (player_state.position.z - 500.0f) * ZASTARI_PICTURE1_Z_SCALE / 1000.0f;
            return TRUE;
        }
        break;
    case 2:
        switch (map_slots[current_map_slot].tag) {
        case MAP_TAG_ANCIENT_CITY_L3_WEST:
            *screen_x = (player_state.position.x - 9300.0f) * ZASTARI_PICTURE2_X_SCALE / 1000.0f + 194.0f;
            *screen_y = 167.0f - (player_state.position.z - -4400.0f) * 11.2f / 1000.0f;
            return TRUE;
        case MAP_TAG_SOUTH_CLIFFS:
            *screen_x = (player_state.position.x - 700.0f) * ZASTARI_PICTURE2_X_SCALE / 1000.0f + 335.0f;
            *screen_y = 328.0f - (player_state.position.z - 1650.0f) * 11.2f / 1000.0f;
            return TRUE;
        case MAP_TAG_ANCIENT_CITY_L3_CENTRAL_TOWER:
        case MAP_TAG_ANCIENT_CITY_L3_SOUTH:
        case MAP_TAG_ANCIENT_CITY_L3_SOUTH_WIDDA_AREAS:
            *screen_x = (player_state.position.x - 0.0f) * ZASTARI_PICTURE2_X_SCALE / 1000.0f + 326.0f;
            *screen_y = 168.0f - (player_state.position.z - 0.0f) * 11.2f / 1000.0f;
            return TRUE;
        case MAP_TAG_ANCIENT_CITY_L3_NORTH:
            *screen_x = (player_state.position.x - 3800.0f) * ZASTARI_PICTURE2_X_SCALE / 1000.0f + 326.0f;
            *screen_y = 168.0f - (player_state.position.z - -7900.0f) * 11.2f / 1000.0f;
            return TRUE;
        case MAP_TAG_ANCIENT_CITY_L3_EAST:
        case MAP_TAG_ANCIENT_CITY_L3_EAST_CATACOMBS:
        case MAP_TAG_ANCIENT_CITY_L3_CRAFTSMANS_ROOM:
            *screen_x = (player_state.position.x - -7050.0f) * ZASTARI_PICTURE2_X_SCALE / 1000.0f + 326.0f;
            *screen_y = 168.0f - (player_state.position.z - 0.0f) * 11.2f / 1000.0f;
            return TRUE;
        }
        break;
    }
    return FALSE;
}

/* Where the player's marker goes on the Earth Folk's Map: TRUE with a spot worked out from the player's
   position on maps 017 to 019 and 024 to 025, FALSE elsewhere. */
int MapViewer_LocateOnEarthFolksMap(int *map_x, int *map_y) {
    int area;
    char *entry;

    entry = (char *)map_slots + current_map_slot * 0x60;
    area = ((u8 *)entry)[1];
    if (area >= 0x11) {
        if (area < 0x14) {
            *map_x = (player_state.position.x - 0.0f) * EARTH_FOLKS_MAP_X_SCALE / 1000.0f + 360.0f;
            *map_y = 206.0f - (player_state.position.z - 0.0f) * 22.4f / 1000.0f;
            return TRUE;
        }
        if (area < 0x1A) {
            if (area >= 0x18) {
                *map_x = (player_state.position.x - 0.0f) * EARTH_FOLKS_MAP_X_SCALE / 1000.0f + 205.0f;
                *map_y = 368.0f - (player_state.position.z - 0.0f) * 22.4f / 1000.0f;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* Where the player's marker goes on Widda's Map: a point worked out from the player's position on maps 026
   and 027, FALSE on other maps. */
int MapViewer_LocateOnWiddasMap(int *screen_x, int *screen_y) {
    switch (map_slots[current_map_slot].tag) {
    case MAP_TAG_CLIFF_PASSAGE:
        *screen_x = (player_state.position.x - -3200.0f) * WIDDAS_MAP_X_SCALE / 1000.0f + 438.0f;
        *screen_y = 214.0f - (player_state.position.z - -5600.0f) * WIDDAS_MAP_Z_SCALE / 1000.0f;
        break;
    case MAP_TAG_OCEAN_SHORE:
        *screen_x = (player_state.position.x - -8800.0f) * WIDDAS_MAP_X_SCALE / 1000.0f + 320.0f;
        *screen_y = 82.0f - (player_state.position.z - 900.0f) * WIDDAS_MAP_Z_SCALE / 1000.0f;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

/* The King's Map, a free camera over the map records: runs its own frame loop, in which the pad turns the
   camera about the player (with PAD_CROSS held) or moves it, and both PAD_L1 and PAD_R1 put it back. */
void MapViewer_RunKingsMap(void) {
    VuVector4 position;
    VuVector4 rotation;
    int reset_held;

    reset_held = FALSE;
    memset(&position, 0, sizeof(position));
    position.z = 10000.0f;
    rotation = (VuVector4){ -HALF_PI, player_state.rotation.y, 0.0f, 0.0f };
    {
        f32 world_screen[16];
        f32 view_screen[16];
        f32 world_view[16];
        f32 unused[16];
        int held;
        int pressed;

        sceVu0ViewScreenMatrix(view_screen, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 1.0f, 1048560.0f, 1.0f, 65535.0f);
        do {
            VuVector4 move;

            Display_SwapBuffers(0);
            Packet_SendChain();
            Packet_SwapBuffers();
            DrawList_Clear();
            Input_Update();
            held = pad_state.held;
            pressed = pad_state.pressed;
            move.x = move.y = move.z = move.w = 0.0f;
            if (held & PAD_CROSS) {
                if (held & PAD_DOWN) {
                    rotation.x = Angle_Wrap(rotation.x + DEG_TO_RAD(5));
                }
                if (held & PAD_UP) {
                    rotation.x = Angle_Wrap(rotation.x - DEG_TO_RAD(5));
                }
                if (held & PAD_LEFT) {
                    rotation.y = Angle_Wrap(rotation.y + DEG_TO_RAD(5));
                }
                if (held & PAD_RIGHT) {
                    rotation.y = Angle_Wrap(rotation.y - DEG_TO_RAD(5));
                }
            } else {
                if (held & PAD_LEFT) {
                    move.x = -1.0f;
                }
                if (held & PAD_RIGHT) {
                    move.x += 1.0f;
                }
                if (held & PAD_UP) {
                    move.y -= 1.0f;
                }
                if (held & PAD_DOWN) {
                    move.y += 1.0f;
                }
            }
            if (!reset_held) {
                if ((held & (PAD_L1 | PAD_R1)) == (PAD_L1 | PAD_R1)) {
                    reset_held = TRUE;
                    position.z = 10000.0f;
                    rotation.x = -HALF_PI;
                    rotation.y = player_state.rotation.y;
                    position.x = 0.0f;
                    position.y = 0.0f;
                    position.w = 0.0f;
                    rotation.z = 0.0f;
                    rotation.w = 0.0f;
                } else if (held & PAD_L1) {
                    move.z += 1.0f;
                } else if (held & PAD_R1) {
                    move.z -= 1.0f;
                }
            } else if (!(held & (PAD_L1 | PAD_R1))) {
                reset_held = FALSE;
            }
            if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f) {
                Vector_Normalize(&move, &move);
                Vector_Scale(&move.x, &move.x, 100.0f);
                Vector_Add(&position, &position, &move);
                if (position.x < player_state.position.x + -48000.0f) {
                    position.x = player_state.position.x + -48000.0f;
                }
                if (position.x > player_state.position.x + 48000.0f) {
                    position.x = player_state.position.x + 48000.0f;
                }
                if (position.y < player_state.position.y + -48000.0f) {
                    position.y = player_state.position.y + -48000.0f;
                }
                if (position.y > player_state.position.y + 48000.0f) {
                    position.y = player_state.position.y + 48000.0f;
                }
                if (position.z < 2500.0f) {
                    position.z = 2500.0f;
                }
                if (position.z > 50000.0f) {
                    position.z = 50000.0f;
                }
            }
            Vector_Scale(&move.x, &player_state.position.x, -1.0f);
            sceVu0TransMatrix(world_view, identity_matrix, &move.x);
            sceVu0RotMatrixY(world_view, world_view, -rotation.y);
            sceVu0RotMatrixX(world_view, world_view, -rotation.x);
            sceVu0TransMatrix(world_view, world_view, &player_state.position.x);
            Vector_Subtract(&move, &player_state, &position);
            Vector_Scale(&move.x, &move.x, -1.0f);
            sceVu0TransMatrix(world_view, world_view, &move.x);
            sceVu0MulMatrix(world_screen, view_screen, world_view);
            GsDraw_QueueVu1Matrix(world_screen);
            Packet_OnFrameStart();
            MapViewer_DrawKingsMap();
            Packet_OnFrameEnd();
            // match: it returns a value here, which keeps $2 busy
            ((int (*)(void))Packet_WaitForSend)();
        } while (!(pressed & PAD_CIRCLE));
    }
}

/* Draws the map viewer: the pointer model, pulsing, then every record of the
   ordinary maps in one flat colour, then the viewer's four pictures. */
static void MapViewer_DrawKingsMap(void) {
    int alpha = 0x10;
    int period;
    int map;
    int i;
    MapSlot *slot;
    MapDrawRecord *record;
    int flags;

    game_dbuff_clear_color[0] = 0;
    game_dbuff_clear_color[30] = 0;
    game_dbuff_context2_clear_color[0] = 0;
    game_dbuff_context2_clear_color[30] = 0;
    if (pulse_frame < 45) {
        alpha = (int)(sinf(pulse_frame * PI / 45.0f) * 108.0f) + 0x10;
    }
    period = 60; // match: the divisor is a variable
    pulse_frame = (pulse_frame + 1) % period;
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_2, 0x51001);
    Packet_AddGsRegister(GS_REG_ALPHA_2, ((s64)alpha << 32) | 0x68);
    Packet_EndGsRegisters();
    pointer_model.light_block = (s32)&viewer_default_lights;
    pointer_model.hit_count = 0;
    pointer_model.fog_block = &map_draw_state.current_fog_block;
    Model_DrawPosed(&pointer_model);
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(0x1A, 0);
    Packet_AddGsRegister(0x1B, 0x248);
    Packet_AddGsRegister(GS_REG_ALPHA_2, 0x4000000068L);
    Packet_AddGsRegister(GS_REG_TEST_2, 0x3080B);
    Packet_AddGsRegister(GS_REG_ZBUF_2, 0x10A000118L);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    for (map = 0; map < 2; map++) {
        slot = &map_slots[map];
        record = (MapDrawRecord *)slot->records;
        if (slot->loaded != 0) {
            switch (slot->tag) {
                case MAP_TAG_VILLAGE_RUINS:
                case MAP_TAG_MINERS_CEMETERY:
                case MAP_TAG_EAST_VALLEY:
                case MAP_TAG_UPPER_MINES:
                case MAP_TAG_LOWER_MINES:
                case MAP_TAG_PALACE_OF_OATH_INTERIOR:
                case MAP_TAG_PALACE_OF_OATH_EXTERIOR:
                    break;
                default:
                    for (i = 0; i < ((MapSlotUnk08 *)slot->header)->record_count; i++, record++) {
                        flags = record->flags;
                        if (flags & 0x40000000) {
                            continue;
                        }
                        if (flags & 0x20000000) {
                            continue;
                        }
                        if (flags < 0) {
                            continue;
                        }
                        if (flags & 0x10) {
                            record->model->light_block = (s32)&map_draw_state.current_light_block;
                            record->model->fog_block = &map_draw_state.current_fog_block;
                        } else {
                            record->model->light_block = (s32)&map_draw_state.light_blocks[map][record->light_block_index];
                            record->model->fog_block = &map_draw_state.fog_blocks[map][record->fog_block_index];
                        }
                        record->model->hit_count = 0;
                        record->model->light_block = (s32)&viewer_default_lights;
                        record->model->fog_block = &map_draw_state.current_fog_block;
                        Model_DrawPosed(record->model);
                    }
                    break;
            }
        }
    }
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_2, 0xA000118);
    Packet_AddGsRegister(0x1A, 1);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    Game_BlendScreenOffsetDown();
    GsDraw_QueueFlush();
    Game_TintScreen(0xEA, 0xB2, 0x46);
    TextureTable_QueueOne((TextureTableHeader *)viewer_data, 0);
    GsDraw_QueueTexFlush();
    GsDraw_QueueFlush();
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_1, 0x31001);
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x48);
    Packet_AddGsRegister(8, 5);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    GsDraw_QueueTexturedSprite(&kings_map_overlay_rect);
    TextureTable_QueueOne((TextureTableHeader *)viewer_data, 1);
    GsDraw_QueueTexFlush();
    GsDraw_QueueFlush();
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_1, 0x31001);
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x54);
    Packet_AddGsRegister(8, 5);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    GsDraw_QueueTexturedSprite(&kings_map_overlay_rect);
    TextureTable_QueueOne((TextureTableHeader *)viewer_data, 2);
    GsDraw_QueueTexFlush();
    GsDraw_QueueFlush();
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_1, 0x31001);
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
    Packet_AddGsRegister(8, 0);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    GsDraw_QueueTexturedSprite(&kings_map_corner_rect);
    TextureTable_QueueOne((TextureTableHeader *)viewer_data, 3);
    GsDraw_QueueTexFlush();
    GsDraw_QueueFlush();
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_1, 0x31001);
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x48);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    GsDraw_QueueTexturedSprite(&kings_map_corner_rect);
    Game_ApplyBrightness(game_options[10]);
}

/* Whether the King's Map works on the map with this tag: not on the maps the Caretaker's Map shows (000,
   002, 004, 008, 009, 098, 099). */
int MapViewer_IsKingsMapUsable(int map_tag) {
    switch (map_tag) {
        case MAP_TAG_VILLAGE_RUINS:
        case MAP_TAG_MINERS_CEMETERY:
        case MAP_TAG_EAST_VALLEY:
        case MAP_TAG_UPPER_MINES:
        case MAP_TAG_LOWER_MINES:
        case MAP_TAG_PALACE_OF_OATH_INTERIOR:
        case MAP_TAG_PALACE_OF_OATH_EXTERIOR:
            return FALSE;
    }
    return TRUE;
}
