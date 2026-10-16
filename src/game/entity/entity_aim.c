/* Where an entity goes and looks: random points near a centre, the matrices and angles of a model's parts
   (bones) for looking at a point, and whether something stands between an entity and the player. */
#include "entity/entity_aim.h"
#include "entity/entity_collision.h"
#include "player/player.h"
#include "collision/collision.h"
#include "map/map.h"

int rand(void);
extern EntityRangeWeights near_point_range_weights;
float sinf(float);
float cosf(float);
void Vector_Add(void *, void *, void *);
extern PlayerState player_state;
extern f32 identity_matrix[16];
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(float *, int *, float *);
void Vector_Scale(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void Matrix_Multiply(void *, void *, void *);
void sceVu0CopyMatrix(void *, void *);
void Vector_Subtract(void *, void *, void *);
void Vector_Normalize(void *, void *);
void Vector_Cross(void *, void *, void *);
void sceVu0TransposeMatrix(void *, void *);
double sqrt(double);
float atan2f(float, float);
void Vector_ApplyMatrix(void *, void *, void *);
float sqrtf(float);
extern int current_map_slot;
extern s32 map_objects_started[2];
extern s32 map_object_counts[2];
extern MapObject map_slot_objects[];
extern MapObject map_slot1_objects[];
void Vector_Copy(void *, void *);
void Collision_HitRayObj(MapObject *, f32 *, f32 *, f32);
extern MapSlot map_slots[];
void Attack_RollPlayerAilments(void *);

/* EntityMath_WrapAngle, which the game inlines here: wraps an angle to +-pi */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return angle;
}

/* the objects of the current map (map 0) or the other loaded one (map 1), which the game inlines */
static inline MapObject *map_objects(s32 map) {
    if (map_objects_started[current_map_slot == map ? 0 : 1] != 0) {
        if (current_map_slot != map) {
            return map_slot1_objects;
        }
        return map_slot_objects;
    }
    return NULL;
}

static inline s32 map_object_count(s32 map) {
    if (map_objects_started[current_map_slot == map ? 0 : 1] != 0) {
        return map_object_counts[current_map_slot == map ? 0 : 1];
    }
    return 0;
}

static s32 Bone_ClampDirection(f32 *direction, f32 *limits, f32 heading);

/* Picks a point as EntityPoint_PickNear, then moves its height by up to +-height_range. */
void EntityPoint_PickNearWithHeight(f32 *point, f32 range, f32 height_range, void *centre, void *position) {
    s32 offset;

    EntityPoint_PickNear(range, point, centre, position);
    offset = (s32)(height_range + height_range);
    offset = rand() % offset - offset / 2;
    point[1] += offset;
}

/* Picks a random point within `range` of `centre`. The nearer half of the range is twice as likely while
   `position` is in the outer half, and the other way round. */
void EntityPoint_PickNear(float range, void *point_arg, void *centre, void *position) {
    EntityRangeWeights table = near_point_range_weights;
    f32 *point = point_arg;
    f32 distance_out[4];
    f32 angle;
    s32 total;
    s32 roll;
    s32 i;

    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xz $vf16, $vf16, $vf16\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(position), "r"(centre), "r"(distance_out) : "memory");
    if ((s32)(distance_out[0] * 100.0f / range) > 50) {
        for (i = 0; i < 5; i++) {
            table.weights[i + 5] *= 2;
        }
    } else {
        for (i = 0; i < 5; i++) {
            table.weights[i] *= 2;
        }
    }
    total = 0;
    for (i = 0; i < 10; i++) {
        total += table.weights[i];
    }
    roll = rand() % total;
    for (i = 0; i < 10; i++) {
        if (roll < table.weights[i]) {
            break;
        }
    }
    range *= (10.0f - i) / 10.0f - (rand() % 10) / 10.0f;
    angle = (rand() % 360) * 0.017453292f;
    if (angle >= 3.1415927f) {
        angle -= 6.2831855f;
    }
    point[0] = -sinf(angle) * range;
    point[1] = 0.0f;
    point[2] = -cosf(angle) * range;
    Vector_Add(point, point, centre);
}

