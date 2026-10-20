#include "player/held_weapon.h"
#include "math/camera.h"
#include "core/game.h"
#include "gfx/gs.h"
#include "map/map.h"
#include "model/model.h"
#include "player/player.h"
#include "gfx/texture.h"
#include "effect/effect.h"
#include "core/file.h"
#include "core/memory.h"
#include "gfx/gs_draw.h"
#include "gfx/packet.h"
#include <libvu0.h>
#include "math/vu0.h"
#include "model/model_matrix.h"
#include "model/motion.h"
#include "sound/sound_game.h"
#include <stdio.h>
#include <string.h>
#include "item/item.h"


/* What lights the held item's models, its trail, its position offsets and the points of its arm. */
MapLightBlockHalf held_item_light_blocks[2] = { /* 0x4145E0 the second hand's block ends with 1.0 four times */
    { { 0 } },
    { { 0, 0, 0, 0, 0, 0, 0x3F8000003F800000, 0x3F8000003F800000 } },
};
/* 0x414660 the colour, GS blend value and life of each kind of weapon trail (WeaponTrail_PickKind picks one) */
WeaponTrailKind weapon_trail_kinds[7] = {
    { 0x80, 0x80, 0x80, 0x40, 0x48, 3 },
    { 0xDF, 0x00, 0x29, 0x40, 0x48, 3 },
    { 0xEE, 0x9C, 0x00, 0x40, 0x48, 3 },
    { 0x00, 0xA4, 0x8F, 0x40, 0x48, 3 },
    { 0x00, 0xB2, 0xEB, 0x40, 0x48, 3 },
    { 0xFD, 0xFD, 0xC4, 0x40, 0x48, 3 },
    { 0x47, 0x4D, 0x7C, 0x40, 0x48, 3 },
};
f32 held_item_offsets[4] = { 16.0f, 19.0f, 22.0f, 1.0f }; /* 0x414740 the held model's position offsets */
/* 0x414750 twelve words after the offsets that nothing in the game reads */
u32 held_item_offsets_tail_words[12] = { 0x44, 0x0, 0x48, 0x0, 0x42, 0x0, 0x80, 0x80, 0x80, 0x40, 0x0, 0x0 };
PlayerArmPoints held_item_arm_points = { 0 }; /* 0x414780 points 30 and 31 of the held model */
PlayerArmPoints held_item_arm_points_prev = { 0 }; /* 0x4147A0 the same for the frame before */

ModelInstance held_item_model __attribute__((section(".bss"))); /* 0x57B6A0 the held item; held_item_model_ptr in asm .sdata points here */
static ModelInstance arm_model;         /* 0x57B860 */
static char held_item_channels[0x90];   /* 0x57BA20 */
static char held_item_setup[0x1000];    /* 0x57BAB0 */
static char arm_channels[0x90];         /* 0x57CAB0 */
static char arm_setup[0x1000];          /* 0x57CB40 */
static Vector4 trail_points[64];        /* 0x57DB40 screen positions of the weapon trail */
static s32 trail_lives[64];             /* 0x57DF40 frames each trail point has left */

static u32 shared_texture_table;        /* 0x447164 */
static u32 held_item_file;              /* 0x447168 */
static u32 held_item_textures;          /* 0x44716C */
static u32 arm_file;                    /* 0x447170 */
static u32 arm_textures;                /* 0x447174 */
static s32 trail_min_life;              /* 0x447178 */
static s32 trail_count;                 /* 0x44717C */
static s32 trail_write_index;           /* 0x447180 */
static s32 trail_read_index;            /* 0x447184 */
static s32 trail_kind;                  /* 0x447188 index into weapon_trail_kinds */

void HeldWeapon_Init(void) {
    int file_handle;
    u32 heap_cursor;
    heap_cursor = (u32)held_weapon_area; // match: retail treats the heap cursor as a plain address
    shared_texture_table = heap_cursor;
    heap_cursor += 0x20000;
    arm_file = heap_cursor;
    heap_cursor += 0x50000;
    held_item_file = heap_cursor;
    memset(&arm_model, 0, 0x1C0);
    memset(&held_item_model, 0, 0x1C0);
    memset((void *)arm_file, 0, 0x50000);
    memset((void *)held_item_file, 0, 0x30000);
    memset(arm_setup, 0, 0x1000);
    memset(held_item_setup, 0, 0x1000);
    memset(&held_item_arm_points, 0, 0x20);
    memset(&held_item_arm_points_prev, 0, 0x20);
    loaded_arm_gauntlet = -1;
    loaded_held_weapon = -1;
    file_handle = File_Open(arm_phong_texture_path, 0, 1);
    File_Read(file_handle, (int)shared_texture_table, -1);
    File_Close(file_handle);
    Player_LoadArmModels();
}

