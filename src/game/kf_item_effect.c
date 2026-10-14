#include "kf_item_effect.h"
#include "kf_effect.h"
#include "kf_game.h"
#include "kf_gs.h"
#include "kf_model.h"
#include "kf_player.h"
#include "kf_splash.h"
#include "kf_message.h"

extern char D_0038A440[];
extern LevelRecord D_003679A0[];
extern GameStatus D_0038CC50;
extern PlayerState D_00413F80;
extern EventRunner D_00420D00;
extern ItemUseEffectDef D_0042BBE0[];
extern char D_0042C110[];
extern void (*D_0042C190[4])(void);
extern char D_004410C8[];
extern char D_004410E0[];
extern char D_004410F8[];
extern char D_00441110[];
extern char D_00441128[];
extern u8 D_004141D8[];
extern int D_00446770;
extern int D_00446F28;
extern char *D_00446F70;
extern PlayerItemDef weapon_defs[];

void *memset(void *, int, u32);
int sprintf(char *, char *, ...);
int func_00102238(char *, int, int);
int func_001024B0(int);
int func_001025C0(int, void *, int);
MotionData *func_00108940(MotionFile *, ModelChannel **, ModelInstance *, int);
void func_00108A08(ModelInstance *, int);
void func_00108AC8(void *);
void func_0010B018(ModelInstance *, s32, s32);
void func_0010B510(char *, int, int);
void func_0010B5D0(ModelPart *);
void func_0010D018(ModelInstance *, char **, char **);
void func_0010DB40(ModelInstance *);
void func_0010E6A8(ModelInstance *, s64, s64);
void func_0010E770(ModelInstance *, u64, s64);
void func_0010E820(ModelInstance *, s64);
void func_0010E8E0(void *, int, int, int, int);
int func_001BF6E8(void);
void func_0010AEB8(void *);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
static void ItemEffect_DrawModel(ModelInstance *model);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
void Vector_Scale(float *, float *, float);

static int item_effect_active;              /* 0x447388 nonzero while an effect runs */
static int item_effect_kind;                /* 0x44738C the item kind (0x46..0x52) */
static int item_effect_step;                /* 0x447390 index into D_0042C190 */
static ModelInstance *item_effect_model_a;  /* 0x447394 the small-item model */
static ModelInstance *item_effect_model_b;  /* 0x447398 the large-item model */
static ModelInstance *item_effect_model;    /* 0x44739C the model shown this time */
static int item_effect_load_pad;            /* 0x4473A0 cleared in ItemEffect_Load */
static ItemUseEffectDef *item_effect_def;   /* 0x4473A4 D_0042BBE0[kind] */
static int item_effect_brightness;          /* 0x4473A8 model colour alpha while fading */
static int item_effect_pending_effects;     /* 0x4473AC set until motion reaches def->unk08 */
static int item_effect_marked;              /* 0x4473B0 spell slot marked by func_001BC990 */

static char *item_effect_textures[7];       /* 0x5F9B40 loaded i0040..i0046.tx2 */

static inline char *align16(char *address) {
    if ((int)address & 0xF) {
        address -= ((int)address & 0xF) - 0x10;
    }
    return address;
}

