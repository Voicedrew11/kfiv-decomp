#include "common.h"
#include "kf_collision.h"
#include "kf_entity.h"
#include "kf_game.h"
#include "kf_gs.h"
#include "kf_map.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_texture.h"
#include "kf_message.h"

int func_0011C678(int);
void func_0011A598(void);
static void func_0011A5E0(void);
static int func_0011A600(void);
void Sound_MuteTracked(void);
void Sound_UnmuteTracked(void);
void Sound_UpdateTagged(void);
void Sound_MuteTagged(void);
void Sound_UnmuteTagged(void);
void Sound_UnmutePositional(void);
void Sound_MutePositional(void);
int func_001CEBE0(void);
void *memset(void *, int, u32);
u8 func_00109800(void); // match: u8 here; declared void, the call site is compiled differently
void func_00109878(int);
int sprintf(char *, char *, ...);
int func_00102238(int, int, int);
int func_001024B0(int);
int func_001025C0(int, void *, int);
void func_00109A38(int *, int *);
void func_0010B510(char *, int, int);
void func_0010B5D0(ModelPart *);
void func_0010D018(ModelInstance *, char **, char **);
void func_0010E6A8(ModelInstance *, s64, s64);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void Vector_Copy(void *, void *);
void Display_SwapBuffers(int);
void func_001096E8(void);
void func_001095E0(void);
void func_0010B000(void);
void func_001095D0(void);
void func_001095D8(void);
void func_001B5460(void);
float cosf(float);
float sinf(float);
void func_00107CD8(int);
void func_00109650(u32 *, u32 *);
u32 *func_00109990(int);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E18(void);
void func_00109E88(void);
void func_0010A100(void *);
void func_0010B410(TextureTableHeader *, s32);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0MulMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
void Vector_Subtract(void *, void *, void *);
void func_0010A7E0(void *);
static void func_0011C160(void);
void func_0011AAE8(void);
float Angle_Wrap(float);
void Vector_Add(void *, void *, void *);
void Vector_Normalize(void *, void *);
void func_00106C58(void);
void func_00107E70(int, int, int);
void func_0010D308(ModelInstance *);

extern int D_0038CC50[];
extern MapSlot D_0038CD20[];
extern int D_00446770;
extern u8 D_00349680[0x10];
extern u16 D_00413DA8[];
extern PlayerState D_00413F80;
extern ViewerEntry D_0038D6B0[];
extern ScreenQuad D_0038D770;
extern ScreenQuad D_0038D7B0;
extern ScreenQuad D_0038D7F0;
extern ScreenQuad D_0038D830;
extern ScreenQuad D_0038D870;
extern ViewerCornerTable D_0043E760;
extern ViewerCornerTable D_0043E770;
extern char D_0043E730[];       /* "map_view/%3.3d.tx2" */
extern char D_0043E748[];       /* "map_view/pointer.omd" */
extern f32 D_00446798[];        /* the largest float */
extern f32 D_0044679C[];        /* the lowest float */
extern f32 D_00431AF0[16];      /* the unit matrix */
extern MapDrawState D_0038A1B0;
extern MapSlotUnk10Words D_0038D930;
extern char D_0038D8B0[0x40];
extern char D_0038D8F0[0x40];
extern s64 D_00432E20[];
extern s64 D_00433050[];

/* the map viewer: the picture data read into the spare area, the viewer picked, and its animation */
static char *viewer_data;                   /* 0x44705C the spare area the viewer's textures are read into */
static int viewer_data_size;                /* 0x447060 */
static int picked_viewer;                   /* 0x447064 index into D_0038D6B0 */
static int viewer_picture;                  /* 0x447068 which of the viewer's pictures is shown */
static int pulse_frame;                     /* 0x44706C frame of the 60-frame pulse of the player marker */
static int square_pulse_frame;              /* 0x447070 frame of the pulse of the fixed square */
static Vector4 bounds_min;                  /* 0x472060 the lowest corner of every map record */
static Vector4 bounds_max;                  /* 0x472070 the highest corner */
static ModelInstance pointer_model;         /* 0x472080 the pointer model, viewer 4 only */