/* Loads the model of the item the player holds (PlayerState.weapon) and of the arm that holds it
   (unk41) when either changed: reads the item_arm file, copies it to its own buffer and sets up
   the model, its motions and its textures from the parts the file has. */
void Player_LoadArmModels(void) {
    char path[0x100];
    int data;
    int buffer;
    GameDefinition *item = &weapon_name_defs[player_state.weapon];
    GameDefinition *arm = &weapon_name_defs[player_state.equipment[EQUIP_GAUNTLET] + 70];
    PlayerArmFile *file;
    s32 size;
    s32 handle;

    if (loaded_held_weapon != player_state.weapon) {
        loaded_held_weapon = player_state.weapon;
        if (player_state.item_action_step != PLAYER_ITEM_ACTION_IDLE) {
            player_state.item_action_step = PLAYER_ITEM_ACTION_IDLE;
            Effect_FreePlayerWeaponKinds();
        }
        memset((void *)held_item_file, 0, 0x30000);
        memset(&held_item_model, 0, 0x1C0);
        memset(held_item_setup, 0, 0x1000);
        memset(held_item_channels, 0, 0x90);
        held_item_textures = 0;
        sprintf(path, item_arm_path_format, item->model_file_number);
        handle = File_Open(path, 0, 1);
        if (handle < 0) {
            handle = File_Open(default_held_item_path, 0, 1);
        }
        File_Read(handle, file_load_buffer, -1);
        File_Close(handle);
        file = (PlayerArmFile *)file_load_buffer;
        if (file->motion_size) {
            size = file->motion_offset + file->motion_size;
        } else if (file->model_size != 0) {
            size = file->model_offset + file->model_size;
        } else if (file->texture_size != 0) {
            size = file->texture_offset + file->texture_size;
        } else {
            size = 0x40;
        }
        memcpy((void *)held_item_file, file_load_buffer, size);
        file = (PlayerArmFile *)held_item_file;
        if (file->model_size != 0) {
            data = (int)file + file->model_offset;
            buffer = (int)held_item_setup;
            Model_SetUpSkinned(&held_item_model, &data, &buffer);
        }
        if (file->motion_size) {
            data = held_item_file + file->motion_offset;
            buffer = (int)held_item_channels;
            ModelMotion_LoadFile(data, &buffer, &held_item_model, 1);
        }
        if (file->texture_size != 0) {
            held_item_textures = held_item_file + file->texture_offset;
        }
        held_item_model.light_block = (s32)held_item_light_blocks;
        held_item_model.fog_block = &current_fog_block;
        MODEL_CHANNEL(&held_item_model)->blend_speed = 0x80;
        Sound_LoadSwBank(player_state.weapon);
        WeaponTrail_Reset();
        WeaponTrail_SetMinLife(3);
        held_item_offsets[0] = weapon_defs[player_state.weapon].held_offset_x;
        held_item_offsets[1] = weapon_defs[player_state.weapon].held_offset_y;
        held_item_offsets[2] = weapon_defs[player_state.weapon].held_offset_z;
    }
    if (loaded_arm_gauntlet != player_state.equipment[EQUIP_GAUNTLET]) {
        loaded_arm_gauntlet = player_state.equipment[EQUIP_GAUNTLET];
        memset((void *)arm_file, 0, 0x50000);
        memset(&arm_model, 0, 0x1C0);
        memset(arm_setup, 0, 0x1000);
        memset(arm_channels, 0, 0x90);
        arm_textures = 0;
        sprintf(path, item_arm_path_format, arm->model_file_number);
        handle = File_Open(path, 0, 1);
        if (handle < 0) {
            handle = File_Open(default_arm_path, 0, 1);
        }
        File_Read(handle, file_load_buffer, -1);
        File_Close(handle);
        file = (PlayerArmFile *)file_load_buffer;
        if (file->model_size != 0) {
            size = file->model_offset + file->model_size;
        } else if (file->texture_size != 0) {
            size = file->texture_offset + file->texture_size;
        } else {
            size = 0x40;
        }
        memcpy((void *)arm_file, file_load_buffer, size);
        file = (PlayerArmFile *)arm_file;
        if (file->model_size != 0) {
            data = (int)file + file->model_offset;
            buffer = (int)arm_setup;
            Model_SetUpSkinned(&arm_model, &data, &buffer);
        }
        if (file->texture_size != 0) {
            arm_textures = arm_file + file->texture_offset;
        }
        arm_model.light_block = (s32)held_item_light_blocks;
        arm_model.fog_block = &current_fog_block;
    }
}

