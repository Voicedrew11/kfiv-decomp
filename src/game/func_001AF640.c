#include "common.h"
#include "kf_effect.h"

void func_00216778(void *, void *);
void sceVu0AddVector(void *, void *, void *);
int rand(void);
double sin(double);
double cos(double);

/* Places the model of a new effect: the definition's offset with a random spread, turned by the start
   rotation or the slot's, from the start position or the slot's. */
void func_001AF640(EffectActor *effect, EffectSlot *spawn_slot) {
    Vector4 offset;
    Vector4 rotated;
    Vector4 origin;
    f32 pitch_sine;
    f32 pitch_cosine;
    f32 heading_sine;
    f32 heading_cosine;

    if (EFFECT_DEF_FLAG_WORD(effect->def) & 0x10000) {
        func_00216778(&effect->model->position, effect->def->offset_base);
        func_00216778(&origin, effect->start_position);
        pitch_sine = sin(-effect->start_rotation->x);
        pitch_cosine = cos(-effect->start_rotation->x);
        heading_sine = sin(-effect->start_rotation->y);
        heading_cosine = cos(-effect->start_rotation->y);
    } else {
        func_00216778(&effect->model->position, &spawn_slot->position);
        func_00216778(&origin, &spawn_slot->position);
        pitch_sine = sin(-spawn_slot->unk10.x);
        pitch_cosine = cos(-spawn_slot->unk10.x);
        heading_sine = sin(-spawn_slot->unk10.y);
        heading_cosine = cos(-spawn_slot->unk10.y);
    }
    offset.x = effect->def->offset_base[0] + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->offset_spread[0];
    offset.y = effect->def->offset_base[1] + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->offset_spread[1];
    offset.z = effect->def->offset_base[2] + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->offset_spread[2];
    if (EFFECT_DEF_FLAG_WORD(effect->def) & 0x40000) {
        rotated.y = pitch_cosine * offset.y + pitch_sine * offset.z;
        rotated.z = -pitch_sine * offset.y + pitch_cosine * offset.z;
        offset.z = rotated.z;
        offset.y = rotated.y;
    }
    rotated.x = heading_cosine * offset.x - heading_sine * offset.z;
    rotated.z = heading_sine * offset.x + heading_cosine * offset.z;
    offset.z = rotated.z;
    offset.x = rotated.x;
    sceVu0AddVector(&effect->model->position, &offset, &origin);
    func_00216778(&effect->unk70, &effect->model->position);
}
