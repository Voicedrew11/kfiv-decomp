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
int func_00102238(char *, int, int);
int func_001025C0(int, void *, int);
int func_001024B0(int);
void func_0010B758(ModelInstance *, int *, int *);
void func_0010B238(TextureTableHeader *);
MotionData *func_00108940(MotionFile *, ModelChannel **, ModelInstance *, int);
void func_00109E88(void);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
void func_0010B5D0(ModelPart *);
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
extern PlayerState D_00413F80;
extern GameDefinition D_00416D78[70];
extern PlayerItemDef weapon_defs[];
extern f32 D_00414740[4];
extern MapSlotUnk10Half D_004145E0[2];
extern MapSlotUnk0C D_0038A250;
extern char D_00249680[0x80000];
extern char D_0043F588[];
extern char D_0043F5A0[];
extern char D_0043F5B8[];
extern char D_0043F5D0[];
extern PlayerArmPoints D_00414780;
extern PlayerArmPoints D_004147A0;
extern MapSlotUnk10Half D_0038A270;
extern CameraState D_004325A0;
extern f32 D_00431AF0[16];
extern f32 D_004323E0[16];
extern WeaponTrailKind D_00414660[7];

extern int D_00446970;
extern int D_00446974;

ModelInstance D_0057B6A0;               /* the held item; D_00446978 in asm .sdata points here */
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
static s32 trail_kind;                  /* 0x447188 index into D_00414660 */

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
    memset(&D_00414780, 0, 0x20);
    memset(&D_004147A0, 0, 0x20);
    D_00446970 = -1;
    D_00446974 = -1;
    file_handle = func_00102238(D_0043F588, 0, 1);
    func_001025C0(file_handle, (int)shared_texture_table, -1);
    func_001024B0(file_handle);
    Player_LoadArmModels();
}

/* Loads the model of the item the player holds (PlayerState.weapon) and of the arm that holds it
   (unk41) when either changed: reads the item_arm file, copies it to its own buffer and sets up
   the model, its motions and its textures from the parts the file has. */
