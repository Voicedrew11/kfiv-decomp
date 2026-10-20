#include "map_object/map_object.h"
#include "model/model.h"
#include "core/file.h"
#include <libvu0.h>
#include <math.h>
#include "math/camera.h"
#include "math/vu0.h"
#include "model/model_matrix.h"
#include "player/player.h"

/* MapObject.step of a distance fade (kind 5) */
#define DISTANCE_FADE_STEP_ACTIVE   1       /* the alpha follows the distance to the player */

/* Start handler of the kind that fades with the distance to the player: takes
   the two distances and the two alphas from its source and starts invisible. */
void MapObject_StartDistanceFade(MapObject *object) {
    MapObjectWorkFade *work = (MapObjectWorkFade *)object->work;
    MapObjectUnk08Words *source;

    if (object->model->model_flags & 1) {
        MapObject_StartAnim(object, MAP_OBJECT_ANIM_OFF);
    }
    source = MAP_OBJECT_WORDS(object);
    ((MapObjectWorkFade *)object->work)->near_distance = source->kind_param_0;
    work->far_distance = source->kind_param_1;
    work->near_alpha = source->kind_param_2;
    work->far_alpha = source->kind_param_3;
    object->model->alpha = 0.0f;
    MAP_OBJECT_BYTES(object)->step = DISTANCE_FADE_STEP_ACTIVE;
}

/* Update handler of the fading kind: the alpha goes from near_alpha to far_alpha
   as the distance to the player goes from near_distance to far_distance. */
void MapObject_UpdateDistanceFade(MapObject *object) {
    MapObjectWorkFade *work = (MapObjectWorkFade *)object->work;
    Vector4 delta;
    f32 distance;

    if (object->model->model_flags & 1) {
        MapObject_StartAnim(object, MAP_OBJECT_ANIM_OFF);
        MapObject_StepAnim(object);
    }
    if (work->near_alpha == 0.0f && work->far_alpha == 0.0f) {
        object->model->alpha = 127.0f;
        return;
    }
    Vector_Subtract(&delta, &object->model->position, &player_state.position);
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
   each part is put where it sits in the world and turned toward camera_state. */
void MapObject_RebuildBillboardMatrices(MapObject *object) {
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
    ModelMatrix_Build(object->model, &object->model->position, &object->model->rotation, &object->model->scale_x);
    part_count = object->model->part_count;
    part = object->model->parts;
    for (; i < part_count; i++) {
        ModelMatrix_BuildPartLocal(object->model, part, part);
        part++;
    }
    if (object->model->model_flags & 1) {
        part = object->model->parts;
        for (i = 0; i < part_count; i++) {
            Model_MultiplyChildMatrices(part);
            part++;
        }
    }
    i = 0;
    sceVu0CopyMatrix(object->model->collision_matrix, object->model->model_matrix);
    part = object->model->parts;
    for (; i < part_count; i++) {
        Matrix_Multiply(matrix, object->model->model_matrix, part);
        Vector_ApplyMatrix(&world, matrix, &unit);
        Vector_Subtract(&delta, &camera_state, &world);
        pitch = atan2f(delta.y, sqrtf(delta.x * delta.x + delta.z * delta.z));
        yaw = atan2f(-delta.x, -delta.z);
        part->position[0] = world.x;
        part->position[1] = world.y;
        part->position[2] = world.z;
        part->position[3] = 1.0f;
        part->rotation_x = pitch;
        part->rotation_y = yaw;
        part->rotation_z = 0.0f;
        part->rotation_w = 1.0f;
        Vector_MultiplyComponents(part->scale, part->scale, &object->model->scale_x);
        part++;
    }
    part = object->model->parts;
    for (i = 0; i < part_count; i++) {
        ModelMatrix_BuildPartLocal(object->model, part, part);
        part++;
    }
    sceVu0CopyMatrix(object->model->model_matrix, scale_matrices);
    object->model->update_bone_points(object->model);
}
