#include "kf_item_effect.h"
#include "kf_effect.h"
#include "kf_game.h"
#include "kf_gs.h"
#include "kf_model.h"
#include "kf_player.h"
#include "kf_splash.h"
#include "kf_message.h"

extern char map_draw_slot_blocks[];
extern LevelRecord weapon_level_records[];
extern GameStatus game_status;
extern PlayerState player_state;
extern EventRunner D_00420D00;
extern ItemUseEffectDef D_0042BBE0[];
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
int File_Open(char *, int, int);
int File_Close(int);
int File_Read(int, void *, int);
MotionData *ModelMotion_LoadFile(MotionFile *, ModelChannel **, ModelInstance *, int);
void ModelMotion_Select(ModelInstance *, int);
void ModelMotion_Step(void *);
void DrawList_Add(ModelInstance *, s32, s32);
void TextureTable_SetBasePointers(char *, int, int);
void Model_MultiplyChildMatrices(ModelPart *);
void Model_SetUpPosed(ModelInstance *, char **, char **);
void Model_DrawPosedDepthByPart(ModelInstance *);
void Model_SetPrimBits(ModelInstance *, s64, s64);
void Model_SetTexture(ModelInstance *, u64, s64);
void Model_SetAlphaBlend(ModelInstance *, s64);
void Model_SetVertexColour(void *, int, int, int, int);
int Player_KnowsItemMenuMagic(void);
void ModelTexture_SendForModel(void *);
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int, s64);
void Packet_EndGsRegisters(void);
static void ItemEffect_DrawModel(ModelInstance *model);
void ItemEffect_StepRequestGameStatus(void);
void ItemEffect_StepWaitOverlay(void);
void ItemEffect_StepPlayMotion(void);
void ItemEffect_StepFadeOut(void);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void Vector_Copy(void *, void *);
void Vector_Add(void *, void *, void *);
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
static int item_effect_marked;              /* 0x4473B0 spell slot marked by Player_LearnSpellInRow */

static char *item_effect_textures[7];       /* 0x5F9B40 loaded i0040..i0046.tx2 */

/* 0x42C040 the effect def of each item kind 0x46 to 0x52 */
ItemUseEffectDef item_effect_defs[13] = {
    { 0x5A, 0x06, 0x55, -1 },
    { 0x5A, 0x06, 0x55, 0x27C },
    { 0x5A, 0x06, 0x55, 0x279 },
    { 0x5A, 0x06, 0x55, 0x276 },
    { 0x5A, 0x06, 0x55, 0x272 },
    { 0x5A, 0x06, 0x55, 0x26F },
    { 0x5A, 0x06, 0x55, 0x26B },
    { 0x5A, 0x06, 0x55, 0x27C },
    { 0x5A, 0x06, 0x55, 0x279 },
    { 0x5A, 0x06, 0x55, 0x276 },
    { 0x5A, 0x06, 0x55, 0x272 },
    { 0x5A, 0x06, 0x55, 0x26F },
    { 0x5A, 0x06, 0x55, 0x26B },
};

/* 0x42C110 the light block the effect's model is lit by (ItemEffect_Start, see effect_light_block) */
Vector4 D_0042C110[8] = {
    { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },
};

/* 0x42C190 the four steps of an item effect, one a frame (ItemEffect_Update) */
void (*D_0042C190[4])(void) = {
    ItemEffect_StepRequestGameStatus,
    ItemEffect_StepWaitOverlay,
    ItemEffect_StepPlayMotion,
    ItemEffect_StepFadeOut,
};

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
    file = File_Open(D_004410C8, 0, 1);
    size = File_Read(file, buffer, -1);
    File_Close(file);
    model_data = buffer;
    buffer += size;
    buffer = align16(buffer);
    Model_SetUpPosed(item_effect_model_a, &model_data, &buffer);
    Model_SetVertexColour(item_effect_model_a, 0x80, 0x80, 0x80, 0x80);
    Model_SetAlphaBlend(item_effect_model_a, 0x44);
    Model_SetPrimBits(item_effect_model_a, 0x60, 0x40);
    Model_SetTexture(item_effect_model_a, 0x4007D805E1313B40, 0x60);
    item_effect_model_a->unk1A8 = Model_DrawPosedDepthByPart;
    buffer = align16(buffer);
    file = File_Open(D_004410E0, 0, 1);
    size = File_Read(file, buffer, -1);
    File_Close(file);
    model_data = buffer;
    buffer += size;
    buffer = align16(buffer);
    ModelMotion_LoadFile((MotionFile *)model_data, (ModelChannel **)&buffer, item_effect_model_a, 1);

    buffer = align16(buffer);
    item_effect_model_b = (ModelInstance *)buffer;
    buffer += sizeof(ModelInstance);
    buffer = align16(buffer);
    file = File_Open(D_004410F8, 0, 1);
    size = File_Read(file, buffer, -1);
    File_Close(file);
    model_data = buffer;
    buffer += size;
    buffer = align16(buffer);
    Model_SetUpPosed(item_effect_model_b, &model_data, &buffer);
    Model_SetVertexColour(item_effect_model_b, 0x80, 0x80, 0x80, 0x80);
    Model_SetAlphaBlend(item_effect_model_b, 0x44);
    Model_SetPrimBits(item_effect_model_b, 0x60, 0x40);
    Model_SetTexture(item_effect_model_b, 0x4007D805E1313B40, 0x60);
    item_effect_model_a->unk1A8 = Model_DrawPosedDepthByPart; // match: the first model again, not the second
    buffer = align16(buffer);
    file = File_Open(D_00441110, 0, 1);
    size = File_Read(file, buffer, -1);
    File_Close(file);
    model_data = buffer;
    buffer += size;
    buffer = align16(buffer);
    ModelMotion_LoadFile((MotionFile *)model_data, (ModelChannel **)&buffer, item_effect_model_b, 1);

    for (i = 0; i < 7; i++) {
        sprintf(name, D_00441128, i + 40);
        file = File_Open(name, 0, 1);
        if (file >= 0) {
            buffer = align16(buffer);
            size = File_Read(file, buffer, -1);
            File_Close(file);
            item_effect_textures[i] = buffer;
            buffer += size;
            TextureTable_SetBasePointers(item_effect_textures[i], 0x3B40, 0x3EC0);
        }
    }
}

