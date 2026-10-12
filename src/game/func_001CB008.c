#include "kf_map.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0020C300(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void func_0010AEB8(void *);
void func_0010BA30(ModelInstance *);
void func_0010B018(ModelInstance *, s32, s32);
extern MapActor *D_004469E0;

/* Draws the map actors in use: solid ones at once, fading ones through the draw list given. */
void func_001CB008(s32 list) {
    MapActor *actor;

    func_00109A50();
    func_00109A80(0x42, 0x44);
    func_00109A80(0x43, 9);
    func_00109A80(0x47, 0x5000D);
    func_00109A80(0x48, 0x5000D);
    func_00109AD0();
    for (actor = D_004469E0; actor != NULL; actor = actor->next) {
        func_0020C300(actor->body, &actor->body->position, &actor->body->rotation, &actor->body->unkE0);
        if (actor->body->unk145 == 0) {
            func_0010AEB8(actor->body);
            func_0010BA30(actor->body);
        } else {
            func_0010B018(actor->body, 0, list);
        }
    }
}
