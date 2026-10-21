/* Entity collision: entities kept out of the player and of each other, walking or flying entities against the
   map pieces and map objects, and the sphere, capsule and ray tests and map queries these use. The results go
   to COLLISION_RESULT, as with the tests of collision/collision.c. */
#include "entity/entity_collision.h"
#include "entity/entity_math.h"
#include "map/map.h"
#include "collision/collision.h"
#include "enemy/enemy.h"
#include <libvu0.h>
#include <math.h>
#include "math/vu0.h"
#include <stdlib.h>
#include "map_object/map_object.h"
#include "player/player.h"

#define VERTICAL_FACE_NORMAL_Y_EPSILON 0.00001f
#define EDGE_PERPENDICULAR_TO_FLAT_EPSILON 0.000001f
/* the motion in whose first two frames a box pair is one ring of points */
#define BOX_PAIR_ONE_RING_MOTION 0


/* tests the entity's upright capsule against the triangle at `point` and pushes `position`
   sideways out of it */
#define PUSH_OUT_OF_WALL()                                                                         \
    Collision_CapsuleTriangle(point[0].position, point[1].position, point[2].position, position,   \
                              entity->collision_radius, entity->collision_height);                 \
    if (COLLISION_RESULT->hit != 0) {                                                              \
        ENTITY_FLAG_BITS(entity)->wall_push = TRUE;                                                    \
        offset[0] = position[0] - COLLISION_RESULT->point[0];                                      \
        offset[1] = 0.0f;                                                                          \
        offset[2] = position[2] - COLLISION_RESULT->point[2];                                      \
        sceVu0Normalize(offset, offset);                                                           \
        Vector_Add(normal_sum, normal_sum, offset);                                                \
        Vector_Scale(offset, offset, entity->collision_radius);                                    \
        position[0] = COLLISION_RESULT->point[0] + offset[0];                                      \
        position[2] = COLLISION_RESULT->point[2] + offset[2];                                      \
    }

