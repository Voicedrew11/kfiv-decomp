#include "common.h"
#include "kf_effect.h"
#include "kf_map.h"
#include "kf_player.h"
#include "kf_screen.h"

extern MapSlot D_0038CD20[2];
extern char D_0038A440[];
extern EffectActor D_003F3550[0x180];
extern PlayerState D_00413F80;
extern Vector4 D_00574260[8];
extern int D_00446770;
extern int D_004468D0;

void func_001A2108(void);
int func_001160B0(f32 *point, AreaNode *node, f32 margin);
AreaNode *func_00115950(f32 *point, f32 margin);
void func_001A2E58(ModelInstance *model, int unused_arg1, int record_id, int id_bits);
void func_00216778(void *, void *);
void func_0020C300(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void func_0020B8B0(ModelInstance *, ModelPart *, ModelPart *);
void func_0010B018(ModelInstance *, s32, s32);

/* Draws the live effects for pass 0 or 1: finds the map record each is in, picks its texture
   frame, places its parts and sends the model. In pass 0 it also frees the effects whose
   frame has reached keyframe_end. */
void func_001A2168(int pass) {
    Vector4 unused[3]; /* unused, but retail reserves the bytes */
    EffectActor *effect;
    ModelInstance *model;
    ModelPart *part;
    s64 placement;
    int i;
    int part_index;
    int step;
    int record_id;

    if (D_004468D0 == 0) {
        return;
    }
    func_001A2108();
    effect = D_003F3550;
    for (i = 0; i < 0x180; i++, effect++) {
        if (!(effect->flags & EFFECT_FLAG_LIVE)) {
            continue;
        }
        placement = EFFECT_DEF_FLAGS(effect->def) & 0x600000;
        model = effect->model;
        if (placement == 0 || placement == 0x600000) {
            // match: the margin is set up before the node, so the call passes it first
            if (((int (*)(f32 *, f32, AreaNode *))func_001160B0)((f32 *)&model->position, 0.0f, (AreaNode *)effect->unk58) == 0) {
                effect->unk58 = (char *)func_00115950((f32 *)&effect->model->position, 0.0f);
            }
            if (effect->unk58 == NULL && effect->owner_entity == (Entity *)&D_00413F80 && ((PlayerState *)effect->owner_entity)->map_index != 0xFFFF) {
                effect->unk58 = D_0038CD20[D_00446770].records + (((PlayerState *)effect->owner_entity)->map_index << 7);
            }
            if (pass == 0 && effect->unk58 == NULL) {
                if (effect->frame >= effect->def->keyframe_end) {
                    func_001A2A18(effect);
                }
                continue;
            }
        }
        if (effect->def->unkF7 != 0) {
            step = (effect->frame + 1) / (effect->def->unkFC + 1);
            switch (effect->def->unkF6) {
            case 0:
                record_id = step % effect->def->unkF7;
                break;
            case 1:
                // match: past the last frame record_id keeps whatever the effect before left in it
                if (step < effect->def->unkF7) {
                    record_id = step;
                }
                break;
            case 2:
                record_id = effect->def->unkF7 - 1;
                if (step < effect->def->unkF7) {
                    record_id = step;
                }
                break;
            default:
                record_id = 0;
                break;
            }
            record_id = effect->def->unkFA + record_id * effect->def->unkF8;
            func_001A2E58(model, model->unk104, effect->def->unkFA, record_id);
        }
        func_00216778(&model->unk150, effect);
        // match: it returns a value
        ((int (*)(ModelInstance *, Vector4 *, Vector4 *, f32 *))func_0020C300)(model, &model->position, &model->rotation, &model->unkE0);
        part = effect->model->parts;
        for (part_index = 0; part_index < model->part_count; part_index++, part++) {
            func_0020B8B0(model, part, part);
        }
        model->unk14C = D_0038A440 + (D_00446770 << 10);
        // match: the flag is read through the union so the test stays behind the unk14C store
        if (!(((EntityFlagWord *)&effect->def->unkF0)->word & (0x8000L << 25))) {
            model->unk148 = (s32)D_00574260;
        } else {
            model->unk148 = (s32)(D_0038A440 - 0x1D0);
        }
        model->hit_count = 0;
        if (pass == 0 || !(EFFECT_DEF_WORD_F0(effect->def) & (0x8000L << 26))) {
            func_0010B018(model, ((EffectTableEntry *)effect->def)->placement, pass);
        }
        if (pass == 0 && effect->frame >= effect->def->keyframe_end) {
            func_001A2A18(effect);
        }
    }
}