/* Loads the two item-effect models, their motions and seven textures into D_00446F70. */
void ItemEffect_Load(void) {
    char name[0x100];
    char *model_data;
    char *buffer;
    int file;
    int size;
    int i;

    memset(D_00446F70, 0, D_00446F28);
    buffer = D_00446F70;
    item_effect_model_a = NULL;
    item_effect_model_b = NULL;
    memset(item_effect_textures, 0, sizeof(item_effect_textures));
    item_effect_load_pad = 0;
    item_effect_model = NULL;

    buffer = align16(buffer);
    item_effect_model_a = (ModelInstance *)buffer;
    buffer += sizeof(ModelInstance);
    buffer = align16(buffer);
    file = func_00102238(D_004410C8, 0, 1);
    size = func_001025C0(file, buffer, -1);
    func_001024B0(file);
    model_data = buffer;
    buffer += size;
    buffer = align16(buffer);
    func_0010D018(item_effect_model_a, &model_data, &buffer);
    func_0010E8E0(item_effect_model_a, 0x80, 0x80, 0x80, 0x80);
    func_0010E820(item_effect_model_a, 0x44);
    func_0010E6A8(item_effect_model_a, 0x60, 0x40);
    func_0010E770(item_effect_model_a, 0x4007D805E1313B40, 0x60);
    item_effect_model_a->unk1A8 = func_0010DB40;
    buffer = align16(buffer);
    file = func_00102238(D_004410E0, 0, 1);
    size = func_001025C0(file, buffer, -1);
    func_001024B0(file);
    model_data = buffer;
    buffer += size;
    buffer = align16(buffer);
    func_00108940((MotionFile *)model_data, (ModelChannel **)&buffer, item_effect_model_a, 1);

    buffer = align16(buffer);
    item_effect_model_b = (ModelInstance *)buffer;
    buffer += sizeof(ModelInstance);
    buffer = align16(buffer);
    file = func_00102238(D_004410F8, 0, 1);
    size = func_001025C0(file, buffer, -1);
    func_001024B0(file);
    model_data = buffer;
    buffer += size;
    buffer = align16(buffer);
    func_0010D018(item_effect_model_b, &model_data, &buffer);
    func_0010E8E0(item_effect_model_b, 0x80, 0x80, 0x80, 0x80);
    func_0010E820(item_effect_model_b, 0x44);
    func_0010E6A8(item_effect_model_b, 0x60, 0x40);
    func_0010E770(item_effect_model_b, 0x4007D805E1313B40, 0x60);
    item_effect_model_a->unk1A8 = func_0010DB40; // match: the first model again, not the second
    buffer = align16(buffer);
    file = func_00102238(D_00441110, 0, 1);
    size = func_001025C0(file, buffer, -1);
    func_001024B0(file);
    model_data = buffer;
    buffer += size;
    buffer = align16(buffer);
    func_00108940((MotionFile *)model_data, (ModelChannel **)&buffer, item_effect_model_b, 1);

    for (i = 0; i < 7; i++) {
        sprintf(name, D_00441128, i + 40);
        file = func_00102238(name, 0, 1);
        if (file >= 0) {
            buffer = align16(buffer);
            size = func_001025C0(file, buffer, -1);
            func_001024B0(file);
            item_effect_textures[i] = buffer;
            buffer += size;
            func_0010B510(item_effect_textures[i], 0x3B40, 0x3EC0);
        }
    }
}

/* Clears the item use effect state. */
int ItemEffect_Reset(void) { // match: func_001000C8 only matches with it declared to return a value
    item_effect_active = 0;
    item_effect_kind = 0;
    item_effect_step = 0;
    item_effect_model = NULL;
    item_effect_load_pad = 0;
    item_effect_brightness = 0;
    item_effect_def = NULL;
}

/* Starts the effect for item kind (0x46..0x52): places the model ahead of the camera and plays its motion. */
void ItemEffect_Start(int kind, int marked) {
    ModelInstance *model;
    int i;

    if (item_effect_active != 0) {
        return;
    }
    if ((u32)(kind - 0x46) >= 13) {
        return;
    }
    item_effect_pending_effects = item_effect_active = 1;
    item_effect_marked = marked;
    item_effect_def = &D_0042BBE0[kind];
    item_effect_kind = kind;
    item_effect_step = 0;
    if (kind >= 0x4D) {
        model = item_effect_model_b;
        model->unk190 = 2;
        model->unk194 = (int)item_effect_textures[kind - 0x4C];
        item_effect_model = model;
    } else {
        model = item_effect_model_a;
        model->unk190 = 2;
        model->unk194 = (int)item_effect_textures[kind - 0x46];
        item_effect_model = model;
    }
    item_effect_model->unk1A8 = func_0010DB40;
    item_effect_model->draw = ItemEffect_DrawModel;
    func_00108A08(item_effect_model, 0);
    func_00108AC8(item_effect_model);
    item_effect_brightness = 0x80;
    func_0010E8E0(item_effect_model, 0x80, 0x80, 0x80, item_effect_brightness);
    Vector_Scale((float *)&item_effect_model->position, D_00420D00.unk10, 75.0f);
    func_00216C48(&item_effect_model->position, &item_effect_model->position, D_00420D00.unk00);
    func_00216778(&item_effect_model->rotation, &D_00413F80.rotation);
    item_effect_model->unkE0 = item_effect_model->unkE4 = item_effect_model->unkE8 = 1.0f;
    item_effect_model->unkEC = 1.0f;
    item_effect_model->unk148 = (s32)D_0042C110;
    item_effect_model->unk14C = &D_0038A440[D_00446770 * 0x400];
    for (i = 0; i < item_effect_model->part_count; i++) {
        if (i == 0) {
            MODEL_PART_FLAG_BITS(&item_effect_model->parts[0])->bit41 = TRUE;
        } else {
            MODEL_PART_FLAG_BITS(&item_effect_model->parts[i])->bit41 = FALSE;
        }
    }
}

