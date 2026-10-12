#include "kf_model.h"
#include "kf_game.h"
#include "kf_map.h"

int func_001CA5E0(int);
int func_001CA630(int);
int func_001CA788(int);
void *func_001CA498(int, int);
ItemNode *func_001CA520(ItemNode *);
void func_00216778(void *, void *);
void func_0010D018(ModelInstance *, char **, char **);
void func_0010ED80(char *, int, int);
void func_0010E8E0(void *, int, int, int, int);
void func_0010B510(char *, int, int);
void func_001CA500(ListNode *);
extern MapSlot D_0038CD20[];
extern DroppedItemRecord D_00358928[0x80][16];
extern GameDefinition D_004147F8[380];
extern u32 D_004469D0;
extern ItemNode D_00580040[2][16];
extern ModelInstance D_0058F6C0[2][16];
extern char D_005A9EC0[2][16][0x200];

/* Lays a dropped item down in a map: takes a free node of the 16 at D_00580040, or the oldest
   one whose priority is not above this one's, sets up its model and fills in the record that is
   saved for it. `order` is the record's own when it is put back from a save, 0xFFFFFFFF for a
   new drop. Returns FALSE when the player may not have another of the item or nothing is free. */
s32 func_001C8F18(s32 map, s32 item, s32 priority, f32 *position, f32 floor_y, f32 *offset, f32 *rotation, f32 *scale,
                  u32 order) {
    char *model_data;
    char *buffer;
    ItemNode *node;
    ItemNode *oldest;
    ItemModelSource *source;
    s32 index;
    s32 i;

    if (!func_001CA5E0(item)) {
        return FALSE;
    }
    if (!func_001CA630(item)) {
        return FALSE;
    }
    if (!func_001CA788(item)) {
        return FALSE;
    }
    source = func_001CA498(map, (D_004147F8 + item)->unk30);
    if (source == NULL) {
        return FALSE;
    }
    index = -1;
    node = D_00580040[map];
    for (i = 0; i < 16; i++, node++) {
        if (node->record == NULL) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        oldest = NULL;
        node = D_00580040[map];
        for (i = 0; i < 16; i++, node++) {
            if ((s32)ITEM_NODE_BITS(node)->priority <= priority) {
                if (oldest == NULL || (u32)oldest->record->order > (u32)node->record->order) {
                    oldest = node;
                    index = i;
                }
            }
        }
        if (oldest == NULL) {
            return FALSE;
        }
        node = oldest;
        func_001CA520(node);
    }
    node->item = item;
    node->def = NULL;
    node->model = &D_0058F6C0[map][index];
    node->record = &D_00358928[D_0038CD20[map].tag][index];
    ITEM_NODE_BITS(node)->map = map;
    func_00216778(&node->model->position, position);
    func_00216778(&node->model->rotation, rotation);
    func_00216778(&node->model->unkE0, scale);
    ITEM_NODE_BITS(node)->bit2 = TRUE;
    node->unk2C = position[1];
    node->unk28 = floor_y;
    node->unk30 = 0.0f;
    model_data = source->model_data;
    buffer = D_005A9EC0[map][index];
    func_0010D018(node->model, &model_data, &buffer);
    func_0010ED80((char *)node->model, 0x3B40, 0x3EC0);
    func_0010E8E0(node->model, 0xFF, 0xFF, 0xFF, -1);
    node->model->unk190 = 2;
    node->model->unk194 = (int)source->texture;
    func_0010B510(source->texture, 0x3B40, 0x3EC0);
    if (order == 0xFFFFFFFF) {
        order = D_004469D0++;
    }
    node->record->position[0] = position[0] + offset[0];
    node->record->position[1] = floor_y + offset[1];
    node->record->position[2] = position[2] + offset[2];
    node->record->rotation[0] = rotation[0] * 16.0f;
    node->record->rotation[1] = rotation[1] * 16.0f;
    node->record->rotation[2] = rotation[2] * 16.0f;
    node->record->scale[0] = scale[0] * 16.0f;
    node->record->scale[1] = scale[1] * 16.0f;
    node->record->scale[2] = scale[2] * 16.0f;
    node->record->item = item;
    node->record->priority = priority;
    node->record->used = TRUE;
    node->record->order = order;
    func_001CA500((ListNode *)node);
    return TRUE;
}