/* Each frame: places the held item's model in front of the camera (turned with the player's view),
   poses its parts, copies the pose to the arm's model and notes two of its points. */
void Player_UpdateArmModels(void) {
    f32 matrix[16];
    f32 position[4];
    f32 unused[4];
    f32 pitch;
    ModelPart *part;
    ModelPart *arm_part;
    s32 count;
    s32 i;

    held_item_arm_points_prev = held_item_arm_points;
    memset(&held_item_arm_points, 0, 0x20);
    if (player_state.item_action_step != PLAYER_ITEM_ACTION_IDLE) {
        sceVu0RotMatrixY(matrix, identity_matrix, PI);
        sceVu0TransMatrix(matrix, matrix, held_item_offsets);
        pitch = player_state.rotation.x - player_state.view_tilt_pitch;
        if (pitch > DEG_TO_RAD(58)) {
            pitch = DEG_TO_RAD(58);
        } else if (pitch < -DEG_TO_RAD(58)) {
            pitch = -DEG_TO_RAD(58);
        }
        sceVu0RotMatrixX(matrix, matrix, pitch);
        sceVu0RotMatrixY(matrix, matrix, player_state.rotation.y);
        Vector_Copy(position, &player_state);
        position[1] = position[1] - 220.0f + player_state.head_bob_offset + player_state.landing_dip_offset;
        Vector_Add(position, position, camera_state.script_shake_offset);
        Vector_Add(position, position, camera_state.entity_shake_offset);
        position[1] += camera_state.height_offset;
        sceVu0TransMatrix((f32 *)held_item_model.model_matrix, matrix, position);
        count = held_item_model.part_count;
        part = held_item_model.parts;
        for (i = 0; i < count; i++, part++) {
            ModelMatrix_BuildPartLocal(&held_item_model, part, part);
        }
        if (held_item_model.model_flags & 1) {
            part = held_item_model.parts;
            for (i = 0; i < count; i++, part++) {
                Model_MultiplyChildMatrices(part);
            }
        }
        Vector_Copy(&held_item_model.rotation, &player_state.rotation);
        Vector_Copy(&arm_model.rotation, &player_state.rotation);
        *(ModelMatrix *)arm_model.model_matrix = *(ModelMatrix *)held_item_model.model_matrix;
        part = held_item_model.parts;
        arm_part = arm_model.parts;
        // match: retail takes the larger part count of the two models, not the smaller
        count = held_item_model.part_count > arm_model.part_count ? held_item_model.part_count : arm_model.part_count;
        for (i = 0; i < count; i++, part++, arm_part++) {
            Vector_Copy(arm_part->position, part->position);
            Vector_Copy(&arm_part->rotation_x, &part->rotation_x);
            Vector_Copy(arm_part->scale, part->scale);
            *(ModelMatrix *)arm_part->matrix = *(ModelMatrix *)part->matrix;
        }
        held_item_model.update_bone_points(&held_item_model);
        Vector_Copy(&held_item_arm_points.points[0], (void *)held_item_model.get_point(&held_item_model, 30));
        Vector_Copy(&held_item_arm_points.points[1], (void *)held_item_model.get_point(&held_item_model, 31));
    }
    held_item_light_blocks[0] = current_light_block;
}