/* loads one edge of the triangle: its two ends, and start minus end into both `edge` and `flat` */
#define LOAD_EDGE(from, to)                                                                        \
    __asm__ volatile(                                                                              \
        "lqc2 $vf16, 0(%0)\n"                                                                      \
        "lqc2 $vf17, 0(%1)\n"                                                                      \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vsub.xyzw $vf18, $vf16, $vf17\n"                                                          \
        "sqc2 $vf16, 0(%2)\n"                                                                      \
        "sqc2 $vf17, 0(%3)\n"                                                                      \
        "vnop\n"                                                                                   \
        "sqc2 $vf18, 0(%4)\n"                                                                      \
        "sqc2 $vf18, 0(%5)\n"                                                                      \
        :                                                                                          \
        : "r"(from), "r"(to), "r"(start), "r"(end), "r"(edge), "r"(flat))

/* records a hit at `from`. The result is written through constant addresses (*&), not through
   a shared base register. */
#define KEEP_NEAREST(distance, from)                                                               \
    *&COLLISION_RESULT->hit = 1;                                                                   \
    nearest_sq = (distance);                                                                       \
    *&COLLISION_RESULT->point[0] = (from)[0];                                                      \
    *&COLLISION_RESULT->point[1] = (from)[1];                                                      \
    *&COLLISION_RESULT->point[2] = (from)[2]

/* the objects of the current map (map 0) or the other loaded one (map 1), which the game inlines */
static inline MapObject *map_objects(s32 map) {
    if (map_objects_started[current_map_slot == map ? 0 : 1] != 0) {
        if (current_map_slot != map) {
            return map_slot1_objects;
        }
        return map_slot_objects[0];
    }
    return NULL;
}

static inline s32 map_object_count(s32 map) {
    if (map_objects_started[current_map_slot == map ? 0 : 1] != 0) {
        return map_object_counts[current_map_slot == map ? 0 : 1];
    }
    return 0;
}

#define PUSH_CENTRE(amount)                                                                     centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * (amount);            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * (amount)

static void EntityCollision_CylinderCylinder(f32 *position, f32 *other, f32 radius, f32 height, f32 other_radius, f32 other_height);
static void EntityCollision_CylinderSphere(f32 *position, f32 *other, f32 radius, f32 height, f32 other_radius);
static void EntityCollision_SphereCylinder(f32 *position, f32 *other, f32 radius, f32 other_radius, f32 other_height);
static void EntityCollision_SphereSphere(f32 *center_a, f32 *center_b, f32 radius_a, f32 radius_b);
static void EntityCollision_SphereSegment(void *start, void *end, void *centre, f32 radius);
static void EntityCollision_SpherePoint(void *point, void *centre, f32 radius);
static void EntityCollision_SphereTriangle(void *corner_a, void *corner_b, void *corner_c, void *centre, f32 radius);
static void EntityCollision_RayTriangle(void *corner_a, void *corner_b, void *corner_c, void *origin, void *direction, f32 length);
static void EntityCollision_PushOutOfBox(CollisionOwner *owner, f32 *centre, f32 radius, f32 push);
static void EntityCollision_PushOutOfBoxPair(CollisionOwner *owner, f32 *centre, f32 radius, f32 push);
static void EntityCollision_PushOutOfQuad(CollisionOwner *owner, f32 *centre, f32 radius, f32 push);
static void EntityCollision_MoveOnMesh(CollisionOwner *owner, f32 *centre, f32 step, f32 radius);
static void EntityCollision_SphereBox(CollisionOwner *owner, void *centre, f32 radius);
static void EntityCollision_SphereBoxPair(CollisionOwner *owner, void *centre, f32 radius);
static void EntityCollision_SphereQuad(CollisionOwner *owner, void *centre, f32 radius);
static void EntityCollision_SphereQuads(CollisionOwner *owner, void *centre, f32 radius);
static void EntityCollision_SphereMesh(CollisionOwner *owner, void *centre, f32 radius);

/* Keeps a living entity out of the player (a cylinder 100 wide and 230 high) and out of the other entities
   that collide (EntityParams.flags 0x10), as cylinders or spheres by EntityParams bit 27; sets flag bits
   48-50 and leaves the summed push in Entity.entity_push. Spawn types 0x14-0x16, 0x2D, 0x63 and 0x97 are not kept
   out of the player. */
void EntityCollision_PushApart(Entity *entity) {
    f32 push[4];
    Entity *other;
    s32 type_id;

    type_id = entity->spawn_info->type_id;
    if (!(type_id >= 0x14 && type_id <= 0x16) && (u16)type_id != 0x2D && (u16)type_id != 0x63 && (u16)type_id != 0x97
        && (ENTITY_FLAG_BITS(entity)->live) && entity->hit_points != 0 && !(entity->flags & (ENTITY_FLAG_NOT_TARGETABLE | ENTITY_FLAG_NO_PLAYER_COLLISION))) {
        if (ENTITY_PARAMS_IS_SPHERE(entity->params)) {
            EntityCollision_SphereCylinder(entity->model->position, (f32 *)&player_state.position, entity->radius, 100.0f, 230.0f);
        } else {
            EntityCollision_CylinderCylinder(entity->model->position, (f32 *)&player_state.position, entity->radius, entity->height, 100.0f, 230.0f);
        }
        if (COLLISION_RESULT->hit != 0) {
            entity->model->position[0] = COLLISION_RESULT->point[0];
            entity->model->position[1] = COLLISION_RESULT->point[1];
            entity->model->position[2] = COLLISION_RESULT->point[2];
            ENTITY_FLAG_BITS(entity)->player_push = TRUE;
            if (COLLISION_RESULT->hit == 2) {
                ENTITY_FLAG_BITS(entity)->on_top = TRUE;
            }
        }
    }
    push[0] = 0.0f;
    push[1] = 0.0f;
    push[2] = 0.0f;
    push[3] = 0.0f;
    if (!(ENTITY_FLAG_BITS(entity)->no_entity_collision)) {
        for (other = active_entities_head; other != NULL; other = other->next) {
            if (!(ENTITY_FLAG_BITS(other)->live)) {
                continue;
            }
            if (ENTITY_FLAG_BITS(other)->dying >= 2) {
                continue;
            }
            if (other == entity) {
                continue;
            }
            if (other->params->flags & ENTITY_PARAMS_FLAG_COLLIDES_WITH_ENTITIES) {
                if (ENTITY_PARAMS_IS_SPHERE(entity->params)) {
                    if (ENTITY_PARAMS_IS_SPHERE(other->params)) {
                        EntityCollision_SphereSphere(entity->model->position, other->model->position, entity->radius, other->radius);
                    } else {
                        EntityCollision_SphereCylinder(entity->model->position, other->model->position, entity->radius, other->radius,
                                      other->height);
                    }
                } else {
                    if (ENTITY_PARAMS_IS_SPHERE(other->params)) {
                        EntityCollision_CylinderSphere(entity->model->position, other->model->position, entity->radius, entity->height,
                                      other->radius);
                    } else {
                        EntityCollision_CylinderCylinder(entity->model->position, other->model->position, entity->radius, entity->height,
                                      other->radius, other->height);
                    }
                }
                if (COLLISION_RESULT->hit != 0) {
                    entity->model->position[0] = COLLISION_RESULT->point[0];
                    entity->model->position[1] = COLLISION_RESULT->point[1];
                    entity->model->position[2] = COLLISION_RESULT->point[2];
                    push[0] += COLLISION_RESULT->normal[0];
                    push[1] += COLLISION_RESULT->normal[1];
                    push[2] += COLLISION_RESULT->normal[2];
                    ENTITY_FLAG_BITS(entity)->entity_push = TRUE;
                    if (COLLISION_RESULT->hit == 2) {
                        ENTITY_FLAG_BITS(entity)->on_top = TRUE;
                    }
                }
            }
        }
    }
    Vector_Normalize(push, push);
    entity->entity_push[0] = push[0];
    entity->entity_push[2] = entity->entity_push[1] = push[2];
}

/* Cylinder against cylinder: is `position` (a cylinder reaching `height` up from it) inside the other one,
   and where should it stand instead. The result's hit is 2 when it stands on the other. */
static void EntityCollision_CylinderCylinder(f32 *position, f32 *other, f32 radius, f32 height, f32 other_radius, f32 other_height) {
    CollisionResult *result = COLLISION_RESULT;
    f32 delta[4];
    f32 reach;
    f32 reach_squared;
    f32 distance_squared;
    f32 other_top;
    f32 top;
    f32 below;
    f32 above;
    f32 depth;

    reach = radius + other_radius;
    result->hit = 0;
    reach_squared = reach * reach;
    Vector_Subtract(delta, position, other);
    distance_squared = delta[0] * delta[0] + delta[2] * delta[2];
    if (reach_squared < distance_squared) {
        return;
    }
    other_top = other[1] - other_height;
    if (position[1] < other_top) {
        return;
    }
    top = position[1] - height;
    if (other[1] < top) {
        return;
    }
    below = position[1] - other_top;
    result->hit = 1;
    below = ABS(below);
    above = top - other[1];
    above = ABS(above);
    depth = reach - sqrt(distance_squared);
    if (below <= above && below <= depth) {
        result->normal[0] = result->normal[1] = result->normal[2] = 0.0f;
        result->hit = 2;
        result->point[0] = position[0];
        result->point[1] = other[1] - other_height;
        result->point[2] = position[2];
    } else if (above <= below && above <= depth) {
        result->hit = 0;
    } else {
        delta[1] = 0.0f;
        sceVu0Normalize(result->normal, delta);
        result->point[0] = other[0] + result->normal[0] * reach;
        result->point[1] = position[1];
        result->point[2] = other[2] + result->normal[2] * reach;
    }
}

/* Cylinder against sphere: is `position` (a cylinder reaching `height` up from it) inside the sphere at
   `other`, and where should it stand instead. */
static void EntityCollision_CylinderSphere(f32 *position, f32 *other, f32 radius, f32 height, f32 other_radius) {
    CollisionResult *result = COLLISION_RESULT;
    f32 delta[4];
    f32 nearest[4];
    f32 reach;
    f32 distance;
    f32 below;
    f32 above;
    f32 depth;
    f32 overlap;

    reach = radius + other_radius;
    result->hit = 0;
    Vector_Subtract(delta, position, other);
    distance = delta[0] * delta[0] + delta[2] * delta[2];
    if (reach * reach <= distance) {
        return;
    }
    if (position[1] < other[1] - other_radius) {
        return;
    }
    if (position[1] - height > other[1] + other_radius) {
        return;
    }
    result->hit = 1;
    distance = sqrt(distance);
    if ((position[1] - height <= other[1] && other[1] <= position[1]) || distance <= radius) {
        below = position[1] - (other[1] - other_radius);
        below = ABS(below);
        above = (position[1] - height) - (other[1] + other_radius);
        above = ABS(above);
        depth = reach - distance;
        if (below <= above && below <= depth) {
            result->normal[0] = result->normal[1] = result->normal[2] = 0.0f;
            result->hit = 2;
            result->point[0] = position[0];
            result->point[1] = other[1] - other_radius;
            result->point[2] = position[2];
        } else if (above <= below && above <= depth) {
            result->normal[0] = result->normal[1] = result->normal[2] = 0.0f;
            result->point[0] = position[0];
            result->point[1] = other[1] + other_radius + height;
            result->point[2] = position[2];
        } else {
            delta[1] = 0.0f;
            sceVu0Normalize(result->normal, delta);
            result->point[0] = other[0] + result->normal[0] * reach;
            result->point[1] = position[1];
            result->point[2] = other[2] + result->normal[2] * reach;
        }
    } else {
        delta[1] = 0.0f;
        sceVu0Normalize(nearest, delta);
        Vector_Scale(nearest, nearest, -radius);
        Vector_Add(nearest, nearest, position);
        if (other[1] <= position[1] - height * 0.5f) {
            nearest[1] -= height;
        }
        Vector_Subtract(delta, nearest, other);
        distance = sqrt(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
        if (other_radius <= distance) {
            result->hit = 0;
            return;
        }
        sceVu0Normalize(result->normal, delta);
        overlap = other_radius - distance;
        result->point[0] = position[0] + result->normal[0] * overlap;
        result->point[1] = position[1] + result->normal[1] * overlap;
        result->point[2] = position[2] + result->normal[2] * overlap;
    }
}

/* Sphere against cylinder: is the sphere at `position` inside the cylinder reaching `other_height` up from
   `other`, and where should it be instead. */
static void EntityCollision_SphereCylinder(f32 *position, f32 *other, f32 radius, f32 other_radius, f32 other_height) {
    CollisionResult *result = COLLISION_RESULT;
    f32 delta[4];
    f32 nearest[4];
    f32 reach;
    f32 reach_squared;
    f32 distance;
    f32 below;
    f32 above;
    f32 depth;

    reach_squared = radius + other_radius;
    result->hit = 0;
    Vector_Subtract(delta, position, other);
    reach = reach_squared;
    reach_squared *= reach_squared;
    distance = delta[0] * delta[0] + delta[2] * delta[2];
    if (reach_squared <= distance) {
        return;
    }
    if (position[1] + radius < other[1] - other_height) {
        return;
    }
    if (other[1] < position[1] - radius) {
        return;
    }
    result->hit = 1;
    distance = sqrt(distance);
    if ((other[1] - other_height <= position[1] && position[1] <= other[1]) || distance <= other_radius) {
        below = (position[1] + radius) - (other[1] - other_height);
        below = ABS(below);
        above = position[1] - radius - other[1];
        above = ABS(above);
        depth = reach - distance;
        if (below <= above && below <= depth) {
            result->normal[0] = result->normal[1] = result->normal[2] = 0.0f;
            result->hit = 2;
            result->point[0] = position[0];
            result->point[1] = other[1] - other_height - radius;
            result->point[2] = position[2];
        } else if (above <= below && above <= depth) {
            result->normal[0] = result->normal[1] = result->normal[2] = 0.0f;
            result->point[0] = position[0];
            result->point[1] = other[1] + radius;
            result->point[2] = position[2];
        } else {
            delta[1] = 0.0f;
            sceVu0Normalize(result->normal, delta);
            result->point[0] = other[0] + result->normal[0] * reach;
            result->point[1] = position[1];
            result->point[2] = other[2] + result->normal[2] * reach;
        }
    } else {
        delta[1] = 0.0f;
        sceVu0Normalize(nearest, delta);
        Vector_Scale(nearest, nearest, other_radius);
        Vector_Add(nearest, nearest, other);
        if (position[1] <= other[1] - other_height * 0.5f) {
            nearest[1] -= other_height;
        }
        reach_squared = radius * radius;
        Vector_Subtract(delta, position, nearest);
        distance = delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2];
        if (reach_squared <= distance) {
            result->hit = 0;
            return;
        }
        sceVu0Normalize(result->normal, delta);
        result->point[0] = nearest[0] + result->normal[0] * radius;
        result->point[1] = nearest[1] + result->normal[1] * radius;
        result->point[2] = nearest[2] + result->normal[2] * radius;
    }
}

/* Sphere against sphere: fills COLLISION_RESULT when the two overlap, with the point on the second one's
   surface. */
static void EntityCollision_SphereSphere(f32 *center_a, f32 *center_b, f32 radius_a, f32 radius_b) {
    float offset[4];
    float reach;
    float reach_squared;

    reach_squared = radius_a + radius_b;
    COLLISION_RESULT->hit = 0;
    Vector_Subtract(offset, center_a, center_b);
    reach = reach_squared;
    reach_squared *= reach_squared;
    if (!(reach_squared <= offset[0] * offset[0] + offset[1] * offset[1] + offset[2] * offset[2])) {
        COLLISION_RESULT->hit = 1;
        sceVu0Normalize(COLLISION_RESULT->normal, offset);
        COLLISION_RESULT->point[0] = center_b[0] + COLLISION_RESULT->normal[0] * reach;
        COLLISION_RESULT->point[1] = center_b[1] + COLLISION_RESULT->normal[1] * reach;
        COLLISION_RESULT->point[2] = center_b[2] + COLLISION_RESULT->normal[2] * reach;
    }
}

/* Applies the effect of a surface the entity touches; Collision_GetSurfaceEffect picks it from the triangle: 1 and 2 start
   the unk20 timer (1800 frames) by a chance EntityParams.ailment_defence_1 lowers, 3 starts it outright, 4 takes up to 50
   less a resistance term (EntityParams.magic_defence + unkDA) of Entity.hit_points (its health), 5 takes 30; 4 and 5 also
   clear flag bits 38-39 and set unkA3 to 3. */
void EntityCollision_ApplySurfaceEffect(Entity *entity, s32 material_first, s32 material_second, s32 material_third) {
    s32 chance;
    s32 amount;

    switch (Collision_GetSurfaceEffect(material_first, material_second, material_third)) {
    case GROUND_EFFECT_POISON_LOW_CHANCE:
        chance = 20 - entity->params->ailment_defence_1;
        if (chance <= 0) {
            return;
        }
        if (rand() % 100 >= chance) {
            return;
        }
        entity->ailment_1_frames = 1800;
        break;
    case GROUND_EFFECT_POISON_HIGH_CHANCE:
        chance = 50 - entity->params->ailment_defence_1;
        if (chance <= 0) {
            return;
        }
        if (rand() % 100 >= chance) {
            return;
        }
        entity->ailment_1_frames = 1800;
        break;
    case GROUND_EFFECT_POISON_CERTAIN:
        entity->ailment_1_frames = 1800;
        break;
    case GROUND_EFFECT_DAMAGE:
        chance = 50 - (200 - (entity->params->magic_defence + entity->params->resist_fire)) / 200;
        if (chance <= 0) {
            return;
        }
        if (entity->hit_points != 0) {
            amount = entity->hit_points;
            if (chance < amount) {
                amount = chance;
            }
            entity->hit_points -= amount;
            entity->flags &= ~ENTITY_FLAG_HIT_SOURCE_MASK;
            entity->hit_flash_frames = 3;
        }
        break;
    case GROUND_EFFECT_WEAR_DAMAGE:
        if (entity->hit_points != 0) {
            entity->hit_points = (entity->hit_points > 30) ? entity->hit_points - 30 : 0;
            entity->hit_flash_frames = 3;
            entity->flags &= ~ENTITY_FLAG_HIT_SOURCE_MASK;
        }
        break;
    }
}

/* Collides a walking entity with a model's collision mesh: applies the surface effect of the unk28 strips
   every 30 frames, pushes `position` out of the walls (adding their normals to `normal_sum`) and lowers
   `*floor_y` to the highest floor under it. Returns TRUE if it found a floor. */
s32 EntityCollision_WalkAgainstMesh(Entity *entity, ModelInstance *model, f32 *position, f32 *normal_sum, f32 *floor_y) {
    f32 offset[4];
    s32 hit_floor;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    u32 period;
    s32 i;
    s32 j;

    hit_floor = FALSE;
    Collision_TransformMeshPoints(model);
    period = 30;
    mesh = (CollisionMesh *)model->strips;
    if (frame_counter % period == entity->spawn_index % 10U) {
        strip = mesh->hazard_strips;
        i = mesh->hazard_strip_count;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                Collision_DropOnTriangle(point[0].position, point[1].position, point[2].position, entity->model->position);
                if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] <= entity->model->position[1]
                    && entity->model->position[1] - entity->collision_height <= COLLISION_RESULT->point[1]) {
                    EntityCollision_ApplySurfaceEffect(entity, point->material_first, point->material_second, point->material_third);
                    // match: the wall pass has to start right after the hazard loop nest, not after the flag test
                    goto walls;
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
    }
walls:
    if (!(ENTITY_FLAG_BITS(entity)->no_wall_collision)) {
        strip = mesh->wall_strips;
        i = mesh->wall_strip_count;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                PUSH_OUT_OF_WALL();
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
    }
    if (!(ENTITY_FLAG_BITS(entity)->no_second_wall_collision)) {
        strip = mesh->second_wall_strips;
        i = mesh->second_wall_strip_count;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                PUSH_OUT_OF_WALL();
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
    }
    if (entity->spawn_info->type_id != 0x12) {
        strip = mesh->barrier_strips;
        i = mesh->barrier_strip_count;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            if (Collision_GetBarrierLevel(point->material_first, point->material_second, point->material_third) >= entity->spawn_info->barrier_level) {
                if (Collision_IsBarrierFloor(point->material_first, point->material_second, point->material_third)) {
                    do {
                        Collision_DropOnTriangle(point[0].position, point[1].position, point[2].position, entity->model->position);
                        if (COLLISION_RESULT->hit != 0
                            && entity->model->position[1] - entity->params->floor_probe_depth <= COLLISION_RESULT->point[1]
                            && COLLISION_RESULT->point[1] <= *floor_y) {
                            *floor_y = COLLISION_RESULT->point[1];
                            hit_floor = TRUE;
                        }
                        point++;
                    } while (--j != 0);
                } else {
                    do {
                        PUSH_OUT_OF_WALL();
                        point++;
                    } while (--j != 0);
                }
                strip = (CollisionStrip *)(point + 2);
            } else {
                strip = (CollisionStrip *)(point + strip->point_count);
            }
        }
    }
    if (!(entity->flags & ENTITY_FLAG_SKIP_FLOOR_MASK)) {
        strip = mesh->floor_strips;
        i = mesh->floor_strip_count;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                Collision_DropOnTriangle(point[0].position, point[1].position, point[2].position, entity->model->position);
                if (COLLISION_RESULT->hit != 0
                    && entity->model->position[1] - entity->params->floor_probe_depth <= COLLISION_RESULT->point[1]
                    && COLLISION_RESULT->point[1] <= *floor_y) {
                    hit_floor = TRUE;
                    *floor_y = COLLISION_RESULT->point[1];
                    ENTITY_FLAG_BITS(entity)->floor_material = Collision_GetFloorMaterial(point->material_first, point->material_second, point->material_third);
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
    }
    return hit_floor;
}

/* Collides a walking entity with the map pieces whose box it is in (its own map's, then the other loaded
   map's): pushes it out of walls, updates its push (unkA4), and puts it on the floor, lets it fall, or stops
   it at a ledge. */
