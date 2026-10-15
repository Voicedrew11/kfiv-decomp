/* Where an entity goes and looks: random points near a centre, the matrices and angles of a model's parts
   (bones) for looking at a point, and whether something stands between an entity and the player. */
#ifndef KF_ENTITY_AIM_H
#define KF_ENTITY_AIM_H

#include "common.h"
#include "kf_entity.h"

/* Picks a point as EntityPoint_PickNear, then moves its height by up to +-height_range. */
void EntityPoint_PickNearWithHeight(f32 *point, f32 range, f32 height_range, void *centre, void *position);

/* Picks a random point within `range` of `centre`. */
void EntityPoint_PickNear(float range, void *point_arg, void *centre, void *position);

/* EntityPoint_PickNear for a caller that already knows how far from `centre` it is. */
void EntityPoint_PickNearFromDistance(f32 *point, f32 *centre, f32 distance, f32 range);

/* A point `distance` away from the player along the player's vector unk1AC (unused). */
void func_0018BC78(f32 *point, s32 distance);

/* Multiplies `matrix` by the transforms on the way from `part` down to `target`; FALSE: not found. */
s32 Bone_MultiplyPathTo(void *part_arg, void *target_arg, void *matrix);

/* The matrix of a part of the model in the world (the parts' transforms, then the model's). */
s32 Bone_GetMatrix(ModelInstance *model, void *bone, f32 *matrix);

/* Builds the rotation matrix that looks from `origin` towards `target`. */
void Bone_LookAtMatrix(f32 *target, f32 *origin, s32 up_sign, f32 *matrix, f32 side_sign);

/* The angles that turn a part of the model to look at `target`; FALSE: no such part. */
s32 Bone_LookAngles(ModelInstance *model, void *part, f32 *target, f32 *limits, f32 *angles);

/* Takes the three angles (x, y, z) out of a rotation matrix. */
void Bone_MatrixToAngles(void *angles_arg, void *matrix);

/* Whether a map object (kinds 1, 2, 6 and 0x11) stands between the entity's middle and the player's. */
s32 EntitySight_ObjectInWay(Entity *entity);

/* Whether another living entity stands on the line from the player to this one (within its radius). */
s32 EntitySight_EntityInWay(Entity *entity);

/* Whether a prop stands between the entity and the player; returns the strip list hit. */
s32 EntitySight_PropInWay(Entity *entity);

/* Whether a prop's wall stands between the entity and the player. */
s32 EntitySight_WallInWay(Entity *entity);

/* Gives the attack's ailment chances to the player when it is in the arc; TRUE if it was. */
s32 EntityAim_AilPlayerInArc(void *target, f32 *point, f32 max_angle, f32 max_height, f32 max_distance, f32 heading);

#endif