/* each frame: ages the weapon trail and queues the two ends of the held weapon, in screen space */
void WeaponTrail_Update(void) {
    f32 world_screen[16];
    f32 unused[4];
    ModelScreenPoint point;
    ModelInstance *weapon;
    void *blade_base;
    void *blade_tip;
    s32 item;

    WeaponTrail_Age();
    if (player_state.item_action_step != PLAYER_ITEM_ACTION_IDLE) {
        weapon = &held_item_model;
        weapon->update_bone_points(weapon);
        if (weapon_defs[player_state.weapon].action_handler_index == 2) {
            return;
        }
        item = player_state.weapon + ITEM_WEAPON_FIRST;
        if (item == 0x97) {
            return;
        }
        if (item == 0x98) {
            return;
        }
        blade_base = (void *)weapon->get_point(weapon, 0);
        blade_tip = (void *)weapon->get_point(weapon, 1);
        sceVu0MulMatrix(world_screen, world_screen_matrix, identity_matrix);
        Vu0_LoadMatrix(world_screen);
        Vu0_ProjectToScreen((int *)&point, blade_base);
        point.z = (s32)point.z >> 4;
        if (point.z > 0xFFFF0 || ((point.x | point.y) & 0xFFFF0000)) {
            point.flags |= MODEL_SCREEN_POINT_FLAG_OFF_SCREEN;
        } else {
            point.flags = 0;
        }
        WeaponTrail_Push(&point);
        Vu0_ProjectToScreen((int *)&point, blade_tip);
        point.z = (s32)point.z >> 4;
        if (point.z > 0xFFFF0 || ((point.x | point.y) & 0xFFFF0000)) {
            point.flags |= MODEL_SCREEN_POINT_FLAG_OFF_SCREEN;
        } else {
            point.flags = 0;
        }
        WeaponTrail_Push(&point);
    }
}

/* draws the held weapon and the hand, then the weapon trail as one triangle strip */
void Player_DrawArm(void) {
    s32 i;
    s32 count;
    u32 *packet;
    u32 *packet_start;
    u32 clipped;
    s32 size;
    u64 tag;

    if (player_state.item_action_step != PLAYER_ITEM_ACTION_IDLE) {
        TextureTable_QueueAll((TextureTableHeader *)shared_texture_table);
        GsDraw_QueueTexFlush();
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
        Packet_AddGsRegister(GS_REG_ALPHA_2, 9);
        Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
        Packet_AddGsRegister(GS_REG_TEST_2, 0x5000D);
        Packet_EndGsRegisters();
        TextureTable_QueueAll((TextureTableHeader *)arm_textures);
        GsDraw_QueueTexFlush();
        arm_model.draw_parts(&arm_model);
        TextureTable_QueueAll((TextureTableHeader *)held_item_textures);
        GsDraw_QueueTexFlush();
        if (player_state.weapon != 0x2E) {
            held_item_model.draw_parts(&held_item_model);
        } else {
            for (i = 0; i < held_item_model.part_count; i++) {
                MODEL_PART_FLAG_BITS(&held_item_model.parts[i])->bit40 = i == 5;
            }
            held_item_model.draw_parts(&held_item_model);
            for (i = 0; i < held_item_model.part_count; i++) {
                MODEL_PART_FLAG_BITS(&held_item_model.parts[i])->bit40 = i != 5;
            }
            Packet_BeginGsRegisters();
            Packet_AddGsRegister(GS_REG_ALPHA_1, 0x48);
            Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
            Packet_EndGsRegisters();
            held_item_model.draw_parts(&held_item_model);
            for (i = 0; i < held_item_model.part_count; i++) {
                MODEL_PART_FLAG_BITS(&held_item_model.parts[i])->bit40 = FALSE;
            }
        }
    }
    count = WeaponTrail_GetCount();
    if (count >= 4) {
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ALPHA_1, weapon_trail_kinds[trail_kind].blend);
        Packet_AddGsRegister(GS_REG_TEST_1, 0x30000);
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
        Packet_EndGsRegisters();
        clipped = 6;
        size = count * 2 + 1;
        packet_start = packet = Packet_Alloc(size + 3);
        *packet = (size + 1) | 0x10000000;
        packet += 7;
        *packet++ = size | 0x51000000;
        /* GIF tag: count vertices of two registers each, a shaded, blended triangle strip */
        tag = count | 0x8000 | (0x8000L << 46);
        tag |= 0x9900L << 38; // match: a second statement, or the two constants fold into one
        *(u64 *)packet = tag;
        packet += 2;
        *(u64 *)packet = 0x51; /* the registers: RGBAQ, XYZ2 */
        packet += 2;
        for (i = 0; i < count; i++) {
            clipped >>= 1;
            *packet++ = weapon_trail_kinds[trail_kind].red;
            *packet++ = weapon_trail_kinds[trail_kind].green;
            *packet++ = weapon_trail_kinds[trail_kind].blue;
            *packet++ = weapon_trail_kinds[trail_kind].alpha * (WeaponTrail_GetLife(i) - 1) / (weapon_trail_kinds[trail_kind].lifetime - 1);
            WeaponTrail_GetPoint(packet, i);
            packet += 3;
            /* a point off the screen keeps the three triangles it is part of from being drawn */
            if (*packet != 0) {
                clipped |= 4;
            }
            if (clipped != 0) {
                *packet++ = 0x8000;
            } else {
                *packet++ = 0;
            }
        }
        Packet_AppendToChain(packet_start, packet);
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
        Packet_EndGsRegisters();
    }
}