/* EntityPoint_PickNear for a caller that already knows how far from `centre` it is. */
void EntityPoint_PickNearFromDistance(f32 *point, f32 *centre, f32 distance, f32 range) {
    EntityRangeWeights table = near_point_range_weights;
    f32 angle;
    s32 total;
    s32 roll;
    s32 i;

    if ((s32)(distance * 100.0f / range) > 50) {
        for (i = 0; i < 5; i++) {
            table.weights[i + 5] *= 2;
        }
    } else {
        for (i = 0; i < 5; i++) {
            table.weights[i] *= 2;
        }
    }
    total = 0;
    for (i = 0; i < 10; i++) {
        total += table.weights[i];
    }
    roll = rand() % total;
    for (i = 0; i < 10; i++) {
        if (roll < table.weights[i]) {
            break;
        }
    }
    range *= (10.0f - i) / 10.0f - (rand() % 10) / 10.0f;
    angle = (rand() % 360) * 0.017453292f;
    if (angle >= 3.1415927f) {
        angle -= 6.2831855f;
    }
    point[0] = -sinf(angle) * range;
    point[1] = 0.0f;
    point[2] = -cosf(angle) * range;
    Vector_Add(point, point, centre);
}

/* Where the player will be after `frames` frames at his present speed: his position plus his sideways,
   falling and walking speeds (unk1B0, unk1AC, unk1B4), turned by his heading, times frames. Nothing calls it. */
void EntityAim_PredictPlayerPosition(f32 *point, s32 frames) {
    f32 offset[4];

    offset[0] = player_state.strafe_speed;
    offset[1] = player_state.fall_speed;
    offset[2] = player_state.walk_speed;
    offset[3] = 0.0f;
    sceVu0RotMatrixY(MODEL_MATRIX_SCRATCH, identity_matrix, player_state.rotation.y);
    sceVu0ApplyMatrix(offset, (int *)MODEL_MATRIX_SCRATCH, offset);
    Vector_Scale(offset, offset, frames);
    Vector_Add(point, &player_state, offset);
}

/* Looks for `target` in the tree of parts under `part` and multiplies `matrix` by the transforms on the way
   to it; FALSE: not found. */
s32 Bone_MultiplyPathTo(void *part_arg, void *target_arg, void *matrix) {
    ModelPart *part = part_arg;
    ModelPart *target = target_arg;
    f32 local[16];
    s32 found;

    if (part == target) {
        sceVu0TransMatrix(local, identity_matrix, target->position);
        Matrix_Multiply(matrix, matrix, local);
        return TRUE;
    }
    if (part->child != NULL && Bone_MultiplyPathTo(part->child, target, matrix) != 0) {
        if ((s32)(MODEL_PART_WORD(part) >> 32) & 0x7F) {
            sceVu0RotMatrixX(local, identity_matrix, wrap_angle(part->rotation_x));
            sceVu0RotMatrixZ(local, local, wrap_angle(part->rotation_z));
            sceVu0RotMatrixY(local, local, wrap_angle(part->rotation_y));
            sceVu0TransMatrix(local, local, part->position);
            Matrix_Multiply(matrix, local, matrix);
        }
        return TRUE;
    }
    if (part->sibling != NULL) {
        found = Bone_MultiplyPathTo(part->sibling, target, matrix);
        if (found != 0) {
            return TRUE;
        }
    }
    return FALSE;
}

/* The matrix of a part of the model in the world (the parts' transforms, then the model's); FALSE: the model
   has no such part. Nothing calls it. */
s32 Bone_GetMatrix(ModelInstance *model, void *bone, f32 *matrix) {
    sceVu0CopyMatrix(matrix, identity_matrix);
    if (Bone_MultiplyPathTo(model->parts, bone, matrix) == 0) {
        return FALSE;
    }
    Matrix_Multiply(matrix, model->model_matrix, matrix);
    return TRUE;
}

/* Builds the rotation matrix that looks from `origin` towards `target`; `up_sign` below 0 takes "up" as down,
   `side_sign` below 0 mirrors the sideways axis. Nothing calls it. */