void func_0011A438(int viewer_number) {
    u32 viewer_index;
    u8 *row;

    if (!(((u32)D_0038CC50[9] >> 8) & 1) && ((viewer_index = viewer_number - 1) < 5U)) {
        if (viewer_number == 5) {
            row = (u8 *)D_0038CD20 + D_00446770 * 0x60;
            if (func_0011C678(row[1]) == 0) {
                Message_ShowNotice(0, 0, 0, 1, 0x2D, MSG_UNABLE_TO_USE_HERE);
                return;
            }
        }
        picked_viewer = viewer_index;
        pulse_frame = 0;
        square_pulse_frame = 0;
        D_0038CC50[9] |= 0x100;
    }
}

/* Loads the picked viewer, runs it until it is left and frees what it loaded. */
void func_0011A500(void) {
    if (func_0011A600() != 0) {
        Sound_MuteTagged();
        Sound_MutePositional();
        Sound_UpdateTagged();
        Sound_MuteTracked();
        func_001CEBE0();
        D_0038D6B0[picked_viewer].run();
        Sound_UnmuteTagged();
        Sound_UnmutePositional();
        Sound_UnmuteTracked();
        func_0011A5E0();
        func_0011A598();
    }
}

void func_0011A598(void) {
    int *game_state = (int *)D_0038CC50;

    picked_viewer = 0;
    viewer_picture = 0;
    game_state[9] = game_state[9] & -0x101;
    viewer_data = 0;
    viewer_data_size = 0;
    pulse_frame = 0;
    square_pulse_frame = 0;
    memset(&pointer_model, 0, sizeof(pointer_model));
}

static void func_0011A5E0(void) {
    func_00109800();
    func_00109878(0);
}

static inline char *align16(char *address) {
    if ((int)address & 0xF) {
        address -= ((int)address & 0xF) - 0x10;
    }
    return address;
}

/* Loads the picked viewer's textures; for viewer 4 also the pointer model, put where the player is, and
   the box that holds every ordinary map record. FALSE when a file is missing. */
static int func_0011A600(void) {
    char name[0x100];
    char *model_data;
    char *buffer;
    int file;
    int size;
    int i;
    int j;
    MapRecordBounds *record;
    int count;

    sprintf(name, D_0043E730, picked_viewer);
    file = func_00102238((int)name, 0, 1);
    if (file < 0) {
        return FALSE;
    }
    func_00109800();
    func_00109878(2);
    viewer_data = NULL;
    viewer_data_size = 0;
    func_00109A38((int *)&viewer_data, &viewer_data_size);
    memset(viewer_data, 0, viewer_data_size);
    size = func_001025C0(file, viewer_data, -1);
    func_001024B0(file);
    func_0010B510(viewer_data, 0x2BC0, 0x3FE0);
    if (picked_viewer != 4) {
        return TRUE;
    }
    model_data = viewer_data + size;
    model_data = align16(model_data);
    file = func_00102238((int)D_0043E748, 0, 1);
    if (file < 0) {
        return FALSE;
    }
    size = func_001025C0(file, model_data, -1);
    func_001024B0(file);
    buffer = model_data + size;
    buffer = align16(buffer);
    func_0010D018(&pointer_model, &model_data, &buffer);
    func_0010E6A8(&pointer_model, 0x260, 0x240);
    Vector_Copy(&pointer_model.position, &D_00413F80);
    pointer_model.position.y -= 100.0f;
    pointer_model.rotation.x = 0.0f;
    pointer_model.rotation.y = D_00413F80.rotation.y;
    pointer_model.rotation.z = 0.0f;
    pointer_model.unkE0 = pointer_model.unkE4 = pointer_model.unkE8 = pointer_model.rotation.w = 4.0f;
    pointer_model.unkEC = 1.0f;
    // match: it returns a value
    ((int (*)(ModelInstance *, Vector4 *, Vector4 *, f32 *))ModelMatrix_Build)(&pointer_model, &pointer_model.position, &pointer_model.rotation, &pointer_model.unkE0);
    for (i = 0; i < pointer_model.part_count; i++) {
        // match: the callee returns a value here, which keeps $2 busy
        ((int (*)(ModelInstance *, ModelPart *, ModelPart *))ModelMatrix_BuildPartLocal)(&pointer_model, &pointer_model.parts[i], &pointer_model.parts[i]);
    }
    for (i = 0; i < pointer_model.part_count; i++) {
        func_0010B5D0(&pointer_model.parts[i]);
    }
    bounds_min.z = bounds_min.y = bounds_min.x = D_00446798[0];
    bounds_max.z = bounds_max.y = bounds_max.x = D_0044679C[0];
    for (i = 0; i < 2; i++) {
        if (D_0038CD20[i].unk00 != 0 && func_0011C678(D_0038CD20[i].tag) != 0) {
            count = ((MapSlotUnk08 *)D_0038CD20[i].header)->record_count;
            record = (MapRecordBounds *)D_0038CD20[i].records;
            for (j = 0; j < count; j++, record++) {
                if (bounds_min.x > record->unk40[0]) {
                    bounds_min.x = record->unk40[0];
                }
                if (bounds_min.y > record->unk40[1]) {
                    bounds_min.y = record->unk40[1];
                }
                if (bounds_min.z > record->unk40[2]) {
                    bounds_min.z = record->unk40[2];
                }
                if (bounds_max.x < record->unk50[0]) {
                    bounds_max.x = record->unk50[0];
                }
                if (bounds_max.y < record->unk50[1]) {
                    bounds_max.y = record->unk50[1];
                }
                if (bounds_max.z < record->unk50[2]) {
                    bounds_max.z = record->unk50[2];
                }
            }
        }
    }
    return TRUE;
}