void EntityCollision_WalkAgainstMap(Entity *entity) {
    f32 normal_sum[4];
    f32 position[4];
    MapSlot *slots[2];
    void *unused[2];
    f32 floor_y;
    MapRecordBounds *record;
    f32 height;
    s32 hit_floor;
    s32 map;
    s32 i;

    hit_floor = FALSE;
    floor_y = FLT_MAX;
    height = entity->model->position[1];
    Vector_Copy(position, entity->model->position);
    normal_sum[0] = normal_sum[1] = normal_sum[2] = normal_sum[3] = 0.0f;
    position[1] -= 0.1f;
    if (!(ENTITY_FLAG_BITS(entity)->no_floor_lift)) {
        height += entity->floor_lift;
    }
    unused[0] = unused[1] = NULL;
    slots[0] = &map_slots[entity->group_id];
    slots[1] = &map_slots[(char)(entity->group_id ^ 1)];
    for (map = 0; map < 2; map++) {
        if (slots[map]->loaded != 0) {
            record = (MapRecordBounds *)slots[map]->records;
            for (i = 0; i < slots[map]->header->record_count; i++, record++) {
                if (record->record_flags & MAP_RECORD_HIDDEN) {
                    continue;
                }
                if (record->collision_mesh_index == 0xFFFF) {
                    continue;
                }
                if (record->model->strips == NULL) {
                    continue;
                }
                if (record->lowest[0] - entity->collision_radius > entity->model->position[0]) {
                    continue;
                }
                if (record->highest[0] + entity->collision_radius < entity->model->position[0]) {
                    continue;
                }
                if (record->lowest[1] - entity->collision_height > entity->model->position[1]) {
                    continue;
                }
                if (record->highest[1] + entity->collision_height < entity->model->position[1]) {
                    continue;
                }
                if (record->lowest[2] - entity->collision_radius > entity->model->position[2]) {
                    continue;
                }
                if (record->highest[2] + entity->collision_radius < entity->model->position[2]) {
                    continue;
                }
                if (EntityCollision_WalkAgainstMesh(entity, record->model, position, normal_sum, &floor_y)) {
                    entity->map_record = record;
                    hit_floor = TRUE;
                }
            }
        }
        if (hit_floor) {
            break;
        }
    }
    normal_sum[0] += entity->wall_push[0];
    normal_sum[1] += entity->wall_push[1];
    normal_sum[2] += entity->wall_push[2];
    sceVu0Normalize(normal_sum, normal_sum);
    entity->wall_push[0] = normal_sum[0];
    entity->wall_push[1] = normal_sum[1];
    entity->wall_push[2] = normal_sum[2];
    if (ENTITY_FLAG_BITS(entity)->on_top) {
        ENTITY_FLAG_BITS(entity)->on_floor = FALSE;
        ENTITY_FLAG_BITS(entity)->lift_up = FALSE;
        ENTITY_FLAG_BITS(entity)->ledge_stop = FALSE;
        entity->floor_lift = 10.0f;
        entity->model->position[0] = position[0];
        entity->model->position[2] = position[2];
        return;
    }
    if (ENTITY_FLAG_BITS(entity)->no_floor_lift) {
        ENTITY_FLAG_BITS(entity)->on_floor = FALSE;
        ENTITY_FLAG_BITS(entity)->lift_up = FALSE;
        ENTITY_FLAG_BITS(entity)->ledge_stop = FALSE;
        entity->floor_lift = 10.0f;
        if (hit_floor && floor_y <= entity->model->position[1]) {
            ENTITY_FLAG_BITS(entity)->on_floor = TRUE;
            entity->model->position[1] = floor_y;
        }
        entity->model->position[0] = position[0];
        entity->model->position[2] = position[2];
        return;
    }
    if (floor_y <= entity->model->position[1]) {
        ENTITY_FLAG_BITS(entity)->on_floor = TRUE;
        ENTITY_FLAG_BITS(entity)->lift_up = FALSE;
        ENTITY_FLAG_BITS(entity)->ledge_stop = FALSE;
        EntityMath_EaseToward(&entity->model->position[1], floor_y, 5.0f);
        entity->model->position[0] = position[0];
        entity->model->position[2] = position[2];
        return;
    }
    if (floor_y <= height) {
        ENTITY_FLAG_BITS(entity)->on_floor = TRUE;
        ENTITY_FLAG_BITS(entity)->lift_up = FALSE;
        ENTITY_FLAG_BITS(entity)->ledge_stop = FALSE;
        entity->model->position[1] = floor_y;
    } else if (ENTITY_FLAG_BITS(entity)->on_floor && entity->params->ledge_behavior != 0 && !(0.0f < entity->knockback_speed)
               && (entity->params->ledge_behavior != 1 || !(floor_y <= entity->model->position[1] + entity->collision_height))) {
        ENTITY_FLAG_BITS(entity)->on_floor = TRUE;
        ENTITY_FLAG_BITS(entity)->lift_up = FALSE;
        ENTITY_FLAG_BITS(entity)->ledge_stop = TRUE;
        entity->model->position[0] = entity->prev_position[0];
        entity->model->position[1] = entity->prev_position[1];
        entity->model->position[2] = entity->prev_position[2];
        return;
    } else {
        ENTITY_FLAG_BITS(entity)->on_floor = FALSE;
        ENTITY_FLAG_BITS(entity)->lift_up = TRUE;
        ENTITY_FLAG_BITS(entity)->ledge_stop = FALSE;
        entity->model->position[1] = height;
    }
    entity->model->position[0] = position[0];
    entity->model->position[2] = position[2];
}

/* Tests an upright capsule (a sphere of `radius` swept from `centre` down by `height`) against a triangle:
   its face at the top and bottom of the capsule first, then the nearest point of each edge. Nothing calls it;
   this module uses Collision_CapsuleTriangle. */