void Bone_LookAtMatrix(f32 *target, f32 *origin, s32 up_sign, f32 *matrix, f32 side_sign) {
    f32 vector[4];
    f32 *forward = &matrix[8];
    f32 *side;

    sceVu0CopyMatrix(matrix, identity_matrix);
    Vector_Subtract(vector, target, origin);
    Vector_Normalize(forward, vector);
    // match: the whole vector is set in both arms
    if (up_sign >= 0) {
        vector[0] = 0.0f;
        vector[1] = 1.0f;
        vector[2] = 0.0f;
        vector[3] = 0.0f;
    } else {
        vector[0] = 0.0f;
        vector[1] = -1.0f;
        vector[2] = 0.0f;
        vector[3] = 0.0f;
    }
    Vector_Cross(matrix, vector, forward);
    side = &matrix[4];
    Vector_Normalize(matrix, matrix);
    Vector_Cross(side, forward, matrix);
    Vector_Normalize(side, side);
    if (side_sign < 0.0f) {
        Vector_Scale(matrix, matrix, -1.0f);
    }
    matrix[3] = matrix[7] = matrix[11] = 0.0f;
}

/* The angles that turn a part of the model to look at `target` (kept within `limits`, if given, of the
   model's heading); FALSE: the model has no such part. */
s32 Bone_LookAngles(ModelInstance *model, void *part, f32 *target, f32 *limits, f32 *angles) {
    f32 vector[4];
    f32 look[16];
    f32 part_matrix[16];

    sceVu0CopyMatrix(part_matrix, identity_matrix);
    if (Bone_MultiplyPathTo(model->parts, part, part_matrix) == 0) {
        return FALSE;
    }
    Matrix_Multiply(part_matrix, model->model_matrix, part_matrix);
    sceVu0CopyMatrix(look, identity_matrix);
    Vector_Subtract(vector, &part_matrix[12], target);
    if (limits != NULL) {
        Bone_ClampDirection(vector, limits, model->rotation.y);
    }
    Vector_Normalize(&look[8], vector);
    vector[0] = 0.0f;
    vector[1] = 1.0f;
    vector[2] = 0.0f;
    vector[3] = 0.0f;
    Vector_Cross(look, vector, &look[8]);
    Vector_Normalize(look, look);
    Vector_Cross(&look[4], &look[8], look);
    Vector_Normalize(&look[4], &look[4]);
    if (model->scale_x < 0.0f) {
        Vector_Scale(look, look, -1.0f);
    }
    look[3] = look[7] = look[11] = 0.0f;
    part_matrix[14] = part_matrix[13] = part_matrix[12] = 0.0f;
    part_matrix[15] = 1.0f;
    sceVu0TransposeMatrix(part_matrix, part_matrix);
    Matrix_Multiply(look, part_matrix, look);
    Bone_MatrixToAngles(angles, look);
    return TRUE;
}

/* Keeps a direction within pitch limits[0]..limits[1] and yaw limits[2]..limits[3] of `heading`; TRUE: it had
   to be changed. */
