#include "kf_game.h"
#include "kf_map.h"
#include "kf_model.h"

void func_00216778(void *, void *);
void func_001CA500(ListNode *);
extern MapSlot D_0038CD20[];
extern u8 D_00356928[0x80][0x40];
extern s32 D_00447190[2];
extern ItemNode *D_004469D4;
extern ItemNode D_0057E240[2][64];

/* Puts fixed item `index` of a map where its definition says and links it into the list of
   items lying about. Returns 0 when it cannot (already linked, no model, or its flag byte has reached the limit). */
int func_001C8B58(int map, int index) {
    ItemNode *node;
    u8 tag = D_0038CD20[map].tag;

    if (index < 0 || index >= D_00447190[map]) {
        return 0;
    }
    node = &D_0057E240[map][index];
    if (node->prev != NULL || node->next != NULL || node == D_004469D4 || node->model == NULL) {
        return 0;
    }
    if (node->def->required_flag != 0 && D_00356928[tag][node->index] >= node->def->required_flag) {
        return 0;
    }
    func_00216778(&node->model->position, &node->def->position);
    func_00216778(&node->model->rotation, &node->def->rotation);
    func_00216778(&node->model->unkE0, &node->def->scale);
    func_001CA500((ListNode *)node);
}
