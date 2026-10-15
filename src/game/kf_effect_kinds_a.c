#include "common.h"
#include "kf_effect.h"
#include "kf_entity.h"
#include "kf_collision.h"
#include "kf_player.h"
#include "kf_vu0.h"

extern PlayerState D_00413F80;
extern f32 D_00431AF0[16];
extern Entity *D_004467E0;
extern int D_004468DC;

int rand(void);
double sin(double);
double cos(double);
double sqrt(double);
void sceVu0AddVector(void *, void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0ApplyMatrix(void *, void *, void *);
char Sound_PlayTrackedKind(int index, int first, int second, int third, int fourth);
void EffectHit_CheckAndEnd(EffectActor *effect, s8 *state);
int EffectHit_CheckAndEndByFlag(EffectActor *effect, s8 *state);
void EffectHit_End(EffectActor *effect, s8 *state);
void EffectAngle_FromVector(float *angles, float *direction);
int EffectHit_FindAimedEntity(EffectActor *effect, Vector4 *position, Vector4 *reference);
Entity *EffectHit_FindNearestEntity(EffectActor *effect, Vector4 *position, Vector4 *unused_rotation);
int func_001B0040(Entity *entity);
int func_001B00A0(float radius, void *point);
float func_001B0178(void *from, void *target);
float func_001B01B0(float *origin, float *target);
s16 func_001B0208(float radius, void *effect, void *motion, void *hit_point);
f32 func_001B0950(f32 fparg0);
float func_001B0970(void *left, void *right);
int func_001B4F90(void *effect_actor);
int func_001C0E40(f32 reach, void *point_arg);
void func_001C1430(f32 scale, void *position_arg, void *direction);

#define TWO_PI 6.2831855f
#define PI 3.14159265f

/* Start handler of the kinds that do nothing of their own (kind 6 and most unused kinds): does
   nothing. */
void EffectIdle_Start(void) {
}

/* Update handler of those kinds: only counts the frame. */
void EffectIdle_Update(EffectActor *effect) {
    effect->frame++;
}

/* Starts a flying effect (kind 0): turns and places the model as the definition says and sets its
   velocity to EffectDef.speed along its heading. Other kinds start with it too. */
void EffectFly_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    TimedMotion *motion;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion = (TimedMotion *)&effect->motion;
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk10 = 0;
    motion->unk14 = 1.0f;
    motion->unk1C = 0;
}

/* EffectFly_Start, then raises the model 220 units (kind 1). */
void EffectFly_StartRaised(EffectActor *effect, EffectSlot *spawn_slot) {
    ModelInstance *model;

    EffectFly_Start(effect, spawn_slot);
    model = effect->model;
    model->position.y -= 220.0f;
}

/* EffectFly_Start; the effects aimed at this one are freed with it (kind 42). */
void EffectFly_StartWithTargeters(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectFly_Start(effect, spawn_slot);
    effect->flags |= EFFECT_FLAG_HAS_TARGETERS;
}

/* Moves a flying effect (kinds 0, 1, 42): with EffectDef flag 0x80000 along its heading at a speed
   that grows by scale_per_frame, otherwise by its velocity under gravity, x and z growing by
   scale_per_frame. Ends on a hit (EffectHit_CheckAndEnd). */
void EffectFly_Update(EffectActor *effect) {
    ScaledFallMotion *motion;
    ModelInstance *model;
    Vector4 step;

    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    motion = (ScaledFallMotion *)&effect->motion;
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        if (EFFECT_DEF_FLAGS(effect->def) & 0x80000) {
            EffectAngle_ToVector(effect->def->speed, &step, &effect->model->rotation);
            motion->scale += effect->def->scale_per_frame;
            Vector_Scale(&step, &step, motion->scale);
        } else {
            if (effect->def->gravity != 0.0f) {
                motion->fall_speed += effect->def->gravity;
                motion->velocity.y += motion->fall_speed;
            }
            Vector_Copy(&step, motion);
            if (effect->def->scale_per_frame != 0.0f) {
                motion->scale += effect->def->scale_per_frame;
                step.x *= motion->scale;
                step.z *= motion->scale;
            }
        }
        model = effect->model;
        sceVu0AddVector(&model->position, &model->position, &step);
    }
    EffectHit_CheckAndEnd(effect, motion->unk1C);
    effect->frame += 1;
}

/* EffectFly_Start with a random growth factor of 1 +- EffectDef.unkDC (kind 49). */
void EffectFly_StartGrowing(EffectActor *effect, EffectSlot *spawn_slot) {
    ScaledFallMotion *motion;

    EffectFly_Start(effect, spawn_slot);
    motion = (ScaledFallMotion *)&effect->motion;
    motion->growth = ((float)(rand() & 0xFFFF) * 0.000030517578f - 1.0f) * effect->def->unkDC + 1.0f;
}

/* EffectFly_Update, then multiplies the model's scale by the growth factor (kind 49). */
void EffectFly_UpdateGrowing(EffectActor *effect) {
    ScaledFallMotion *motion;
    ModelInstance *model;

    motion = (ScaledFallMotion *)&effect->motion;
    EffectFly_Update(effect);
    model = effect->model;
    Vector_Scale(&model->unkE0, &model->unkE0, motion->growth);
}

/* Starts an effect on the entity the player faces (EffectHit_FindAimedEntity from the player's view), half its
   height up, or where the definition says when there is none (kind 2). */
void EffectAtTarget_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    TargetMotion *motion;
    EffectTarget *target;
    int target_id;

    target_id = EffectHit_FindAimedEntity(effect, &D_00413F80.position, &D_00413F80.rotation);
    effect->unk6C = target_id;
    motion = (TargetMotion *)&effect->motion;
    if (target_id == 0) {
        EffectStart_PlaceModel(effect, spawn_slot);
        EffectStart_TurnModel(effect, spawn_slot);
    } else {
        target = (EffectTarget *)target_id;
        effect->model->position.x = target->model->position.x;
        effect->model->position.y = target->model->position.y - target->height * 0.5f;
        effect->model->position.z = target->model->position.z;
        effect->model->rotation.x = 0.0f;
        effect->model->rotation.y = 0.0f;
        effect->model->rotation.z = 0.0f;
    }
    motion->unk00 = effect->def->radius_half;
    motion->unk04 = 0;
}

/* Runs the hit check that spawns the end effect (EffectHit_CheckAndEnd) once, on frame
   EffectDef.radius_half (kind 2). */
void EffectAtTarget_Update(EffectActor *effect) {
    CountdownMotion *motion = (CountdownMotion *)&effect->motion;
    s8 state;

    motion->countdown -= 1;
    if (effect->def->radius_half == effect->frame) {
        EffectHit_CheckAndEnd(effect, &state);
    }
    effect->frame += 1;
}

/* Starts an effect at the feet of the entity the player faces (or where the definition says) and
   puts it on the floor within EffectDef.unkE0; with no floor there it ends at once (kind 3). */
void EffectAtTargetFloor_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    SnappedMotion *motion;
    Entity *source;
    ModelInstance *source_model;

    source = (Entity *)EffectHit_FindAimedEntity(actor, &D_00413F80.position, &D_00413F80.rotation);
    motion = (SnappedMotion *)&actor->motion;
    actor->unk6C = (int)source;
    motion->unk04 = 1;
    if (actor->unk6C == 0) {
        EffectStart_PlaceModel(actor, spawn_slot);
        EffectStart_TurnModel(actor, spawn_slot);
        motion->unk04 = 0;
    } else {
        source_model = ENTITY_MODEL((Entity *)actor->unk6C);
        actor->model->position.x = source_model->position.x;
        actor->model->position.y = source_model->position.y;
        actor->model->position.z = source_model->position.z;
        actor->model->rotation.x = 0;
        actor->model->rotation.y = 0;
        actor->model->rotation.z = 0;
    }
    actor->model->position.y -= actor->def->unkE0 * 0.5f;
    if (func_001C0E40(actor->def->unkE0, &actor->model->position) == 0) {
        motion->unk04 = 1;
        actor->frame = actor->def->keyframe_end;
        actor->model->position.y += actor->def->unkE0 * 0.5f;
    } else {
        motion->unk04 = 0;
        actor->model->position.y = COLLISION_RESULT->point[1];
    }
    motion->radius_half = actor->def->radius_half;
}

/* After EffectDef.radius_half frames runs the hit check that spawns the end effect every frame,
   unless the start found no floor (kind 3). */
void EffectAtTargetFloor_Update(EffectActor *effect) {
    CountdownMotion *motion = (CountdownMotion *)&effect->motion;
    s8 state_buffer[0x10];
    int remaining;

    remaining = motion->countdown - 1;
    motion->countdown = remaining;
    if (motion->unk04 == 0 && remaining <= 0) {
        EffectHit_CheckAndEnd(effect, state_buffer);
    }
    effect->frame += 1;
}

/* Turns and places the model as the definition says; clears the parent effect (kinds 4, 48). */
void EffectLoop_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    effect->target = NULL;
}

/* EffectLoop_Update; on the first frame gives the model a random roll (kind 48). */
void EffectLoop_UpdateRandomRoll(EffectActor *effect) {
    if (effect->frame == 0) {
        effect->model->rotation.z = ((float)(rand() & 0xFFFF) * 0.000030517578125f - 1.0f) * 3.1415f;
    }
    EffectLoop_Update(effect);
}

/* Runs the three keyframe channels and loops the frame counter at EffectDef.keyframe_end (kind 4;
   other kinds use it too). */
void EffectLoop_Update(EffectActor *effect) {
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    effect->frame += 1;
    if (effect->frame >= effect->def->keyframe_end) {
        effect->frame = 0;
    }
}

/* Starts an emitter (kind 5): it spawns effect radius_half up to interval times, each unkF0 plus a
   random part of unkF2 frames after the last. */
void EffectEmitter_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    SpawnerMotion *motion = (SpawnerMotion *)&effect->motion;
    EffectDef *def;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion->spawn_count = 0;
    def = effect->def;
    motion->spawn_effect_id = def->radius_half;
    motion->spawn_limit = def->interval;
    motion->spawn_frames_base = def->unkF0;
    motion->spawn_frames_span = def->unkF2;
    motion->next_spawn_frame = motion->spawn_frames_base + rand() % motion->spawn_frames_span;
}

/* Spawns the emitter's effect when its frame is due and picks the next frame; after the last one
   the emitter ends (kind 5). */