void Player_LoadArmModels(void) {
    char path[0x100];
    int data;
    int buffer;
    GameDefinition *item = &D_00416D78[D_00413F80.weapon];
    GameDefinition *arm = &D_00416D78[D_00413F80.equipment[EQUIP_GAUNTLET] + 70];
    PlayerArmFile *file;
    s32 size;
    s32 handle;

    if (D_00446974 != D_00413F80.weapon) {
        D_00446974 = D_00413F80.weapon;
        if (D_00413F80.unk258 != 0) {
            D_00413F80.unk258 = 0;
            Effect_FreePlayerWeaponKinds();
        }
        memset((void *)held_item_file, 0, 0x30000);
        memset(&D_0057B6A0, 0, 0x1C0);
        memset(held_item_setup, 0, 0x1000);
        memset(held_item_channels, 0, 0x90);
        held_item_textures = 0;
        sprintf(path, D_0043F5A0, item->unk30);
        handle = func_00102238(path, 0, 1);
        if (handle < 0) {
            handle = func_00102238(D_0043F5B8, 0, 1);
        }
        func_001025C0(handle, D_00249680, -1);
        func_001024B0(handle);
        file = (PlayerArmFile *)D_00249680;
        if (file->motion_size != 0) {
            size = file->motion_offset + file->motion_size;
        } else if (file->model_size != 0) {
            size = file->model_offset + file->model_size;
        } else if (file->texture_size != 0) {
            size = file->texture_offset + file->texture_size;
        } else {
            size = 0x40;
        }
        memcpy((void *)held_item_file, D_00249680, size);
        file = (PlayerArmFile *)held_item_file;
        if (file->model_size != 0) {
            data = (int)file + file->model_offset;
            buffer = (int)held_item_setup;
            func_0010B758(&D_0057B6A0, &data, &buffer);
        }
        if (file->motion_size != 0) {
            data = held_item_file + file->motion_offset;
            buffer = (int)held_item_channels;
            func_00108940(data, &buffer, &D_0057B6A0, 1);
        }
        if (file->texture_size != 0) {
            held_item_textures = held_item_file + file->texture_offset;
        }
        D_0057B6A0.unk148 = (s32)D_004145E0;
        D_0057B6A0.unk14C = &D_0038A250;
        MODEL_CHANNEL(&D_0057B6A0)->unk8F = 0x80;
        Sound_LoadSwBank(D_00413F80.weapon);
        WeaponTrail_Reset();
        WeaponTrail_SetMinLife(3);
        D_00414740[0] = weapon_defs[D_00413F80.weapon].unk87;
        D_00414740[1] = weapon_defs[D_00413F80.weapon].unk88;
        D_00414740[2] = weapon_defs[D_00413F80.weapon].unk89;
    }
    if (D_00446970 != D_00413F80.equipment[EQUIP_GAUNTLET]) {
        D_00446970 = D_00413F80.equipment[EQUIP_GAUNTLET];
        memset((void *)arm_file, 0, 0x50000);
        memset(&arm_model, 0, 0x1C0);
        memset(arm_setup, 0, 0x1000);
        memset(arm_channels, 0, 0x90);
        arm_textures = 0;
        sprintf(path, D_0043F5A0, arm->unk30);
        handle = func_00102238(path, 0, 1);
        if (handle < 0) {
            handle = func_00102238(D_0043F5D0, 0, 1);
        }
        func_001025C0(handle, D_00249680, -1);
        func_001024B0(handle);
        file = (PlayerArmFile *)D_00249680;
        if (file->model_size != 0) {
            size = file->model_offset + file->model_size;
        } else if (file->texture_size != 0) {
            size = file->texture_offset + file->texture_size;
        } else {
            size = 0x40;
        }
        memcpy((void *)arm_file, D_00249680, size);
        file = (PlayerArmFile *)arm_file;
        if (file->model_size != 0) {
            data = (int)file + file->model_offset;
            buffer = (int)arm_setup;
            func_0010B758(&arm_model, &data, &buffer);
        }
        if (file->texture_size != 0) {
            arm_textures = arm_file + file->texture_offset;
        }
        arm_model.unk148 = (s32)D_004145E0;
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

    D_004147A0 = D_00414780;
    memset(&D_00414780, 0, 0x20);
    if (D_00413F80.unk258 != 0) {
        sceVu0RotMatrixY(matrix, D_00431AF0, 3.1415927f);
        sceVu0TransMatrix(matrix, matrix, D_00414740);
        pitch = D_00413F80.rotation.x - D_00413F80.unk238;
        if (pitch > 1.0122910f) {
            pitch = 1.0122910f;
        } else if (pitch < -1.0122910f) {
            pitch = -1.0122910f;
        }
        sceVu0RotMatrixX(matrix, matrix, pitch);
        sceVu0RotMatrixY(matrix, matrix, D_00413F80.rotation.y);
        Vector_Copy(position, &D_00413F80);
        position[1] = position[1] - 220.0f + D_00413F80.unk1C4 + D_00413F80.unk1F0;
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
                func_0010B5D0(part);
            }
        }
        Vector_Copy(&D_0057B6A0.rotation, &D_00413F80.rotation);
        Vector_Copy(&arm_model.rotation, &D_00413F80.rotation);
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
        Vector_Copy(&D_00414780.points[0], (void *)D_0057B6A0.get_point(&D_0057B6A0, 30));
        Vector_Copy(&D_00414780.points[1], (void *)D_0057B6A0.get_point(&D_0057B6A0, 31));
    }
    D_004145E0[0] = D_0038A270;
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
    if (D_00413F80.unk258 != 0) {
        weapon = &D_0057B6A0;
        weapon->unk1B4(weapon);
        if (weapon_defs[D_00413F80.weapon].unk63 == 2) {
            return;
        }
        item = D_00413F80.weapon + ITEM_WEAPON_FIRST;
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

    if (D_00413F80.unk258 != 0) {
        func_0010B238((TextureTableHeader *)shared_texture_table);
        func_00109E88();
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x44);
        func_00109A80(GS_REG_ALPHA_2, 9);
        func_00109A80(GS_REG_TEST_1, 0x5000D);
        func_00109A80(GS_REG_TEST_2, 0x5000D);
        func_00109AD0();
        func_0010B238((TextureTableHeader *)arm_textures);
        func_00109E88();
        arm_model.unk1A8(&arm_model);
        func_0010B238((TextureTableHeader *)held_item_textures);
        func_00109E88();
        if (D_00413F80.weapon != 0x2E) {
            D_0057B6A0.unk1A8(&D_0057B6A0);
        } else {
            for (i = 0; i < D_0057B6A0.part_count; i++) {
                MODEL_PART_FLAG_BITS(&D_0057B6A0.parts[i])->bit40 = i == 5;
            }
            D_0057B6A0.unk1A8(&D_0057B6A0);
            for (i = 0; i < D_0057B6A0.part_count; i++) {
                MODEL_PART_FLAG_BITS(&D_0057B6A0.parts[i])->bit40 = i != 5;
            }
            func_00109A50();
            func_00109A80(GS_REG_ALPHA_1, 0x48);
            func_00109A80(GS_REG_TEST_1, 0x5000D);
            func_00109AD0();
            D_0057B6A0.unk1A8(&D_0057B6A0);
            for (i = 0; i < D_0057B6A0.part_count; i++) {
                MODEL_PART_FLAG_BITS(&D_0057B6A0.parts[i])->bit40 = FALSE;
            }
        }
    }
    count = WeaponTrail_GetCount();
    if (count >= 4) {
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, D_00414660[trail_kind].blend);
        func_00109A80(GS_REG_TEST_1, 0x30000);
        func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
        func_00109AD0();
        clipped = 6;
        size = count * 2 + 1;
        packet_start = packet = func_00109990(size + 3);
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
            *packet++ = D_00414660[trail_kind].red;
            *packet++ = D_00414660[trail_kind].green;
            *packet++ = D_00414660[trail_kind].blue;
            *packet++ = D_00414660[trail_kind].alpha * (WeaponTrail_GetLife(i) - 1) / (D_00414660[trail_kind].lifetime - 1);
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
        func_00109650(packet_start, packet);
        func_00109A50();
        func_00109A80(GS_REG_ZBUF_1, 0xA000118);
        func_00109AD0();
    }
}