void EntityCollision_CapsuleTriangle(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *centre, f32 radius, f32 height) {
    f32 point[4];
    f32 edge[4];
    f32 flat[4];
    f32 offset[4];
    f32 unused_a[4];
    f32 normal[4];
    f32 start[4];
    f32 end[4];
    f32 origin[4];
    f32 unused_b[4];
    f32 *result_normal;
    f32 nearest_sq;
    f32 distance;   /* match: one variable for each distance, numerator and squared distance */
    f32 bottom;
    f32 along;
    f32 ratio;      /* match: one variable for each denominator and the quotient it becomes */
    s32 i;

    COLLISION_RESULT->hit = 0;
    result_normal = COLLISION_RESULT->normal;
    __asm__ volatile(
        "lqc2 $vf16, 0(%0)\n"
        "lqc2 $vf17, 0(%1)\n"
        "lqc2 $vf18, 0(%2)\n"
        "vnop\n"
        "vnop\n"
        "vsub.xyz $vf19, $vf16, $vf17\n"
        "vsub.xyz $vf20, $vf18, $vf17\n"
        "vsub.xyz $vf21, $vf18, $vf16\n"
        "vnop\n"
        "vnop\n"
        "vopmula.xyz ACC, $vf19, $vf20\n"
        "vopmsub.xyz $vf16, $vf20, $vf19\n"
        "sqc2 $vf19, 0(%3)\n"
        "sqc2 $vf20, 0(%4)\n"
        "sqc2 $vf21, 0(%5)\n"
        "vmul.xyz $vf17, $vf16, $vf16\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vaddx.xyz $vf17, $vf17, $vf17x\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vaddy.xyz $vf17, $vf17, $vf17y\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "vrsqrt Q, $vf0w, $vf17z\n"
        "vwaitq\n"
        "vmulq.xyz $vf16, $vf16, Q\n"
        "sqc2 $vf16, 0(%6)\n"
        "sqc2 $vf16, 0(%7)\n"
        :
        : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(edge), "r"(flat), "r"(offset), "r"(normal),
          "r"(result_normal));

    nearest_sq = radius * radius;
    ratio = -normal[1];
    distance = normal[0] * corner_b[0] + normal[1] * corner_b[1] + normal[2] * corner_b[2];
    distance += -normal[0] * centre[0] - normal[1] * centre[1] - normal[2] * centre[2];
    bottom = centre[1] - height;
    if (-VERTICAL_FACE_NORMAL_Y_EPSILON < ratio && ratio < VERTICAL_FACE_NORMAL_Y_EPSILON) {
        if (radius < distance) {
            return;
        }
        if (distance < -radius) {
            return;
        }
        normal[1] = 0.0f;
        if (distance < -0.0f) {
            normal[0] = -normal[0];
            normal[2] = -normal[2];
        }
        sceVu0Normalize(normal, normal);
        origin[0] = centre[0];
        origin[1] = centre[1];
        origin[2] = centre[2];
        EntityCollision_RayTriangle(corner_a, corner_b, corner_c, origin, normal, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        origin[1] = bottom;
        EntityCollision_RayTriangle(corner_a, corner_b, corner_c, origin, normal, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
    } else {
        ratio = distance / ratio;
        if (ratio < -0.0f) {
            normal[1] = 0.0f;
            if (distance < -0.0f) {
                normal[0] = -normal[0];
                normal[2] = -normal[2];
            }
            sceVu0Normalize(normal, normal);
            EntityCollision_RayTriangle(corner_a, corner_b, corner_c, centre, normal, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            origin[0] = centre[0];
            origin[2] = centre[2];
            origin[1] = bottom;
            EntityCollision_RayTriangle(corner_a, corner_b, corner_c, origin, normal, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
        } else if (height < ratio) {
            normal[1] = 0.0f;
            if (distance < -0.0f) {
                normal[0] = -normal[0];
                normal[2] = -normal[2];
            }
            sceVu0Normalize(normal, normal);
            origin[0] = centre[0];
            origin[2] = centre[2];
            origin[1] = bottom;
            EntityCollision_RayTriangle(corner_a, corner_b, corner_c, origin, normal, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            EntityCollision_RayTriangle(corner_a, corner_b, corner_c, centre, normal, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
        }
    }
    for (i = 0; i < 3; i++) {
        switch (i) {
        case 0:
            LOAD_EDGE(corner_a, corner_b);
            break;
        case 1:
            LOAD_EDGE(corner_b, corner_c);
            break;
        case 2:
            LOAD_EDGE(corner_c, corner_a);
            break;
        }
        if (centre[1] < end[1] && centre[1] < start[1]) {
            continue;
        }
        if (end[1] < bottom && start[1] < bottom) {
            continue;
        }
        origin[0] = flat[0];
        origin[1] = 0.0f;
        origin[2] = flat[2];
        origin[3] = 1.0f;
        sceVu0Normalize(origin, origin);
        along = origin[0] * edge[0] + origin[2] * edge[2];
        if (-EDGE_PERPENDICULAR_TO_FLAT_EPSILON < along && along < EDGE_PERPENDICULAR_TO_FLAT_EPSILON) {
            point[0] = end[0];
            point[1] = centre[1];
            point[2] = end[2];
            Vector_Subtract(edge, centre, point);
            distance = edge[0] * edge[0] + edge[2] * edge[2];
            if (distance <= nearest_sq) {
                KEEP_NEAREST(distance, point);
            }
            continue;
        }
        if (start[1] > centre[1] || end[1] > centre[1]) {
            distance = -centre[1];
            distance += end[1];
            ratio = -edge[1];
            ratio = distance / ratio;
            point[0] = end[0] + edge[0] * ratio;
            point[1] = end[1] + edge[1] * ratio;
            point[2] = end[2] + edge[2] * ratio;
            Vector_Subtract(offset, centre, point);
            distance = offset[0] * offset[0] + offset[2] * offset[2];
            if (distance < nearest_sq) {
                KEEP_NEAREST(distance, point);
            }
        }
        if (start[1] < bottom || end[1] < bottom) {
            distance = -bottom;
            distance += end[1];
            ratio = -edge[1];
            ratio = distance / ratio;
            point[0] = end[0] + edge[0] * ratio;
            point[1] = end[1] + edge[1] * ratio;
            point[2] = end[2] + edge[2] * ratio;
            Vector_Subtract(offset, centre, point);
            distance = offset[0] * offset[0] + offset[2] * offset[2];
            if (distance < nearest_sq) {
                KEEP_NEAREST(distance, point);
            }
        }
        if (start[1] < centre[1] && bottom < start[1]) {
            Vector_Subtract(offset, centre, start);
            along = offset[0] * offset[0] + offset[2] * offset[2];
            if (along < nearest_sq) {
                KEEP_NEAREST(along, start);
            }
        }
        ratio = origin[0] * centre[0] + origin[2] * centre[2];
        ratio += -origin[0] * end[0] - origin[2] * end[2];
        along = origin[0] * edge[0] + origin[2] * edge[2];
        ratio /= along;
        if (ratio < -0.0f) {
            continue;
        }
        if (1.0f < ratio) {
            continue;
        }
        point[0] = end[0] + edge[0] * ratio;
        point[1] = end[1] + edge[1] * ratio;
        point[2] = end[2] + edge[2] * ratio;
        Vector_Subtract(offset, centre, point);
        distance = offset[0] * offset[0] + offset[2] * offset[2];
        if (distance <= nearest_sq && point[1] <= centre[1] && bottom <= point[1]) {
            KEEP_NEAREST(distance, point);
        }
    }
}

/* Pushes the entity's sphere (at `position`, radius unk30) out of every triangle of a model's collision mesh
   it touches, adding the wall normals to `normal_sum`. Returns TRUE if it touched the floor strips. */
s32 EntityCollision_SphereAgainstMesh(Entity *entity, ModelInstance *model, f32 *position, f32 *normal_sum) {
    f32 offset[4];
    s32 hit_floor;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 i;
    s32 j;

    hit_floor = FALSE;
    Collision_TransformMeshPoints(model);
    mesh = (CollisionMesh *)model->strips;
    if (!(ENTITY_FLAG_BITS(entity)->no_wall_collision)) {
        strip = mesh->wall_strips;
        i = mesh->wall_strip_count;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                EntityCollision_SphereTriangle(point[0].position, point[1].position, point[2].position, position, entity->collision_radius);
                if (COLLISION_RESULT->hit != 0) {
                    entity->flags |= ENTITY_FLAG_WALL_PUSH;
                    Vector_Subtract(offset, position, COLLISION_RESULT);
                    sceVu0Normalize(offset, offset);
                    Vector_Add(normal_sum, normal_sum, offset);
                    Vector_Scale(offset, offset, entity->collision_radius);
                    Vector_Add(position, COLLISION_RESULT, offset);
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
    }
    if (!(ENTITY_FLAG_BITS(entity)->no_second_wall_collision)) {
        strip = mesh->second_wall_strips;
        i = mesh->second_wall_strip_count;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                EntityCollision_SphereTriangle(point[0].position, point[1].position, point[2].position, position, entity->collision_radius);
                if (COLLISION_RESULT->hit != 0) {
                    entity->flags |= ENTITY_FLAG_WALL_PUSH;
                    Vector_Subtract(offset, position, COLLISION_RESULT);
                    sceVu0Normalize(offset, offset);
                    Vector_Add(normal_sum, normal_sum, offset);
                    Vector_Scale(offset, offset, entity->collision_radius);
                    Vector_Add(position, COLLISION_RESULT, offset);
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
    }
    strip = mesh->barrier_strips;
    i = mesh->barrier_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        if (Collision_GetBarrierLevel(point->material_first, point->material_second, point->material_third) >= entity->spawn_info->barrier_level) {
            do {
                EntityCollision_SphereTriangle(point[0].position, point[1].position, point[2].position, position, entity->collision_radius);
                if (COLLISION_RESULT->hit != 0) {
                    entity->flags |= ENTITY_FLAG_WALL_PUSH;
                    Vector_Subtract(offset, position, COLLISION_RESULT);
                    sceVu0Normalize(offset, offset);
                    Vector_Add(normal_sum, normal_sum, offset);
                    Vector_Scale(offset, offset, entity->collision_radius);
                    Vector_Add(position, COLLISION_RESULT, offset);
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        } else {
            strip = (CollisionStrip *)(point + strip->point_count);
        }
    }
    if (!(entity->flags & ENTITY_FLAG_SKIP_FLOOR_MASK)) {
        strip = mesh->floor_strips;
        i = mesh->floor_strip_count;
        while (--i != -1) {
            point = strip->points;
            j = strip->point_count - 2;
            do {
                EntityCollision_SphereTriangle(point[0].position, point[1].position, point[2].position, position, entity->collision_radius);
                if (COLLISION_RESULT->hit != 0) {
                    hit_floor = TRUE;
                    entity->flags |= ENTITY_FLAG_ON_FLOOR;
                    Vector_Subtract(offset, position, COLLISION_RESULT);
                    sceVu0Normalize(offset, offset);
                    Vector_Scale(offset, offset, entity->collision_radius);
                    Vector_Add(position, COLLISION_RESULT, offset);
                }
                point++;
            } while (--j != 0);
            strip = (CollisionStrip *)(point + 2);
        }
    }
    return hit_floor;
}

/* Pushes the entity out of the map pieces whose box it is in (its own map's, then the other loaded map's),
   remembers the piece it stands on in unk14 and updates its push (unkA4). */
void EntityCollision_SphereAgainstMap(Entity *entity) {
    f32 normal_sum[4];
    f32 position[4];
    MapSlot *slots[2];
    MapRecordBounds *record;
    f32 lift;
    s32 hit_floor;
    s32 map;
    s32 i;

    hit_floor = FALSE;
    lift = 0.0f;
    Vector_Copy(position, entity->model->position);
    normal_sum[0] = normal_sum[1] = normal_sum[2] = normal_sum[3] = 0.0f;
    if (!(ENTITY_FLAG_BITS(entity)->no_floor_lift)) {
        entity->flags &= ~ENTITY_FLAG_LIFT_UP;
        position[1] += 0.1f;
        lift = entity->floor_lift;
    }
    slots[0] = &map_slots[entity->group_id];
    slots[1] = &map_slots[(char)(entity->group_id ^ 1)];
    for (map = 0; map < 2; map++) {
        if (slots[map]->loaded != 0) {
            record = (MapRecordBounds *)slots[map]->records;
            for (i = 0; i < slots[map]->header->record_count; i++, record++) {
                if (record->record_flags & MAP_RECORD_HIDDEN) {
                    continue;
                }
                if (record->collision_mesh_index == 0xFFFF) {
                    continue;
                }
                if (record->model->strips == NULL) {
                    continue;
                }
                if (record->lowest[0] - entity->collision_radius > entity->model->position[0]) {
                    continue;
                }
                if (record->highest[0] + entity->collision_radius < entity->model->position[0]) {
                    continue;
                }
                if (record->lowest[1] - entity->collision_radius > entity->model->position[1]) {
                    continue;
                }
                if (record->highest[1] + entity->collision_radius < entity->model->position[1]) {
                    continue;
                }
                if (record->lowest[2] - entity->collision_radius > entity->model->position[2]) {
                    continue;
                }
                if (record->highest[2] + entity->collision_radius < entity->model->position[2]) {
                    continue;
                }
                if (EntityCollision_SphereAgainstMesh(entity, record->model, position, normal_sum)) {
                    entity->map_record = record;
                    hit_floor = TRUE;
                }
            }
        }
        if (hit_floor) {
            break;
        }
    }
    normal_sum[0] += entity->wall_push[0];
    normal_sum[1] += entity->wall_push[1];
    normal_sum[2] += entity->wall_push[2];
    sceVu0Normalize(normal_sum, normal_sum);
    entity->wall_push[0] = normal_sum[0];
    entity->wall_push[1] = normal_sum[1];
    entity->wall_push[2] = normal_sum[2];
    if (ENTITY_FLAG_BITS(entity)->on_top) {
        ENTITY_FLAG_BITS(entity)->on_floor = FALSE;
        ENTITY_FLAG_BITS(entity)->lift_up = FALSE;
        entity->model->position[0] = position[0];
        entity->model->position[2] = position[2];
        normal_sum[1] += -1.0f;
        sceVu0Normalize(normal_sum, normal_sum);
        entity->wall_push[0] = normal_sum[0];
        entity->wall_push[1] = normal_sum[1];
        entity->wall_push[2] = normal_sum[2];
        return;
    }
    if (hit_floor) {
        ENTITY_FLAG_BITS(entity)->lift_up = FALSE;
        entity->model->position[1] = position[1];
        normal_sum[1] += -1.0f;
        sceVu0Normalize(normal_sum, normal_sum);
        entity->wall_push[0] = normal_sum[0];
        entity->wall_push[1] = normal_sum[1];
        entity->wall_push[2] = normal_sum[2];
    } else {
        ENTITY_FLAG_BITS(entity)->on_floor = FALSE;
        entity->model->position[1] = position[1] + lift;
    }
    entity->model->position[0] = position[0];
    entity->model->position[2] = position[2];
}

/* Tests a sphere against the segment start..end: a hit when the closest point of the segment is nearer to
   `centre` than `radius`. */
static void EntityCollision_SphereSegment(void *start, void *end, void *centre, f32 radius) {
    f32 direction[4];
    f32 along;
    f32 distance;
    CollisionResult *result;
    int float_bits;

    __asm__ volatile(
        "lui %2, 0x7000\n"
        "lqc2 $vf16, 0(%4)\n"
        "lqc2 $vf17, 0(%5)\n"
        "lqc2 $vf18, 0(%6)\n"
        "vsub.xyz $vf19, $vf17, $vf16\n"
        "vsub.xyz $vf20, $vf18, $vf16\n"
        "vmul.xyz $vf20, $vf19, $vf20\n"
        "vaddy.x $vf20, $vf20, $vf20y\n"
        "vaddz.x $vf20, $vf20, $vf20z\n"
        "vmul.xyz $vf21, $vf19, $vf19\n"
        "vaddy.x $vf21, $vf21, $vf21y\n"
        "vaddz.x $vf21, $vf21, $vf21z\n"
        "vdiv Q, $vf20x, $vf21x\n"
        "sqc2 $vf19, %1\n"
        "vwaitq\n"
        "vaddq.x $vf20, $vf0, Q\n"
        "qmfc2 %3, $vf20\n"
        "mtc1 %3, %0\n"
        : "=f"(along), "=m"(direction), "=r"(result), "=r"(float_bits)
        : "r"(start), "r"(end), "r"(centre));

    if (0.0f <= along && along <= 1.0f) {
        __asm__ volatile(
            "lqc2 $vf16, 0(%3)\n"
            "lqc2 $vf17, 0(%4)\n"
            "lqc2 $vf18, %6\n"
            "mfc1 %1, %2\n"
            "qmtc2 %1, $vf20\n"
            "vmulx.xyzw $vf19, $vf18, $vf20x\n"
            "vadd.xyz $vf19, $vf19, $vf16\n"
            "vsub.xyz $vf20, $vf17, $vf19\n"
            "vmul.xyz $vf20, $vf20, $vf20\n"
            "vaddy.x $vf20, $vf20, $vf20y\n"
            "vaddz.x $vf20, $vf20, $vf20z\n"
            "vsqrt Q, $vf20x\n"
            "sqc2 $vf19, 0(%5)\n"
            "vwaitq\n"
            "vaddq.x $vf20, $vf0, Q\n"
            "qmfc2 %1, $vf20\n"
            "mtc1 %1, %0\n"
            : "=f"(distance), "=&r"(float_bits)
            : "f"(along), "r"(start), "r"(centre), "r"(result), "m"(direction[0]));

        COLLISION_RESULT->distance = distance;
        if (!(radius <= distance)) {
            __asm__ volatile(
                "lqc2 $vf16, 0(%0)\n"
                "lqc2 $vf17, 0(%2)\n"
                "vsub.xyz $vf18, $vf16, $vf17\n"
                "vmul.xyz $vf19, $vf18, $vf18\n"
                "vaddy.x $vf19, $vf19, $vf19y\n"
                "vaddz.x $vf19, $vf19, $vf19z\n"
                "vsqrt Q, $vf19x\n"
                "vwaitq\n"
                "vaddq.x $vf19, $vf0, Q\n"
                "vdiv Q, $vf0w, $vf19x\n"
                "vwaitq\n"
                "vmulq.xyz $vf18, $vf18, Q\n"
                "sqc2 $vf18, 0(%1)\n"
                : : "r"(centre), "r"(COLLISION_RESULT->normal), "r"(result));

            COLLISION_RESULT->hit = 1;
        }
    }
}

/* Tests a sphere against a point: a hit when `point` is closer to `centre` than `radius`. */
static void EntityCollision_SpherePoint(void *point, void *centre, f32 radius) {
    CollisionResult *result;
    f32 distance;
    int distance_bits;

    __asm__ volatile(
        "lui %1, 0x7000\n"
        "lqc2 $vf16, 0(%3)\n"
        "lqc2 $vf17, 0(%4)\n"
        "vsub.xyz $vf18, $vf17, $vf16\n"
        "vmul.xyz $vf18, $vf18, $vf18\n"
        "vaddy.x $vf18, $vf18, $vf18y\n"
        "vaddz.x $vf18, $vf18, $vf18z\n"
        "vsqrt Q, $vf18x\n"
        "vwaitq\n"
        "vaddq.x $vf18, $vf0, Q\n"
        "qmfc2.ni %2, $vf18\n"
        "mtc1 %2, %0\n"
        : "=f"(distance), "=r"(result), "=r"(distance_bits)
        : "r"(point), "r"(centre));

    result->distance = distance;
    if (!(radius <= distance)) {
        __asm__ volatile(
            "lqc2 $vf16, 0(%0)\n"
            "lqc2 $vf17, 0(%1)\n"
            "sqc2 $vf16, 0(%2)\n"
            "vsub.xyz $vf18, $vf17, $vf16\n"
            "vmul.xyz $vf19, $vf18, $vf18\n"
            "vaddy.x $vf19, $vf19, $vf19y\n"
            "vaddz.x $vf19, $vf19, $vf19z\n"
            "vsqrt Q, $vf19x\n"
            "vwaitq\n"
            "vaddq.x $vf19, $vf0, Q\n"
            "vdiv Q, $vf0w, $vf19x\n"
            "vwaitq\n"
            "vmulq.xyz $vf18, $vf18, Q\n"
            "sqc2 $vf18, 0(%3)\n"
            : : "r"(point), "r"(centre), "r"(result), "r"(COLLISION_RESULT->normal));

        COLLISION_RESULT->hit = 1;
    }
}

/* Tests a sphere against a triangle: its face first, then its three edges and three corners. */
static void EntityCollision_SphereTriangle(void *corner_a, void *corner_b, void *corner_c, void *centre, f32 radius) {
    CollisionResult *result = COLLISION_RESULT;
    f32 *normal;
    f32 unused[8];
    f32 distance;
    f32 side_a, side_b, side_c;

    result->hit = 0;
    normal = COLLISION_RESULT->normal;
    __asm__ volatile(
        "lqc2 $vf16, 0(%1)\n"
        "lqc2 $vf17, 0(%2)\n"
        "lqc2 $vf18, 0(%3)\n"
        "lqc2 $vf19, 0(%4)\n"
        "vsub.xyz $vf21, $vf16, $vf17\n"
        "vsub.xyz $vf22, $vf18, $vf17\n"
        "vopmula.xyz ACC, $vf22, $vf21\n"
        "vopmsub.xyz $vf20, $vf21, $vf22\n"
        "vmul.xyz $vf21, $vf20, $vf20\n"
        "vaddy.x $vf21, $vf21, $vf21y\n"
        "vaddz.x $vf21, $vf21, $vf21z\n"
        "vsqrt Q, $vf21x\n"
        "vwaitq\n"
        "vaddq.x $vf21, $vf0, Q\n"
        "vdiv Q, $vf0w, $vf21x\n"
        "vwaitq\n"
        "vmulq.xyz $vf20, $vf20, Q\n"
        "sqc2 $vf20, 0(%5)\n"
        "vmul.xyz $vf21, $vf20, $vf17\n"
        "vaddy.x $vf21, $vf21, $vf21y\n"
        "vaddz.x $vf21, $vf21, $vf21z\n"
        "vmul.xyz $vf22, $vf20, $vf19\n"
        "vsubx.x $vf21, $vf21, $vf22x\n"
        "vsuby.x $vf21, $vf21, $vf22y\n"
        "vsubz.x $vf21, $vf21, $vf22z\n"
        "qmfc2.ni $2, $vf21\n"
        "mtc1 $2, %0\n"
        "vmulx.xyz $vf22, $vf20, $vf21x\n"
        "vadd.xyz $vf22, $vf22, $vf19\n"
        "sqc2 $vf22, 0(%6)\n"
        : "=f"(distance)
        : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(centre), "r"(normal), "r"(result)
        : "$2");

    result->distance = distance;
    if (radius <= distance) {
        return;
    }
    if (distance <= -radius) {
        return;
    }
    __asm__ volatile(
        "lqc2 $vf16, 0(%3)\n"
        "lqc2 $vf17, 0(%4)\n"
        "lqc2 $vf18, 0(%5)\n"
        "lqc2 $vf19, 0(%6)\n"
        "vsub.xyz $vf20, $vf16, $vf19\n"
        "vsub.xyz $vf21, $vf17, $vf19\n"
        "vsub.xyz $vf22, $vf18, $vf19\n"
        "vopmula.xyz ACC, $vf20, $vf21\n"
        "vopmsub.xyz $vf16, $vf21, $vf20\n"
        "vopmula.xyz ACC, $vf21, $vf22\n"
        "vopmsub.xyz $vf17, $vf22, $vf21\n"
        "vopmula.xyz ACC, $vf22, $vf20\n"
        "vopmsub.xyz $vf18, $vf20, $vf22\n"
        "vmul.xyz $vf20, $vf16, $vf17\n"
        "vaddy.x $vf20, $vf20, $vf20y\n"
        "vaddz.x $vf20, $vf20, $vf20z\n"
        "qmfc2.ni $2, $vf20\n"
        "mtc1 $2, %0\n"
        "vmul.xyz $vf21, $vf17, $vf18\n"
        "vaddy.x $vf21, $vf21, $vf21y\n"
        "vaddz.x $vf21, $vf21, $vf21z\n"
        "qmfc2.ni $2, $vf21\n"
        "mtc1 $2, %1\n"
        "vmul.xyz $vf22, $vf18, $vf16\n"
        "vaddy.x $vf22, $vf22, $vf22y\n"
        "vaddz.x $vf22, $vf22, $vf22z\n"
        "qmfc2.ni $2, $vf22\n"
        "mtc1 $2, %2\n"
        : "=f"(side_a), "=f"(side_b), "=f"(side_c)
        : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(result)
        : "$2");

    if ((0.0f <= side_a && 0.0f <= side_b && 0.0f <= side_c) || (side_a < 0.0f && side_b < 0.0f && side_c < 0.0f)) {
        COLLISION_RESULT->hit = 1;
        return;
    }
    EntityCollision_SphereSegment(corner_a, corner_b, centre, radius);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    EntityCollision_SphereSegment(corner_b, corner_c, centre, radius);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    EntityCollision_SphereSegment(corner_c, corner_a, centre, radius);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    EntityCollision_SpherePoint(corner_a, centre, radius);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    EntityCollision_SpherePoint(corner_b, centre, radius);
    if (COLLISION_RESULT->hit != 0) {
        return;
    }
    EntityCollision_SpherePoint(corner_c, centre, radius);
}

/* Tests a ray (origin, unit direction, length) against a triangle. */
static void EntityCollision_RayTriangle(void *corner_a, void *corner_b, void *corner_c, void *origin, void *direction, f32 length) {
    f32 offset[4];
    f32 unused[12];
    CollisionResult *result = COLLISION_RESULT;
    f32 *normal;
    f32 facing;
    f32 distance;
    f32 side_a, side_b, side_c;

    result->hit = 0;
    normal = COLLISION_RESULT->normal;
    __asm__ volatile(
        "lqc2 $vf16, 0(%1)\n"
        "lqc2 $vf17, 0(%2)\n"
        "lqc2 $vf18, 0(%3)\n"
        "lqc2 $vf19, 0(%4)\n"
        "lqc2 $vf20, 0(%5)\n"
        "vsub.xyz $vf22, $vf16, $vf17\n"
        "vsub.xyz $vf23, $vf18, $vf17\n"
        "vopmula.xyz ACC, $vf23, $vf22\n"
        "vopmsub.xyz $vf21, $vf22, $vf23\n"
        "vmul.xyz $vf22, $vf21, $vf21\n"
        "vaddy.x $vf22, $vf22, $vf22y\n"
        "vaddz.x $vf22, $vf22, $vf22z\n"
        "vsqrt Q, $vf22x\n"
        "vwaitq\n"
        "vaddq.x $vf22, $vf0, Q\n"
        "vdiv Q, $vf0w, $vf22x\n"
        "vwaitq\n"
        "vmulq.xyz $vf21, $vf21, Q\n"
        "sqc2 $vf21, 0(%6)\n"
        "vmul.xyz $vf22, $vf21, $vf20\n"
        "vaddy.x $vf22, $vf22, $vf22y\n"
        "vaddz.x $vf22, $vf22, $vf22z\n"
        "qmfc2.ni $2, $vf22\n"
        "mtc1 $2, %0\n"
        : "=f"(facing)
        : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(origin), "r"(direction), "r"(normal)
        : "$2");

    if (facing < 0.02f && -0.02f < facing) {
        return;
    }
    __asm__ volatile(
        "lqc2 $vf16, 0(%2)\n"
        "lqc2 $vf17, 0(%3)\n"
        "lqc2 $vf18, 0(%4)\n"
        "mfc1 $2, %1\n"
        "qmtc2.ni $2, $vf19\n"
        "vsub.xyz $vf21, $vf16, $vf17\n"
        "vmul.xyz $vf22, $vf21, $vf18\n"
        "vaddy.x $vf22, $vf22, $vf22y\n"
        "vaddz.x $vf22, $vf22, $vf22z\n"
        "vdiv Q, $vf22x, $vf19x\n"
        "vwaitq\n"
        "vaddq.x $vf20, $vf0, Q\n"
        "qmfc2.ni $2, $vf20\n"
        "mtc1 $2, %0\n"
        : "=f"(distance)
        : "f"(facing), "r"(corner_b), "r"(origin), "r"(normal)
        : "$2");

    if (length < distance) {
        return;
    }
    if (distance < 0.0f) {
        return;
    }
    COLLISION_RESULT->distance = distance;
    Vector_Scale(offset, direction, distance);
    Vector_Add(COLLISION_RESULT, offset, origin);
    __asm__ volatile(
        "lqc2 $vf16, 0(%3)\n"
        "lqc2 $vf17, 0(%4)\n"
        "lqc2 $vf18, 0(%5)\n"
        "lqc2 $vf19, 0(%6)\n"
        "vsub.xyz $vf20, $vf16, $vf19\n"
        "vsub.xyz $vf21, $vf17, $vf19\n"
        "vsub.xyz $vf22, $vf18, $vf19\n"
        "vopmula.xyz ACC, $vf20, $vf21\n"
        "vopmsub.xyz $vf16, $vf21, $vf20\n"
        "vopmula.xyz ACC, $vf21, $vf22\n"
        "vopmsub.xyz $vf17, $vf22, $vf21\n"
        "vopmula.xyz ACC, $vf22, $vf20\n"
        "vopmsub.xyz $vf18, $vf20, $vf22\n"
        "vmul.xyz $vf20, $vf16, $vf17\n"
        "vaddy.x $vf20, $vf20, $vf20y\n"
        "vaddz.x $vf20, $vf20, $vf20z\n"
        "qmfc2.ni $2, $vf20\n"
        "mtc1 $2, %0\n"
        "vmul.xyz $vf21, $vf17, $vf18\n"
        "vaddy.x $vf21, $vf21, $vf21y\n"
        "vaddz.x $vf21, $vf21, $vf21z\n"
        "qmfc2.ni $2, $vf21\n"
        "mtc1 $2, %1\n"
        "vmul.xyz $vf22, $vf18, $vf16\n"
        "vaddy.x $vf22, $vf22, $vf22y\n"
        "vaddz.x $vf22, $vf22, $vf22z\n"
        "qmfc2.ni $2, $vf22\n"
        "mtc1 $2, %2\n"
        : "=f"(side_a), "=f"(side_b), "=f"(side_c)
        : "r"(corner_a), "r"(corner_b), "r"(corner_c), "r"(result)
        : "$2");

    if ((0.0f <= side_a && 0.0f <= side_b && 0.0f <= side_c) || (side_a < 0.0f && side_b < 0.0f && side_c < 0.0f)) {
        COLLISION_RESULT->hit = 1;
    }
}

/* Pushes a walking entity out of the map objects it touches (by object kind: a box, a box pair, a quad, or
   the object's mesh) and adds their normals to its push (unkA4). */
void EntityCollision_WalkAgainstObjects(Entity *entity) {
    f32 position[4];
    f32 normal_sum[4];
    f32 unused[4]; // match: 16 bytes of the frame nothing uses
    f32 *sum;
    MapObject *object;
    f32 reach;
    s32 map;
    s32 count;
    s32 i;
    s32 hit;

    normal_sum[0] = normal_sum[1] = normal_sum[2] = normal_sum[3] = 0.0f;
    Vector_Copy(position, entity->model->position);
    sum = normal_sum;
    for (map = 0; map < 2; map++) {
        object = map_objects(map);
        count = map_object_count(map);
        for (i = 0; i < count; i++, object++) {
            if (!(object->state_flags & MAP_OBJECT_IN_USE)) {
                continue;
            }
            if (object->reach_radius <= 0.0f) {
                continue;
            }
            reach = entity->collision_radius + object->reach_radius;
            if (reach < EntityMath_DistanceXZ(position, (f32 *)&object->model->position)) {
                continue;
            }
            COLLISION_RESULT->hit = 0;
            switch (object->definition->kind) {
            case MAP_OBJECT_KIND_DOOR:
                EntityCollision_PushOutOfBox((CollisionOwner *)object, position, entity->collision_height, entity->collision_radius);
                break;
            case MAP_OBJECT_KIND_DOUBLE_DOOR:
                EntityCollision_PushOutOfBoxPair((CollisionOwner *)object, position, entity->collision_height, entity->collision_radius);
                break;
            case MAP_OBJECT_KIND_SIGNAL_DOOR:
                EntityCollision_PushOutOfQuad((CollisionOwner *)object, position, entity->collision_height, entity->collision_radius);
                break;
            default:
                if (object->model->strips != NULL) {
                    EntityCollision_MoveOnMesh((CollisionOwner *)object, position, entity->collision_height, entity->collision_radius);
                }
                break;
            }
            hit = COLLISION_RESULT->hit;
            if (hit & 1) {
                Vector_Add(normal_sum, normal_sum, COLLISION_RESULT->normal);
                ENTITY_FLAG_BITS(entity)->object_push = TRUE;
                ENTITY_FLAG_BITS(entity)->wall_push = TRUE;
                hit = COLLISION_RESULT->hit; // match: read again after the call, into the same register
            }
            if (hit & 2) {
                ENTITY_FLAG_BITS(entity)->object_push = TRUE;
                ENTITY_FLAG_BITS(entity)->on_top = TRUE;
            }
        }
    }
    normal_sum[0] += entity->wall_push[0];
    normal_sum[1] += entity->wall_push[1];
    normal_sum[2] += entity->wall_push[2];
    sceVu0Normalize(sum, sum);
    entity->wall_push[0] = normal_sum[0];
    entity->wall_push[1] = normal_sum[1];
    entity->wall_push[2] = normal_sum[2];
    entity->model->position[0] = position[0];
    entity->model->position[1] = position[1];
    entity->model->position[2] = position[2];
}

/* Pushes the entity (as a sphere of radius unk30) out of the map objects it touches and adds their normals to
   its push (unkA4). */
void EntityCollision_SphereAgainstObjects(Entity *entity) {
    f32 position[4];
    f32 normal_sum[4];
    f32 offset[4];
    f32 *sum;
    MapObject *object;
    f32 reach;
    s32 map;
    s32 count;
    s32 i;
    s32 hit;

    normal_sum[0] = normal_sum[1] = normal_sum[2] = normal_sum[3] = 0.0f;
    Vector_Copy(position, entity->model->position);
    sum = normal_sum;
    for (map = 0; map < 2; map++) {
        object = map_objects(map);
        count = map_object_count(map);
        for (i = 0; i < count; i++, object++) {
            if (!(object->state_flags & MAP_OBJECT_IN_USE)) {
                continue;
            }
            if (object->reach_radius <= 0.0f) {
                continue;
            }
            reach = entity->collision_radius + object->reach_radius;
            if (reach < EntityMath_DistanceXZ(position, (f32 *)&object->model->position)) {
                continue;
            }
            COLLISION_RESULT->hit = 0;
            switch (object->definition->kind) {
            case MAP_OBJECT_KIND_DOOR:
                EntityCollision_SphereBox((CollisionOwner *)object, position, entity->collision_radius);
                break;
            case MAP_OBJECT_KIND_DOUBLE_DOOR:
                EntityCollision_SphereBoxPair((CollisionOwner *)object, position, entity->collision_radius);
                break;
            case MAP_OBJECT_KIND_SIGNAL_DOOR:
                EntityCollision_SphereQuad((CollisionOwner *)object, position, entity->collision_radius);
                break;
            case MAP_OBJECT_KIND_PITFALL:
                EntityCollision_SphereQuads((CollisionOwner *)object, position, entity->collision_radius);
                break;
            default:
                if (object->model->strips != NULL) {
                    EntityCollision_SphereMesh((CollisionOwner *)object, position, entity->collision_radius);
                }
                break;
            }
            hit = COLLISION_RESULT->hit;
            if (hit == 1) {
                Vector_Subtract(offset, position, COLLISION_RESULT);
                sceVu0Normalize(offset, offset);
                Vector_Add(normal_sum, normal_sum, offset);
                Vector_Scale(offset, offset, entity->collision_radius);
                Vector_Add(position, COLLISION_RESULT, offset);
                ENTITY_FLAG_BITS(entity)->object_push = TRUE;
                ENTITY_FLAG_BITS(entity)->wall_push = TRUE;
            } else if (hit == 2) {
                Vector_Add(normal_sum, normal_sum, COLLISION_RESULT->normal);
                ENTITY_FLAG_BITS(entity)->object_push = TRUE;
                ENTITY_FLAG_BITS(entity)->wall_push = TRUE;
            }
        }
    }
    normal_sum[0] += entity->wall_push[0];
    normal_sum[1] += entity->wall_push[1];
    normal_sum[2] += entity->wall_push[2];
    sceVu0Normalize(sum, sum);
    entity->wall_push[0] = normal_sum[0];
    entity->wall_push[1] = normal_sum[1];
    entity->wall_push[2] = normal_sum[2];
    entity->model->position[0] = position[0];
    entity->model->position[1] = position[1];
    entity->model->position[2] = position[2];
}

/* Pushes an upright cylinder out of the four walls standing on the object's points 0-3 (as high as point 0x1F
   is above point 0), then out of the four corner posts. */
static void EntityCollision_PushOutOfBox(CollisionOwner *owner, f32 *centre, f32 radius, f32 push) {
    f32 *corner_a, *corner_b, *corner_c, *corner_d;
    f32 *top;
    f32 height;

    COLLISION_RESULT->hit = 0;
    corner_a = (f32 *)owner->model->get_point(owner->model, 0);
    if (corner_a == NULL) {
        return;
    }
    corner_b = (f32 *)owner->model->get_point(owner->model, 1);
    if (corner_b == NULL) {
        return;
    }
    corner_c = (f32 *)owner->model->get_point(owner->model, 2);
    if (corner_c == NULL) {
        return;
    }
    corner_d = (f32 *)owner->model->get_point(owner->model, 3);
    if (corner_d == NULL) {
        return;
    }
    top = (f32 *)owner->model->get_point(owner->model, 0x1F);
    if (top != NULL) {
        height = corner_a[1] - top[1];
        Collision_CylinderEdge(corner_a, corner_b, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;
            return;
        }
        Collision_CylinderEdge(corner_b, corner_c, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;
            return;
        }
        Collision_CylinderEdge(corner_c, corner_d, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;
            return;
        }
        Collision_CylinderEdge(corner_d, corner_a, height, centre, radius, push);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;
            return;
        }
        Collision_CylinderCylinder(centre, push, radius, corner_a, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * (push + 0.1f);
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * (push + 0.1f);
            return;
        }
        Collision_CylinderCylinder(centre, push, radius, corner_b, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * (push + 0.1f);
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * (push + 0.1f);
            return;
        }
        Collision_CylinderCylinder(centre, push, radius, corner_c, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * (push + 0.1f);
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * (push + 0.1f);
            return;
        }
        Collision_CylinderCylinder(centre, push, radius, corner_d, 0.0f, height);
        if (COLLISION_RESULT->hit != 0) {
            centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * (push + 0.1f);
            centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * (push + 0.1f);
            return;
        }
    }
}

/* As EntityCollision_PushOutOfBox, on one ring of four points (0, 5, 6, 3) while the model plays motion 0
   before its frame 2, else on two rings (points 0-3 and 4-7). */
static void EntityCollision_PushOutOfBoxPair(CollisionOwner *owner, f32 *centre, f32 radius, f32 push) {
    f32 *corner_a, *corner_b, *corner_c, *corner_d;
    f32 *corner_e, *corner_f, *corner_g, *corner_h;
    f32 *top;
    f32 height;

    COLLISION_RESULT->hit = 0;
    if (((EntityModel *)owner->model)->level_tables->motion_index == BOX_PAIR_ONE_RING_MOTION && ((EntityModel *)owner->model)->level_tables->frame < 2) {
        corner_a = (f32 *)owner->model->get_point(owner->model, 0);
        if (corner_a == NULL) {
            return;
        }
        corner_b = (f32 *)owner->model->get_point(owner->model, 5);
        if (corner_b == NULL) {
            return;
        }
        corner_c = (f32 *)owner->model->get_point(owner->model, 6);
        if (corner_c == NULL) {
            return;
        }
        corner_d = (f32 *)owner->model->get_point(owner->model, 3);
        if (corner_d == NULL) {
            return;
        }
        top = (f32 *)owner->model->get_point(owner->model, 0x1F);
        if (top != NULL) {
            height = corner_a[1] - top[1];
            Collision_CylinderEdge(corner_a, corner_b, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_b, corner_c, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_c, corner_d, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_d, corner_a, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_a, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_b, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_c, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_d, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
        }
    } else {
        corner_a = (f32 *)owner->model->get_point(owner->model, 0);
        if (corner_a == NULL) {
            return;
        }
        corner_b = (f32 *)owner->model->get_point(owner->model, 1);
        if (corner_b == NULL) {
            return;
        }
        corner_c = (f32 *)owner->model->get_point(owner->model, 2);
        if (corner_c == NULL) {
            return;
        }
        corner_d = (f32 *)owner->model->get_point(owner->model, 3);
        if (corner_d == NULL) {
            return;
        }
        corner_e = (f32 *)owner->model->get_point(owner->model, 4);
        if (corner_e == NULL) {
            return;
        }
        corner_f = (f32 *)owner->model->get_point(owner->model, 5);
        if (corner_f == NULL) {
            return;
        }
        corner_g = (f32 *)owner->model->get_point(owner->model, 6);
        if (corner_g == NULL) {
            return;
        }
        corner_h = (f32 *)owner->model->get_point(owner->model, 7);
        if (corner_h == NULL) {
            return;
        }
        top = (f32 *)owner->model->get_point(owner->model, 0x1F);
        if (top != NULL) {
            height = corner_a[1] - top[1];
            if (EntityMath_DistanceXZ(corner_b, corner_e) <= push * 2.5f) {
                Collision_CylinderEdge(corner_g, corner_d, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_CENTRE(push);
                    return;
                }
                Collision_CylinderEdge(corner_b, corner_e, height, centre, radius, push);
                if (COLLISION_RESULT->hit != 0) {
                    PUSH_CENTRE(push);
                    return;
                }
            }
            Collision_CylinderEdge(corner_a, corner_b, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_b, corner_c, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_c, corner_d, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_d, corner_a, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_e, corner_f, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_f, corner_g, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_g, corner_h, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderEdge(corner_h, corner_e, height, centre, radius, push);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_a, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_b, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_c, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_d, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_e, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_f, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_g, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
            Collision_CylinderCylinder(centre, push, radius, corner_h, 0.0f, height);
            if (COLLISION_RESULT->hit != 0) {
                PUSH_CENTRE(push + 0.1f);
                return;
            }
        }
    }
}

/* Tests an upright capsule against the two triangles of the object's points 0-3 and pushes `centre` out along
   the normal. */
static void EntityCollision_PushOutOfQuad(CollisionOwner *owner, f32 *centre, f32 radius, f32 push) {
    f32 *point_a, *point_b, *point_c, *point_d;

    COLLISION_RESULT->hit = 0;
    if (owner->flags & 0x10000) {
        point_a = (f32 *)owner->model->get_point(owner->model, 0);
        point_b = (f32 *)owner->model->get_point(owner->model, 1);
        point_c = (f32 *)owner->model->get_point(owner->model, 2);
        point_d = (f32 *)owner->model->get_point(owner->model, 3);
        Collision_CapsuleTriangle(point_a, point_b, point_c, centre, push, radius);
        if (COLLISION_RESULT->hit == 0) {
            Collision_CapsuleTriangle(point_a, point_c, point_d, centre, push, radius);
            if (COLLISION_RESULT->hit == 0) {
                return;
            }
        }
        centre[0] = COLLISION_RESULT->point[0] + COLLISION_RESULT->normal[0] * push;
        centre[2] = COLLISION_RESULT->point[2] + COLLISION_RESULT->normal[2] * push;
    }
}

/* Moves a capsule against the object's mesh: finds the floor under `centre` (no more than `step` above it)
   and pushes it out of the walls. The result's hit is 1 for a wall plus 2 for a floor. */
static void EntityCollision_MoveOnMesh(CollisionOwner *owner, f32 *centre, f32 step, f32 radius) {
    f32 offset[4];
    f32 position[4];
    f32 normal_sum[4];
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    f32 floor_y;
    s32 hit_wall;
    s32 hit_floor;
    s32 i;
    s32 j;

    hit_wall = 0;
    hit_floor = 0;
    COLLISION_RESULT->hit = 0;
    Collision_TransformMeshPoints(owner->model);
    mesh = (CollisionMesh *)owner->model->strips;
    floor_y = FLT_MAX; // match: FLT_MAX as a literal (the compiler keeps it in .sdata, at 0x446840)
    normal_sum[0] = normal_sum[1] = normal_sum[2] = normal_sum[3] = 0.0f;
    Vector_Copy(position, centre);
    strip = mesh->floor_strips;
    i = mesh->floor_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            Collision_DropOnTriangle(point[0].position, point[1].position, point[2].position, centre);
            if (COLLISION_RESULT->hit != 0) {
                if (centre[1] - step <= COLLISION_RESULT->point[1] && COLLISION_RESULT->point[1] <= centre[1] + 10.0f
                    && COLLISION_RESULT->point[1] < floor_y) {
                    floor_y = COLLISION_RESULT->point[1];
                    hit_floor = 2;
                }
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    if (hit_floor != 0) {
        position[1] = floor_y;
    }
    position[1] -= 0.1f;
    strip = mesh->wall_strips;
    i = mesh->wall_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            Collision_CapsuleTriangle(point[0].position, point[1].position, point[2].position, position, radius, step);
            if (COLLISION_RESULT->hit != 0) {
                hit_wall = 1;
                offset[0] = position[0] - COLLISION_RESULT->point[0];
                offset[1] = 0.0f;
                offset[2] = position[2] - COLLISION_RESULT->point[2];
                sceVu0Normalize(offset, offset);
                Vector_Add(normal_sum, normal_sum, offset);
                Vector_Scale(offset, offset, radius);
                position[0] = COLLISION_RESULT->point[0] + offset[0];
                position[2] = COLLISION_RESULT->point[2] + offset[2];
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    if (hit_wall != 0) {
        centre[0] = position[0];
        centre[2] = position[2];
        sceVu0Normalize(normal_sum, normal_sum);
        Vector_Copy(COLLISION_RESULT->normal, normal_sum);
    }
    if (hit_floor != 0) {
        EntityMath_EaseToward(&centre[1], floor_y, 3.0f);
    }
    COLLISION_RESULT->hit = hit_wall + hit_floor;
}

/* Tests a sphere against the four walls standing on the object's points 0-3, as high as point 0x1F is above
   point 0. */
static void EntityCollision_SphereBox(CollisionOwner *owner, void *centre, f32 radius) {
    f32 *corner_a, *corner_b, *corner_c, *corner_d;
    f32 *top;
    f32 height;

    COLLISION_RESULT->hit = 0;
    if (owner->model->get_point(owner->model, 0x1F) == 0) {
        return;
    }
    corner_a = (f32 *)owner->model->get_point(owner->model, 0);
    if (corner_a == NULL) {
        return;
    }
    corner_b = (f32 *)owner->model->get_point(owner->model, 1);
    if (corner_b == NULL) {
        return;
    }
    corner_c = (f32 *)owner->model->get_point(owner->model, 2);
    if (corner_c == NULL) {
        return;
    }
    corner_d = (f32 *)owner->model->get_point(owner->model, 3);
    if (corner_d == NULL) {
        return;
    }
    top = (f32 *)owner->model->get_point(owner->model, 0x1F);
    if (top != NULL) {
        height = corner_a[1] - top[1];
        Collision_BallEdge(corner_a, corner_b, height, centre, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        Collision_BallEdge(corner_b, corner_c, height, centre, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        Collision_BallEdge(corner_c, corner_d, height, centre, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        Collision_BallEdge(corner_d, corner_a, height, centre, radius);
    }
}

/* As EntityCollision_SphereBox, on one ring of points (0, 0, 6, 3: point 0 twice in retail) while the model
   plays motion 0 before its frame 2, else on two rings (points 0-3 and 4-7). */
static void EntityCollision_SphereBoxPair(CollisionOwner *owner, void *centre, f32 radius) {
    f32 *corner_a, *corner_b, *corner_c, *corner_d;
    f32 *corner_e, *corner_f, *corner_g, *corner_h;
    f32 *top;
    f32 height;

    COLLISION_RESULT->hit = 0;
    if (((EntityModel *)owner->model)->level_tables->motion_index == BOX_PAIR_ONE_RING_MOTION && ((EntityModel *)owner->model)->level_tables->frame < 2) {
        corner_a = (f32 *)owner->model->get_point(owner->model, 0);
        if (corner_a == NULL) {
            return;
        }
        corner_b = (f32 *)owner->model->get_point(owner->model, 0);
        if (corner_b == NULL) {
            return;
        }
        corner_c = (f32 *)owner->model->get_point(owner->model, 6);
        if (corner_c == NULL) {
            return;
        }
        corner_d = (f32 *)owner->model->get_point(owner->model, 3);
        if (corner_d == NULL) {
            return;
        }
        top = (f32 *)owner->model->get_point(owner->model, 0x1F);
        if (top != NULL) {
            height = corner_a[1] - top[1];
            Collision_BallEdge(corner_a, corner_b, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_b, corner_c, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_c, corner_d, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_d, corner_a, height, centre, radius);
        }
    } else {
        corner_a = (f32 *)owner->model->get_point(owner->model, 0);
        if (corner_a == NULL) {
            return;
        }
        corner_b = (f32 *)owner->model->get_point(owner->model, 1);
        if (corner_b == NULL) {
            return;
        }
        corner_c = (f32 *)owner->model->get_point(owner->model, 2);
        if (corner_c == NULL) {
            return;
        }
        corner_d = (f32 *)owner->model->get_point(owner->model, 3);
        if (corner_d == NULL) {
            return;
        }
        corner_e = (f32 *)owner->model->get_point(owner->model, 4);
        if (corner_e == NULL) {
            return;
        }
        corner_f = (f32 *)owner->model->get_point(owner->model, 5);
        if (corner_f == NULL) {
            return;
        }
        corner_g = (f32 *)owner->model->get_point(owner->model, 6);
        if (corner_g == NULL) {
            return;
        }
        corner_h = (f32 *)owner->model->get_point(owner->model, 7);
        if (corner_h == NULL) {
            return;
        }
        top = (f32 *)owner->model->get_point(owner->model, 0x1F);
        if (top != NULL) {
            height = corner_a[1] - top[1];
            Collision_BallEdge(corner_a, corner_b, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_b, corner_c, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_c, corner_d, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_d, corner_a, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_e, corner_f, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_f, corner_g, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_g, corner_h, height, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            Collision_BallEdge(corner_h, corner_e, height, centre, radius);
        }
    }
}

/* Tests a sphere against the two triangles of the object's points 0-3. */
static void EntityCollision_SphereQuad(CollisionOwner *owner, void *centre, f32 radius) {
    void *point_a, *point_b, *point_c, *point_d;

    COLLISION_RESULT->hit = 0;
    if (owner->flags & 0x10000) {
        point_a = (void *)owner->model->get_point(owner->model, 0);
        point_b = (void *)owner->model->get_point(owner->model, 1);
        point_c = (void *)owner->model->get_point(owner->model, 2);
        point_d = (void *)owner->model->get_point(owner->model, 3);
        EntityCollision_SphereTriangle(point_a, point_b, point_c, centre, radius);
        if (COLLISION_RESULT->hit != 0) {
            return;
        }
        EntityCollision_SphereTriangle(point_a, point_c, point_d, centre, radius);
    }
}

/* Tests a sphere against the object's eight quads (four points each), or its mesh when object flag 0x20000 is
   clear; a quad with a corner at the model's own position ends the test. */
static void EntityCollision_SphereQuads(CollisionOwner *owner, void *centre, f32 radius) {
    f32 *corner_a, *corner_b, *corner_c, *corner_d;
    s32 i;

    COLLISION_RESULT->hit = 0;
    if (owner->flags & 0x10000) {
        if (!(owner->flags & 0x20000)) {
            EntityCollision_SphereMesh(owner, centre, radius);
            return;
        }
        for (i = 0; i < 0x20; i += 4) {
            corner_a = (f32 *)owner->model->get_point(owner->model, i);
            corner_b = (f32 *)owner->model->get_point(owner->model, i + 1);
            corner_c = (f32 *)owner->model->get_point(owner->model, i + 2);
            corner_d = (f32 *)owner->model->get_point(owner->model, i + 3);
            if (corner_a[0] == owner->model->position.x && corner_a[1] == owner->model->position.y
                && corner_a[2] == owner->model->position.z) {
                return;
            }
            if (corner_b[0] == owner->model->position.x && corner_b[1] == owner->model->position.y
                && corner_b[2] == owner->model->position.z) {
                return;
            }
            if (corner_c[0] == owner->model->position.x && corner_c[1] == owner->model->position.y
                && corner_c[2] == owner->model->position.z) {
                return;
            }
            if (corner_d[0] == owner->model->position.x && corner_d[1] == owner->model->position.y
                && corner_d[2] == owner->model->position.z) {
                return;
            }
            EntityCollision_SphereTriangle(corner_a, corner_b, corner_c, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
            EntityCollision_SphereTriangle(corner_a, corner_c, corner_d, centre, radius);
            if (COLLISION_RESULT->hit != 0) {
                return;
            }
        }
    }
}

/* Pushes a sphere out of every triangle of the object's mesh it touches; leaves the summed normals in the
   result (hit 2) and moves `centre`. */
static void EntityCollision_SphereMesh(CollisionOwner *owner, void *centre, f32 radius) {
    f32 offset[4];
    f32 position[4];
    f32 normal_sum[4];
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 hit_floor;
    s32 hit_unk18;
    s32 hit_unk20;
    s32 i;
    s32 j;

    COLLISION_RESULT->hit = 0;
    if (owner->model->strips == NULL) {
        return;
    }
    hit_floor = FALSE;
    hit_unk18 = FALSE;
    hit_unk20 = FALSE;
    Collision_TransformMeshPoints(owner->model);
    mesh = (CollisionMesh *)owner->model->strips;
    normal_sum[0] = normal_sum[1] = normal_sum[2] = normal_sum[3] = 0.0f;
    Vector_Copy(position, centre);
    strip = mesh->floor_strips;
    i = mesh->floor_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            EntityCollision_SphereTriangle(point[0].position, point[1].position, point[2].position, position, radius);
            if (COLLISION_RESULT->hit != 0) {
                hit_floor = TRUE;
                Vector_Subtract(offset, position, COLLISION_RESULT);
                sceVu0Normalize(offset, offset);
                Vector_Add(normal_sum, normal_sum, offset);
                Vector_Scale(offset, offset, radius);
                Vector_Add(position, COLLISION_RESULT, offset);
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    strip = mesh->wall_strips;
    i = mesh->wall_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            EntityCollision_SphereTriangle(point[0].position, point[1].position, point[2].position, position, radius);
            if (COLLISION_RESULT->hit != 0) {
                hit_unk18 = TRUE;
                Vector_Subtract(offset, position, COLLISION_RESULT);
                sceVu0Normalize(offset, offset);
                Vector_Add(normal_sum, normal_sum, offset);
                Vector_Scale(offset, offset, radius);
                Vector_Add(position, COLLISION_RESULT, offset);
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    strip = mesh->second_wall_strips;
    i = mesh->second_wall_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            EntityCollision_SphereTriangle(point[0].position, point[1].position, point[2].position, position, radius);
            if (COLLISION_RESULT->hit != 0) {
                hit_unk20 = TRUE;
                Vector_Subtract(offset, position, COLLISION_RESULT);
                sceVu0Normalize(offset, offset);
                Vector_Add(normal_sum, normal_sum, offset);
                Vector_Scale(offset, offset, radius);
                Vector_Add(position, COLLISION_RESULT, offset);
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    if (hit_floor || hit_unk18 || hit_unk20) {
        Vector_Copy(COLLISION_RESULT->normal, normal_sum);
        Vector_Copy(centre, position);
        COLLISION_RESULT->hit = 2;
    }
}

/* Tests the line from `from` to `destination` against `target` with Collision_HitRayObj; returns the result's hit. */
s32 EntityCollision_LineHits(Entity *target, f32 *from, f32 *destination) {
    f32 direction[4];

    Vector_Subtract(direction, destination, from);
    Vector_Normalize(direction, direction);
    Collision_HitRayObj((MapObject *)target, from, direction, EntityMath_Distance(from, destination));
    return COLLISION_RESULT->hit;
}

/* How far the nearest floor of the prop's mesh lies below `position` (y grows downwards); FLT_MAX when there
   is none. */
f32 EntityCollision_DistanceToPropFloor(void *prop_arg, void *position_arg) {
    MapProp *prop = prop_arg;
    f32 *position = position_arg;
    f32 result;
    f32 lowest;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 found;
    s32 i;
    s32 j;

    result = lowest = FLT_MAX;
    if (prop == NULL || prop->collision_strip_index == 0xFFFF || prop->model == NULL || prop->model->strips == NULL) {
        return lowest;
    }
    found = FALSE;
    // match: called as if it returned nothing (other files declare it int)
    ((void (*)(void *))Collision_TransformMeshPoints)(prop->model);
    mesh = (CollisionMesh *)prop->model->strips;
    strip = mesh->floor_strips;
    i = mesh->floor_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            Collision_DropOnTriangle(point[0].position, point[1].position, point[2].position, position);
            if (COLLISION_RESULT->hit != 0) {
                f32 y = COLLISION_RESULT->point[1];

                if (position[1] <= y && y <= lowest) {
                    lowest = y;
                    found = TRUE;
                }
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    if (found) {
        result = lowest - position[1];
    }
    return result;
}

/* Finds the nearest triangle of the prop's mesh a ray hits. Returns which of the four strip lists it is in
   (1-4, 0: none), leaves the distance in the result and copies the point to `out_point`. */
s32 EntityCollision_RayProp(void *prop_arg, f32 *origin, f32 *direction, f32 length, s32 out_point_arg) {
    MapProp *prop = prop_arg;
    void *out_point = (void *)out_point_arg;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    f32 nearest;
    s32 kind;
    s32 i;
    s32 j;

    if (prop == NULL || prop->collision_strip_index == 0xFFFF || prop->model == NULL || prop->model->strips == NULL) {
        return 0;
    }
    kind = 0;
    // match: called as if it returned nothing (other files declare it int)
    ((void (*)(void *))Collision_TransformMeshPoints)(prop->model);
    mesh = (CollisionMesh *)prop->model->strips;
    nearest = FLT_MAX;
    strip = mesh->floor_strips;
    i = mesh->floor_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            EntityCollision_RayTriangle(point[0].position, point[1].position, point[2].position, origin, direction, length);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->distance < nearest) {
                nearest = COLLISION_RESULT->distance;
                kind = 1;
                if (out_point != NULL) {
                    Vector_Copy(out_point, COLLISION_RESULT);
                }
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    strip = mesh->wall_strips;
    i = mesh->wall_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            EntityCollision_RayTriangle(point[0].position, point[1].position, point[2].position, origin, direction, length);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->distance < nearest) {
                nearest = COLLISION_RESULT->distance;
                kind = 2;
                if (out_point != NULL) {
                    Vector_Copy(out_point, COLLISION_RESULT);
                }
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    strip = mesh->second_wall_strips;
    i = mesh->second_wall_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            EntityCollision_RayTriangle(point[0].position, point[1].position, point[2].position, origin, direction, length);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->distance < nearest) {
                nearest = COLLISION_RESULT->distance;
                kind = 3;
                if (out_point != NULL) {
                    Vector_Copy(out_point, COLLISION_RESULT);
                }
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    strip = mesh->barrier_strips;
    i = mesh->barrier_strip_count;
    while (--i != -1) {
        point = strip->points;
        j = strip->point_count - 2;
        do {
            EntityCollision_RayTriangle(point[0].position, point[1].position, point[2].position, origin, direction, length);
            if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->distance < nearest) {
                nearest = COLLISION_RESULT->distance;
                kind = 4;
                if (out_point != NULL) {
                    Vector_Copy(out_point, COLLISION_RESULT);
                }
            }
            point++;
        } while (--j != 0);
        strip = (CollisionStrip *)(point + 2);
    }
    COLLISION_RESULT->distance = nearest;
    return kind;
}

/* Tests the ray (origin, unit direction, length) against the props of the loaded maps, the current map first.
   Returns the strip list hit (1-4, 0: none) and copies the point to `out_point`; `nearest` is never lowered,
   so the last prop hit wins. */
s32 EntityCollision_RayProps(f32 *origin, f32 *direction, void *out_point, f32 length) {
    MapSlot *slots[2];
    f32 end[4];
    f32 point[4];
    f32 lowest[4];
    f32 highest[4];
    MapRecord *record;
    f32 nearest;
    s32 result;
    s32 kind;
    s32 map;
    s32 i;

    Vector_Scale(end, direction, length);
    Vector_Add(end, end, origin);
    if (origin[0] < end[0]) {
        lowest[0] = origin[0];
        highest[0] = end[0];
    } else {
        lowest[0] = end[0];
        highest[0] = origin[0];
    }
    if (origin[1] < end[1]) {
        lowest[1] = origin[1];
        highest[1] = end[1];
    } else {
        lowest[1] = end[1];
        highest[1] = origin[1];
    }
    if (origin[2] < end[2]) {
        lowest[2] = origin[2];
        highest[2] = end[2];
    } else {
        lowest[2] = end[2];
        highest[2] = origin[2];
    }
    nearest = FLT_MAX; // match: FLT_MAX as a literal (the compiler keeps it in .sdata, at 0x44684C)
    result = 0;
    slots[0] = &map_slots[current_map_slot];
    slots[1] = &map_slots[current_map_slot ^ 1];
    for (map = 0; map < 2; map++) {
        if (slots[map]->loaded != 0) {
            record = (MapRecord *)slots[map]->records;
            for (i = 0; i < slots[map]->header->record_count; i++, record++) {
                if (((EntitySpawnFlags *)&record->record_flags)->halves[1] & 0x40000000) {
                    continue;
                }
                if (record->collision_mesh_index == 0xFFFF) {
                    continue;
                }
                if (((MapProp *)record)->model->strips == NULL) {
                    continue;
                }
                if (highest[0] < record->lowest[0] || record->highest[0] < lowest[0]) {
                    continue;
                }
                if (highest[1] < record->lowest[1] || record->highest[1] < lowest[1]) {
                    continue;
                }
                if (highest[2] < record->lowest[2] || record->highest[2] < lowest[2]) {
                    continue;
                }
                kind = EntityCollision_RayProp(record, origin, direction, length, (s32)point);
                // match: retail never lowers `nearest`, so every hit passes and the last one wins
                if (kind != 0 && COLLISION_RESULT->distance < nearest) {
                    result = kind;
                    if (out_point != NULL) {
                        Vector_Copy(out_point, point);
                    }
                }
            }
            if (result != 0) {
                break;
            }
        }
    }
    return result;
}

/* TRUE when the ray (origin, unit direction, length) hits a wall (strip list unk18) of any prop of the loaded
   maps. */
s32 EntityCollision_RayHitsPropWall(f32 *origin, f32 *direction, f32 length, f32 *unused) {
    f32 end[4];
    MapSlot *slot;
    MapRecord *record;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *point;
    s32 map;
    s32 i;
    s32 strips_left;
    s32 points_left;

    Vector_Scale(end, direction, length);
    Vector_Add(end, end, origin);
    for (map = 0; map < 2; map++) {
        slot = current_map_slot == map ? &map_slots[0] : &map_slots[1];
        record = (MapRecord *)slot->records;
        if (slot->loaded != 0) {
            for (i = 0; i < slot->header->record_count; i++, record++) {
                if (record->collision_mesh_index == 0xFFFF) {
                    continue;
                }
                if (origin[0] < record->lowest[0] && end[0] < record->lowest[0]) {
                    continue;
                }
                if (record->highest[0] < origin[0] && record->highest[0] < end[0]) {
                    continue;
                }
                if (origin[1] < record->lowest[1] && end[1] < record->lowest[1]) {
                    continue;
                }
                if (record->highest[1] < origin[1] && record->highest[1] < end[1]) {
                    continue;
                }
                if (origin[2] < record->lowest[2] && end[2] < record->lowest[2]) {
                    continue;
                }
                if (record->highest[2] < origin[2] && record->highest[2] < end[2]) {
                    continue;
                }
                // match: called as void, or the model pointer does not take $2
                ((void (*)(void *))Collision_TransformMeshPoints)(((MapProp *)record)->model);
                mesh = (CollisionMesh *)((MapProp *)record)->model->strips;
                strips_left = mesh->wall_strip_count;
                strip = mesh->wall_strips;
                while (--strips_left != -1) {
                    point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        EntityCollision_RayTriangle(point[0].position, point[1].position, point[2].position, origin, direction, length);
                        if (COLLISION_RESULT->hit != 0) {
                            return TRUE;
                        }
                        point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&point[2];
                }
            }
        }
    }
    return FALSE;
}

/* Whether `point` lies within `radius` of the segment start..end (FALSE beyond its ends). */
int EntityCollision_PointNearSegment(float radius, int *start_arg, int end_arg, int point_arg) {
    f32 *start = (f32 *)start_arg;
    f32 *end = (f32 *)end_arg;
    f32 *point = (f32 *)point_arg;
    f32 along[4];
    f32 to_point[4];
    f32 nearest[4];
    f32 fraction;

    if (start[0] == end[0] && start[1] == end[1] && start[2] == end[2]) {
        return FALSE;
    }
    Vector_Subtract(along, end, start);
    Vector_Subtract(to_point, point, start);
    fraction = (along[0] * to_point[0] + along[1] * to_point[1] + along[2] * to_point[2]) /
               (along[0] * along[0] + along[1] * along[1] + along[2] * along[2]);
    if (fraction < 0.0f) {
        return FALSE;
    }
    if (fraction > 1.0f) {
        return FALSE;
    }
    Vector_Scale(nearest, along, fraction);
    Vector_Add(nearest, nearest, start);
    if (EntityMath_Distance(point, nearest) <= radius) {
        return TRUE;
    }
    return FALSE;
}

/* The height of the ground of the loaded maps (strip lists unk2C) at `point` less the point's own (positive
   when the ground is below, y growing downwards), from the first triangle found; FLT_MAX when none is under
   or over it. */
f32 EntityCollision_DistanceToGround(f32 *point) {
    MapSlot *slot;
    MapRecord *record;
    ModelInstanceBlock *model;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    s32 map;
    s32 record_count;
    s32 i;
    s32 strips_left;
    s32 points_left;

    for (map = 0; map < 2; map++) {
        slot = current_map_slot == map ? &map_slots[0] : &map_slots[1];
        if (slot->loaded != 0) {
            record = (MapRecord *)slot->records;
            record_count = slot->header->record_count;
            model = map_slot_record_models[current_map_slot == map ? 0 : 22];
            for (i = 0; i < record_count; i++, model++, record++) {
                if (record->collision_mesh_index == 0xFFFF) {
                    continue;
                }
                if (point[0] < record->lowest[0] || record->highest[0] < point[0] || point[2] < record->lowest[2] ||
                    record->highest[2] < point[2]) {
                    continue;
                }
                // match: called as void, or the mesh pointer does not take $2
                ((void (*)(void *))Collision_TransformMeshPoints)(model);
                mesh = (CollisionMesh *)model->model.strips;
                strips_left = mesh->water_strip_count;
                strip = mesh->water_strips;
                while (--strips_left != -1) {
                    strip_point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        Collision_DropOnTriangle(strip_point[0].position, strip_point[1].position, strip_point[2].position, point);
                        if (COLLISION_RESULT->hit != 0) {
                            return COLLISION_RESULT->point[1] - point[1];
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
            }
        }
    }
    return FLT_MAX;
}