void EffectEmitter_Update(EffectActor *effect) {
    SpawnerMotion *motion = (SpawnerMotion *)&effect->motion;
    ModelInstance *model;

    if (motion->next_spawn_frame < effect->frame) {
        model = effect->model;
        Effect_Spawn(motion->spawn_effect_id, &model->position, &model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        motion->spawn_count += 1;
        if (motion->spawn_count >= motion->spawn_limit) {
            effect->frame = effect->def->keyframe_end;
        } else {
            effect->frame = 0;
        }
        motion->next_spawn_frame = motion->spawn_frames_base + rand() % motion->spawn_frames_span;
    }
    effect->frame += 1;
}

/* Starts a homing missile (kind 70): raised 220 units, aimed at the entity in front of it
   (EffectHit_FindAimedEntity), turning at EffectDef.unkDC; the effects aimed at it are freed with it. */
void EffectTrailHoming_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    HomingMotion *motion = (HomingMotion *)&effect->motion;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    effect->model->position.y -= 220.0f;
    effect->unk6C = EffectHit_FindAimedEntity(effect, &effect->model->position, &effect->model->rotation);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk14 = 0;
    motion->unkDC = effect->def->unkDC;
    motion->unk18 = 0;
    motion->unk1C = 0;
    motion->unk20 = 0;
    motion->unk24 = 0;
    ENTITY_FLAG_WORD(effect->flags) |= EFFECT_FLAG_HAS_TARGETERS;
}

/* Homes on its entity with a turn that grows by EffectDef.unkE0 and leaves effect unkF2 behind each
   frame. A hit with bit 2 spawns effect radius_half and makes it circle the entity, hittable again
   after 15 frames; any other hit spawns the end effect (0x14 on surface kinds 3 and 5) and it ends
   the next frame (kind 70). */
void EffectTrailHoming_Update(EffectActor *actor) {
    HomingMotion *motion;
    Vector4 delta;
    int hit;
    int mode;
    u16 effect_id;

    EffectKeys_ApplyScale(actor);
    motion = (HomingMotion *)&actor->motion;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    if (motion->unk24 != 0) {
        Effect_Free(actor);
        return;
    }
    if (actor->unk6C != 0) {
        Vector_Subtract(&delta, ((Entity *)actor->unk6C)->model->position, &actor->model->position);
        if (motion->unk18 == 0) {
            motion->unkDC += actor->def->unkE0;
            delta.y -= ((Entity *)actor->unk6C)->height * 0.8f;
        } else {
            if (actor->frame & 1) {
                motion->unk1C = (((rand() & 0xFFFF) * 0.000030517578f) - 1.0f) * 200.0f;
                motion->unk20 = (((rand() & 0xFFFF) * 0.000030517578f) - 1.0f) * 200.0f;
            }
            delta.y -= ((Entity *)actor->unk6C)->height;
            if (func_001B0970(&delta, motion) < 0.0f) {
                delta.x += motion->unk1C;
                delta.y -= 50.0f;
                delta.z += motion->unk20;
            }
        }
        EffectSteer_Toward(&motion->velocity, &delta, actor->def->speed, motion->unkDC);
        EffectAngle_FromVector(&actor->model->rotation.x, &motion->velocity.x);
        if (ENTITY_FLAG_BITS((Entity *)actor->unk6C)->bits2 > 0) {
            actor->unk6C = (int)EffectHit_FindNearestEntity(actor, &actor->model->position, &actor->model->rotation);
        }
    }
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        sceVu0AddVector(&actor->model->position, &actor->model->position, motion);
        Effect_Spawn(actor->def->unkF2, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
    }
    motion->unk14--;
    COLLISION_RESULT->unk18 = 6;
    hit = func_001B4F90(actor);
    if (hit != 0) {
        if (hit & 2) {
            if (motion->unk14 < 0) {
                motion->unkDC = actor->def->unkF0;
                Effect_Spawn(actor->def->radius_half, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
                motion->unk18 = 1;
                motion->unk14 = 15;
            }
        } else {
            mode = COLLISION_RESULT->unk18;
            if (mode != 3) {
                effect_id = 0x14;
                if (mode != 5) {
                    effect_id = actor->def->end_effect;
                }
            } else {
                effect_id = 0x14;
            }
            Effect_Spawn(effect_id, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            motion->unk24 = 1;
        }
    }
    actor->frame++;
}

/* Starts a homing missile aimed at the entity the player faces (kind 71): raised 220 units, turning
   at EffectDef.unkDC. */
void EffectAimedHoming_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    PlayerHomingMotion *motion = (PlayerHomingMotion *)&effect->motion;
    ModelInstance *model;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    model = effect->model;
    model->position.y -= 220.0f;
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &model->rotation);
    effect->unk6C = EffectHit_FindAimedEntity(effect, &D_00413F80.position, &D_00413F80.rotation);
    motion->unkDC = effect->def->unkDC;
}

/* Homes on its entity with a turn that grows by EffectDef.unkE0, while the entity can be hit. On a
   hit frees its spawners and ends; a hit with bit 2 first spawns the end effect on the floor within
   350 (effect unkF0 if there is none) (kind 71). */
void EffectAimedHoming_Update(EffectActor *actor) {
    ScaledFallMotion *motion;
    Vector4 vec;
    int hit;
    u16 end_effect;

    EffectKeys_ApplyScale(actor);
    motion = (ScaledFallMotion *)&actor->motion;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    if (actor->unk6C != 0) {
        if (((u32)(((Entity *)actor->unk6C)->flags >> 2) & 3) == 0) {
            Vector_Subtract(&vec, &((Entity *)actor->unk6C)->model->position, &actor->model->position);
            motion->fall_speed += actor->def->unkE0;
            vec.y -= ((Entity *)actor->unk6C)->height * 0.5f;
            EffectSteer_Toward(&motion->velocity, &vec, actor->def->speed, motion->fall_speed);
        } else {
            actor->unk6C = 0;
        }
    }
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
    }
    hit = func_001B4F90(actor);
    if (hit != 0) {
        Effect_FreeSpawnersOf(actor);
        if (hit & 2) {
            Vector_Copy(&vec, &actor->model->position);
            if (func_001C0E40(350.0f, &actor->model->position) == 1) {
                vec.y = COLLISION_RESULT->point[1];
                end_effect = actor->def->end_effect;
            } else {
                end_effect = actor->def->unkF0;
            }
            Effect_Spawn(end_effect, &vec, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
        }
        Effect_Free(actor);
    }
    actor->frame += 1;
}

/* Starts a shot (kind 72): raised 220 units, flying at EffectDef.speed; keeps the frame its path
   comes within radius_half of the scenery (func_001B0208). */
void EffectShot_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    LaunchMotion *motion = (LaunchMotion *)&effect->motion;
    ModelInstance *model;
    float scratch[3];

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    model = effect->model;
    model->position.y -= 220.0f;
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &model->rotation);
    motion->unk10 = func_001B0208((float)effect->def->radius_half, effect, motion, scratch);
}

/* Flies on; on a hit or at the frame it reaches the scenery spawns the end effect and ends (kind
   72). */
void EffectShot_Update(EffectActor *effect) {
    ProjectileMotion *motion;
    ModelInstance *model;

    EffectKeys_ApplyScale(effect);
    motion = (ProjectileMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    if (func_001B4F90(effect) != 0 || motion->end_frame == effect->frame) {
        model = effect->model;
        Effect_Spawn(effect->def->end_effect, &model->position, &model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        Effect_Free(effect);
    }
    effect->frame += 1;
}

/* Starts a lobbed effect (kind 10): raised 220 units, thrown at EffectDef.speed. */
void EffectLob_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    DropMotion *motion = (DropMotion *)&effect->motion;
    ModelInstance *model;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    model = effect->model;
    model->position.y -= 220.0f;
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &model->rotation);
    motion->unk14 = 0;
    motion->unk10 = 0;
}

/* Flies under gravity, turned along its path; at the first hit spawns the end effect (0x16 on
   surface kind 5) and effect 0x122, and ends the next frame (kind 10). */
