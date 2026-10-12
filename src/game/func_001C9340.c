#include "kf_model.h"
#include "kf_game.h"

int func_001CA5E0(int);
int func_001CA630(int);
int func_001CA788(int);
void *func_001CA498(int, int);
void func_00216778(void *, void *);
void func_0010D018(ModelInstance *, char **, char **);
void func_0010ED80(char *, int, int);
void func_0010E8E0(void *, int, int, int, int);
void func_0010B510(char *, int, int);
void func_001CA500(ListNode *);
extern GameDefinition D_004147F8[380];
extern ItemNode D_005807C0[2][32];
extern ModelInstance D_00592EC0[2][32];
extern char D_005ADEC0[2][32][0x200];

/* Puts an item into a map for a map object (a chest's contents, say): takes a free node of the 32 at
   D_005807C0 and sets up its model. Returns NULL when the player may not have another of the item, when
   the object already has one out, or when no node or no model for the item is there. */
ItemNode *func_001C9340(int map, int item, void *position, void *rotation, void *scale, void *object) {
    char *sources[2];
    ItemNode *node;
    ItemModelSource *source;
    ModelInstance *model;
    s32 i;

    if (!func_001CA5E0(item)) {
        return NULL;
    }
    if (!func_001CA630(item)) {
        return NULL;
    }
    if (!func_001CA788(item)) {
        return NULL;
    }
    node = D_005807C0[map];
    for (i = 0; i < 32; i++, node++) {
        if ((void *)node->unk08 == object) {
            return NULL;
        }
    }
    node = D_005807C0[map];
    for (i = 0; i < 32 && node->unk08 != 0; i++, node++) {
    }
    if (i >= 32) {
        return NULL;
    }
    source = func_001CA498(map, (D_004147F8 + item)->unk30);
    if (source == NULL) {
        return NULL;
    }
    model = &D_00592EC0[map][i];
    ITEM_NODE_MAP_BIT(node)->map = map;
    node->item = item;
    node->unk08 = (s32)object;
    node->model = model;
    func_00216778(&model->position, position);
    func_00216778(&node->model->rotation, rotation);
    func_00216778(&node->model->unkE0, scale);
    sources[0] = source->model_data;
    sources[1] = D_005ADEC0[map][i];
    func_0010D018(node->model, &sources[0], &sources[1]);
    func_0010ED80((char *)node->model, 0x3B40, 0x3EC0);
    func_0010E8E0(node->model, 0xFF, 0xFF, 0xFF, -1);
    node->model->unk190 = 2;
    node->model->unk194 = (int)source->texture;
    func_0010B510(source->texture, 0x3B40, 0x3EC0);
    func_001CA500((ListNode *)node);
    return node;
}