/* Clears the item use effect state. */
int ItemEffect_Reset(void) { // match: GameLoop_ResetSystems only matches with it declared to return a value
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
    item_effect_model->unk1A8 = Model_DrawPosedDepthByPart;
    item_effect_model->draw = ItemEffect_DrawModel;
    ModelMotion_Select(item_effect_model, 0);
    ModelMotion_Step(item_effect_model);
    item_effect_brightness = 0x80;
    Model_SetVertexColour(item_effect_model, 0x80, 0x80, 0x80, item_effect_brightness);
    Vector_Scale((float *)&item_effect_model->position, D_00420D00.unk10, 75.0f);
    Vector_Add(&item_effect_model->position, &item_effect_model->position, D_00420D00.unk00);
    Vector_Copy(&item_effect_model->rotation, &player_state.rotation);
    item_effect_model->unkE0 = item_effect_model->unkE4 = item_effect_model->unkE8 = 1.0f;
    item_effect_model->unkEC = 1.0f;
    item_effect_model->unk148 = (s32)D_0042C110;
    item_effect_model->unk14C = &map_draw_slot_blocks[D_00446770 * 0x400];
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
        Effect_StartSpawner(0x26A, &player_state.position, &player_state.rotation, item_effect_model);
        def = &weapon_defs[player_state.weapon];
        item_effect_pending_effects = 0;
        if (player_state.equipped_magic != 0 || def->unk42 != 0xFFFF ||
            (def->unk52 != 0xFFFF && weapon_level_records[player_state.weapon].unk14 + 1 >= 3) || Player_KnowsItemMenuMagic() != 0) {
            player_state.unkC0 = 5000;
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
        Model_MultiplyChildMatrices(part);
    }
}

/* Item-effect step 0: asks the game for status 0xC5F and frees spawner 0x5D. */
void ItemEffect_StepRequestGameStatus(void) {
    if (game_status.unk30 == 0) {
        game_status.unk30 = 0xC5F;
        game_status.unk40 = item_effect_model;
        item_effect_step += 2;
        Effect_FreeSpawnersByParam(0x5D);
    }
}

/* Item-effect step 1: waits until the confirmation overlay flag is clear. */
void ItemEffect_StepWaitOverlay(void) {
    if (D_004141D8[0] == 0) {
        game_status.unk30 = 0xC5F;
        game_status.unk40 = item_effect_model;
        item_effect_step++;
    }
}

/* Item-effect step 2: advances the model motion until the kind's first frame. */
void ItemEffect_StepPlayMotion(void) {
    ModelMotion_Step(item_effect_model);
    if (MODEL_CHANNEL(item_effect_model)->param >= item_effect_def->unk00) {
        item_effect_step++;
    }
}

/* Item-effect step 3: fades the model out and clears the effect when done. */
void ItemEffect_StepFadeOut(void) {
    int brightness;

    if (MODEL_CHANNEL(item_effect_model)->param <= 0 && item_effect_brightness <= 0) {
        int *status_words = (int *)&game_status; // match: field stores get scheduled in the other order

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
        ModelMotion_Step(item_effect_model);
    }
    if (item_effect_brightness > 0) {
        brightness = item_effect_brightness - item_effect_def->unk04;
        item_effect_brightness = brightness;
        if (brightness < 0) {
            item_effect_brightness = 0;
        }
        Model_SetVertexColour(item_effect_model, 0x80, 0x80, 0x80, item_effect_brightness);
    }
    MODEL_PART_FLAG_WORD(item_effect_model->parts) &= ~MODEL_PART_BIT41;
}

/* Draws the item-effect model while the effect runs. */
void ItemEffect_Draw(void) {
    if (item_effect_active != 0 && item_effect_step > 0 && item_effect_model != NULL) {
        DrawList_Add(item_effect_model, 0, 0);
    }
}

/* The item effect model's draw handler: draws the model, then writes TEST_1 = 0x5010D (alpha test greater than
   0x10, depth test on). */
static void ItemEffect_DrawModel(ModelInstance *model) {
    ModelTexture_SendForModel(model);
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_TEST_1, 0x5010D);
    Packet_EndGsRegisters();
}
