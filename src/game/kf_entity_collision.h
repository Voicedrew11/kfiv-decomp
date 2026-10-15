/* Entity collision: entities kept out of the player and of each other, walking or flying entities against the
   map pieces and map objects, and the sphere, capsule and ray tests and map queries these use. The results go
   to COLLISION_RESULT, as with the tests of kf_collision.c. */
#ifndef KF_ENTITY_COLLISION_H
#define KF_ENTITY_COLLISION_H

#include "common.h"
#include "kf_entity.h"
#include "kf_collision.h"

/* Keeps a living entity out of the player and out of the other entities. */
void EntityCollision_PushApart(Entity *entity);

/* Applies the effect of a surface the entity touches (a timer, or damage). */
void EntityCollision_ApplySurfaceEffect(Entity *entity, s32 unknown_a, s32 unknown_b, s32 unknown_c);

/* Collides a walking entity with a model's mesh; TRUE if it found a floor. */
s32 EntityCollision_WalkAgainstMesh(Entity *entity, ModelInstance *model, f32 *position, f32 *normal_sum, f32 *floor_y);

/* Collides a walking entity with the map pieces it is in and puts it on the floor. */
void EntityCollision_WalkAgainstMap(Entity *entity);

/* Tests an upright capsule against a triangle (unused). */
void EntityCollision_CapsuleTriangle(f32 *corner_a, f32 *corner_b, f32 *corner_c, f32 *centre, f32 radius, f32 height);

/* Pushes the entity's sphere out of a model's mesh; TRUE if it touched the floor strips. */
s32 EntityCollision_SphereAgainstMesh(Entity *entity, ModelInstance *model, f32 *position, f32 *normal_sum);

/* Pushes the entity's sphere out of the map pieces it is in. */
void EntityCollision_SphereAgainstMap(Entity *entity);

/* Pushes a walking entity out of the map objects it touches. */
void EntityCollision_WalkAgainstObjects(Entity *entity);

/* Pushes the entity's sphere out of the map objects it touches. */
void EntityCollision_SphereAgainstObjects(Entity *entity);

/* Tests the line from `from` to `destination` against `target`. */
s32 EntityCollision_LineHits(Entity *target, f32 *from, PlayerData *destination);

/* How far the nearest floor of the prop's mesh lies below `position`. */
f32 EntityCollision_DistanceToPropFloor(void *prop_arg, void *position_arg);

/* The strip list (1-4) of the nearest triangle of the prop a ray hits, 0: none. */
s32 EntityCollision_RayProp(void *prop_arg, f32 *origin, f32 *direction, f32 length, s32 out_point_arg);

/* The strip list (1-4) a ray hits among the props of the loaded maps, 0: none. */
s32 EntityCollision_RayProps(f32 *origin, f32 *direction, void *out_point, f32 length);

/* TRUE when a ray hits a wall of a prop of the loaded maps. */
s32 EntityCollision_RayHitsPropWall(f32 *origin, f32 *direction, f32 length, f32 *unused);

/* Whether `point` lies within `radius` of the segment start..end. */
int EntityCollision_PointNearSegment(float radius, int *start_arg, int end_arg, int point_arg);

/* The height of the ground of the loaded maps at `point` less the point's own. */
f32 EntityCollision_DistanceToGround(f32 *point);

#endif