/* One frame of the running effect: runs the current step, then poses the model. */
void ItemEffect_Update(void) {
    Vector4 origin;
    PlayerItemDef *def;
    ModelPart *part;
    int count;
    int i;

    memset(&origin, 0, sizeof(origin));
    origin.w = 1.0f;
    if (item_effect_active == 0) {
        return;
    }
    D_0042C190[item_effect_step]();
    if (item_effect_active == 0) {
        return;
    }
    if (item_effect_pending_effects != 0 && item_effect_model != NULL &&
        MODEL_CHANNEL(item_effect_model)->param >= item_effect_def->unk08) {
        Effect_StartSpawner(item_effect_def->unk0C, &origin, &origin, item_effect_model);
        Effect_StartSpawner(0x26A, &D_00413F80.position, &D_00413F80.rotation, item_effect_model);
        def = &weapon_defs[D_00413F80.weapon];
        item_effect_pending_effects = 0;
        if (D_00413F80.equipped_magic != 0 || def->unk42 != 0xFFFF ||
            (def->unk52 != 0xFFFF && D_003679A0[D_00413F80.weapon].unk14 + 1 >= 3) || func_001BF6E8() != 0) {
            D_00413F80.unkC0 = 5000;
        }
        if (item_effect_marked != 0) {
            Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_MAGIC_LEARNED);
            item_effect_marked = 0;
        }
    }
    ModelMatrix_Build(item_effect_model, &item_effect_model->position, &item_effect_model->rotation, &item_effect_model->unkE0);
    count = item_effect_model->part_count;
    part = item_effect_model->parts;
    for (i = 0; i < count; i++, part++) {
        ModelMatrix_BuildPartLocal(item_effect_model, part, part);
    }
    part = item_effect_model->parts;
    for (i = 0; i < count; i++, part++) {
        func_0010B5D0(part);
    }
}

/* Item-effect step 0: asks the game for status 0xC5F and frees spawner 0x5D. */
void ItemEffect_StepRequestGameStatus(void) {
    if (D_0038CC50.unk30 == 0) {
        D_0038CC50.unk30 = 0xC5F;
        D_0038CC50.unk40 = item_effect_model;
        item_effect_step += 2;
        Effect_FreeSpawnersByParam(0x5D);
    }
}

/* Item-effect step 1: waits until the confirmation overlay flag is clear. */
void ItemEffect_StepWaitOverlay(void) {
    if (D_004141D8[0] == 0) {
        D_0038CC50.unk30 = 0xC5F;
        D_0038CC50.unk40 = item_effect_model;
        item_effect_step++;
    }
}

/* Item-effect step 2: advances the model motion until the kind's first frame. */
void ItemEffect_StepPlayMotion(void) {
    func_00108AC8(item_effect_model);
    if (MODEL_CHANNEL(item_effect_model)->param >= item_effect_def->unk00) {
        item_effect_step++;
    }
}

/* Item-effect step 3: fades the model out and clears the effect when done. */
void ItemEffect_StepFadeOut(void) {
    int brightness;

    if (MODEL_CHANNEL(item_effect_model)->param <= 0 && item_effect_brightness <= 0) {
        int *status_words = (int *)&D_0038CC50; // match: field stores get scheduled in the other order

        item_effect_active = 0;
        item_effect_step = 0;
        status_words[0x30 / 4] = 0; /* unk30 */
        status_words[0x40 / 4] = 0; /* unk40 */
        item_effect_model = NULL;
        item_effect_load_pad = 0;
        item_effect_def = NULL;
        return;
    }
    if (MODEL_CHANNEL(item_effect_model)->param > 0) {
        func_00108AC8(item_effect_model);
    }
    if (item_effect_brightness > 0) {
        brightness = item_effect_brightness - item_effect_def->unk04;
        item_effect_brightness = brightness;
        if (brightness < 0) {
            item_effect_brightness = 0;
        }
        func_0010E8E0(item_effect_model, 0x80, 0x80, 0x80, item_effect_brightness);
    }
    MODEL_PART_FLAG_WORD(item_effect_model->parts) &= ~MODEL_PART_BIT41;
}

/* Draws the item-effect model while the effect runs. */
void ItemEffect_Draw(void) {
    if (item_effect_active != 0 && item_effect_step > 0 && item_effect_model != NULL) {
        func_0010B018(item_effect_model, 0, 0);
    }
}

/* The item effect model's draw handler: draws the model, then writes TEST_1 = 0x5010D (alpha test greater than
   0x10, depth test on). */
static void ItemEffect_DrawModel(ModelInstance *model) {
    func_0010AEB8(model);
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x5010D);
    func_00109AD0();
}
