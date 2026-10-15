#include "kf_map_object.h"
#include "kf_model.h"

void Vector_Subtract(void *, void *, void *);
float sqrtf(float);
void *memset(void *, int, u32);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void Model_MultiplyChildMatrices(ModelPart *);
void sceVu0CopyMatrix(void *, void *);
void Matrix_Multiply(void *, void *, void *);
void Vector_ApplyMatrix(void *, void *, void *);
float atan2f(float, float);
void Vector_MultiplyComponents(float *, float *, float *);
extern float player_state[];
extern float D_004325A0[];
extern f32 D_00431B30[4][4];

/* Start handler of the kind that fades with the distance to the player: takes
   the two distances and the two alphas from its source and starts invisible. */
void func_001201E0(MapObject *object) {
    MapObjectWorkFade *work = (MapObjectWorkFade *)object->work;
    MapObjectUnk08Words *source;

    if (object->model->unk100 & 1) {
        MapObject_StartAnim(object, 4);
    }
    source = MAP_OBJECT_WORDS(object);
    ((MapObjectWorkFade *)object->work)->near_distance = source->unk44;
    work->far_distance = source->unk46;
    work->near_alpha = source->unk48;
    work->far_alpha = source->unk4A;
    object->model->alpha = 0.0f;
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Update handler of the fading kind: the alpha goes from near_alpha to far_alpha
   as the distance to the player goes from near_distance to far_distance. */
void func_00120278(MapObject *object) {
    MapObjectWorkFade *work = (MapObjectWorkFade *)object->work;
    Vector4 delta;
    f32 distance;

    if (object->model->unk100 & 1) {
        MapObject_StartAnim(object, 4);
        MapObject_StepAnim(object);
    }
    if (work->near_alpha == 0.0f && work->far_alpha == 0.0f) {
        object->model->alpha = 127.0f;
        return;
    }
    Vector_Subtract(&delta, &object->model->position, player_state);
    distance = sqrtf(delta.x * delta.x + delta.z * delta.z);
    if (distance < work->near_distance) {
        object->model->alpha = work->near_alpha;
    } else if (work->far_distance < distance) {
        object->model->alpha = work->far_alpha;
    } else {
        object->model->alpha = work->near_alpha + (work->far_alpha - work->near_alpha) * (distance - work->near_distance) / (work->far_distance - work->near_distance);
    }
    if (object->model->alpha < 0.0f) {
        object->model->alpha = 0.0f;
    } else if (object->model->alpha > 255.0f) {
        object->model->alpha = 255.0f;
    }
}

/* Rebuilds the model matrices of an object whose parts always face the camera:
   each part is put where it sits in the world and turned toward D_004325A0. */
void func_001203F0(MapObject *object) {
    f32 matrix[16];
    Vector4 world;
    Vector4 delta;
    Vector4 unit;
    int i = 0;
    ModelPart *part;
    int part_count;
    f32 pitch;
    f32 yaw;

    memset(&unit, 0, sizeof(unit));
    unit.w = 1.0f;
    ModelMatrix_Build(object->model, &object->model->position, &object->model->rotation, &object->model->unkE0);
    part_count = object->model->part_count;
    part = object->model->parts;
    for (; i < part_count; i++) {
        ModelMatrix_BuildPartLocal(object->model, part, part);
        part++;
    }
    if (object->model->unk100 & 1) {
        part = object->model->parts;
        for (i = 0; i < part_count; i++) {
            Model_MultiplyChildMatrices(part);
            part++;
        }
    }
    i = 0;
    sceVu0CopyMatrix(object->model->unk80, object->model->unk40);
    part = object->model->parts;
    for (; i < part_count; i++) {
        Matrix_Multiply(matrix, object->model->unk40, part);
        Vector_ApplyMatrix(&world, matrix, &unit);
        Vector_Subtract(&delta, D_004325A0, &world);
        pitch = atan2f(delta.y, sqrtf(delta.x * delta.x + delta.z * delta.z));
        yaw = atan2f(-delta.x, -delta.z);
        part->unk40[0] = world.x;
        part->unk40[1] = world.y;
        part->unk40[2] = world.z;
        part->unk40[3] = 1.0f;
        part->unk50 = pitch;
        part->unk54 = yaw;
        part->unk58 = 0.0f;
        part->unk5C = 1.0f;
        Vector_MultiplyComponents(part->unk60, part->unk60, &object->model->unkE0);
        part++;
    }
    part = object->model->parts;
    for (i = 0; i < part_count; i++) {
        ModelMatrix_BuildPartLocal(object->model, part, part);
        part++;
    }
    sceVu0CopyMatrix(object->model->unk40, D_00431B30);
    object->model->unk1B4(object->model);
}
