#include "kf_entity.h"

extern float D_00413F84[];

double atan2(double, double);
float func_00194FB8(float);
float func_00195388(float *, float, float);

/* Aim: turn the angle toward the target into a clamped value, smooth it, and copy it to the part. */
void func_00177520(Entity *actor) {
    ModelInstance *model = ENTITY_MODEL(actor);
    ModelPart *part = &model->parts[ENTITY_CONTROLLER(actor)->unk434];
    EntityUnkE0 *sub = &actor->unkE0;
    float angle;

    if (part != NULL) {
        angle = func_00194FB8(atan2(D_00413F84[0] - model->position.y, actor->distance_to_player));
        if (0.87266463f < angle) {
            angle = 0.87266463f;
        } else if (angle < -0.87266463f) {
            angle = -0.87266463f;
        }
        func_00195388(&sub->unk48, angle, 10.0f);
        part->unk50 = sub->unk48;
    }
}