/* picks the weapon trail kind: the strongest of six values, else by unk43 */
void WeaponTrail_PickKind(void) {
    s32 best = 0;

    trail_kind = 0;
    if (D_00413F80.unk122[0] != 0 && D_00413F80.unk18E[0] > best) {
        best = D_00413F80.unk18E[0];
        trail_kind = 1;
    }
    if (D_00413F80.unk122[1] != 0 && D_00413F80.unk18E[1] > best) {
        best = D_00413F80.unk18E[1];
        trail_kind = 2;
    }
    if (D_00413F80.unk122[2] != 0 && D_00413F80.unk18E[2] > best) {
        best = D_00413F80.unk18E[2];
        trail_kind = 3;
    }
    if (D_00413F80.unk122[3] != 0 && D_00413F80.unk18E[3] > best) {
        best = D_00413F80.unk18E[3];
        trail_kind = 4;
    }
    if (D_00413F80.unk122[4] != 0 && D_00413F80.unk18E[4] > best) {
        best = D_00413F80.unk18E[4];
        trail_kind = 5;
    }
    if (D_00413F80.unk122[5] != 0 && D_00413F80.unk18E[5] > best) {
        best = D_00413F80.unk18E[5];
        trail_kind = 6;
    }
    if (trail_kind == 0) {
        if (D_00413F80.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_FIRE)) {
            trail_kind = 1;
        }
        if (D_00413F80.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_EARTH)) {
            trail_kind = 2;
        }
        if (D_00413F80.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_WIND)) {
            trail_kind = 3;
        }
        if (D_00413F80.equipment[EQUIP_RING] == ITEM_WORN_INDEX(ITEM_RING_OF_WATER)) {
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
        trail_lives[trail_write_index] = D_00414660[trail_kind].lifetime;
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
