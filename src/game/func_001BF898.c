#include "kf_game.h"
#include "kf_map.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern GameDefinition D_00416D78[70];
extern PlayerItemDef D_0041B9B8[];
extern f32 D_00414740[4];
extern MapSlotUnk10Half D_004145E0[2];
extern MapSlotUnk0C D_0038A250;
extern char D_00249680[0x80000];
extern int D_00446970;
extern int D_00446974;
extern u32 D_00447168;
extern u32 D_0044716C;
extern u32 D_00447170;
extern u32 D_00447174;
extern ModelInstance D_0057B6A0;
extern ModelInstance D_0057B860;
extern char D_0057BA20[];
extern char D_0057BAB0[];
extern char D_0057CAB0[];
extern char D_0057CB40[];
extern char D_0043F5A0[];
extern char D_0043F5B8[];
extern char D_0043F5D0[];

void *memset(void *, int, u32);
void *memcpy(void *, const void *, u32);
int sprintf(char *, char *, ...);
int func_00102238(char *, int, int);
int func_001025C0(int, void *, int);
int func_001024B0(int);
void func_0010B758(ModelInstance *, int *, int *);
void func_00108940(int, int *, ModelInstance *, int);
void func_001A35D0(void);
void func_001A1160(int);
void func_001C0840(void);
void func_001C0890(s32);

/* Loads the model of the item the player holds (PlayerState.unk3C) and of the arm that holds it
   (unk41) when either changed: reads the item_arm file, copies it to its own buffer and sets up
   the model, its motions and its textures from the parts the file has. */
void func_001BF898(void) {
    char path[0x100];
    int data;
    int buffer;
    GameDefinition *item = &D_00416D78[D_00413F80.unk3C];
    GameDefinition *arm = &D_00416D78[D_00413F80.equipment[3] + 70];
    PlayerArmFile *file;
    s32 size;
    s32 handle;

    if (D_00446974 != D_00413F80.unk3C) {
        D_00446974 = D_00413F80.unk3C;
        if (D_00413F80.unk258 != 0) {
            D_00413F80.unk258 = 0;
            func_001A35D0();
        }
        memset((void *)D_00447168, 0, 0x30000);
        memset(&D_0057B6A0, 0, 0x1C0);
        memset(D_0057BAB0, 0, 0x1000);
        memset(D_0057BA20, 0, 0x90);
        D_0044716C = 0;
        sprintf(path, D_0043F5A0, item->unk30);
        handle = func_00102238((int)path, 0, 1);
        if (handle < 0) {
            handle = func_00102238((int)D_0043F5B8, 0, 1);
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
        memcpy((void *)D_00447168, D_00249680, size);
        file = (PlayerArmFile *)D_00447168;
        if (file->model_size != 0) {
            data = (int)file + file->model_offset;
            buffer = (int)D_0057BAB0;
            func_0010B758(&D_0057B6A0, &data, &buffer);
        }
        if (file->motion_size != 0) {
            data = D_00447168 + file->motion_offset;
            buffer = (int)D_0057BA20;
            func_00108940(data, &buffer, &D_0057B6A0, 1);
        }
        if (file->texture_size != 0) {
            D_0044716C = D_00447168 + file->texture_offset;
        }
        D_0057B6A0.unk148 = (s32)D_004145E0;
        D_0057B6A0.unk14C = &D_0038A250;
        MODEL_CHANNEL(&D_0057B6A0)->unk8F = 0x80;
        func_001A1160(D_00413F80.unk3C);
        func_001C0840();
        func_001C0890(3);
        D_00414740[0] = D_0041B9B8[D_00413F80.unk3C].unk87;
        D_00414740[1] = D_0041B9B8[D_00413F80.unk3C].unk88;
        D_00414740[2] = D_0041B9B8[D_00413F80.unk3C].unk89;
    }
    if (D_00446970 != D_00413F80.equipment[3]) {
        D_00446970 = D_00413F80.equipment[3];
        memset((void *)D_00447170, 0, 0x50000);
        memset(&D_0057B860, 0, 0x1C0);
        memset(D_0057CB40, 0, 0x1000);
        memset(D_0057CAB0, 0, 0x90);
        D_00447174 = 0;
        sprintf(path, D_0043F5A0, arm->unk30);
        handle = func_00102238((int)path, 0, 1);
        if (handle < 0) {
            handle = func_00102238((int)D_0043F5D0, 0, 1);
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
        memcpy((void *)D_00447170, D_00249680, size);
        file = (PlayerArmFile *)D_00447170;
        if (file->model_size != 0) {
            data = (int)file + file->model_offset;
            buffer = (int)D_0057CB40;
            func_0010B758(&D_0057B860, &data, &buffer);
        }
        if (file->texture_size != 0) {
            D_00447174 = D_00447170 + file->texture_offset;
        }
        D_0057B860.unk148 = (s32)D_004145E0;
        D_0057B860.unk14C = &D_0038A250;
    }
}