/* picks the weapon trail kind: the strongest of six values, else by unk43 */
void WeaponTrail_PickKind(void) {
    s32 best = 0;

    trail_kind = 0;
    if (player_state.magic_strength_bonus[0] != 0 && player_state.magic_bonus_stamp[0] > best) {
        best = player_state.magic_bonus_stamp[0];
        trail_kind = 1;
    }
    if (player_state.magic_strength_bonus[1] != 0 && player_state.magic_bonus_stamp[1] > best) {
        best = player_state.magic_bonus_stamp[1];
        trail_kind = 2;
    }
    if (player_state.magic_strength_bonus[2] != 0 && player_state.magic_bonus_stamp[2] > best) {
        best = player_state.magic_bonus_stamp[2];
        trail_kind = 3;
    }
    if (player_state.magic_strength_bonus[3] != 0 && player_state.magic_bonus_stamp[3] > best) {
        best = player_state.magic_bonus_stamp[3];
        trail_kind = 4;
    }
    if (player_state.magic_strength_bonus[4] != 0 && player_state.magic_bonus_stamp[4] > best) {
        best = player_state.magic_bonus_stamp[4];
        trail_kind = 5;
    }
    if (player_state.magic_strength_bonus[5] != 0 && player_state.magic_bonus_stamp[5] > best) {
        best = player_state.magic_bonus_stamp[5];
        trail_kind = 6;
    }
    if (trail_kind == 0) {
        if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_FIRE)) {
            trail_kind = 1;
        }
        if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_EARTH)) {
            trail_kind = 2;
        }
        if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_WIND)) {
            trail_kind = 3;
        }
        if (player_state.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_WATER)) {
            trail_kind = 4;
        }
    }
}

void WeaponTrail_Reset(void) {
    memset(trail_points, 0, 0x400);
    memset(trail_lives, 0, 0x100);
    trail_count = 0;
    trail_min_life = 1;
    trail_write_index = 0;
    trail_read_index = 0;
}

void WeaponTrail_SetMinLife(s32 min_life) {
    if (min_life > 0) {
        trail_min_life = min_life;
    }
}

void WeaponTrail_GetPoint(void *dst, s32 index) {
    s32 table_index;

    if (index < trail_count) {
        table_index = trail_read_index + index;
        Vector_CopyQuadword(dst, &trail_points[table_index % 64]);
    }
}

/* ages every queued trail point and drops the expired ones from the front */
void WeaponTrail_Age(void) {
    s32 i;
    s32 slot;
    s32 count = trail_count;

    for (i = 0; i < count; i++) {
        slot = (trail_read_index + i) % 64;
        trail_lives[slot]--;
        if (trail_lives[slot] <= 0) {
            trail_count--;
            trail_read_index = (trail_read_index + 1) % 64;
        }
    }
}

/* queues a weapon trail point; it lives as long as the current kind says */
void WeaponTrail_Push(void *position) {
    if (trail_count + 1 <= 64) {
        Vector_CopyQuadword(&trail_points[trail_write_index], position);
        trail_lives[trail_write_index] = weapon_trail_kinds[trail_kind].lifetime;
        trail_write_index = (trail_write_index + 1) % 64;
        trail_count++;
    }
}

/* frames left of the index-th queued trail point, oldest first */
s32 WeaponTrail_GetLife(s32 index) {
    // match: retail accepts index == count, one past the newest point (WeaponTrail_GetPoint tests index < count)
    if (index <= trail_count) {
        return trail_lives[(trail_read_index + index) % 64];
    }
    return 0;
}

int WeaponTrail_GetCount(void) {
    return trail_count;
}