void func_0011AA08(void) {
    int next_picture;
    ViewerEntry *viewer;

    viewer = &D_0038D6B0[picked_viewer];
    do {
        Display_SwapBuffers(0);
        func_001096E8();
        func_001095E0();
        func_0010B000();
        func_001095D0();
        func_0011AAE8();
        func_001095D8();
        func_001B5460();
        if ((D_00413DA8[1] & 0x10) && (viewer_picture > 0)) {
            viewer_picture -= 1;
        } else if (D_00413DA8[1] & 0x40) {
            next_picture = viewer_picture + 1;
            if (next_picture < viewer->count) {
                viewer_picture = next_picture;
            }
        }
        func_00109800();
    } while ((D_00413DA8[1] & 0x2000) == 0);
}

/* Draws the picked viewer's picture and the player's marker on it: a turned, pulsing arrow where the
   position is known, a pulsing square at a fixed spot otherwise; then the picture arrows when the viewer
   has more than one picture. */
void func_0011AAE8(void) {
    ViewerCornerTable corner_us;
    ViewerCornerTable corner_vs;
    int corner_xs[4];
    int corner_ys[4];
    int x;
    int y;
    ViewerEntry *entry = &D_0038D6B0[picked_viewer];
    int found;
    int alpha;
    int i;
    f32 cosine;
    f32 sine;
    u32 *packet_start;
    u32 *packet;

    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109AD0();
    func_0010B410((TextureTableHeader *)viewer_data, viewer_picture);
    func_00109E88();
    func_00109E18();
    func_0010A100(&D_0038D7B0);
    found = ((int (*)(int *, int *))entry->unk04)(&x, &y);
    if (found == 1) {
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x44);
        func_00109AD0();
        func_0010B410((TextureTableHeader *)viewer_data, entry->count);
        func_00109E88();
        func_00109E18();
        corner_us = D_0043E760;
        corner_vs = D_0043E770;
        if (pulse_frame < 45) {
            alpha = sinf(pulse_frame * 3.1415927f / 45.0f) * 128.0f;
        } else {
            alpha = 0;
        }
        pulse_frame = (pulse_frame + 1) % 60;
        cosine = cosf(D_00413F80.rotation.y);
        sine = sinf(D_00413F80.rotation.y);
        corner_xs[0] = x + (int)((cosine * -32.0f - sine * -44.0f) * 0.5f);
        corner_ys[0] = y + (int)((sine * -32.0f + cosine * -44.0f) * 0.5f);
        corner_xs[1] = x + (int)((cosine * 32.0f - sine * -44.0f) * 0.5f);
        corner_ys[1] = y + (int)((sine * 32.0f + cosine * -44.0f) * 0.5f);
        corner_xs[2] = x + (int)((cosine * -32.0f - sine * 20.0f) * 0.5f);
        corner_ys[2] = y + (int)((sine * -32.0f + cosine * 20.0f) * 0.5f);
        corner_xs[3] = x + (int)((cosine * 32.0f - sine * 20.0f) * 0.5f);
        corner_ys[3] = y + (int)((sine * 32.0f + cosine * 20.0f) * 0.5f);
        packet_start = packet = func_00109990(0x13);
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
        func_00109650(packet_start, packet);
    }
    if (found == -1) {
        if (square_pulse_frame < 45) {
            D_0038D770.alpha = sinf(square_pulse_frame * 3.1415927f / 45.0f) * 128.0f;
        } else {
            D_0038D770.alpha = 0;
        }
        square_pulse_frame = (square_pulse_frame + 1) % 60;
        D_0038D770.first_x = (x + 0x690) << 4;
        D_0038D770.first_y = (y + 0x6F0) << 4;
        D_0038D770.second_x = (x + 0x6F0) << 4;
        D_0038D770.second_y = (y + 0x750) << 4;
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x48);
        func_00109AD0();
        func_0010A100(&D_0038D770);
    }
    if (entry->count >= 2) {
        func_0010B410((TextureTableHeader *)viewer_data, entry->count * 2 | 1);
        func_00109E88();
        func_00109E18();
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x44);
        func_00109AD0();
        func_0010A100(&D_0038D7F0);
        func_0010B410((TextureTableHeader *)viewer_data, entry->count * 2 + 2);
        func_00109E88();
        func_00109E18();
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x48);
        func_00109AD0();
        func_0010A100(&D_0038D830);
        func_0010B410((TextureTableHeader *)viewer_data, entry->count + 1 + viewer_picture);
        func_00109E88();
        func_00109E18();
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x48);
        func_00109AD0();
        func_0010A100(&D_0038D870);
    }
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109AD0();
    func_00107CD8(D_00349680[10]);
}

