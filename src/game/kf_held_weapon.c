#include "kf_held_weapon.h"
#include "kf_game.h"
#include "kf_gs.h"
#include "kf_map.h"
#include "kf_model.h"
#include "kf_player.h"
#include "kf_texture.h"
#include "kf_effect.h"

void *memset(void *, int, u32);
void *memcpy(void *, const void *, u32);
int sprintf(char *, char *, ...);
int File_Open(char *, int, int);
int File_Read(int, void *, int);
int File_Close(int);
void Model_SetUpSkinned(ModelInstance *, int *, int *);
void TextureTable_QueueAll(TextureTableHeader *);
MotionData *ModelMotion_LoadFile(MotionFile *, ModelChannel **, ModelInstance *, int);
void GsDraw_QueueTexFlush(void);
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int, s64);
void Packet_EndGsRegisters(void);
u32 *Packet_Alloc(int);
void Packet_AppendToChain(u32 *, u32 *);
void Model_MultiplyChildMatrices(ModelPart *);
void Sound_LoadSwBank(int);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0MulMatrix(void *, void *, void *);
void Vector_Copy(void *, void *);
void Vector_CopyQuadword(void *, void *);
void Vector_Add(void *, void *, void *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void Vu0_LoadMatrix(void *);
void Vu0_ProjectToScreen(int *, void *);

extern char *D_00446F64;
extern PlayerState player_state;
extern GameDefinition D_00416D78[70];
extern PlayerItemDef weapon_defs[];
extern MapSlotUnk0C D_0038A250;
extern char file_load_buffer[0x80000];
extern char D_0043F588[];
extern char D_0043F5A0[];
extern char D_0043F5B8[];
extern char D_0043F5D0[];
extern MapSlotUnk10Half D_0038A270;
extern CameraState D_004325A0;
extern f32 D_00431AF0[16];
extern f32 D_004323E0[16];
extern int D_00446970;
extern int D_00446974;

/* What lights the held item's models, its trail, its position offsets and the points of its arm. */
MapSlotUnk10Half held_item_light_blocks[2] = { /* 0x4145E0 the second hand's block ends with 1.0 four times */
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
u32 D_00414750[12] = { 0x44, 0x0, 0x48, 0x0, 0x42, 0x0, 0x80, 0x80, 0x80, 0x40, 0x0, 0x0 };
PlayerArmPoints held_item_arm_points = { 0 }; /* 0x414780 points 30 and 31 of the held model */
PlayerArmPoints held_item_arm_points_prev = { 0 }; /* 0x4147A0 the same for the frame before */

ModelInstance D_0057B6A0 __attribute__((section(".bss"))); /* 0x57B6A0 the held item; D_00446978 in asm .sdata points here */
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

void WeaponTrail_Reset(void);
void WeaponTrail_SetMinLife(s32);
void WeaponTrail_Age(void);
void WeaponTrail_Push(void *);
void WeaponTrail_GetPoint(void *, s32);
s32 WeaponTrail_GetLife(s32);
int WeaponTrail_GetCount(void);

void HeldWeapon_Init(void) {
    int file_handle;
    u32 heap_cursor;
    heap_cursor = (u32)D_00446F64; // match: retail treats the heap cursor as a plain address
    shared_texture_table = heap_cursor;
    heap_cursor += 0x20000;
    arm_file = heap_cursor;
    heap_cursor += 0x50000;
    held_item_file = heap_cursor;
    memset(&arm_model, 0, 0x1C0);
    memset(&D_0057B6A0, 0, 0x1C0);
    memset((void *)arm_file, 0, 0x50000);
    memset((void *)held_item_file, 0, 0x30000);
    memset(arm_setup, 0, 0x1000);
    memset(held_item_setup, 0, 0x1000);
    memset(&held_item_arm_points, 0, 0x20);
    memset(&held_item_arm_points_prev, 0, 0x20);
    D_00446970 = -1;
    D_00446974 = -1;
    file_handle = File_Open(D_0043F588, 0, 1);
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
    GameDefinition *item = &D_00416D78[player_state.weapon];
    GameDefinition *arm = &D_00416D78[player_state.equipment[EQUIP_GAUNTLET] + 70];
    PlayerArmFile *file;
    s32 size;
    s32 handle;

    if (D_00446974 != player_state.weapon) {
        D_00446974 = player_state.weapon;
        if (player_state.unk258 != 0) {
            player_state.unk258 = 0;
            Effect_FreePlayerWeaponKinds();
        }
        memset((void *)held_item_file, 0, 0x30000);
        memset(&D_0057B6A0, 0, 0x1C0);
        memset(held_item_setup, 0, 0x1000);
        memset(held_item_channels, 0, 0x90);
        held_item_textures = 0;
        sprintf(path, D_0043F5A0, item->unk30);
        handle = File_Open(path, 0, 1);
        if (handle < 0) {
            handle = File_Open(D_0043F5B8, 0, 1);
        }
        File_Read(handle, file_load_buffer, -1);
        File_Close(handle);
        file = (PlayerArmFile *)file_load_buffer;
        if (file->motion_size != 0) {
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
            Model_SetUpSkinned(&D_0057B6A0, &data, &buffer);
        }
        if (file->motion_size != 0) {
            data = held_item_file + file->motion_offset;
            buffer = (int)held_item_channels;
            ModelMotion_LoadFile(data, &buffer, &D_0057B6A0, 1);
        }
        if (file->texture_size != 0) {
            held_item_textures = held_item_file + file->texture_offset;
        }
        D_0057B6A0.unk148 = (s32)held_item_light_blocks;
        D_0057B6A0.unk14C = &D_0038A250;
        MODEL_CHANNEL(&D_0057B6A0)->unk8F = 0x80;
        Sound_LoadSwBank(player_state.weapon);
        WeaponTrail_Reset();
        WeaponTrail_SetMinLife(3);
        held_item_offsets[0] = weapon_defs[player_state.weapon].unk87;
        held_item_offsets[1] = weapon_defs[player_state.weapon].unk88;
        held_item_offsets[2] = weapon_defs[player_state.weapon].unk89;
    }
    if (D_00446970 != player_state.equipment[EQUIP_GAUNTLET]) {
        D_00446970 = player_state.equipment[EQUIP_GAUNTLET];
        memset((void *)arm_file, 0, 0x50000);
        memset(&arm_model, 0, 0x1C0);
        memset(arm_setup, 0, 0x1000);
        memset(arm_channels, 0, 0x90);
        arm_textures = 0;
        sprintf(path, D_0043F5A0, arm->unk30);
        handle = File_Open(path, 0, 1);
        if (handle < 0) {
            handle = File_Open(D_0043F5D0, 0, 1);
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
        arm_model.unk148 = (s32)held_item_light_blocks;
        arm_model.unk14C = &D_0038A250;
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
    if (player_state.unk258 != 0) {
        sceVu0RotMatrixY(matrix, D_00431AF0, 3.1415927f);
        sceVu0TransMatrix(matrix, matrix, held_item_offsets);
        pitch = player_state.rotation.x - player_state.unk238;
        if (pitch > 1.0122910f) {
            pitch = 1.0122910f;
        } else if (pitch < -1.0122910f) {
            pitch = -1.0122910f;
        }
        sceVu0RotMatrixX(matrix, matrix, pitch);
        sceVu0RotMatrixY(matrix, matrix, player_state.rotation.y);
        Vector_Copy(position, &player_state);
        position[1] = position[1] - 220.0f + player_state.unk1C4 + player_state.unk1F0;
        Vector_Add(position, position, D_004325A0.unk50);
        Vector_Add(position, position, D_004325A0.unk60);
        position[1] += D_004325A0.unk4C;
        sceVu0TransMatrix((f32 *)D_0057B6A0.unk40, matrix, position);
        count = D_0057B6A0.part_count;
        part = D_0057B6A0.parts;
        for (i = 0; i < count; i++, part++) {
            ModelMatrix_BuildPartLocal(&D_0057B6A0, part, part);
        }
        if (D_0057B6A0.unk100 & 1) {
            part = D_0057B6A0.parts;
            for (i = 0; i < count; i++, part++) {
                Model_MultiplyChildMatrices(part);
            }
        }
        Vector_Copy(&D_0057B6A0.rotation, &player_state.rotation);
        Vector_Copy(&arm_model.rotation, &player_state.rotation);
        *(ModelMatrix *)arm_model.unk40 = *(ModelMatrix *)D_0057B6A0.unk40;
        part = D_0057B6A0.parts;
        arm_part = arm_model.parts;
        // match: retail takes the larger part count of the two models, not the smaller
        count = D_0057B6A0.part_count > arm_model.part_count ? D_0057B6A0.part_count : arm_model.part_count;
        for (i = 0; i < count; i++, part++, arm_part++) {
            Vector_Copy(arm_part->unk40, part->unk40);
            Vector_Copy(&arm_part->unk50, &part->unk50);
            Vector_Copy(arm_part->unk60, part->unk60);
            *(ModelMatrix *)arm_part->unk00 = *(ModelMatrix *)part->unk00;
        }
        D_0057B6A0.unk1B4(&D_0057B6A0);
        Vector_Copy(&held_item_arm_points.points[0], (void *)D_0057B6A0.get_point(&D_0057B6A0, 30));
        Vector_Copy(&held_item_arm_points.points[1], (void *)D_0057B6A0.get_point(&D_0057B6A0, 31));
    }
    held_item_light_blocks[0] = D_0038A270;
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
    if (player_state.unk258 != 0) {
        weapon = &D_0057B6A0;
        weapon->unk1B4(weapon);
        if (weapon_defs[player_state.weapon].unk63 == 2) {
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
        sceVu0MulMatrix(world_screen, D_004323E0, D_00431AF0);
        Vu0_LoadMatrix(world_screen);
        Vu0_ProjectToScreen((int *)&point, blade_base);
        point.z = (s32)point.z >> 4;
        if (point.z > 0xFFFF0 || ((point.x | point.y) & 0xFFFF0000)) {
            point.unk0C |= 0x8000;
        } else {
            point.unk0C = 0;
        }
        WeaponTrail_Push(&point);
        Vu0_ProjectToScreen((int *)&point, blade_tip);
        point.z = (s32)point.z >> 4;
        if (point.z > 0xFFFF0 || ((point.x | point.y) & 0xFFFF0000)) {
            point.unk0C |= 0x8000;
        } else {
            point.unk0C = 0;
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

    if (player_state.unk258 != 0) {
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
        arm_model.unk1A8(&arm_model);
        TextureTable_QueueAll((TextureTableHeader *)held_item_textures);
        GsDraw_QueueTexFlush();
        if (player_state.weapon != 0x2E) {
            D_0057B6A0.unk1A8(&D_0057B6A0);
        } else {
            for (i = 0; i < D_0057B6A0.part_count; i++) {
                MODEL_PART_FLAG_BITS(&D_0057B6A0.parts[i])->bit40 = i == 5;
            }
            D_0057B6A0.unk1A8(&D_0057B6A0);
            for (i = 0; i < D_0057B6A0.part_count; i++) {
                MODEL_PART_FLAG_BITS(&D_0057B6A0.parts[i])->bit40 = i != 5;
            }
            Packet_BeginGsRegisters();
            Packet_AddGsRegister(GS_REG_ALPHA_1, 0x48);
            Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
            Packet_EndGsRegisters();
            D_0057B6A0.unk1A8(&D_0057B6A0);
            for (i = 0; i < D_0057B6A0.part_count; i++) {
                MODEL_PART_FLAG_BITS(&D_0057B6A0.parts[i])->bit40 = FALSE;
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
    if (player_state.unk122[0] != 0 && player_state.unk18E[0] > best) {
        best = player_state.unk18E[0];
        trail_kind = 1;
    }
    if (player_state.unk122[1] != 0 && player_state.unk18E[1] > best) {
        best = player_state.unk18E[1];
        trail_kind = 2;
    }
    if (player_state.unk122[2] != 0 && player_state.unk18E[2] > best) {
        best = player_state.unk18E[2];
        trail_kind = 3;
    }
    if (player_state.unk122[3] != 0 && player_state.unk18E[3] > best) {
        best = player_state.unk18E[3];
        trail_kind = 4;
    }
    if (player_state.unk122[4] != 0 && player_state.unk18E[4] > best) {
        best = player_state.unk18E[4];
        trail_kind = 5;
    }
    if (player_state.unk122[5] != 0 && player_state.unk18E[5] > best) {
        best = player_state.unk18E[5];
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
