#include "kf_effect.h"

extern FxTableEntry D_00402DA8[];

void *memset(void *, int, u32);
void func_00216778(void *, void *);
s32 func_001B1470(Unk1B1470Request *, Unk1B1470Extra *);

#define SHOT_REQUEST ((Unk1B1470Request *)shot->unk04)

/* The hit check of an effect: builds the attack request from the effect table's row
   (or, for a weapon's shot, from the request the weapon left) and runs it. Returns what
   func_001B1470 found. */
int func_001B4F90(void *effect_actor) {
    Unk1B1470Request request;
    EffectActor *effect = effect_actor;
    FxTableHitCheck *check;
    Vector4 *position;
    Unk1B1470Extra *shot;
    s32 hits;

    if (effect == NULL) {
        return 0;
    }
    if (effect->effect_param < 0x100 && effect->def->unk10B < 3) {
        check = &D_00402DA8[effect->effect_param].hit_checks[effect->def->unk10B];
        memset(&request, 0, sizeof(request));
        switch (check->shape) {
        case 0:
            position = &effect->unk70;
            request.unk30 = check->unk1E;
            request.unk4C_0 = 0;
            break;
        case 1:
            position = &effect->unk70;
            request.unk30 = check->unk1E;
            request.unk4C_0 = 1;
            request.unk34 = check->unk20;
            break;
        case 2:
            position = &effect->unk70;
            request.unk4C_0 = 2;
            func_00216778(request.unk20, position);
            break;
        default:
            return 0;
        }
    } else {
        return 0;
    }
    func_00216778(request.position, &effect->model->position);
    func_00216778(request.unk10, position);
    request.owner = effect->owner_entity;
    request.unk4C_2 = 0;
    request.unk4D = 0;
    if (check->unk23 & 1) {
        request.unk4D = 1;
    }
    if (check->unk23 & 2) {
        request.unk4D |= 2;
    }
    if (check->unk23 & 4) {
        request.unk4D |= 4;
    }
    if (check->unk23 & 8) {
        request.unk4D |= 8;
    }
    if (check->unk23 & 0x10) {
        request.unk4D |= 0x10;
    }
    if (check->unk23 & 0x20) {
        request.unk4D |= 0x20;
    }
    if (check->unk23 & 0x80) {
        request.unk4D |= 0x80;
    }
    shot = effect->owner;
    if (shot->unk04 == 0) {
        request.unk50 = check->strengths;
        request.unk62 = check->unk12;
        request.unk4C_3 = check->unk26 != 0;
        request.unk4F = check->unk25;
        request.unk70 = check->unk27;
        request.unk72 = EFFECT_FLAG_PARTS(effect)->unk40;
        request.unk6F = 0;
        hits = func_001B1470(&request, shot);
        if (hits != 0 && (s8)request.unk74 != 0 && EFFECT_FLAG_PARTS(effect)->unk40 == 0) {
            EFFECT_FLAG_PARTS(effect)->unk40 = check->unk24;
        }
    } else {
        request.unk50 = SHOT_REQUEST->unk50;
        request.unk62 = SHOT_REQUEST->unk62;
        request.unk4C_3 = ((Unk1B1470RequestBits *)SHOT_REQUEST)->unk4C_3;
        request.unk70 = 0;
        request.unk4F = 0;
        request.unk6F = SHOT_REQUEST->unk6F;
        hits = func_001B1470(&request, shot);
    }
    return hits;
}