/* Where the player's marker goes on the map viewer's picture viewer_picture: -1 for a fixed spot on the
   first picture, TRUE for a spot worked out from the player's position, FALSE for none. */
int func_0011B1C0(int *screen_x, int *screen_y) {
    switch (viewer_picture) {
    case 0:
        switch (D_0038CD20[D_00446770].tag) {
        case 0:
            if (D_00413F80.map_index != 0xFFFF
                && ((MapRecord *)D_0038CD20[D_00446770].records)[D_00413F80.map_index].unk4E == 0xC8) {
                *screen_x = 0x96;
                *screen_y = 0xDB;
            } else {
                *screen_x = 0xB6;
                *screen_y = 0x69;
            }
            return -1;
        case 2:
            if (D_00413F80.map_index != 0xFFFF
                && ((MapRecord *)D_0038CD20[D_00446770].records)[D_00413F80.map_index].unk4E == 0xC8) {
                *screen_x = 0x96;
                *screen_y = 0xDB;
            } else {
                *screen_x = 0x64;
                *screen_y = 0x151;
            }
            return -1;
        case 4:
            *screen_x = 0x214;
            *screen_y = 0x12E;
            return -1;
        case 8:
        case 9:
            *screen_x = 0x214;
            *screen_y = 0x94;
            return -1;
        case 0x62:
            *screen_x = 0x169;
            *screen_y = 0x5C;
            return -1;
        case 0x63:
            *screen_x = 0x14A;
            *screen_y = 0xFF;
            return -1;
        default:
            return FALSE;
        }
    case 1:
        if (D_00413F80.position.y > -700.0f) {
            switch (D_0038CD20[D_00446770].tag) {
            case 0x5F:
            case 0x60:
            case 0x61:
                *screen_x = (D_00413F80.position.x - 0.0f) * 24.615385f / 1000.0f + 148.0f;
                *screen_y = 273.0f - (D_00413F80.position.z - 0.0f) * 24.88889f / 1000.0f;
                return TRUE;
            }
        } else {
            switch (D_0038CD20[D_00446770].tag) {
            case 0x5F:
                if (D_00413F80.position.x > 4800.0f && D_00413F80.position.z > 8000.0f) {
                    return FALSE;
                }
            case 0x60:
                *screen_x = (D_00413F80.position.x - 3600.0f) * 24.615385f / 1000.0f + 490.0f;
                *screen_y = 143.0f - (D_00413F80.position.z - 5200.0f) * 24.88889f / 1000.0f;
                return TRUE;
            case 0x61: // match: named although it does what the default does; it shapes the compare tree
            default:
                return FALSE;
            }
        }
        break;
    }
    return FALSE;
}

/* Where the player's marker goes on the map viewer's picture viewer_picture (0 to 2): TRUE with a spot worked
   out from the player's position when the current map is on that picture, FALSE when it is not. */
