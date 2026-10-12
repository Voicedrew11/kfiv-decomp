#include "common.h"
#include "kf_effect.h"
#include "kf_map.h"

extern EffectActor D_003F3550[0x180];
extern MapSlot D_0038CD20[2];
extern MapObject D_003A1A30[][0x200];
extern Entity D_003BEDF0[][0x80];
extern Entity D_003D6C70;           /* the last entity of D_003BEDF0 */
extern void (*D_003F1710[4][4])(ModelInstance *);
extern int D_00446770;
extern char *D_00447100;
extern char **D_00447104;

void func_0020B2E8(void *, u32);
void func_0010D018(ModelInstance *, char **, char **);
int func_00111C78(ModelInstance *model, int first, int last);
u32 *func_00116BD0(int, int);

/* Sets up the model of effect number index from its definition's model file. */
void func_001A2C68(int index) {
    char *file_cursor;
    char *memory;
    EffectActor *effect = &D_003F3550[index];
    ModelInstance *model = effect->model;
    EffectDef *def;
    int map;

    func_0020B2E8(model, 0x70);
    def = effect->def;
    if (EFFECT_DEF_FLAGS(def) & 0x20000) {
        if ((EFFECT_DEF_FLAGS(def) & 0x600000) == 0 || (EFFECT_DEF_FLAGS(def) & 0x600000) == 0x600000) {
            model->unk102 |= 0x300;
        } else {
            model->unk102 |= 0x400;
        }
    }
    model->unk100 |= 0x1800;
    file_cursor = D_00447104[def->unkFE];
    memory = D_00447100 + (index << 11);
    func_0010D018(model, &file_cursor, &memory);
    model->draw = D_003F1710[((EffectTableEntry *)effect->def)->placement][effect->def->unkF4];
    model->unk100 |= 0x800;
    if (func_00111C78(model, 0x3BC0, 0x3C3F) != 0) {
        // match: the owner is read from the effect again for every test (a macro in the game, most likely)
        if (effect->owner_entity == (Entity *)&D_0038CD20[0]) {
            map = 0; /* the owner is a map slot itself */
        } else if (effect->owner_entity == (Entity *)&D_0038CD20[1]) {
            map = 1;
        } else if ((MapObject *)effect->owner_entity >= D_003A1A30[0] && (MapObject *)effect->owner_entity <= &D_003A1A30[1][0x1FF]) {
            map = (u8)((MapObject *)effect->owner_entity)->unk10 & 1;
        } else if (effect->owner_entity >= D_003BEDF0[0] && effect->owner_entity <= &D_003D6C70) {
            map = effect->owner_entity->group_id; /* an entity: the map it is in */
        } else {
            map = D_00446770;
        }
        effect->model->unk190 = 3;
        effect->model->unk194 = (int)func_00116BD0(map, 4);
        effect->model->unk192 = 0;
        effect->model->unk198 = (int)func_00116BD0(map, 5);
    }
}
