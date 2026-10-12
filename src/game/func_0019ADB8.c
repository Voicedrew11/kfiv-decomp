#include "kf_entity.h"
#include "kf_model.h"

extern LinkedPoint *D_004470C8[2];

void sceVu0SubVector(void *, void *, void *);
float sqrtf(float);

/* Collects the handles of the linked points of one owner (or any owner) whose reach touches the model (at most 4). */
void func_0019ADB8(ModelInstance *model, int owner) {
    int i;
    LinkedPoint *node;
    f32 delta[4];
    f32 dist;

    model->hit_count = 0;
    for (i = 1; i >= 0; i--) {
        for (node = D_004470C8[i]; node != NULL; node = node->next) {
            if (node->unk32 != -1 && node->unk32 != owner) {
                continue;
            }
            sceVu0SubVector(delta, &model->position, node);
            dist = sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
            if (dist - model->radius <= node->duration) {
                model->hit_handles[model->hit_count++] = node->handle;
                if (model->hit_count >= 4) {
                    return;
                }
            }
        }
    }
}