int func_0011B4F0(int *screen_x, int *screen_y) {
    switch (viewer_picture) {
    case 0:
        switch (D_0038CD20[D_00446770].tag) {
        case 0x70:
        case 0x71:
            *screen_x = (D_00413F80.position.x - 0.0f) * 18.626263f / 1000.0f + 321.0f;
            *screen_y = 280.0f - (D_00413F80.position.z - 0.0f) * 18.666666f / 1000.0f;
            return TRUE;
        case 0x73:
            *screen_x = (D_00413F80.position.x - -10500.0f) * 18.626263f / 1000.0f + 321.0f;
            *screen_y = 280.0f - (D_00413F80.position.z - 500.0f) * 18.666666f / 1000.0f;
            return TRUE;
        case 0x74:
            *screen_x = (D_00413F80.position.x - -9700.0f) * 18.626263f / 1000.0f + 321.0f;
            *screen_y = 280.0f - (D_00413F80.position.z - -8900.0f) * 18.666666f / 1000.0f;
            return TRUE;
        }
        break;
    case 1:
        switch (D_0038CD20[D_00446770].tag) {
        case 0x1F:
            *screen_x = (D_00413F80.position.x - 0.0f) * 11.243243f / 1000.0f + 348.0f;
            *screen_y = 263.0f - (D_00413F80.position.z - -12100.0f) * 11.48718f / 1000.0f;
            return TRUE;
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x6E:
        case 0x6F:
            *screen_x = (D_00413F80.position.x - 0.0f) * 11.243243f / 1000.0f + 348.0f;
            *screen_y = 263.0f - (D_00413F80.position.z - 0.0f) * 11.48718f / 1000.0f;
            return TRUE;
        case 0x73:
            *screen_x = (D_00413F80.position.x - -10500.0f) * 11.243243f / 1000.0f + 348.0f;
            *screen_y = 263.0f - (D_00413F80.position.z - 500.0f) * 11.48718f / 1000.0f;
            return TRUE;
        }
        break;
    case 2:
        switch (D_0038CD20[D_00446770].tag) {
        case 0xF:
            *screen_x = (D_00413F80.position.x - 9300.0f) * 11.186992f / 1000.0f + 194.0f;
            *screen_y = 167.0f - (D_00413F80.position.z - -4400.0f) * 11.2f / 1000.0f;
            return TRUE;
        case 0x10:
            *screen_x = (D_00413F80.position.x - 700.0f) * 11.186992f / 1000.0f + 335.0f;
            *screen_y = 328.0f - (D_00413F80.position.z - 1650.0f) * 11.2f / 1000.0f;
            return TRUE;
        case 0x64:
        case 0x69:
        case 0x6A:
            *screen_x = (D_00413F80.position.x - 0.0f) * 11.186992f / 1000.0f + 326.0f;
            *screen_y = 168.0f - (D_00413F80.position.z - 0.0f) * 11.2f / 1000.0f;
            return TRUE;
        case 0x65:
            *screen_x = (D_00413F80.position.x - 3800.0f) * 11.186992f / 1000.0f + 326.0f;
            *screen_y = 168.0f - (D_00413F80.position.z - -7900.0f) * 11.2f / 1000.0f;
            return TRUE;
        case 0x66:
        case 0x67:
        case 0x68:
            *screen_x = (D_00413F80.position.x - -7050.0f) * 11.186992f / 1000.0f + 326.0f;
            *screen_y = 168.0f - (D_00413F80.position.z - 0.0f) * 11.2f / 1000.0f;
            return TRUE;
        }
        break;
    }
    return FALSE;
}

