#include "kf_player.h"
#include "kf_entity.h"

extern PlayerState D_00413F80;
extern int D_00447128;
extern ObjectReference *D_0044712C;
extern f32 D_00447130;
extern int D_00447134;
extern f32 D_00447138;
extern int D_0044713C;
extern int D_00447140;
extern int D_00447144;
extern int D_00447148;
extern int D_0044714C;
extern Vector4 D_0057B350;
extern Vector4 D_0057B360;
extern Vector4 D_0057B370;
extern Vector4 D_0057B380;
extern Vector4 D_0057B390;

float sqrtf(float);
void func_00216778(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(void *, void *, float);

/* Puts the player on the object (a ladder, it seems): notes its two end points, the walk to the end the player
   starts from (12 units a frame) and the number of steps. */
void func_001BD420(ObjectReference *object, int from_far_end) {
    Vector4 span;
    int step_count;
    ReferencedObject *def;

    if ((s32)(D_00413F80.unk1C8 >> 36) & 1) {
        return;
    }
    if ((s32)(D_00413F80.unk1C8 >> 34) & 1) {
        return;
    }
    func_00216778(&D_0057B370, (void *)object->model->get_point(object->model, 1));
    func_00216778(&D_0057B380, (void *)object->model->get_point(object->model, 0));
    KF_Vector_Subtract(&D_0057B390, &D_0057B370, &D_0057B380);
    PLAYER_STATUS_BITS(&D_00413F80)->bit36 = TRUE;
    PLAYER_STATUS_BITS(&D_00413F80)->bit34 = FALSE;
    PLAYER_STATUS_BITS(&D_00413F80)->bit33 = FALSE;
    D_00413F80.unk1AC = 0.0f;
    D_00413F80.unk1B0 = 0.0f;
    D_00413F80.unk1B4 = 0.0f;
    D_00413F80.unk1B8 = 0.0f;
    D_00413F80.unk1BC = 0.0f;
    D_00447128 = 0;
    D_00447130 = object->model->rotation.y;
    D_0044712C = object;
    if (from_far_end) {
        func_00216778(&D_0057B350, &D_0057B370);
    } else {
        func_00216778(&D_0057B350, &D_0057B380);
    }
    KF_Vector_Subtract(&D_0057B360, &D_0057B350, &D_00413F80.position);
    D_00447134 = sqrtf(D_0057B360.x * D_0057B360.x + D_0057B360.y * D_0057B360.y + D_0057B360.z * D_0057B360.z) / 12.0f;
    KF_Vector_Scale(&D_0057B360, &D_0057B360, 1.0f / (f32)D_00447134);
    def = object->unk08;
    step_count = def->unk50;
    D_0044713C = step_count;
    if (from_far_end) {
        D_00447144 = step_count;
        D_00447140 = step_count;
    } else {
        D_00447144 = 0;
        D_00447140 = 0;
    }
    D_00447148 = def->unk52;
    D_0044714C = 0;
    KF_Vector_Subtract(&span, &D_0057B380, &D_0057B370);
    D_00447138 = sqrtf(span.x * span.x + span.y * span.y + span.z * span.z) / (f32)D_0044713C;
}
