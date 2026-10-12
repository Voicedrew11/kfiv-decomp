#include "kf_model.h"
#include "kf_game.h"
#include "kf_map.h"

s32 func_001C8F18(s32, s32, s32, f32 *, f32, f32 *, f32 *, f32 *, u32);
void *func_001CA498(int, int);
void func_0010D018(ModelInstance *, char **, char **);
void func_0010ED80(char *, int, int);
void func_0010E8E0(void *, int, int, int, int);
void func_0010B510(char *, int, int);
void func_001CA578(void *);
extern MapSlot D_0038CD20[];
extern DroppedItemRecord D_00358928[0x80][16];
extern GameDefinition D_004147F8[380];
extern MapItemDef *D_004469B8[2];
extern s16 *D_004469C0[2];
extern s32 D_00447190[2];
extern ItemModelSource D_0057E040[2][32];
extern ItemNode D_0057E240[2][64];
extern ModelInstance D_005816C0[2][64];
extern char D_00599EC0[2][64][0x200];
extern s8 D_005B5EC0[2][0x100];

/* Sets up a map's items once it is loaded: notes where the model and texture of each kind of
   item lie in the map file, gives every fixed item its node and model, and lays the dropped
   items of the map's save record down again. */
void func_001C82C0(int map) {
    DroppedItemRecords saved;
    f32 position[4];
    f32 rotation[4];
    f32 scale[4];
    f32 offset[4];
    char *model_data;
    char *cursor;
    MapSlot *slot = &D_0038CD20[map];
    s16 *kinds = D_004469C0[map];
    MapHeader *header;
    MapItemDef *def;
    ItemNode *node;
    GameDefinition *definition;
    ItemModelSource *source;
    DroppedItemRecord *record;
    s32 count;
    s32 i;

    memset(offset, 0, sizeof(offset));
    header = slot->header;
    D_00447190[map] = header->item_count;
    cursor = (char *)header + header->item_models_offset;
    for (i = 0; i < 32 && kinds[i] >= 0; i++) {
        D_0057E040[map][i].model_data = cursor;
        cursor += *(s32 *)cursor;
        D_0057E040[map][i].texture = cursor;
        cursor += *(s32 *)cursor;
    }
    count = D_00447190[map];
    def = D_004469B8[map];
    node = D_0057E240[map];
    for (i = 0; i < count; i++, def++, node++) {
        D_005B5EC0[map][def->slot_id] = i;
        definition = &D_004147F8[def->item];
        source = func_001CA498(map, definition->unk30);
        if (source != NULL) {
            ITEM_NODE_BITS(node)->map = map;
            node->def = def;
            node->item = def->item;
            node->record = NULL;
            node->index = i;
            node->model = &D_005816C0[map][i];
            model_data = source->model_data;
            cursor = D_00599EC0[map][i];
            func_0010D018(node->model, &model_data, &cursor);
            func_0010ED80((char *)node->model, 0x3B40, 0x3EC0);
            func_0010E8E0(node->model, 0xFF, 0xFF, 0xFF, -1);
            node->model->draw = (void (*)(ModelInstance *))func_001CA578;
            node->model->unk194 = (int)source->texture;
            node->model->unk190 = 2;
            func_0010B510(source->texture, 0x3B40, 0x3EC0);
        }
    }
    saved = *(DroppedItemRecords *)D_00358928[D_0038CD20[map].tag];
    memset(D_00358928[D_0038CD20[map].tag], 0, sizeof(D_00358928[0]));
    position[3] = rotation[3] = scale[3] = 1.0f;
    record = saved.records;
    for (i = 0; i < 16; i++, record++) {
        if (record->used) {
            position[0] = record->position[0];
            position[1] = record->position[1];
            position[2] = record->position[2];
            rotation[0] = record->rotation[0] * 0.0625f;
            rotation[1] = record->rotation[1] * 0.0625f;
            rotation[2] = record->rotation[2] * 0.0625f;
            scale[0] = record->scale[0] * 0.0625f;
            scale[1] = record->scale[1] * 0.0625f;
            scale[2] = record->scale[2] * 0.0625f;
            func_001C8F18(map, record->item, record->priority, position, position[1], offset, rotation, scale,
                          record->order);
        }
    }
}