/* Map the player position to map-screen coordinates; FALSE if the area has no such map. */
int func_0011BAB8(int *map_x, int *map_y) {
    int area;
    char *entry;

    entry = (char *)D_0038CD20 + D_00446770 * 0x60;
    area = ((u8 *)entry)[1];
    if (area >= 0x11) {
        if (area < 0x14) {
            *map_x = (D_00413F80.position.x - 0.0f) * 22.857143f / 1000.0f + 360.0f;
            *map_y = 206.0f - (D_00413F80.position.z - 0.0f) * 22.4f / 1000.0f;
            return TRUE;
        }
        if (area < 0x1A) {
            if (area >= 0x18) {
                *map_x = (D_00413F80.position.x - 0.0f) * 22.857143f / 1000.0f + 205.0f;
                *map_y = 368.0f - (D_00413F80.position.z - 0.0f) * 22.4f / 1000.0f;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* Turns the player's position into a point on the map picture of maps 0x1A and 0x1B; FALSE on other maps. */
int func_0011BBC0(int *screen_x, int *screen_y) {
    switch (D_0038CD20[D_00446770].tag) {
    case 0x1A:
        *screen_x = (D_00413F80.position.x - -3200.0f) * 21.333334f / 1000.0f + 438.0f;
        *screen_y = 214.0f - (D_00413F80.position.z - -5600.0f) * 20.363636f / 1000.0f;
        break;
    case 0x1B:
        *screen_x = (D_00413F80.position.x - -8800.0f) * 21.333334f / 1000.0f + 320.0f;
        *screen_y = 82.0f - (D_00413F80.position.z - 900.0f) * 20.363636f / 1000.0f;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

/* The free-camera viewer of func_0011C160's picture: runs its own frame loop, in which the pad turns the
   camera about the player (with 0x4000 held) or moves it, and both 0x400 and 0x800 put it back. */
void func_0011BCD0(void) {
    VuVector4 position;
    VuVector4 rotation;
    int reset_held;

    reset_held = FALSE;
    memset(&position, 0, sizeof(position));
    position.z = 10000.0f;
    rotation = (VuVector4){ -1.5707964f, D_00413F80.rotation.y, 0.0f, 0.0f };
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
            func_001096E8();
            func_001095E0();
            func_0010B000();
            func_001B5460();
            held = D_00413DA8[0];
            pressed = D_00413DA8[1];
            move.x = move.y = move.z = move.w = 0.0f;
            if (held & 0x4000) {
                if (held & 0x40) {
                    rotation.x = Angle_Wrap(rotation.x + 0.08726646f);
                }
                if (held & 0x10) {
                    rotation.x = Angle_Wrap(rotation.x - 0.08726646f);
                }
                if (held & 0x80) {
                    rotation.y = Angle_Wrap(rotation.y + 0.08726646f);
                }
                if (held & 0x20) {
                    rotation.y = Angle_Wrap(rotation.y - 0.08726646f);
                }
            } else {
                if (held & 0x80) {
                    move.x = -1.0f;
                }
                if (held & 0x20) {
                    move.x += 1.0f;
                }
                if (held & 0x10) {
                    move.y -= 1.0f;
                }
                if (held & 0x40) {
                    move.y += 1.0f;
                }
            }
            if (!reset_held) {
                if ((held & 0xC00) == 0xC00) {
                    reset_held = TRUE;
                    position.z = 10000.0f;
                    rotation.x = -1.5707964f;
                    rotation.y = D_00413F80.rotation.y;
                    position.x = 0.0f;
                    position.y = 0.0f;
                    position.w = 0.0f;
                    rotation.z = 0.0f;
                    rotation.w = 0.0f;
                } else if (held & 0x400) {
                    move.z += 1.0f;
                } else if (held & 0x800) {
                    move.z -= 1.0f;
                }
            } else if (!(held & 0xC00)) {
                reset_held = FALSE;
            }
            if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f) {
                Vector_Normalize(&move, &move);
                Vector_Scale(&move.x, &move.x, 100.0f);
                Vector_Add(&position, &position, &move);
                if (position.x < D_00413F80.position.x + -48000.0f) {
                    position.x = D_00413F80.position.x + -48000.0f;
                }
                if (position.x > D_00413F80.position.x + 48000.0f) {
                    position.x = D_00413F80.position.x + 48000.0f;
                }
                if (position.y < D_00413F80.position.y + -48000.0f) {
                    position.y = D_00413F80.position.y + -48000.0f;
                }
                if (position.y > D_00413F80.position.y + 48000.0f) {
                    position.y = D_00413F80.position.y + 48000.0f;
                }
                if (position.z < 2500.0f) {
                    position.z = 2500.0f;
                }
                if (position.z > 50000.0f) {
                    position.z = 50000.0f;
                }
            }
            Vector_Scale(&move.x, &D_00413F80.position.x, -1.0f);
            sceVu0TransMatrix(world_view, D_00431AF0, &move.x);
            sceVu0RotMatrixY(world_view, world_view, -rotation.y);
            sceVu0RotMatrixX(world_view, world_view, -rotation.x);
            sceVu0TransMatrix(world_view, world_view, &D_00413F80.position.x);
            Vector_Subtract(&move, &D_00413F80, &position);
            Vector_Scale(&move.x, &move.x, -1.0f);
            sceVu0TransMatrix(world_view, world_view, &move.x);
            sceVu0MulMatrix(world_screen, view_screen, world_view);
            func_0010A7E0(world_screen);
            func_001095D0();
            func_0011C160();
            func_001095D8();
            // match: it returns a value here, which keeps $2 busy
            ((int (*)(void))func_00109800)();
        } while (!(pressed & 0x2000));
    }
}

/* Draws the map viewer: the pointer model, pulsing, then every record of the
   ordinary maps in one flat colour, then the viewer's four pictures. */
static void func_0011C160(void) {
    int alpha = 0x10;
    int period;
    int map;
    int i;
    MapSlot *slot;
    MapDrawRecord *record;
    int flags;

    D_00432E20[0] = 0;
    D_00432E20[30] = 0;
    D_00433050[0] = 0;
    D_00433050[30] = 0;
    if (pulse_frame < 45) {
        alpha = (int)(sinf(pulse_frame * 3.1415927f / 45.0f) * 108.0f) + 0x10;
    }
    period = 60; // match: the divisor is a variable
    pulse_frame = (pulse_frame + 1) % period;
    func_00109A50();
    func_00109A80(GS_REG_TEST_2, 0x51001);
    func_00109A80(GS_REG_ALPHA_2, ((s64)alpha << 32) | 0x68);
    func_00109AD0();
    pointer_model.unk148 = (s32)&D_0038D930;
    pointer_model.hit_count = 0;
    pointer_model.unk14C = &D_0038A1B0.unkA0;
    func_0010D308(&pointer_model);
    func_00109A50();
    func_00109A80(0x1A, 0);
    func_00109A80(0x1B, 0x248);
    func_00109A80(GS_REG_ALPHA_2, 0x4000000068L);
    func_00109A80(GS_REG_TEST_2, 0x3080B);
    func_00109A80(GS_REG_ZBUF_2, 0x10A000118L);
    func_00109AD0();
    func_00109E18();
    for (map = 0; map < 2; map++) {
        slot = &D_0038CD20[map];
        record = (MapDrawRecord *)slot->records;
        if (slot->unk00 != 0) {
            switch (slot->tag) {
                case 0:
                case 2:
                case 4:
                case 8:
                case 9:
                case 0x62:
                case 0x63:
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
                            record->model->unk148 = (s32)&D_0038A1B0.unkC0;
                            record->model->unk14C = &D_0038A1B0.unkA0;
                        } else {
                            record->model->unk148 = (s32)&D_0038A1B0.unkA90[map][record->unk35];
                            record->model->unk14C = &D_0038A1B0.unk290[map][record->unk34];
                        }
                        record->model->hit_count = 0;
                        record->model->unk148 = (s32)&D_0038D930;
                        record->model->unk14C = &D_0038A1B0.unkA0;
                        func_0010D308(record->model);
                    }
                    break;
            }
        }
    }
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_2, 0xA000118);
    func_00109A80(0x1A, 1);
    func_00109AD0();
    func_00109E18();
    func_00106C58();
    func_00109E18();
    func_00107E70(0xEA, 0xB2, 0x46);
    func_0010B410((TextureTableHeader *)viewer_data, 0);
    func_00109E88();
    func_00109E18();
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x31001);
    func_00109A80(GS_REG_ALPHA_1, 0x48);
    func_00109A80(8, 5);
    func_00109AD0();
    func_00109E18();
    func_0010A100(D_0038D8B0);
    func_0010B410((TextureTableHeader *)viewer_data, 1);
    func_00109E88();
    func_00109E18();
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x31001);
    func_00109A80(GS_REG_ALPHA_1, 0x54);
    func_00109A80(8, 5);
    func_00109AD0();
    func_00109E18();
    func_0010A100(D_0038D8B0);
    func_0010B410((TextureTableHeader *)viewer_data, 2);
    func_00109E88();
    func_00109E18();
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x31001);
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(8, 0);
    func_00109AD0();
    func_00109E18();
    func_0010A100(D_0038D8F0);
    func_0010B410((TextureTableHeader *)viewer_data, 3);
    func_00109E88();
    func_00109E18();
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x31001);
    func_00109A80(GS_REG_ALPHA_1, 0x48);
    func_00109AD0();
    func_00109E18();
    func_0010A100(D_0038D8F0);
    func_00107CD8(D_00349680[10]);
}