static s32 Bone_ClampDirection(f32 *direction, f32 *limits, f32 heading) {
    f32 local[4];
    f32 matrix[16];
    f32 pitch;
    f32 yaw;
    f32 length;
    s32 clamped = FALSE;

    sceVu0RotMatrixY(matrix, identity_matrix, -heading);
    Vector_ApplyMatrix(local, matrix, direction);
    length = sqrt(local[0] * local[0] + local[2] * local[2]);
    pitch = atan2f(-local[1], length);
    yaw = atan2f(local[0], local[2]);
    if (pitch < limits[0]) {
        pitch = limits[0];
        clamped = TRUE;
    }
    if (limits[1] < pitch) {
        pitch = limits[1];
        clamped = TRUE;
    }
    if (yaw < limits[2]) {
        yaw = limits[2];
        clamped = TRUE;
    }
    if (limits[3] < yaw) {
        yaw = limits[3];
        clamped = TRUE;
    }
    if (clamped) {
        sceVu0RotMatrixX(matrix, identity_matrix, pitch);
        sceVu0RotMatrixY(matrix, matrix, yaw);
        sceVu0RotMatrixY(matrix, matrix, heading);
        length = sqrt(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
        local[0] = matrix[8];
        local[1] = matrix[9];
        local[2] = matrix[10];
        local[3] = matrix[11];
        Vector_Scale(direction, local, length);
    }
    return clamped;
}

/* Takes the three angles (x, y, z) out of a rotation matrix. */
void Bone_MatrixToAngles(void *angles_arg, void *matrix) {
    f32 *angles = angles_arg;
    f32 local[16];
    f32 axis[4];
    f32 turned[4];

    sceVu0CopyMatrix(local, matrix);
    local[12] = 0.0f;
    local[13] = 0.0f;
    local[14] = 0.0f;
    local[15] = 1.0f;
    axis[0] = 1.0f;
    axis[1] = 0.0f;
    axis[2] = 0.0f;
    axis[3] = 0.0f;
    Vector_ApplyMatrix(turned, local, axis);
    angles[1] = -atan2f(turned[2], turned[0]);
    angles[2] = atan2f(turned[1], sqrtf(turned[0] * turned[0] + turned[2] * turned[2]));
    sceVu0RotMatrixY(local, local, -angles[1]);
    sceVu0RotMatrixZ(local, local, -angles[2]);
    axis[2] = 1.0f;
    axis[0] = 0.0f;
    axis[1] = 0.0f;
    axis[3] = 0.0f;
    Vector_ApplyMatrix(turned, local, axis);
    angles[0] = atan2f(-turned[1], turned[2]);
    if (local[5] < 0.0f) {
        angles[0] = 3.1415927f - angles[0];
    }
    angles[0] = wrap_angle(angles[0]);
    angles[1] = wrap_angle(angles[1]);
    angles[2] = wrap_angle(angles[2]);
}

/* Whether a map object (kinds 1, 2, 6 and 0x11) stands between the entity's middle and the player's. */
s32 EntitySight_ObjectInWay(Entity *entity) {
    f32 target[4];
    f32 position[4];
    f32 direction[4];
    f32 lowest[4];
    f32 highest[4];
    f32 distance_out[4];
    MapObject *object;
    f32 distance;
    f32 reach;
    s32 map;
    s32 count;
    s32 i;

    Vector_Copy(position, entity->model->position);
    position[1] -= entity->collision_height * 0.5f;
    Vector_Copy(target, &player_state);
    target[1] -= 115.0f;
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xyz $vf16, $vf16, $vf16\n"
        "vaddy.x $vf16, $vf16, $vf16y\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(target), "r"(position), "r"(distance_out) : "memory");
    distance = distance_out[0];
    Vector_Subtract(direction, target, position);
    Vector_Normalize(direction, direction);
    if (target[0] < position[0]) {
        lowest[0] = target[0];
        highest[0] = position[0];
    } else {
        lowest[0] = position[0];
        highest[0] = target[0];
    }
    if (target[1] < position[1]) {
        lowest[1] = target[1];
        highest[1] = position[1];
    } else {
        lowest[1] = position[1];
        highest[1] = target[1];
    }
    if (target[2] < position[2]) {
        lowest[2] = target[2];
        highest[2] = position[2];
    } else {
        lowest[2] = position[2];
        highest[2] = target[2];
    }
    for (map = 0; map < 2; map++) {
        object = map_objects(map);
        count = map_object_count(map);
        for (i = 0; i < count; i++, object++) {
            if (object->definition->kind == 1 || object->definition->kind == 2 || object->definition->kind == 0x11 || object->definition->kind == 6) {
                reach = object->reach_radius;
                if (reach <= 0.0f) {
                    continue;
                }
                if (object->model->position.y < lowest[1]) {
                    continue;
                }
                if (object->model->position.x + reach < lowest[0]) {
                    continue;
                }
                if (highest[0] < object->model->position.x - reach) {
                    continue;
                }
                if (object->model->position.z + reach < lowest[2]) {
                    continue;
                }
                if (highest[2] < object->model->position.z - reach) {
                    continue;
                }
                Collision_HitRayObj(object, position, direction, distance);
                if (COLLISION_RESULT->hit != 0) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

/* Whether another living entity stands on the line from the player to this one (within its radius). */
s32 EntitySight_EntityInWay(Entity *entity) {
    Entity *other;
    s64 flags;

    other = active_entities_head;
    while (other != NULL) {
        flags = other->flags;
        if (((s32)(flags >> 1) & 1) && ((s32)(flags >> 2) & 3) < 2 && other != entity) {
            if (EntityCollision_PointNearSegment(other->radius, (int *)&player_state, (int)entity->model->position, (int)other->model->position)) {
                return TRUE;
            }
        }
        other = other->next;
    }
    return FALSE;
}

/* Whether a prop stands between the entity and the player: tests the line from the entity's middle to the
   player's against the entity's own map record and the player's; returns the strip list hit. */
s32 EntitySight_PropInWay(Entity *entity) {
    f32 position[4];
    f32 target[4];
    f32 distance_out[4];
    MapRecord *player_record;
    f32 distance;
    s32 hit;

    // match: index first, or the two address registers swap
    player_record = &((MapRecord *)(current_map_slot + map_slots)->records)[player_state.map_index];
    Vector_Copy(position, entity->model->position);
    position[1] -= entity->collision_height * 0.5f;
    Vector_Copy(target, &player_state);
    target[1] -= 115.0f;
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xyz $vf16, $vf16, $vf16\n"
        "vaddy.x $vf16, $vf16, $vf16y\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(target), "r"(position), "r"(distance_out) : "memory");
    distance = distance_out[0];
    Vector_Subtract(target, target, position);
    Vector_Normalize(target, target);
    if (entity->map_record != player_record && (((EntityParamsWord *)entity->params)->unk00 & 0x6000000) != 0x4000000) {
        hit = EntityCollision_RayProp(entity->map_record, position, target, distance, 0);
        if (hit != 0) {
            return hit;
        }
    }
    return EntityCollision_RayProp(player_record, position, target, distance, 0);
}

/* Whether a prop's wall stands between the entity's middle and the player's
   (EntityCollision_RayHitsPropWall). Nothing calls it. */
s32 EntitySight_WallInWay(Entity *entity) {
    f32 position[4];
    f32 target[4];
    f32 bounds[4];
    f32 distance;

    Vector_Copy(position, entity->model->position);
    position[1] -= entity->collision_height * 0.5f;
    Vector_Copy(target, &player_state);
    target[1] -= 115.0f;
    if (position[0] < target[0]) {
        bounds[0] = position[0];
        bounds[1] = target[0];
    } else {
        bounds[0] = target[0];
        bounds[1] = position[0];
    }
    if (position[2] < target[2]) {
        bounds[2] = position[2];
        bounds[3] = target[2];
    } else {
        bounds[2] = target[2];
        bounds[3] = position[2];
    }
    Vector_Subtract(target, target, position);
    distance = sqrt(target[0] * target[0] + target[1] * target[1] + target[2] * target[2]);
    Vector_Normalize(target, target);
    return EntityCollision_RayHitsPropWall(position, target, distance, bounds);
}

/* When the player is within `max_distance` of `point` (flat), in its height band and within `max_angle` of
   `heading`, gives the attack's ailment chances to the player (Attack_RollPlayerAilments) and returns TRUE. */
s32 EntityAim_AilPlayerInArc(void *target, f32 *point, f32 max_angle, f32 max_height, f32 max_distance, f32 heading) {
    f32 distance_out[4];
    f32 angle;

    if (point[1] < player_state.position.y - 230.0f) {
        return FALSE;
    }
    if (player_state.position.y < point[1] - max_height) {
        return FALSE;
    }
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xz $vf16, $vf16, $vf16\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%2)\n"
        : : "r"(point), "r"(&player_state), "r"(distance_out) : "memory");
    if (max_distance < distance_out[0]) {
        return FALSE;
    }
    angle = heading - atan2f(player_state.position.x - point[0], player_state.position.z - point[2]);
    angle = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    if (ABS(angle) > max_angle) {
        return FALSE;
    }
    Attack_RollPlayerAilments(target);
    return TRUE;
}