void EffectLob_Update(EffectActor *effect) {
    LandedFlagMotion *motion = (LandedFlagMotion *)&effect->motion;
    int spawn_id;

    if (motion->landed) {
        Effect_Free(effect);
        return;
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        if (effect->def->gravity != 0.0f) {
            motion->velocity.y += effect->def->gravity;
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
        EffectAngle_FromVector(&effect->model->rotation.x, &motion->velocity.x);
    }
    if (func_001B4F90(effect)) {
        spawn_id = COLLISION_RESULT->unk18 == 5 ? 0x16 : effect->def->end_effect;
        Effect_Spawn(spawn_id, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        Effect_Spawn(0x122, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        motion->landed = 1;
    }
    effect->frame += 1;
}

/* Starts a link of an effect that runs along the floor at an entity (kind 8). The first link (its
   parent's EffectDef.unkF2 is not 0) aims at the entity the player faces and has unkF2 links to go;
   later links take their parent's velocity and entity and one link less. Without a floor within
   EffectDef.unkE0 it ends at once. */
void EffectGroundChase_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    LandingMotion *motion;
    LandingMotion *target_motion;

    motion = (LandingMotion *)&actor->motion;
    target_motion = (LandingMotion *)&actor->target->motion;
    EffectStart_PlaceModel(actor, spawn_slot);
    EffectStart_TurnModel(actor, spawn_slot);
    if (actor->target->def->unkF2 != 0) {
        EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
        actor->unk6C = EffectHit_FindAimedEntity(actor, &D_00413F80.position, &D_00413F80.rotation);
        motion->unk14 = actor->target->def->unkF2;
    } else {
        Vector_Copy(motion, target_motion);
        actor->unk6C = actor->target->unk6C;
        motion->unk14 = target_motion->unk14 - 1;
        if (motion->unk14 == 0) {
            actor->frame = actor->def->keyframe_end;
        }
    }
    actor->model->position.y -= actor->def->unkE0 * 0.5f;
    if (func_001C0E40(actor->def->unkE0, &actor->model->position) == 0) {
        actor->frame = actor->def->keyframe_end;
        actor->model->position.y += actor->def->unkE0 * 0.5f;
    } else {
        actor->model->position.y = COLLISION_RESULT->point[1];
    }
    motion->state = 0;
    motion->unk10 = actor->def->radius_half;
}

/* Turns toward its entity by EffectDef.unkDC and runs along the floor, ending where there is none;
   on its first frame, within 100 of the entity, spawns the end effect and ends (kind 8). */
void EffectGroundChase_Update(EffectActor *actor) {
    LandingMotion *motion;
    Entity *target;
    Vector4 offset;

    motion = (LandingMotion *)&actor->motion;
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        target = (Entity *)actor->unk6C;
        if (target != NULL) {
            if (((int)(target->flags >> 2) & 3) == 0) { // match: the int cast gives retail's dsll/dsra32
                Vector_Subtract(&offset, &target->model->position, &actor->model->position);
                EffectSteer_Toward(&motion->velocity, &offset, actor->def->speed, actor->def->unkDC);
            } else {
                actor->unk6C = 0;
            }
        }
        actor->model->position.y -= actor->def->unkE0 * 0.5f;
        motion->velocity.w = 0;
        sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        if (func_001C0E40(actor->def->unkE0, &actor->model->position) == 0) {
            actor->frame = actor->def->keyframe_end;
            actor->model->position.y += actor->def->unkE0 * 0.5f;
        } else {
            actor->model->position.y = COLLISION_RESULT->point[1];
        }
    }
    if (motion->state == 1) {
        Effect_Free(actor);
    } else if (actor->frame == 0) {
        Vector_Copy(&offset, &actor->model->position);
        if (actor->unk6C != 0) {
            target = (Entity *)actor->unk6C;
            if (func_001B0178(&offset, &target->model->position) < 100.0f) {
                Effect_Spawn(actor->def->end_effect, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
                motion->state = 1;
            }
        }
    }
    actor->frame += 1;
}

/* Starts a falling shot (kind 11): raised 220 units, thrown at EffectDef.speed; the effects aimed
   at it are freed with it. */
void EffectFall_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    ModelInstance *model;

    motion = (TurningMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    model = actor->model;
    model->position.y -= 220.0f;
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &model->rotation);
    motion->unk10[0] = 0;
    actor->flags |= EFFECT_FLAG_HAS_TARGETERS;
}

/* Falls under gravity (no faster than 45 a frame) until its hit check reports something. On a
   surface of kind 3 or 5 it ends in effect 0x14 (and 0x16 first on kind 5); otherwise a hit with
   bit 8 ends in the end effect if there is ground within reach, any other in radius_half
   (kind 11). */
void EffectFall_Update(EffectActor *effect) {
    Vector4 unused[5];
    Vector4 position;
    EffectMotion *motion;
    int hit;

    EffectKeys_ApplyScale(effect);
    motion = &effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        motion->velocity.y += effect->def->gravity;
        if (motion->velocity.y > 45.0f) {
            motion->velocity.y = 45.0f;
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
        EffectAngle_FromVector(&effect->model->rotation.x, &motion->velocity.x);
    }
    hit = func_001B4F90(effect);
    if (hit & 8) {
        if (COLLISION_RESULT->unk18 == 3 || COLLISION_RESULT->unk18 == 5) {
            if (COLLISION_RESULT->unk18 == 5) {
                Effect_Spawn(0x16, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            }
            Effect_Spawn(0x14, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        } else {
            Vector_Copy(&position, &effect->model->position);
            position.y -= effect->def->unkDC;
            if (func_001C0E40(effect->def->unkDC + effect->def->unkDC, &position) == 1) {
                effect->model->position.y = COLLISION_RESULT->point[1];
                Effect_Spawn(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            }
        }
        Effect_Free(effect);
    } else if (hit != 0) {
        if (COLLISION_RESULT->unk18 == 3 || COLLISION_RESULT->unk18 == 5) {
            if (COLLISION_RESULT->unk18 == 5) {
                Effect_Spawn(0x16, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            }
            Effect_Spawn(0x14, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        } else {
            Effect_Spawn(effect->def->radius_half, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        }
        Effect_Free(effect);
    }
    effect->frame += 1;
}

/* Starts a chaser (kind 13): raised 220 units, aimed at the entity in front of it, with a starting
   turn that is larger the nearer the entity is (EffectDef.interval is the range, unkDC the
   most). */
void EffectChase_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    ChaseMotion *motion;
    ModelInstance *target;
    float offset[3];
    double remaining;

    motion = (ChaseMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    actor->model->position.y -= 220.0f;
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    actor->unk6C = EffectHit_FindAimedEntity(actor, &actor->model->position, &actor->model->rotation);
    motion->ended = 0;
    if (actor->unk6C != 0) {
        target = *(ModelInstance **)actor->unk6C;
        Vector_Subtract(offset, &target->position, &actor->model->position);
        remaining = (double)(float)actor->def->interval - sqrt((double)(offset[0] * offset[0] + offset[2] * offset[2]));
        motion->turn = remaining / (double)((float)actor->def->interval + (float)actor->def->interval);
        motion->turn *= actor->def->unkDC;
    } else {
        motion->turn = 0;
    }
}

/* Chases its entity, turning harder each frame by EffectDef.unkE0, and gives up (jumps to
   keyframe_end) once it has passed it; ends on a hit or at keyframe_end (kind 13). */
void EffectChase_Update(EffectActor *actor) {
    ChaseMotion *motion;
    char unused[0x40]; /* unused, but retail reserves the bytes */
    f32 delta[4];
    char unused_tail[0x10]; /* unused, but retail reserves the bytes */

    EffectKeys_ApplyScale(actor);
    motion = (ChaseMotion *)&actor->motion;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    if (actor->unk6C != 0) {
        if (((u32)(((Entity *)actor->unk6C)->flags >> 2) & 3) < 2) {
            Vector_Subtract(delta, ((Entity *)actor->unk6C)->model->position, &actor->model->position);
            if (func_001B0970(delta, motion) > 0.0f) {
                motion->turn += actor->def->unkE0;
                delta[1] -= ((Entity *)actor->unk6C)->height * 0.5f;
            } else {
                actor->frame = actor->def->keyframe_end;
            }
            EffectSteer_Toward(&motion->velocity, (Vector4 *)delta, actor->def->speed, motion->turn);
            EffectAngle_FromVector(&actor->model->rotation.x, &motion->velocity.x);
            motion->velocity.w = 0;
        } else {
            actor->unk6C = 0;
        }
    }
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        sceVu0AddVector(&actor->model->position, &actor->model->position, motion);
    }
    EffectHit_CheckAndEnd(actor, &motion->ended);
    actor->frame++;
    if (actor->frame >= actor->def->keyframe_end) {
        EffectHit_End(actor, &motion->ended);
    }
}

/* Starts an effect that runs along the floor (kind 14): flies at EffectDef.speed and stands on the
   floor it finds below within unkDC, or ends next frame without one. With EffectDef.unkF2 set it
   skips all that and only waits for a hit. */
void EffectGroundRun_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    GroundRunMotion *motion = (GroundRunMotion *)&effect->motion;
    ModelInstance *model;
    float start_y;
    float height;
    float half_height;

    EffectStart_PlaceModel(effect, spawn_slot);
    EffectStart_TurnModel(effect, spawn_slot);
    if (effect->def->unkF2 == 0) {
        EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
        height = effect->def->unkDC;
        half_height = height * 0.5f;
        motion->ended = 0;
        motion->height = height;
        motion->fall_speed = 0;
        motion->probe.x = 0;
        motion->probe.y = effect->def->unkDC;
        motion->probe.z = 0;
        motion->probe.w = 0;
        model = effect->model;
        start_y = model->position.y;
        model->position.y -= half_height;
        func_001C1430(1.0f, &model->position, &motion->probe);
        effect->model->position.y = start_y;
        if (COLLISION_RESULT->hit == 0) {
            motion->ended = 1;
        } else {
            effect->model->position.y = COLLISION_RESULT->point[1];
        }
        motion->hit_offset = effect->def->unkE0;
    }
}

/* Runs along the ground: moves level, follows the ground up at once and down by falling, and
   between frames radius_half and interval checks for hits hit_offset below itself. On a surface of
   kind 3 or 5 and on a hit it ends, on such a surface in effect unkF0 and effect 0x123
   (kind 14). */
void EffectGroundRun_Update(EffectActor *effect) {
    Vector4 unused; /* unused, but retail reserves the bytes */
    GroundRunMotion *motion = (GroundRunMotion *)&effect->motion;

    if (motion->ended) {
        Effect_Free(effect);
        return;
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->def->unkF2 == 0) {
        if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
            motion->velocity.w = 0;
            sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
            effect->model->position.y -= motion->height * 0.5f;
            func_001C1430(1.0f, &effect->model->position, &motion->probe);
            if (COLLISION_RESULT->hit == 0) {
                effect->model->position.y += motion->height * 0.5f;
                Effect_Free(effect);
                return;
            }
            if (COLLISION_RESULT->unk18 == 3 || COLLISION_RESULT->unk18 == 5) {
                Effect_Spawn(effect->def->unkF0, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                Effect_Spawn(0x123, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                Effect_Free(effect);
                return;
            }
            effect->model->position.y += motion->height * 0.5f;
            if (effect->model->position.y < COLLISION_RESULT->point[1]) {
                motion->fall_speed += effect->def->gravity;
                effect->model->position.y += motion->fall_speed;
            } else if (COLLISION_RESULT->point[1] < effect->model->position.y - 5.0f) {
                effect->model->position.y -= 5.0f;
            } else {
                motion->fall_speed = 0;
                effect->model->position.y = COLLISION_RESULT->point[1];
            }
        }
        if (effect->frame >= effect->def->radius_half && effect->frame <= effect->def->interval) {
            effect->model->position.y -= motion->hit_offset;
            if (func_001B4F90(effect) & 0x14) {
                if (COLLISION_RESULT->unk18 == 3 || COLLISION_RESULT->unk18 == 5) {
                    Effect_Spawn(effect->def->unkF0, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                    Effect_Spawn(0x123, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                }
                Effect_Free(effect);
            }
            effect->model->position.y += motion->hit_offset;
        }
    } else {
        EffectHit_CheckAndEnd(effect, &motion->ended);
    }
    effect->frame += 1;
}

/* Places and turns the model as the definition says (kind 9). */
void EffectGroundRunWait_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectStart_TurnModel(effect, spawn_slot);
}

/* Lives at least 6 frames and then as long as any effect of kind 14 (EffectGroundRun) does (kind
   9). */
void EffectGroundRunWait_Update(EffectActor *actor) {
    EffectActor *slot;
    int i;

    slot = D_003F3550;
    if (actor->frame < 6) {
        actor->frame++;
        return;
    }
    for (i = 0; i < 0x180; i++, slot++) {
        if (slot->flags & (0x8000L << 41)) {
            if (slot->def->unk100 == 0xE) {
                actor->frame++;
                return;
            }
        }
    }
    Effect_Free(actor);
}

/* Starts a shot that skims the floor (kind 15): flies at EffectDef.speed and keeps the frame its
   path meets the scenery. */
void EffectFloorSkim_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    TurningMotion *motion;

    motion = (TurningMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    motion->unk20 = func_001B0208(30.0f, actor, motion, motion->unk10);
    motion->unk25 = 0;
    motion->unk22 = 0;
    motion->unk24 = 0;
}

/* Flies on; where it touches the floor (hit bit 8) it leaves a mark there at most every radius_half
   frames: the end effect with effect 0x33, or on surface kind 5 a splash (0x16, or 0x19 for a big
   model) with 0x122 every other time. A hit with bit 2 ends it with the end effect and 0x33 when
   EffectDef.unkF2 is set (kind 15). */
void EffectFloorSkim_Update(EffectActor *effect) {
    TurningMotion *motion;
    Vector4 point;
    int hits;
    int spawn_count;
    int spawn_id;

    motion = (TurningMotion *)&effect->motion;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    if ((s8)motion->unk25 != 0) { // match: retail loads unk25 as a signed byte
        Effect_Free(effect);
    } else {
        motion->unk22 -= 1;
        hits = func_001B4F90(effect);
        if (hits != 0) {
            if ((hits & 2) && effect->def->unkF2 != 0) {
                Effect_Spawn(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                Effect_Spawn(0x33, &point, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                motion->unk25 = 1;
            } else if ((hits & 8) && motion->unk22 < 0) {
                Vector_Copy(&point, &effect->model->position);
                point.y -= 100.0f;
                point.y = (func_001C0E40(200.0f, &point) == 1) ? COLLISION_RESULT->point[1] : effect->model->position.y;
                if (COLLISION_RESULT->unk18 == 5) {
                    if (3.0f < effect->model->unkE8) {
                        spawn_id = 0x19;
                        D_004468DC = 0;
                    } else {
                        spawn_id = 0x16;
                    }
                    Effect_Spawn(spawn_id, &point, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                    spawn_count = D_004468DC + 1;
                    D_004468DC = spawn_count;
                    if (spawn_count & 1) {
                        Effect_Spawn(0x122, &point, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                    }
                } else {
                    Effect_Spawn(effect->def->end_effect, &point, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                    D_004468DC += 1;
                    if (3.0f < effect->model->unkE8) {
                        D_004468DC = 0;
                    }
                    if ((s8)motion->unk24 == 0 || (D_004468DC & 1) == 0) {
                        Effect_Spawn(0x33, &point, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                        motion->unk24 = 1;
                    }
                }
                motion->unk22 = effect->def->radius_half;
            }
        }
    }
    effect->frame += 1;
}

/* Starts an effect that winds along the floor (kind 16): flies at EffectDef.speed and stands on the
   floor within 1.5 unkDC below, or ends next frame without one. */
void EffectGroundSway_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    GroundSwayMotion *motion;
    float point[3];

    motion = (GroundSwayMotion *)&effect->motion;
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectStart_TurnModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->height = effect->def->unkDC;
    motion->fall_speed = 0.0f;
    motion->sway_rate = effect->def->unkE0;
    motion->angle_speed = effect->def->unkE4;
    motion->angle = 1.57f;
    motion->ended = FALSE;
    Vector_Copy(point, &effect->model->position);
    point[1] -= motion->height * 1.5f;
    if (func_001C0E40(motion->height * 3.0f, point) == 0) {
        motion->ended = TRUE;
    } else {
        effect->model->position.y = COLLISION_RESULT->point[1];
    }
}

/* Moves along the floor, swaying from side to side by the sine of a growing angle and falling to
   the floor under gravity; ends where there is no floor or on a hit with bit 2 or 4 (checked 110
   above itself); starts its sound on the first frame (kind 16). */
void EffectGroundSway_Update(EffectActor *effect) {
    GroundSwayMotion *motion;
    Vector4 offset;
    float sine;
    float angle;
    int hits;
    int result;

    motion = (GroundSwayMotion *)&effect->motion;
    if (motion->ended != 0) {
        Effect_Free(effect);
        return;
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    motion->velocity.y = -(motion->height * 0.5f);
    motion->velocity.w = 0.0f;
    offset.y = 0.0f;
    offset.x = -motion->velocity.z;
    offset.z = effect->motion.velocity.x;
    motion->angle_speed += effect->def->unkE8;
    motion->angle += motion->angle_speed;
    angle = motion->sway_rate * motion->angle;
    angle -= (float)(int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    motion->angle = angle;
    sine = (float)sin((double)angle);
    Vector_Scale(&offset, &offset, sine);
    sceVu0AddVector(&effect->model->position, &effect->model->position, &offset);
    sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    if (func_001C0E40(motion->height, &effect->model->position) == 0) {
        effect->model->position.y += motion->height * 0.5f;
        effect->frame = effect->def->keyframe_end;
    } else {
        effect->model->position.y += motion->height * 0.5f;
        if (effect->model->position.y < COLLISION_RESULT->point[1]) {
            motion->fall_speed += effect->def->gravity;
            effect->model->position.y += motion->fall_speed;
        } else {
            motion->fall_speed = 0.0f;
            effect->model->position.y = COLLISION_RESULT->point[1];
        }
    }
    effect->model->position.y -= 110.0f;
    hits = func_001B4F90(effect);
    effect->model->position.y += 110.0f;
    if (hits & 0x14) {
        Effect_Free(effect);
        return;
    }
    if (effect->frame == 0) {
        result = Sound_PlayTrackedKind(1, 10, 0, (int)&D_00413F80, EFFECT_FLAG_PARTS(effect)->unk46);
        EFFECT_FLAG_PARTS(effect)->unk46 = result;
        if ((s8)result > 0) {
            D_004468C8 += 1;
        }
    }
    effect->frame += 1;
}

/* Starts an effect that runs along the floor leaving effects (kind 12): flies at EffectDef.speed
   and stands on the floor within 1.5 unkDC below, or ends next frame without one. */
void EffectGroundTrail_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    GroundTrailMotion *motion;
    float point[3];

    motion = (GroundTrailMotion *)&effect->motion;
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectStart_TurnModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->height = effect->def->unkDC;
    motion->fall_speed = 0.0f;
    motion->ended = FALSE;
    Vector_Copy(point, &effect->model->position);
    point[1] -= motion->height * 1.5f;
    if (func_001C0E40(motion->height * 3.0f, point) == 0) {
        motion->ended = TRUE;
    } else {
        effect->model->position.y = COLLISION_RESULT->point[1];
    }
}

/* Spawns effect_id from the effect at a point distance ahead of the model along its heading. */
static void EffectGroundTrail_SpawnAhead(EffectActor *effect, int effect_id, float distance) {
    Vector4 offset;
    Vector4 spawn_position;
    float rotation_matrix[16];

    offset.z = distance;
    offset.x = 0;
    offset.y = 0;
    offset.w = 0;
    sceVu0RotMatrixY(rotation_matrix, D_00431AF0, effect->model->rotation.y);
    sceVu0ApplyMatrix(&spawn_position.x, rotation_matrix, &offset.x);
    Vector_Add(&spawn_position, &spawn_position, &effect->model->position);
    Effect_SpawnFromEffectAt(effect_id, effect, &spawn_position);
}

/* Moves along the floor, falling to it under gravity; every other frame spawns effect radius_half
   unkF0 ahead and effect interval unkF2 behind. Ends where there is no floor or on a hit with bit 2
   or 4 (checked 110 above itself); starts its sound on the first frame (kind 12). */
void EffectGroundTrail_Update(EffectActor *effect) {
    f32 unused_matrix[16]; // match: retail reserves 0x60 bytes of stack it never uses
    Vector4 unused_offset;
    Vector4 unused_position;
    GroundTrailMotion *motion;
    int hits;
    int result;

    motion = (GroundTrailMotion *)&effect->motion;
    if (motion->ended) {
        Effect_Free(effect);
        return;
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    motion->velocity.y = -(motion->height * 0.5f);
    motion->velocity.w = 0.0f;
    Vector_Add(&effect->model->position, &effect->model->position, &motion->velocity);
    if (effect->frame & 1) {
        EffectGroundTrail_SpawnAhead(effect, effect->def->radius_half, (f32)effect->def->unkF0);
        EffectGroundTrail_SpawnAhead(effect, effect->def->interval, (f32)-effect->def->unkF2);
    }
    if (func_001C0E40(motion->height, &effect->model->position) == 0) {
        effect->model->position.y += motion->height * 0.5f;
        effect->frame = effect->def->keyframe_end;
    } else {
        effect->model->position.y += motion->height * 0.5f;
        if (effect->model->position.y < COLLISION_RESULT->point[1]) {
            motion->fall_speed += effect->def->gravity;
            effect->model->position.y += motion->fall_speed;
        } else {
            motion->fall_speed = 0.0f;
            effect->model->position.y = COLLISION_RESULT->point[1];
        }
    }
    effect->model->position.y -= 110.0f;
    hits = func_001B4F90(effect);
    effect->model->position.y += 110.0f;
    if (hits & 0x14) {
        Effect_Free(effect);
        return;
    }
    if (effect->frame == 0) {
        result = Sound_PlayTrackedKind(1, 10, 0, (int)&D_00413F80, EFFECT_FLAG_PARTS(effect)->unk46);
        EFFECT_FLAG_PARTS(effect)->unk46 = result;
        if ((s8)result > 0) {
            D_004468C8 += 1;
        }
    }
    effect->frame += 1;
}

/* Starts an effect that stands on the floor (kind 17): puts the model on the floor within
   EffectDef.unkDC, or ends at once without one. */
void EffectOnFloor_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    SpeedMotion *motion;
    Vector4 position;

    motion = (SpeedMotion *)&effect->motion;
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectStart_TurnModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk14 = 0;
    motion->speed = effect->def->unkDC;
    Vector_Copy(&position, &effect->model->position);
    position.y -= motion->speed * 0.5f;
    if (func_001C0E40(motion->speed, &position.x) == 0) {
        effect->frame = effect->def->keyframe_end;
    } else {
        effect->model->position.y = COLLISION_RESULT->point[1];
    }
    motion->flag18 = 0;
}

/* Only counts the frame (kind 17). */
void EffectOnFloor_Update(EffectActor *effect) {
    effect->frame++;
}

/* Starts a thrown effect (kind 18): level, EffectDef.speed forward and unkDC vertical; spawns
   effect unkF2 from it if set; the effects aimed at it are freed with it. */
void EffectArc_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    SpawnMotion *motion;
    char stack_reserve[0x10]; /* match: retail reserves 0x10 bytes of stack here */

    motion = (SpawnMotion *)&effect->motion;
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectStart_TurnModel(effect, spawn_slot);
    effect->model->rotation.x = 0.0f;
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->velocity.y = effect->def->unkDC;
    if (effect->def->unkF2 != 0) {
        Effect_SpawnFromEffect(effect->def->unkF2, effect);
    }
    motion->unk10 = 0;
    effect->flags |= EFFECT_FLAG_HAS_TARGETERS;
}

/* Flies under gravity, turned along its path; in its first 20 frames a hit spawns the end effect
   (effect unkF0 and 0x122 on surface kinds 3 and 5) and it ends the next frame (kind 18). */
void EffectArc_Update(EffectActor *actor) {
    LiftedMotion *motion;
    u16 end_effect;
    char pad[0x10]; /* unused, but retail reserves the 0x10 bytes */

    EffectKeys_ApplyScale(actor);
    motion = (LiftedMotion *)&actor->motion;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        motion->velocity.y += actor->def->gravity;
        sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        EffectAngle_FromVector(&actor->model->rotation.x, &motion->velocity.x);
    }
    if (actor->frame < 0x14) {
        if (motion->unk10 != 0) {
            Effect_Free(actor);
        } else if (func_001B4F90(actor) != 0) {
            if (COLLISION_RESULT->unk18 == 3 || COLLISION_RESULT->unk18 == 5) {
                end_effect = actor->def->unkF0;
                Effect_Spawn(0x122, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            } else {
                end_effect = actor->def->end_effect;
            }
            Effect_Spawn(end_effect, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            motion->unk10 = 1;
        }
    }
    actor->frame++;
}

/* Starts a leading effect (kind 19), which kind 20 (EffectFollow) trails: flies at
   EffectDef.speed. */
void EffectLead_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    SpeedFlagMotion *motion;

    motion = (SpeedFlagMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->speed = effect->def->unkDC;
    motion->flag14 = 0;
}

/* Flies on; from the second frame it ends on a hit (EffectHit_CheckAndEnd), within 150 of an entity or
   after radius_half frames, which tells its followers to dive (kind 19). */
void EffectLead_Update(EffectActor *effect) {
    SpeedFlagMotion *motion = (SpeedFlagMotion *)&effect->motion;
    EffectDef *def;
    u16 max_frames;
    int expired;
    f32 unused[4]; // match: retail reserves 0x10 bytes of stack it never uses

    if ((s8)motion->flag14 != 0) {
        Effect_Free(effect);
        return;
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    def = effect->def;
    expired = def->radius_half < effect->frame;
    max_frames = def->max_frames;
    if (effect->frame < max_frames || max_frames == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    if (effect->frame != 0) {
        EffectHit_CheckAndEnd(effect, &motion->flag14);
        if (func_001B00A0(150.0f, &effect->model->position)) {
            motion->flag14 = 1;
        }
        if (expired) {
            motion->flag14 = 1;
        }
    }
    effect->frame++;
}

/* Starts an effect that trails its parent, a kind 19 EffectLead (kind 20). */
void EffectFollow_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;

    motion = (TurningMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk24 = 0;
    motion->unk26 = 0;
    motion->unk25 = 0;
}

/* Looks for the floor up to 700 below position; if there is one, sets the aim 600 above it under
   the parent and returns 1, else returns 0. */
static int EffectFollow_AimAboveFloor(EffectActor *effect, float *position, AimMotion *motion) {
    Vector4 local;
    ModelInstance *target_model;
    float target_z;

    Vector_Copy(&local, position);
    local.y += -300.0f;
    if (func_001C0E40(1000.0f, &local.x)) {
        target_model = effect->target->model;
        motion->target_x = target_model->position.x;
        motion->unk14 = COLLISION_RESULT->point[1] - 600.0f;
        target_z = target_model->position.z;
        motion->flag25 = 1;
        motion->target_z = target_z;
        return 1;
    }
    return 0;
}

/* Trails its leader, turning harder the nearer it is; once the leader has stopped flies to a point
   600 above the floor under the leader (or under itself; with neither it heads for the leader and
   ends soon) and spawns the end effect within 50 of it (kind 20). */
void EffectFollow_Update(EffectActor *effect) {
    Vector4 to_aim;
    AimMotion *motion;
    SpeedFlagMotion *target_motion;
    Vector4 *aim;
    f32 distance;
    s8 reached;

    motion = (AimMotion *)&effect->motion;
    target_motion = (SpeedFlagMotion *)&effect->target->motion;
    if (motion->ended == 1) {
        Effect_Free(effect);
        return;
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (motion->aimed == 0 && (s8)target_motion->flag14 != 0) {
        motion->aimed = 1;
        if (!EffectFollow_AimAboveFloor(effect, &effect->target->model->position.x, motion) && !EffectFollow_AimAboveFloor(effect, &effect->model->position.x, motion)) {
            Vector_Copy(&motion->target_x, &effect->target->model->position);
            effect->frame = effect->def->keyframe_end - 4;
        }
    }
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        aim = (Vector4 *)&motion->target_x;
        distance = func_001B0178(&effect->model->position, aim);
        if (motion->aimed == 0) {
            Vector_Subtract(&to_aim, &effect->target->model->position, &effect->model->position);
            motion->turn = 3000.0f / distance * 8.0f;
            if (motion->turn < 8.0f) {
                motion->turn = 8.0f;
            }
        } else {
            Vector_Subtract(&to_aim, aim, &effect->model->position);
            motion->turn = 65535.0f;
        }
        if (effect->frame >= 13) {
            EffectSteer_Toward(&motion->velocity, &to_aim, effect->def->speed, motion->turn);
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        if (motion->aimed == 1) {
            reached = motion->flag25; // match: a local keeps this test apart from the one on aimed
            if (reached == 1 && distance < 50.0f) {
                Effect_Spawn(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                motion->ended = 1;
            }
        }
    }
    effect->frame++;
}

/* Picks the four nearest entities in range more than 10 below the effect and within 45 degrees of
   straight down, nearest first, for the kind 21 effects (EffectBeam) it spawns (kind 28). */
void EffectPickTargets_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    PickedMotion *motion;
    Entity *entity;
    f32 distance;
    f32 height;
    int list_index;
    int slot;
    int moved;

    motion = (PickedMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    for (slot = 0; slot < 4; slot++) {
        motion->list_index[slot] = -1;
        motion->entity[slot] = NULL;
        motion->distance[slot] = 65535.0f;
    }
    for (entity = D_004467E0, list_index = 0; entity != NULL; entity = entity->next, list_index++) {
        if (func_001B0040(entity) != 0) {
            continue;
        }
        distance = func_001B0178(&ENTITY_MODEL(entity)->position, &effect->model->position);
        if ((f32)effect->def->interval < distance) {
            continue;
        }
        height = ENTITY_MODEL(entity)->position.y - effect->model->position.y;
        if (height <= 10.0f) {
            continue;
        }
        if (func_001B01B0(&ENTITY_MODEL(entity)->position.x, &effect->model->position.x) / height > 1.0f) {
            continue;
        }
        for (slot = 0; slot < 4; slot++) {
            if (distance < motion->distance[slot]) {
                for (moved = 3; moved > slot; moved--) {
                    motion->list_index[moved] = motion->list_index[moved - 1];
                    motion->distance[moved] = motion->distance[moved - 1];
                    motion->entity[moved] = motion->entity[moved - 1];
                }
                motion->list_index[slot] = list_index;
                motion->distance[slot] = distance;
                motion->entity[slot] = entity;
                break;
            }
        }
    }
}

/* Only counts the frame (kind 28). */
void EffectPickTargets_Update(EffectActor *effect) {
    effect->frame++;
}

/* Sets angles_out to the pitch and heading from origin toward aim_point. */
static void EffectBeam_AnglesToward(void *angles_out, void *aim_point, void *origin) {
    Vector4 difference;

    Vector_Subtract(&difference, aim_point, origin);
    EffectAngle_FromVector(angles_out, &difference.x);
}

/* Starts a beam from its parent (a kind 28 EffectPickTargets) to the entity the parent picked for
   this firing, or to the scenery in a random direction below within 1500; with neither it ends at
   once (kind 21). */
void EffectBeam_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    ScalingEndMotion *motion = (ScalingEndMotion *)&actor->motion;
    PickedMotion *picked = (PickedMotion *)&actor->target->motion;
    Entity *chosen;
    f32 offset[4];

    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    motion->ended = 0;
    motion->scale_per_mille = 0;
    chosen = picked->entity[EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level - 1];
    if (chosen != NULL) {
        actor->unk6C = (int)chosen;
        motion->scale_per_mille = picked->distance[EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level - 1];
        Vector_Copy(&motion->end_position, chosen->model->position);
        EffectBeam_AnglesToward(&actor->model->rotation, ((Entity *)actor->unk6C)->model->position, &actor->model->position);
        return;
    }
    offset[0] = ((rand() & 0xFFFF) * 3.0517578e-05f - 1.0f) * 0.7f;
    offset[1] = 1.0f;
    offset[2] = ((rand() & 0xFFFF) * 3.0517578e-05f - 1.0f) * 0.7f;
    func_001C1430(1500.0f, &actor->model->position, offset);
    if (COLLISION_RESULT->hit != 0) {
        EffectBeam_AnglesToward(&actor->model->rotation, COLLISION_RESULT->point, &actor->model->position);
        motion->scale_per_mille = func_001B0178(COLLISION_RESULT->point, &actor->model->position);
        Vector_Copy(&motion->end_position, COLLISION_RESULT->point);
    } else {
        motion->ended = 1;
    }
}

/* Stretches the model along z to the beam's length (the model is 1000 long) and spawns the end
   effect at the far end on frame radius_half (kind 21). */
void EffectBeam_Update(EffectActor *effect) {
    ScalerMotion *motion;
    ModelInstance *model;
    EffectDef *def;

    motion = (ScalerMotion *)&effect->motion;
    if ((s8)((TurningMotion *)motion)->unk24 != 0) {
        Effect_Free(effect);
        return;
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    model = effect->model;
    def = effect->def;
    model->unkE8 *= motion->scale_per_mille / 1000.0f;
    if (effect->frame == def->radius_half) {
        Effect_Spawn(def->end_effect, &effect->motion.unk10, &model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
    }
    effect->frame += 1;
}

/* Starts a link of a chain that steps toward an entity (kind 22). The first link (its parent's
   EffectDef.unkF2 is not 0) aims at the entity the player faces and has unkF2 links to go, ending
   at once on a hit; later links take their parent's entity and one link less. Turns its velocity
   toward the entity and finds where its step meets the scenery. */
void EffectHomingChain_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    AimingMotion *target_motion = (AimingMotion *)&actor->target->motion;
    AimingMotion *motion = (AimingMotion *)&actor->motion;
    Vector4 offset;
    s32 timer;

    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    if (actor->target->def->unkF2 != 0) {
        actor->unk6C = EffectHit_FindAimedEntity(actor, &D_00413F80.position, &D_00413F80.rotation);
        motion->timer = actor->target->def->unkF2;
        if (EffectHit_Check(actor)) {
            Effect_Spawn(actor->def->end_effect, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            Effect_Free(actor);
            return;
        }
    } else {
        timer = target_motion->timer - 1;
        actor->unk6C = actor->target->unk6C;
        motion->timer = timer;
    }
    if (actor->unk6C != 0) {
        Vector_Subtract(&offset, &((Entity *)actor->unk6C)->model->position, &actor->model->position);
        offset.y -= ((Entity *)actor->unk6C)->height * 0.5f;
        EffectSteer_Toward(&motion->velocity, &offset, actor->def->speed, actor->def->unkDC);
    }
    motion->flags = 0xFFFF;
    func_001C1430(1.0f, &actor->model->position, motion);
    if (COLLISION_RESULT->hit != 0) {
        Vector_Copy(&motion->aim, COLLISION_RESULT->point);
        motion->flags = 0;
    }
}

/* On its first frame takes its step; then spawns the end effect on a hit, or where the step met the
   scenery, or else the next link (effect radius_half while links are left, unkF0 for the last)
   (kind 22). */
void EffectHomingChain_Update(EffectActor *effect) {
    SelectMotion *motion;

    EffectKeys_ApplyScale(effect);
    motion = (SelectMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        EffectAngle_FromVector(&effect->model->rotation.x, &motion->velocity.x);
        if (EffectHit_Check(effect) != 0) {
            Effect_Spawn(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        } else {
            if (motion->unk24 == 0) {
                Effect_Spawn(effect->def->end_effect, &motion->unk10, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            } else {
                u16 effect_id;

                if (motion->unk20 > 0) {
                    effect_id = effect->def->radius_half;
                } else {
                    effect_id = effect->def->unkF0;
                }
                Effect_SpawnFromEffect(effect_id, effect);
            }
        }
    }
    effect->frame++;
}

/* Starts one effect of a fan (kind 23): EffectDef.speed pitched by unkE0 and rolled by unkDC per
   firing round the spawner's heading, drifting by unkE8 along that heading. */
void EffectFan_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    ScalingEndMotion *motion;
    f32 matrix[16];
    f32 angle;
    f32 step;

    motion = (ScalingEndMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    actor->motion.velocity.x = motion->velocity.y = motion->velocity.w = 0.0f;
    motion->velocity.z = actor->def->unkE8;
    sceVu0RotMatrixY(matrix, D_00431AF0, spawn_slot->unk10.y);
    sceVu0ApplyMatrix(&motion->end_position.x, matrix, &motion->velocity.x);
    actor->motion.velocity.x = motion->velocity.y = motion->velocity.w = 0.0f;
    motion->velocity.z = actor->def->speed;
    sceVu0RotMatrixX(matrix, D_00431AF0, actor->def->unkE0);
    step = (actor->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level) * actor->def->unkDC;
    angle = actor->model->rotation.z + step;
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    sceVu0RotMatrixZ(matrix, matrix, angle);
    sceVu0RotMatrixY(matrix, matrix, spawn_slot->unk10.y);
    sceVu0ApplyMatrix(&motion->velocity.x, matrix, &motion->velocity.x);
    motion->scale_per_mille = 1.0f;
}

/* Adds the drift to the velocity, scales it by a factor that grows by scale_per_frame and moves
   (kind 23). */
void EffectFan_Update(EffectActor *effect) {
    DriftMotion *motion;
    EffectDef *def;

    EffectKeys_ApplyScale(effect);
    motion = (DriftMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    def = effect->def;
    if (effect->frame < def->max_frames || def->max_frames == 0) {
        motion->unk20 += def->scale_per_frame;
        Vector_Add(&motion->velocity, &motion->end_position, &motion->velocity);
        Vector_Scale(&motion->velocity.x, &motion->velocity.x, motion->unk20);
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    effect->frame += 1;
}

/* Starts one effect of a fan of bobbing homers (kind 24): EffectDef.speed along the heading turned
   by unkDC per firing, a random bob; it, not its parent, frees the effects aimed at it. */
void EffectBobHoming_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    HomingBobMotion *motion;
    f32 rotation[16];
    f32 angle;
    f32 *identity;
    EffectDef *def;

    EffectStart_TurnModel(actor, spawn_slot);
    motion = (HomingBobMotion *)&actor->motion;
    EffectStart_PlaceModel(actor, spawn_slot);
    def = actor->def;
    identity = D_00431AF0;
    actor->motion.velocity.x = motion->velocity.y = motion->velocity.w = 0.0f;
    motion->velocity.z = def->speed;
    angle = actor->model->rotation.y + (def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level) * def->unkDC;
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    sceVu0RotMatrixY(rotation, identity, angle);
    sceVu0ApplyMatrix(&motion->velocity, rotation, &motion->velocity);
    actor->flags |= EFFECT_FLAG_HAS_TARGETERS;
    actor->target->flags &= ~EFFECT_FLAG_HAS_TARGETERS;
    motion->bob_speed = (((rand() & 0xFFFF) * 0.000030517578125f) - 1.0f) * 0.5f;
    motion->bob_limit = (((rand() & 0xFFFF) * 0.000030517578125f) - 1.0f) * 0.1f + 0.6f;
    motion->bob_rising = FALSE;
    actor->unk6C = 0;
    motion->velocity_scale = 1.0f;
    motion->end_countdown = 0;
    motion->countdown = 0;
}


/* Spawns effect 0xDF unless an effect of its kind did in the last 5 frames. */
static void EffectBobHoming_SpawnSharedBurst(EffectActor *effect) {
    HomingBobMotion *motion;
    EffectActor *other;
    int i;
    s64 flag;  // match: kept in a register across the loop
    int five;  // match: retail keeps the constant 5 in a register set before the pool index

    motion = (HomingBobMotion *)&effect->motion;
    if (motion->countdown != 0) {
        return;
    }
    flag = EFFECT_FLAG_LIVE;
    five = 5;
    other = D_003F3550;
    for (i = EFFECT_COUNT - 1; i >= 0; i--) {
        if ((other->flags & flag) && other->def->unk100 == effect->def->unk100) {
            ((HomingBobMotion *)&other->motion)->countdown = five;
        }
        other++;
    }
    Effect_Spawn(0xDF, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
}

/* Spawns the end effect and the shared burst, and marks the effect ended. */
static void EffectBobHoming_End(EffectActor *effect) {
    HomingBobMotion *motion;
    ModelInstance *model;

    motion = (HomingBobMotion *)&effect->motion;
    model = effect->model;
    Effect_Spawn(effect->def->end_effect, &model->position, &model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
    EffectBobHoming_SpawnSharedBurst(effect);
    motion->ended = TRUE;
}

/* Per-frame update of a bobbing effect that speeds up along its heading, then homes on the nearest
   entity; when it ends, the other effects of its group end one after another, by distance (kind
   24). */
void EffectBobHoming_Update(EffectActor *effect) {
    Vector4 step;
    Vector4 to_target;
    HomingBobMotion *motion;
    EffectActor *other;
    int i;

    motion = (HomingBobMotion *)&effect->motion;
    if (motion->countdown != 0) {
        motion->countdown--;
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        motion->velocity_scale += effect->def->scale_per_frame;
        Vector_Scale(&step, &motion->velocity, motion->velocity_scale);
        sceVu0AddVector(&effect->model->position, &effect->model->position, &step);
    }
    motion->bob_speed += motion->bob_rising != 0 ? 0.01f : -0.01f;
    if (motion->bob_limit < motion->bob_speed) {
        motion->bob_rising = FALSE;
    }
    if (motion->bob_speed < -motion->bob_limit) {
        motion->bob_rising = TRUE;
    }
    effect->model->position.y += motion->bob_speed;
    if (effect->frame == effect->def->max_frames && effect->def->interval != 0) {
        // match: retail passes the rotation as a third argument the function does not take
        effect->unk6C = (int)((Entity *(*)(EffectActor *, Vector4 *, Vector4 *))EffectHit_FindNearestEntity)(effect, &effect->model->position, &effect->model->rotation);
        motion->turn = 0.0f;
    }
    if (effect->unk6C != 0) {
        if (((s32)(((EffectTarget *)effect->unk6C)->flags >> 2) & 3) == 0) {
            Vector_Subtract(&to_target, &((EffectTarget *)effect->unk6C)->model->position, &effect->model->position);
            to_target.y -= ((EffectTarget *)effect->unk6C)->height * 0.5f;
            if (effect->def->unkE0 < motion->turn) {
                motion->turn = effect->def->unkE0;
            } else {
                motion->turn += 0.1; /* the double is D_0043EF50 */
            }
            EffectSteer_Toward(&motion->velocity, &to_target, motion->turn, 100.0f);
            sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        } else {
            effect->unk6C = 0;
        }
    }
    if (motion->ended != 0) {
        other = D_003F3550;
        for (i = 0; i < 0x180; i++, other++) {
            if ((other->flags & EFFECT_FLAG_LIVE) && other->target == effect->target && ((HomingBobMotion *)&other->motion)->end_countdown == 0 && other != effect) {
                ((HomingBobMotion *)&other->motion)->end_countdown = (int)(func_001B0178(&other->model->position, &effect->model->position) / 20.0f);
            }
        }
        Effect_Free(effect);
        return;
    }
    if (effect->frame >= 3 && effect->frame < effect->def->keyframe_end - 3 && EffectHit_Check(effect) != 0) {
        EffectBobHoming_End(effect);
    }
    if (motion->ended == 0) {
        if (motion->end_countdown > 0) {
            motion->end_countdown--;
            if (motion->end_countdown == 0) {
                EffectBobHoming_End(effect);
            }
        }
        if (motion->ended == 0 && effect->frame == effect->def->keyframe_end - 1) {
            EffectBobHoming_End(effect);
        }
    }
    effect->frame++;
}

/* Starts a bobbing effect (kind 37): EffectDef.speed along its heading, a random bob speed and
   limit; the effects aimed at it are freed with it. */
void EffectBobFly_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    BobbingGrowMotion *motion;
    float random_fraction;
    float bob_limit;

    motion = (BobbingGrowMotion *)&effect->motion;
    ((void (*)(void))EffectStart_TurnModel)(); // match: retail calls with no arguments
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    effect->flags |= EFFECT_FLAG_HAS_TARGETERS;
    motion->bob_speed = (((float)(rand() & 0xFFFF) * 0.000030517578f) - 1.0f) * 0.5f;
    random_fraction = (float)(rand() & 0xFFFF);
    bob_limit = ((random_fraction * 0.000030517578f) - 1.0f) * 0.1f + 0.6f;
    motion->velocity_scale = 1.0f;
    motion->bob_rising = FALSE;
    motion->bob_limit = bob_limit;
}

/* Speeds up along its heading while bobbing up and down; between frames 3 and 47 it ends on a hit,
   spawning effect 0xDF too (kind 37). */
void EffectBobFly_Update(EffectActor *effect) {
    BobbingGrowMotion *motion;
    Vector4 step;
    EffectDef *def;
    u16 max_frames;

    motion = (BobbingGrowMotion *)&effect->motion;
    if (motion->ended != 0) {
        Effect_Free(effect);
        return;
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    def = effect->def;
    max_frames = def->max_frames;
    if (effect->frame < max_frames || max_frames == 0) {
        motion->velocity_scale += def->scale_per_frame;
        Vector_Scale(&step, &motion->velocity, motion->velocity_scale);
        sceVu0AddVector(&effect->model->position, &effect->model->position, &step);
    }
    motion->bob_speed += motion->bob_rising != 0 ? 0.01f : -0.01f;
    if (motion->bob_limit < motion->bob_speed) {
        motion->bob_rising = FALSE;
    }
    if (motion->bob_speed < -motion->bob_limit) {
        motion->bob_rising = TRUE;
    }
    effect->model->position.y += motion->bob_speed;
    if (effect->frame >= 3 && effect->frame < 48) {
        EffectHit_CheckAndEnd(effect, &motion->ended);
        if (motion->ended != 0) {
            Effect_Spawn(0xDF, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        }
    }
    effect->frame++;
}

/* Starts an effect that hovers round the player, at a random point in the box the definition gives,
   before it flies off (kind 25); the effects aimed at it are freed with it. */
void EffectHoverShot_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    EndAtFrameMotion *motion;

    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    motion = (EndAtFrameMotion *)&actor->motion;
    motion->end_position.x = actor->def->offset_base[0] + ((rand() & 0xFFFF) * (1.0f / 32768) - 1.0f) * actor->def->offset_spread[0];
    motion->end_position.y = actor->def->offset_base[1] + ((rand() & 0xFFFF) * (1.0f / 32768) - 1.0f) * actor->def->offset_spread[1];
    motion->end_position.z = actor->def->offset_base[2] + ((rand() & 0xFFFF) * (1.0f / 32768) - 1.0f) * actor->def->offset_spread[2];
    motion->end_position.w = 0;
    motion->end_frame = -1;
    actor->model->position.y -= 220.0f;
    motion->ended = 0;
    actor->unk6C = 0;
    actor->flags |= (s64)0x8000 << 0x2B;
}

/* Until frame radius_half hovers at its point turned with the player's view, facing where the
   player faces; then flies on, homing on its entity. A hit with result 1 (EffectDef.interval set)
   makes it pick a new entity unkF0 frames later (kind 25). */
void EffectHoverShot_Update(EffectActor *effect) {
    TimerMotion *motion;
    f32 rotation_matrix[16];
    Vector4 angles;
    Vector4 offset;
    int timer;
    int hit;
    int gone_bits;

    motion = (TimerMotion *)&effect->motion;
    if (effect->unk6C != 0) {
        gone_bits = (int)(((EffectTarget *)effect->unk6C)->flags << 30 >> 32);
        if ((gone_bits & 3) == 0) {
            Vector_Subtract(&offset, &((EffectTarget *)effect->unk6C)->model->position, &effect->model->position);
            offset.y -= ((EffectTarget *)effect->unk6C)->height * 0.5f;
            EffectSteer_Toward(&motion->velocity, &offset, effect->def->speed, effect->def->unkDC);
        } else {
            effect->unk6C = 0;
        }
    }
    if (effect->frame >= effect->def->radius_half) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
        hit = EffectHit_CheckAndEndByFlag(effect, motion->unk24);
        if (hit == 1 && effect->def->interval != 0) {
            motion->timer = effect->def->unkF0;
        }
    } else {
        sceVu0RotMatrixX(rotation_matrix, D_00431AF0, D_00413F80.rotation.x);
        sceVu0RotMatrixY(rotation_matrix, rotation_matrix, D_00413F80.rotation.y);
        sceVu0ApplyMatrix(&offset, rotation_matrix, &motion->unk10);
        Vector_Copy(&effect->model->rotation, &D_00413F80.rotation);
        EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
        Vector_Add(&effect->model->position, &D_00413F80.position, &offset);
        effect->model->position.y -= 220.0f;
    }
    timer = motion->timer;
    if (timer > 0) {
        timer -= 1;
        motion->timer = timer;
        if (timer == 0) {
            EffectAngle_FromVector(&angles.x, &motion->velocity.x);
            effect->unk6C = EffectHit_FindAimedEntity(effect, &effect->model->position, &angles);
        }
    }
    effect->frame += 1;
}

/* Places and turns the model as the definition says (kind 76); the effects aimed at it are freed
   with it. */
void EffectAimAtPlayer_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;

    motion = (TurningMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion->unk24 = 0;
    effect->flags |= EFFECT_FLAG_HAS_TARGETERS;
}

/* Until frame radius_half turns to face a point 200 above the player; then flies that way at
   EffectDef.speed and ends on a hit (kind 76). */
void EffectAimAtPlayer_Update(EffectActor *actor) {
    ScaledFallMotion *motion;
    char unused[0x10];  /* unused, but retail reserves the 0x10 bytes */
    Vector4 offset;

    motion = (ScaledFallMotion *)&actor->motion;
    if (actor->frame >= actor->def->radius_half) {
        if (actor->frame == actor->def->radius_half) {
            EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
        }
        sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        EffectHit_CheckAndEnd(actor, &motion->unk1C[8]);
    } else {
        Vector_Subtract(&offset, &D_00413F80.position, &actor->model->position);
        offset.y -= 200.0f;
        EffectAngle_FromVector(&actor->model->rotation.x, &offset.x);
        Vector_Normalize(&motion->velocity, &offset);
    }
    actor->frame++;
}

/* Starts an effect that trails its parent (kind 26): placed as the definition says, stood across
   the parent's path with a random roll; remembers the parent's position. */
void EffectTrail_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectMotion *motion;
    f32 rotation_matrix[16];
    Vector4 up_vector;
    Vector4 angles;

    motion = &effect->motion;
    EffectStart_PlaceModel(effect, spawn_slot);
    up_vector.y = 1.0f;
    up_vector.x = up_vector.z = up_vector.w = 0.0f;
    EffectAngle_FromVector(&angles.x, &effect->target->motion.velocity.x);
    sceVu0RotMatrixX(rotation_matrix, D_00431AF0, angles.x);
    sceVu0RotMatrixY(rotation_matrix, rotation_matrix, angles.y);
    sceVu0ApplyMatrix(&up_vector, rotation_matrix, &up_vector);
    EffectAngle_FromVector(&effect->model->rotation.x, &up_vector.x);
    effect->model->rotation.z = effect->model->rotation.z + (effect->def->rotation_base[2] + ((float)(rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->rotation_spread[2]);
    Vector_Copy(motion, &effect->target->model->position);
    Vector_Copy(&motion->unk10, &effect->target->model->rotation);
}

/* Stands across the parent's path again and moves to where the parent was a frame ago (kind 26). */
void EffectTrail_Update(EffectActor *actor) {
    f32 matrix[16];
    Vector4 direction;
    Vector4 angles;
    EffectMotion *motion;

    motion = &actor->motion;
    EffectKeys_ApplyScale(actor);
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    direction.x = direction.z = direction.w = 0.0f;
    direction.y = 1.0f;
    EffectAngle_FromVector(&angles.x, &actor->target->motion.velocity.x);
    sceVu0RotMatrixX(matrix, D_00431AF0, angles.x);
    sceVu0RotMatrixY(matrix, matrix, angles.y);
    sceVu0ApplyMatrix(&direction.x, matrix, &direction.x);
    EffectAngle_FromVector(&actor->model->rotation.x, &direction.x);
    Vector_Copy(&actor->model->position, motion);
    Vector_Copy(motion, &actor->target->model->position);
    actor->frame += 1;
}

/* Starts a shot that splits (kind 27): with EffectDef.unkE8 set it starts at offset_base turned
   with the player's view, 220 up, facing where the player faces; flies at EffectDef.speed and keeps
   the frame its path meets the scenery. */
void EffectSplitShot_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    AngleAimMotion *motion;
    EffectDef *def;
    PlayerState *player;
    f32 rotation_matrix[16];
    Vector4 offset;

    EffectStart_TurnModel(effect, spawn_slot);
    motion = (AngleAimMotion *)&effect->motion;
    EffectStart_PlaceModel(effect, spawn_slot);
    def = effect->def;
    if (def->unkE8 != 0.0f) {
        player = &D_00413F80;
        offset.x = def->offset_base[0];
        offset.y = def->offset_base[1];
        offset.z = def->offset_base[2];
        offset.w = 0.0f;
        sceVu0RotMatrixX(rotation_matrix, D_00431AF0, player->rotation.x);
        sceVu0RotMatrixY(rotation_matrix, rotation_matrix, player->rotation.y);
        sceVu0ApplyMatrix(&offset, rotation_matrix, &offset);
        Vector_Add(&effect->model->position, &player->position, &offset);
        Vector_Copy(&effect->model->rotation, &player->rotation);
        effect->model->position.y -= 220.0f;
    }
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->angle = func_001B0208(30.0f, effect, motion, motion->unk10);
    motion->unk20 = 0;
    motion->unk28 = 0;
    motion->unk26 = 0;
}

/* Spawns effect 0x47 unless an effect of its kind did in the last 5 frames. */
static void EffectSplitShot_SpawnSharedBurst(EffectActor *effect) {
    EffectActor *other;
    int other_index;

    if (((CooldownMotion *)&effect->motion)->cooldown != 0) {
        return;
    }
    other = D_003F3550;
    for (other_index = 0; other_index < 0x180; other_index++) {
        if (other->flags & EFFECT_FLAG_LIVE) {
            if (other->def->unk100 == effect->def->unk100) {
                ((CooldownMotion *)&other->motion)->cooldown = 5;
            }
        }
        other++;
    }
    Effect_Spawn(0x47, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
}

/* Flies on and ends at the frame it reaches the scenery, spawning the end effect there. A hit
   spawns the shared burst; a hit with result 1 (EffectDef.interval set) makes it split interval
   frames later into radius_half effects unkF0, fanned out from unkDC by unkE0 (kind 27). */
void EffectSplitShot_Update(EffectActor *effect) {
    CooldownTimerMotion *motion;
    f32 rotation_matrix[16];
    Vector4 forward;
    Vector4 angles;
    float turn_angle;
    float next_angle;
    int hit;
    int step;

    motion = (CooldownTimerMotion *)&effect->motion;
    if (motion->cooldown != 0) {
        motion->cooldown -= 1;
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    if (effect->frame >= motion->end_frame) {
        Effect_Spawn(effect->def->end_effect, &motion->unk10, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        EffectSplitShot_SpawnSharedBurst(effect);
        Effect_Free(effect);
    } else {
        hit = EffectHit_CheckAndEndByFlag(effect, motion->unk28);
        if (hit != 0) {
            EffectSplitShot_SpawnSharedBurst(effect);
            if (hit == 1 && effect->def->interval != 0) {
                motion->timer = effect->def->interval;
            }
        }
        if (motion->timer != 0) {
            motion->timer -= 1;
            if (motion->timer == 0) {
                turn_angle = effect->def->unkDC;
                for (step = 0; step < effect->def->radius_half; step++) {
                    forward.x = forward.y = forward.w = 0.0f;
                    forward.z = 1.0f;
                    sceVu0RotMatrixY(rotation_matrix, D_00431AF0, turn_angle);
                    sceVu0RotMatrixX(rotation_matrix, rotation_matrix, effect->model->rotation.x);
                    sceVu0RotMatrixY(rotation_matrix, rotation_matrix, effect->model->rotation.y);
                    sceVu0ApplyMatrix(&forward, rotation_matrix, &forward);
                    EffectAngle_FromVector(&angles.x, &forward.x);
                    Effect_Spawn(effect->def->unkF0, &effect->model->position, &angles, effect->owner_entity, effect->effect_param, effect->owner);
                    next_angle = turn_angle + effect->def->unkE0;
                    next_angle -= (float)(int)(next_angle / TWO_PI) * TWO_PI;
                    if (PI < next_angle) {
                        next_angle -= TWO_PI;
                    } else if (next_angle < -PI) {
                        next_angle += TWO_PI;
                    }
                    turn_angle = next_angle;
                }
                Effect_Free(effect);
            }
        }
        effect->frame += 1;
    }
}

/* Starts a chaser aimed at the entity the player faces (kind 32), with a starting turn that is
   larger the nearer the entity is (EffectDef.interval is the range, unkDC the most). */
void EffectAimedChase_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    ChaseMotion *motion;
    Vector4 offset;
    double distance;
    double interval;
    double remaining;

    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    motion = (ChaseMotion *)&actor->motion;
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    actor->unk6C = EffectHit_FindAimedEntity(actor, &D_00413F80.position, &D_00413F80.rotation);
    motion->ended = 0;
    if (actor->unk6C) {
        Vector_Subtract(&offset, &ENTITY_MODEL((Entity *)actor->unk6C)->position, &actor->model->position);
        interval = (f32)actor->def->interval;
        distance = sqrt(offset.x * offset.x + offset.z * offset.z);
        remaining = interval - distance;
        motion->turn = (f32)(remaining / (double)(actor->def->interval * 2.0f));
        motion->turn *= actor->def->unkDC;
    } else {
        motion->turn = 0.0f;
    }
}

/* Chases its entity, turning harder by EffectDef.unkE0 while it is ahead, and drops it once it
   cannot be hit; ends on a hit (kind 32). */
void EffectAimedChase_Update(EffectActor *actor) {
    ChaseMotion *motion;
    char unused[0x40]; /* unused, but retail reserves the bytes */
    f32 delta[4];
    char unused_tail[0x10]; /* unused, but retail reserves the bytes */

    EffectKeys_ApplyScale(actor);
    motion = (ChaseMotion *)&actor->motion;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    if (actor->unk6C != 0) {
        if (ENTITY_FLAG_BITS((Entity *)actor->unk6C)->bits2 == 0) {
            Vector_Subtract(delta, ((Entity *)actor->unk6C)->model->position, &actor->model->position);
            if (func_001B0970(delta, motion) > 0.0f) {
                motion->turn += actor->def->unkE0;
                delta[1] -= ((Entity *)actor->unk6C)->height * 0.5f;
            }
            EffectSteer_Toward(&motion->velocity, (Vector4 *)delta, actor->def->speed, motion->turn);
            EffectAngle_FromVector(&actor->model->rotation.x, &motion->velocity.x);
            motion->velocity.w = 0;
        } else {
            actor->unk6C = 0;
        }
    }
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        sceVu0AddVector(&actor->model->position, &actor->model->position, motion);
    }
    EffectHit_CheckAndEnd(actor, &motion->ended);
    actor->frame++;
}

/* Starts a shot (kind 31): flies at EffectDef.speed and keeps the frame its path comes within
   radius_half of the scenery. */
void EffectLingerShot_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    CountedMotion *motion;

    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    motion = (CountedMotion *)&actor->motion;
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    motion->count = func_001B0208(actor->def->radius_half, actor, motion, &motion->unk10);
    motion->flag = 0;
}

/* Flies on; at that frame spawns the end effect where it meets the scenery, or ends on a hit; then
   frees its spawners and lingers interval frames before it ends (kind 31). */
void EffectLingerShot_Update(EffectActor *actor) {
    EndCountdownMotion *motion;
    s8 spawn_flag = 0;

    EffectKeys_ApplyScale(actor);
    motion = (EndCountdownMotion *)&actor->motion;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    if (motion->countdown > 0) {
        motion->countdown--;
        if (motion->countdown <= 0) {
            Effect_Free(actor);
            return;
        }
    } else {
        if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
            sceVu0AddVector(&actor->model->position, &actor->model->position, motion);
        }
        if (actor->frame >= motion->end_frame) {
            Effect_Spawn(actor->def->end_effect, &motion->unk10, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            spawn_flag = 1;
        } else {
            EffectHit_CheckAndEnd(actor, &spawn_flag);
        }
        if (spawn_flag) {
            Effect_FreeSpawnersOf(actor);
            motion->countdown = actor->def->interval;
        }
    }
    actor->frame++;
}

/* Turns the model toward target and keeps target as the aim, with the frames to it at 500 a frame
   less radius_half. */
static void EffectSweep_AimAt(EffectActor *actor, Vector4 *target) {
    ScaledMotion *motion;
    Vector4 offset;
    float distance;

    motion = (ScaledMotion *)&actor->motion;
    Vector_Subtract(&offset, target, &actor->model->position);
    EffectAngle_FromVector(&actor->model->rotation.x, &offset.x);
    distance = func_001B0178(target, &actor->model->position);
    motion->unk24 = func_001B0950((distance - actor->def->radius_half) / 500.0f);
    Vector_Copy(&motion->unk10, target);
}

/* Starts a sweeping effect (kind 33) aimed at the entity the player faces, or with none at the
   floor EffectDef.unkF2 ahead of the player along the spawner's heading. */
void EffectSweep_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    Vector4 aim;
    Vector4 rotated;
    SweepMotion *motion;
    f32 sine;
    f32 cosine;
    f32 distance;

    motion = (SweepMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion->arrived = 0;
    motion->on_ground = 0;
    motion->hit_spawned = 0;
    effect->unk6C = EffectHit_FindAimedEntity(effect, &D_00413F80.position, &D_00413F80.rotation);
    motion->level_step = effect->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level;
    if (effect->unk6C != 0) {
        EffectSweep_AimAt(effect, &((EffectTarget *)effect->unk6C)->model->position);
        Vector_Copy(&aim, &((EffectTarget *)effect->unk6C)->model->position);
        aim.y -= 50.0f;
        if (func_001C0E40(100.0f, &aim) == 1) {
            motion->on_ground = 1;
        }
        motion->has_aim = 1;
    } else {
        sine = sin(-spawn_slot->unk10.y);
        cosine = cos(-spawn_slot->unk10.y);
        aim.x = 0.0f;
        aim.y = 0.0f;
        aim.w = 0.0f;
        distance = effect->def->unkF2;
        rotated.x = cosine * aim.x - sine * distance;
        rotated.z = sine * aim.x + cosine * distance;
        aim.z = rotated.z;
        aim.x = rotated.x;
        Vector_Add(&aim, &aim, &D_00413F80.position);
        aim.y -= 300.0f;
        if (func_001C0E40(2000.0f, &aim) == 1) {
            aim.y = COLLISION_RESULT->point[1];
            motion->has_aim = 1;
            motion->on_ground = 1;
        } else {
            motion->has_aim = 0;
            aim.y += 300.0f;
        }
        EffectSweep_AimAt(effect, &aim);
    }
    if (EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level == 1) {
        motion->first_level = 1;
    }
}
