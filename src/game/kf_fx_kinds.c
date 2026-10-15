/* The per-kind behaviours of the effect actors from kind 29 on (and kind 7). An effect's kind is
   EffectDef.unk100, its row of the handler table D_00402550: a start function (column 0, called with the
   spawner slot when the effect is made) and a per-frame update (column 1). Most updates run the scale,
   spin and colour keys (kf_fx_shared.c), move the model, check for hits and advance the frame. */
#include "common.h"
#include "kf_effect.h"
#include "kf_player.h"
#include "kf_entity.h"
#include "kf_game.h"
#include "kf_linked_point.h"
#include "kf_collision.h"

#define TWO_PI 6.2831855f
#define PI 3.1415927f
extern PlayerState D_00413F80;
extern float D_00431AF0[16];
int func_001B4F90(void *);
float func_001B0178(void *from, void *target);
void EffectAngle_FromVector(void *angles, void *direction);
void Vector_Copy(void *dst, void *src);
void Vector_Add(void *dst, void *lhs, void *rhs);
void Vector_Scale(void *dst, void *src, float scale);
void Vector_Subtract(void *dst, void *lhs, void *rhs);
void sceVu0RotMatrixY(void *, void *, float);
void sceVu0ApplyMatrix(void *, void *, void *);
/* Wraps an angle into -pi..pi. */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (float)(int)(angle / TWO_PI) * TWO_PI;
    if (angle > PI) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    return angle;
}
s16 func_001B0208(float, void *, void *, void *);
void EffectHit_CheckAndEnd(EffectActor *effect, void *state);
void sceVu0AddVector(void *, void *, void *);
extern int D_004468E0;
int func_001C0E40(f32, void *);
typedef float Mat[4][4] __attribute__((aligned(16)));
int rand(void);
void sceVu0RotMatrixX(void *, void *, float);
void Vector_Normalize(void *, void *);
extern void (*D_00402550[][2])();
extern u8 D_00413FBC[];
extern int D_004468E4;
void func_0010C9E8(ModelInstance *);
char *func_0010CAB8(char *, u32);
void sceVu0RotMatrixZ(void *, void *, float);
void *memset(void *, int, u32);
extern int D_004468E8;
void func_001C1430(f32, void *, void *);
int Sound_PlayMapAt(u8 sound, float *position);
int func_001C1138(f32, void *);
extern GameStatus D_0038CC50;
extern u16 D_00413DA8[];
extern u16 D_0044693C;
int EffectHit_FindAimedEntity(void *, void *, void *);
extern s64 D_0043EF60[2];
/* LinkedPoint_StartDefault called as returning s16 (the caller keeps the handle in a halfword) */
// match: only this type matches; kf_linked_point.c defines it as returning int
#define LINKED_POINT_START_S16 ((s16(*)(f32, void *, void *))LinkedPoint_StartDefault)
extern u8 D_0034A019[];
extern u8 D_0034AFD6[];
void Sound_PlaySw(int mode);
extern u32 D_00402D50[14];

void EffectImpactLob_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectImpactLob_Update(EffectActor *actor);
void EffectGroundFan_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectGroundFan_Update(EffectActor *actor);
void EffectLevelHeight_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectLevelHeight_Update(EffectActor *effect);
void EffectWhileGroundFan_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectWhileGroundFan_Update(EffectActor *effect);
void EffectConverge_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectConverge_StartInSpread(EffectActor *effect, EffectSlot *spawn_slot);
void EffectConverge_Update(EffectActor *effect);
void EffectHandover_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectHandover_StartBySurface(EffectActor *effect, EffectSlot *spawn_slot);
void EffectHandover_StartNext(EffectActor *effect, Vector4 *position, int effect_id);
static void EffectHandover_RunKeys(EffectActor *effect);
void EffectHandover_Update(EffectActor *effect);
void EffectHitWindow_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectHitWindow_Update(EffectActor *effect);
void EffectBolt_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectBolt_StartBySurface(EffectActor *effect, EffectSlot *spawn_slot);
void EffectBolt_StartWithTargeters(EffectActor *effect, EffectSlot *spawn_slot);
void EffectBolt_StartCounted(EffectActor *effect, EffectSlot *spawn_slot);
void EffectBolt_Update(EffectActor *actor);
void EffectFollowPose_Start(EffectActor *effect);
void EffectFollowPose_Update(EffectActor *effect);
void EffectFollowPosition_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectFollowPosition_Update(EffectActor *effect);
void EffectBoneFade_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectBoneFade_Update(EffectActor *actor);
void EffectDistanceFade_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectDistanceFade_Update(EffectActor *effect);
void EffectSpray_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSpray_Update(EffectActor *effect);
void EffectLateFall_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLateFall_StartWithTargeters(EffectActor *actor, EffectSlot *spawn_slot);
void EffectLateFall_Update(EffectActor *actor);
void EffectFallToFloor_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectFallToFloor_Update(EffectActor *actor);
void EffectFloorChase_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectFloorChase_Update(EffectActor *actor);
void EffectHomeOnPlayer_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectHomeOnPlayer_Update(EffectActor *effect);
void EffectTiltFall_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectTiltFall_Update(EffectActor *effect);
void EffectFloorDelay_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectFloorDelay_Update(EffectActor *effect);
void EffectTurnFan_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectTurnFan_Update(EffectActor *effect);
void EffectDelayedShot_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectDelayedShot_Update(EffectActor *effect);
void EffectLevelFan_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLevelFan_Update(EffectActor *effect);
void EffectRay_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectRay_StartAtPlayer(EffectActor *effect, EffectSlot *spawn_slot);
void EffectRay_Update(EffectActor *effect);
void EffectBeamStretch_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectBeamStretch_Update(EffectActor *effect);
void EffectWobble_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectWobble_Update(EffectActor *effect);
void EffectSeekPlayer_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSeekPlayer_Update(EffectActor *actor);
void EffectProbedFall_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectProbedFall_Update(EffectActor *effect);
void EffectSurroundPlayer_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSurroundPlayer_Update(EffectActor *effect);
void EffectChain_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectChain_Update(EffectActor *effect);
void EffectOwnerFloor_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectOwnerFloor_Update(EffectActor *effect);
void EffectLaunchFromOwner_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLaunchFromOwner_Update(EffectActor *effect);
void EffectBowDraw_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectBowDraw_Update(EffectActor *actor);
void EffectInheritSpeed_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectInheritSpeed_Update(EffectActor *effect);
void EffectHomeOnEntity_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectHomeOnEntity_Update(EffectActor *actor);
void EffectImpactShot_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectImpactShot_Update(EffectActor *effect);
void EffectFollowLight_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectFollowLight_Update(EffectActor *effect);
void EffectLight_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLight_Update(EffectActor *effect);
void EffectSpiral_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSpiral_Update(EffectActor *effect);
void EffectScatterAroundPlayer_Start(EffectActor *effect);
void EffectScatterAroundPlayer_Update(EffectActor *effect);
void EffectDrop_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectDrop_Update(EffectActor *effect);
void EffectPitchScale_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectPitchScale_Update(EffectActor *effect);
void EffectRepeater_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectRepeater_Update(EffectActor *effect);
void EffectCorkscrew_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectCorkscrew_Update(EffectActor *effect);
void EffectWander_Start(EffectActor *actor, EffectSlot *spawn_slot);
void EffectWander_Update(EffectActor *effect);
void EffectFollowPoseScaled_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectFollowPoseScaled_Update(EffectActor *effect);
void EffectJitterWobble_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectJitterWobble_Update(EffectActor *effect);
void EffectJitterOrbit_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectJitterOrbit_Update(EffectActor *effect);
void EffectSeekPoint_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSeekPoint_Update(EffectActor *effect);
void EffectLoopUntilFlag_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectLoopUntilFlag_Update(EffectActor *effect);
void EffectWeaponSound_Start(EffectActor *effect, EffectSlot *spawn_slot);
void EffectSound_Start(EffectActor *effect, EffectSlot *spawn_slot);

/* Kind 33 update: at the arrival frame spawns the end effect at the aim (first level only) and, when the
   aim has ground under it, starts a sweep: the aim then circles outward and spawns def->unkF0 where the hit
   check first finds something. The model's z scale grows with the distance to the aim. */
void EffectSweep_Update(EffectActor *effect) {
    f32 matrix[16];
    Vector4 to_aim;
    Vector4 saved_position;
    SweepMotion *motion;
    int hits;

    motion = (SweepMotion *)&effect->motion;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame == motion->arrival_frame) {
        if (motion->has_aim != 0) {
            if (motion->first_level != 0) {
                Effect_Spawn(effect->def->end_effect, &motion->aim, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            }
            if (motion->on_ground != 0) {
                effect->motion.velocity.x = 0.0f;
                motion->velocity.z = 50.0f;
                motion->velocity.y = 0.0f;
                motion->velocity.w = 0.0f;
                motion->sweep_turn = effect->def->keys_b[3].y;
                sceVu0RotMatrixY(matrix, D_00431AF0, wrap_angle(D_00413F80.rotation.y + effect->def->unkDC + effect->def->unkE0 * (f32)motion->level_step));
                sceVu0ApplyMatrix(&motion->velocity, matrix, &motion->velocity);
            }
        }
        motion->arrived = 1;
    }
    if (motion->arrived != 0) {
        if (motion->on_ground != 0 && motion->arrival_frame + 10 < effect->frame) {
            sceVu0RotMatrixY(matrix, D_00431AF0, motion->sweep_turn);
            sceVu0ApplyMatrix(&motion->velocity, matrix, &motion->velocity);
            Vector_Scale(&motion->velocity, &motion->velocity, 1.02f);
            Vector_Add(&motion->aim, &motion->aim, &motion->velocity);
            Vector_Subtract(&to_aim, &motion->aim, &effect->model->position);
            EffectAngle_FromVector(&effect->model->rotation, &to_aim);
            if (motion->hit_spawned == 0) {
                Vector_Copy(&saved_position, &effect->model->position);
                Vector_Copy(&effect->model->position, &motion->aim);
                hits = func_001B4F90(effect);
                Vector_Copy(&effect->model->position, &saved_position);
                if (hits != 0) {
                    Effect_Spawn(effect->def->unkF0, &motion->aim, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                    motion->hit_spawned = 1;
                }
            }
        }
        effect->model->unkE8 = func_001B0178(&motion->aim, &effect->model->position) / 100.0f + 1.0f;
    }
    effect->frame++;
}

/* Kind 34 start: launches along the heading, takes the frame it reaches what lies ahead and the kind of
   surface there (func_001B0208), and spawns def->unkF0 unless a kind-34 effect younger than def->interval
   frames already did. */
void EffectImpactLob_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    FallingMotion *motion;
    EffectActor *slot;
    int i;
    char scratch[0x10];

    motion = (FallingMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    motion->end_frame = func_001B0208((float)actor->def->radius_half, actor, motion, scratch);
    if (motion->end_frame != 0xFFFF) {
        motion->mode = COLLISION_RESULT->unk18;
    }
    motion->unk17[0] = 0;
    motion->spawned = 0;
    slot = D_003F3550;
    for (i = 0; i < EFFECT_COUNT; i++, slot++) {
        if (slot->flags & EFFECT_FLAG_LIVE) {
            if (slot->def->unk100 == actor->def->unk100 && slot->frame < actor->def->interval) {
                if (((FallingMotion *)&slot->motion)->spawned != 0) {
                    return;
                }
            }
        }
    }
    Effect_Spawn(actor->def->unkF0, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
    motion->spawned = 1;
}

/* Kind 34 update: falls under def->gravity; past the impact frame spawns effect 0x15 on surface kinds 3 and
   5 and ends; checks for hits. */
void EffectImpactLob_Update(EffectActor *actor) {
    FallingMotion *motion;
    EffectKeys_ApplyScale(actor);
    motion = (FallingMotion *)&actor->motion;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    motion->velocity.y += actor->def->gravity;
    sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
    if (actor->frame > motion->end_frame) {
        if (motion->mode == 5 || motion->mode == 3) {
            Effect_Spawn(0x15, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
        }
        Effect_Free(actor);
        return;
    }
    EffectHit_CheckAndEnd(actor, motion->unk17);
    actor->frame++;
}

/* Kind 35 start: turns the heading by def->unkDC for each kind-35 effect started before it this frame (a
   fan), launches along it and lowers velocity.y by def->radius_half. */
void EffectGroundFan_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    PhaseMotion *motion;
    Vector4 heading;
    EffectDef *def;
    f32 angle;
    f32 new_y;
    int quotient;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion = (PhaseMotion *)&effect->motion;
    Vector_Copy(&heading, &effect->model->rotation);
    def = effect->def;
    heading.y = heading.y + (float)D_004468E0 * def->unkDC;
    angle = heading.y;
    quotient = (int)(angle / 6.28318548f);
    angle = angle - (float)quotient * 6.28318548f;
    if (3.14159274f < angle) {
        angle -= 6.28318548f;
    } else if (angle < -3.14159274f) {
        angle += 6.28318548f;
    }
    heading.y = angle;
    EffectAngle_ToVector(def->speed, &motion->velocity, &heading);
    EffectAngle_FromVector(&effect->model->rotation, motion);
    new_y = motion->velocity.y - (float)effect->def->radius_half;
    motion->unk18 = 0;
    motion->frame_stamp = D_004468E0;
    motion->velocity.y = new_y;
    motion->phase = D_004468E0 % effect->def->interval;
    D_004468E0 += 1;
}

/* Kind 35 update: every def->interval frames steps by its velocity and settles on the floor there, ending
   where there is none; checks for hits from frame 21. Resets the fan count for the next frame. */
void EffectGroundFan_Update(EffectActor *actor) {
    EmitterMotion *motion;
    int countdown;
    u16 interval;

    D_004468E0 = 0;
    EffectKeys_ApplyScale(actor);
    motion = (EmitterMotion *)&actor->motion;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    countdown = motion->countdown;
    if (countdown <= 0) {
        if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
            sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
            if (func_001C0E40(actor->def->radius_half * 2, &actor->model->position) == 0) {
                Effect_Free(actor);
                return;
            }
            actor->model->position.y = COLLISION_RESULT->point[1];
        }
        interval = actor->def->interval;
        motion->countdown = interval;
        countdown = interval;
    }
    motion->countdown = countdown - 1;
    if (actor->frame >= 0x15) {
        EffectHit_CheckAndEnd(actor, motion->unk18);
    }
    actor->frame += 1;
}

/* Kind 36 start: keeps how many levels the spawner is below def->max_level. */
void EffectLevelHeight_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    *(int *)&actor->motion = actor->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level;
}

/* Kind 36 update: sets the model's y scale to def->unkDC plus the keyed y scale times that share of
   def->max_level. */
void EffectLevelHeight_Update(EffectActor *effect) {
    ProgressMotion *motion;
    EffectDef *def;
    ModelInstance *model;

    motion = (ProgressMotion *)&effect->motion;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    def = effect->def;
    model = effect->model;
    model->unkE4 = def->unkDC + model->unkE4 * ((float)motion->progress / (float)def->max_level);
    effect->frame += 1;
}

/* Kind 7 start: only places and turns the model. */
void EffectWhileGroundFan_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    EffectStart_PlaceModel(actor, spawn_slot);
    EffectStart_TurnModel(actor, spawn_slot);
}

/* Kind 7 update: lives as long as any kind-35 effect (EffectGroundFan) does, then ends. */
void EffectWhileGroundFan_Update(EffectActor *effect) {
    EffectActor *candidate;
    int i;

    candidate = D_003F3550;
    for (i = 0; i < EFFECT_COUNT; i++, candidate++) {
        if ((candidate->flags & EFFECT_FLAG_LIVE) && candidate->def->unk100 == 0x23) {
            effect->frame++;
            return;
        }
    }
    Effect_Free(effect);
}

/* Kind 47 start: a random point def->radius_half from the spawn point, moving toward it at def->speed. */
void EffectConverge_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    Mat rotation_matrix;
    Vector4 offset;
    EffectMotion *motion;
    f32 random_angle_y;

    EffectStart_PlaceModel(effect, spawn_slot);
    motion = &effect->motion;
    offset.x = offset.y = offset.w = 0.0f;
    offset.z = (float)effect->def->radius_half;
    effect->model->rotation.x = ((float)(rand() & 0xFFFF) * 3.0517578125e-05f - 1.0f) * 3.1415f;
    random_angle_y = ((float)(rand() & 0xFFFF) * 3.0517578125e-05f - 1.0f) * 3.1415f;
    effect->model->rotation.w = effect->model->rotation.z = 0.0f;
    effect->model->rotation.y = random_angle_y;
    sceVu0RotMatrixX(rotation_matrix, D_00431AF0, effect->model->rotation.x);
    sceVu0RotMatrixY(rotation_matrix, rotation_matrix, effect->model->rotation.y);
    sceVu0ApplyMatrix(&offset, rotation_matrix, &offset);
    Vector_Add(&effect->model->position, &effect->model->position, &offset);
    Vector_Normalize(&offset, &offset);
    Vector_Scale(motion, &offset, -effect->def->speed);
}

/* Kind 103 start: EffectConverge_Start with the direction turned from the model's own by up to def->unkDC
   (pitch) and def->unkE0 (heading). */
void EffectConverge_StartInSpread(EffectActor *effect, EffectSlot *spawn_slot) {
    Mat rotation_matrix;
    Vector4 offset;
    EffectMotion *motion;
    f32 random_angle_y;

    EffectStart_PlaceModel(effect, spawn_slot);
    motion = &effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    offset.x = offset.y = offset.w = 0.0f;
    offset.z = (float)effect->def->radius_half;
    effect->model->rotation.x += ((float)(rand() & 0xFFFF) * 3.0517578125e-05f - 1.0f) * effect->def->unkDC;
    random_angle_y = ((float)(rand() & 0xFFFF) * 3.0517578125e-05f - 1.0f) * effect->def->unkE0;
    effect->model->rotation.w = effect->model->rotation.z = 0.0f;
    effect->model->rotation.y += random_angle_y;
    sceVu0RotMatrixX(rotation_matrix, D_00431AF0, effect->model->rotation.x);
    sceVu0RotMatrixY(rotation_matrix, rotation_matrix, effect->model->rotation.y);
    sceVu0ApplyMatrix(&offset, rotation_matrix, &offset);
    Vector_Add(&effect->model->position, &effect->model->position, &offset);
    Vector_Normalize(&offset, &offset);
    Vector_Scale(motion, &offset, -effect->def->speed);
}

/* Kinds 47 and 103 update: moves by the velocity until def->max_frames. */
void EffectConverge_Update(EffectActor *effect) {
    EffectMotion *motion;

    motion = &effect->motion;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    effect->frame += 1;
}

/* Kind 46 start: launches along the heading; the frame it comes within def->unkDC of what lies ahead
   (none when later than def->unkF0) is when it hands over to def->end_effect there; def->unkE8 is the
   countdown that holds the frame after a hand-over. */
void EffectHandover_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    s16 angle;
    float unk_e8;

    motion = (TurningMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &effect->motion.velocity, &effect->model->rotation);
    angle = func_001B0208(effect->def->unkDC, effect, motion, &motion->unk10);
    motion->unk22 = angle;
    if (effect->def->unkF0 < (u32)(angle & 0xFFFF)) {
        *(u16 *)&motion->unk22 = 0xFFFF; // match: stored as an unsigned halfword
    }
    unk_e8 = effect->def->unkE8;
    motion->unk25 = 0;
    motion->unk24 = (int)unk_e8;
    motion->unk20 = effect->def->end_effect;
}

/* Kind 29 start: EffectHandover_Start that hands over to def->unkF2 on surface kinds 3 and 5. */
void EffectHandover_StartBySurface(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    int mode;

    motion = (TurningMotion *)&effect->motion;
    EffectHandover_Start(effect, spawn_slot);
    mode = COLLISION_RESULT->unk18;
    if (mode == 3 || mode == 5) {
        motion->unk20 = effect->def->unkF2;
    }
}

/* Starts effect_id at position from the effect and takes it back to the frame def->unkF0 (later by what is
   left of the first def->interval frames), which it holds for a while. */
void EffectHandover_StartNext(EffectActor *effect, Vector4 *position, int effect_id) {
    StartFrameMotion *motion;
    EffectDef *def;
    int start_frame;

    motion = (StartFrameMotion *)&effect->motion;
    def = effect->def;
    start_frame = def->unkF0;
    if (effect->frame < def->interval) {
        start_frame += def->interval - effect->frame;
    }
    motion->unk26 = start_frame;
    effect->frame = (s8)start_frame;
    Effect_SpawnFromEffectAt(effect_id, effect, position);
    motion->unk25 = 1;
}

/* Runs the effect's scale, spin and colour keys. */
static void EffectHandover_RunKeys(EffectActor *effect) {
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
}

/* Kinds 29 and 46 update: moves; up to frame def->unkF0 a hit or the hand-over frame starts the next effect,
   and frame def->unkF0 itself spawns def->radius_half. After a hand-over the frame is held while the
   countdown runs. */
void EffectHandover_Update(EffectActor *effect) {
    RestartMotion *motion;
    EffectDef *def;
    u16 max_frames;
    int hits;
    int effect_id;

    motion = (RestartMotion *)&effect->motion;
    motion->countdown--;
    if ((s8)motion->countdown > 0 && motion->started != 0) {
        effect->frame = (s8)motion->start_frame;
        EffectHandover_RunKeys(effect);
        return;
    }
    EffectHandover_RunKeys(effect);
    def = effect->def;
    max_frames = def->max_frames;
    if (effect->frame < max_frames || max_frames == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    if (effect->frame <= effect->def->unkF0) {
        hits = func_001B4F90(effect);
        if (hits & 2) {
            effect_id = (int)effect->def->unkE0;
            if (effect_id <= 0) {
                effect_id = effect->def->end_effect;
            }
            EffectHandover_StartNext(effect, &effect->model->position, effect_id);
        } else if (hits & 4) {
            if (D_00402550[effect->def->unk100][0] == EffectHandover_StartBySurface && (COLLISION_RESULT->unk18 == 3 || COLLISION_RESULT->unk18 == 5)) {
                EffectHandover_StartNext(effect, &effect->model->position, effect->def->unkF2);
            } else {
                EffectHandover_StartNext(effect, &effect->model->position, effect->def->end_effect);
            }
        } else if (effect->frame >= motion->end_frame) {
            EffectHandover_StartNext(effect, &motion->aim, motion->end_effect);
        } else if (effect->frame == effect->def->unkF0 && effect->def->radius_half != 0) {
            Effect_SpawnFromEffect(effect->def->radius_half, effect);
        }
    }
    effect->frame++;
}

/* Kind 43 start: keeps the frame window [def->radius_half, def->interval) and spawns def->unkF0 and
   def->unkF2 from the effect. */
void EffectHitWindow_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    PulseMotion *motion;
    EffectDef *def;

    motion = (PulseMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    def = actor->def;
    motion->radius_half = def->radius_half;
    motion->interval = def->interval;
    if (def->unkF0 != 0) {
        Effect_SpawnFromEffect(def->unkF0, actor);
    }
    if (actor->def->unkF2 != 0) {
        Effect_SpawnFromEffect(actor->def->unkF2, actor);
    }
}

/* Kind 43 update: checks for hits while the frame is inside the window; runs no keys. */
void EffectHitWindow_Update(EffectActor *effect) {
    PulseMotion *motion;
    u8 hit;

    hit = 0;
    motion = (PulseMotion *)&effect->motion;
    if (effect->frame >= motion->radius_half && effect->frame < motion->interval) {
        EffectHit_CheckAndEnd(effect, &hit);
    }
    effect->frame += 1;
}

/* Kind 41 start: launches along the heading and takes the frame it reaches what lies ahead (within
   def->radius_half); def->end_effect is spawned there. Def->unkF2 set: the effects aimed at it go with it. */
void EffectBolt_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    EffectDef *def;
    s16 angle;

    motion = (TurningMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    angle = func_001B0208((float)actor->def->radius_half, actor, motion, motion->unk10);
    motion->unk24 = 0;
    motion->unk22 = angle;
    def = actor->def;
    motion->unk20 = def->end_effect;
    if (def->unkF2 != 0) {
        actor->flags |= EFFECT_FLAG_HAS_TARGETERS;
    }
}

/* Kind 30 start: EffectBolt_Start that spawns def->interval instead on surface kinds 3 and 5. */
void EffectBolt_StartBySurface(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    int mode;
    char frame_pad[0x10]; // match: unused, but retail reserves the 0x10 bytes

    motion = (TurningMotion *)&effect->motion;
    EffectBolt_Start(effect, spawn_slot);
    mode = COLLISION_RESULT->unk18;
    if (mode == 3 || mode == 5) {
        motion->unk20 = effect->def->interval;
    }
}

/* Kind 38 start: EffectBolt_StartBySurface; the effects aimed at it go with it. */
void EffectBolt_StartWithTargeters(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectBolt_StartBySurface(effect, spawn_slot);
    effect->flags |= EFFECT_FLAG_HAS_TARGETERS;
}

/* Kind 39 start: only while weapon 0x26 is in hand (else the spawner is freed): an EffectBolt_Start 220
   higher, and every def->interval-th one also spawns def->unkF0. */
void EffectBolt_StartCounted(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    s16 angle;

    motion = (TurningMotion *)&effect->motion;
    if (D_00413FBC[0] != 0x26) {
        Effect_FreeSpawner(spawn_slot);
        return;
    }
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    effect->model->position.y -= 220.0f;
    angle = func_001B0208((float)effect->def->radius_half, effect, motion, &motion->unk10);
    motion->unk24 = 0;
    motion->unk22 = angle;
    motion->unk20 = effect->def->end_effect;
    D_004468E4 += 1;
    if (D_004468E4 < effect->def->interval) {
        return;
    }
    Effect_Spawn(effect->def->unkF0, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
    D_004468E4 = 0;
}

/* Kinds 30, 38, 39 and 41 update: moves; past the impact frame spawns its effect and ends. On a hit spawns
   def->end_effect (hit flag 2), def->interval (the BySurface starts on surface kinds 3 and 5) or its
   effect, and ends. */
void EffectBolt_Update(EffectActor *actor) {
    DelayedMotion *motion;
    int hit;

    EffectKeys_ApplyScale(actor);
    motion = (DelayedMotion *)&actor->motion;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    sceVu0AddVector(&actor->model->position, &actor->model->position, motion);
    if (motion->ended != 0) {
        Effect_Free(actor);
    } else if (motion->end_frame < actor->frame) {
        Effect_Spawn(motion->effect_id, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
        motion->ended = 1;
    } else if (actor->def->unk10B != 0xFF) {
        hit = func_001B4F90(actor);
        if (hit != 0 && actor->def->end_effect != 0xFFFF) {
            if (hit & 2) {
                Effect_Spawn(actor->def->end_effect, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            } else if ((D_00402550[actor->def->unk100][0] == EffectBolt_StartBySurface || D_00402550[actor->def->unk100][0] == EffectBolt_StartWithTargeters) && (COLLISION_RESULT->unk18 == 3 || COLLISION_RESULT->unk18 == 5)) {
                Effect_Spawn(actor->def->interval, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            } else {
                Effect_Spawn(motion->effect_id, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            }
            motion->ended = 1;
        }
    }
    actor->frame++;
}

/* Kind 40 start: keeps the target's position and rotation; the model is not placed. */
void EffectFollowPose_Start(EffectActor *effect) {
    Vector_Copy(&effect->motion, &effect->target->model->position);
    Vector_Copy(&effect->motion.unk10, &effect->target->model->rotation);
}

/* Kind 40 update: takes the pose the target had a frame ago and keeps its current one; checks for hits. */
void EffectFollowPose_Update(EffectActor *effect) {
    Vector4 *velocity;
    Vector4 *motion_unk10;
    u8 state;

    state = 0;
    velocity = &effect->motion.velocity;
    motion_unk10 = &effect->motion.unk10;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    Vector_Copy(&effect->model->position, velocity);
    Vector_Copy(&effect->model->rotation, motion_unk10);
    Vector_Copy(velocity, &effect->target->model->position);
    Vector_Copy(motion_unk10, &effect->target->model->rotation);
    EffectHit_CheckAndEnd(effect, &state);
    effect->frame++;
}

/* Kind 44 start: turns the model and keeps the target's position. */
void EffectFollowPosition_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectMotion *motion;

    motion = &effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    Vector_Copy(motion, &effect->target->model->position);
}

/* Kind 44 update: moves to where the target was a frame ago; checks for hits. */
void EffectFollowPosition_Update(EffectActor *effect) {
    char ended;

    ended = 0;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    Vector_Copy(&effect->model->position, &effect->motion);
    Vector_Copy(&effect->motion, &effect->target->model->position);
    EffectHit_CheckAndEnd(effect, &ended);
    effect->frame += 1;
}

/* Kind 50 start: keeps the fade distances def->unkDC and def->unkE0 in velocity.x and y. */
void EffectBoneFade_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    Vector4 *velocity;

    velocity = &effect->motion.velocity;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    velocity->x = effect->def->unkDC;
    velocity->y = effect->def->unkE0;
}

/* Kind 50 update: sits on a bone of the owner entity (its controller's unkC5), turned like it; fades in as
   the player's distance goes from the first fade distance to the second; loops. */
void EffectBoneFade_Update(EffectActor *actor) {
    float distance;
    float fade;
    FadingMotion *motion;
    char *bone;

    EffectKeys_ApplyScale(actor);
    motion = (FadingMotion *)&actor->motion;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    func_0010C9E8(ENTITY_MODEL(actor->owner_entity));
    bone = func_0010CAB8((char *)actor->owner_entity->model, ENTITY_CONTROLLER(actor->owner_entity)->unkC5);
    Vector_Copy(&actor->model->position, bone);
    Vector_Copy(&actor->model->rotation, &ENTITY_MODEL(actor->owner_entity)->rotation);
    distance = func_001B0178(&D_00413F80.position, &actor->model->position);
    if (motion->range[1] < distance) {
        fade = 1.0f;
    } else if (distance < motion->range[0]) {
        fade = 0.0f;
    } else {
        fade = (distance - motion->range[0]) / (motion->range[1] - motion->range[0]);
    }
    actor->frame++;
    actor->scale *= fade;
    if (actor->frame >= actor->def->keyframe_end) {
        actor->frame = 0;
    }
}

/* Kind 69 start: keeps the fade distances def->unkDC and def->unkE0 in velocity.x and y. */
void EffectDistanceFade_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    Vector4 *velocity;
    EffectDef *def;

    velocity = &effect->motion.velocity;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    def = effect->def;
    velocity->x = def->unkDC;
    velocity->y = def->unkE0;
}

/* Kind 69 update: at its start pose; fades in as the player's distance goes from the first fade distance
   to the second. */
void EffectDistanceFade_Update(EffectActor *effect) {
    Vector4 *velocity;
    float distance_to_player;
    float fade;

    EffectKeys_ApplyScale(effect);
    velocity = &effect->motion.velocity;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    distance_to_player = func_001B0178(&D_00413F80, &effect->model->position);
    if (velocity->y < distance_to_player) {
        fade = 1.0f;
    } else {
        if (distance_to_player < velocity->x) {
            fade = 0.0f;
        } else {
            fade = (distance_to_player - velocity->x) / (velocity->y - velocity->x);
        }
    }
    effect->scale *= fade;
    Vector_Copy(&effect->model->position, effect->start_position);
    Vector_Copy(&effect->model->rotation, effect->start_rotation);
    effect->frame += 1;
}

/* Kind 53 start: a random velocity and spin in the definition's offset and rotation ranges, and an
   acceleration of def->speed along the heading. */
void EffectSpray_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    SprayMotion *motion;

    motion = (SprayMotion *)&effect->motion;
    EffectStart_PlaceModel(effect, spawn_slot);
    Vector_Copy(&effect->model->rotation, effect->start_rotation);
    motion->velocity.x = effect->def->offset_base[0] + ((rand() & 0xFFFF) / 32768.0f - 1.0f) * effect->def->offset_spread[0];
    motion->velocity.y = effect->def->offset_base[1] + ((rand() & 0xFFFF) / 32768.0f - 1.0f) * effect->def->offset_spread[1];
    motion->velocity.z = effect->def->offset_base[2] + ((rand() & 0xFFFF) / 32768.0f - 1.0f) * effect->def->offset_spread[2];
    motion->velocity.w = 0.0f;
    motion->spin.x = effect->def->rotation_base[0] + ((rand() & 0xFFFF) / 32768.0f - 1.0f) * effect->def->rotation_spread[0];
    motion->spin.y = effect->def->rotation_base[1] + ((rand() & 0xFFFF) / 32768.0f - 1.0f) * effect->def->rotation_spread[1];
    motion->spin.z = effect->def->rotation_base[2] + ((rand() & 0xFFFF) / 32768.0f - 1.0f) * effect->def->rotation_spread[2];
    motion->spin.w = 0.0f;
    EffectAngle_ToVector(effect->def->speed, &motion->accel, &effect->model->rotation);
}

/* Kind 53 update: the velocity gathers the acceleration (and def->gravity) until def->max_frames; the model
   sits at its start point plus the velocity turned by its rotation, and spins. Loops when def->radius_half
   is set. */
void EffectSpray_Update(EffectActor *effect) {
    SprayMotion *motion;
    float matrix[16];
    float offset[4];

    motion = (SprayMotion *)&effect->motion;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    Vector_Copy(&effect->model->position, effect->start_position);
    Vector_Copy(&effect->model->rotation, effect->start_rotation);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        if (effect->def->gravity != 0.0f) {
            motion->accel.y += effect->def->gravity;
        }
        sceVu0AddVector(&motion->velocity, &motion->velocity, &motion->accel);
    }
    sceVu0RotMatrixZ(matrix, D_00431AF0, effect->model->rotation.z);
    sceVu0RotMatrixX(matrix, matrix, effect->model->rotation.x);
    sceVu0RotMatrixY(matrix, matrix, effect->model->rotation.y);
    sceVu0ApplyMatrix(offset, matrix, &motion->velocity.x);
    Vector_Add(&effect->model->position, &effect->model->position, offset);
    Vector_Add(&effect->model->rotation, &effect->model->rotation, &motion->spin);
    effect->frame += 1;
    if (effect->def->radius_half != 0 && effect->frame >= effect->def->keyframe_end) {
        effect->frame = 0;
    }
}

/* Kind 51 start: launches along the heading; it falls after def->radius_half frames. */
void EffectLateFall_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    RadiusMotion *motion = (RadiusMotion *)&effect->motion;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk14 = 1.0f;
    motion->radius_half = effect->def->radius_half;
    motion->unk10 = 0;
    motion->unk1C = 0;
}

/* Kind 54 start: EffectLateFall_Start; the effects aimed at it go with it. */
void EffectLateFall_StartWithTargeters(EffectActor *actor, EffectSlot *spawn_slot) {
    EffectLateFall_Start(actor, spawn_slot);
    actor->flags |= EFFECT_FLAG_HAS_TARGETERS;
}

/* Kinds 51 and 54 update: flies, falling under def->gravity after the delay, faces its velocity; checks
   for hits. */
void EffectLateFall_Update(EffectActor *actor) {
    DelayedFallMotion *motion;

    EffectKeys_ApplyScale(actor);
    motion = (DelayedFallMotion *)&actor->motion;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        if (motion->unk18 < actor->frame) {
            motion->velocity.y += actor->def->gravity;
        }
        sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        EffectAngle_FromVector(&actor->model->rotation, &motion->velocity);
    }
    EffectHit_CheckAndEnd(actor, &motion->unk1C);
    actor->frame += 1;
}

/* Kind 55 start: launches along the heading. */
void EffectFallToFloor_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    HeadingMotion *motion = (HeadingMotion *)&effect->motion;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk10 = 0;
}

/* Kind 55 update: falls under def->gravity; where it meets a floor spawns def->end_effect on it and ends the
   next frame. */
void EffectFallToFloor_Update(EffectActor *actor) {
    EndingMotion *motion;
    Vector4 spawn_position;

    EffectKeys_ApplyScale(actor);
    motion = (EndingMotion *)&actor->motion;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    if (motion->ending != 0) {
        Effect_Free(actor);
    }
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        motion->velocity.y += actor->def->gravity;
        if (func_001C0E40(motion->velocity.y, &actor->model->position) == 0) {
            sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        } else {
            Vector_Copy(&spawn_position, &actor->model->position);
            spawn_position.y = COLLISION_RESULT->point[1];
            Effect_Spawn(actor->def->end_effect, &spawn_position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            motion->ending = 1;
        }
    }
    actor->frame += 1;
}

/* Kind 52 start: one of a row: the first takes def->unkF2 of the effect that spawned it as the count and
   launches along the heading, the next ones take the velocity and the count less one of the one before (0
   ends it). Settles on the floor, ending where there is none. */
void EffectFloorChase_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    FollowCountMotion *motion;
    FollowCountMotion *target_motion;

    motion = (FollowCountMotion *)&actor->motion;
    target_motion = (FollowCountMotion *)&actor->target->motion;
    EffectStart_PlaceModel(actor, spawn_slot);
    EffectStart_TurnModel(actor, spawn_slot);
    if (actor->target->def->unkF2 != 0) {
        motion->countdown = actor->target->def->unkF2;
        EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    } else {
        Vector_Copy(&motion->velocity, target_motion);
        motion->countdown = target_motion->countdown - 1;
        if (motion->countdown == 0) {
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
    motion->unk11 = 0;
}

/* Kind 52 update: steers toward the player along the floor; on its first frame checks for hits 100 higher:
   flag 0x10 ends it, flag 1 spawns def->end_effect and ends it. */
void EffectFloorChase_Update(EffectActor *actor) {
    BobbingMotion *motion;
    Vector4 offset;
    int check;

    motion = (BobbingMotion *)&actor->motion;
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        Vector_Subtract(&offset, &D_00413F80.position, &actor->model->position);
        EffectSteer_Toward(&motion->velocity, &offset, actor->def->speed, actor->def->unkDC);
        actor->model->position.y -= actor->def->unkE0 * 0.5f;
        motion->velocity.w = 0.0f;
        sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        if (func_001C0E40(actor->def->unkE0, &actor->model->position) == 0) {
            actor->frame = actor->def->keyframe_end;
            actor->model->position.y += actor->def->unkE0 * 0.5f;
        } else {
            actor->model->position.y = COLLISION_RESULT->point[1];
        }
    }
    if (motion->ending == 1) {
        Effect_Free(actor);
    } else if (actor->frame == 0) {
        actor->model->position.y -= 100.0f;
        check = func_001B4F90(actor);
        actor->model->position.y += 100.0f;
        if (check & 0x10) {
            motion->ending = 1;
        } else if (check & 1) {
            Effect_Spawn(actor->def->end_effect, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            motion->ending = 1;
        }
    }
    actor->frame += 1;
}

/* Kind 56 start: launches along the heading. */
void EffectHomeOnPlayer_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    HeadingMotion *motion = (HeadingMotion *)&effect->motion;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk10 = 0;
}

/* Kind 56 update: for def->radius_half frames steers toward 220 above the player and faces its velocity;
   moves until def->max_frames; checks for hits. */
void EffectHomeOnPlayer_Update(EffectActor *effect) {
    Vector4 to_player;
    EffectMotion *motion = &effect->motion;
    EffectDef *def;
    u16 max_frames;

    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    Vector_Subtract(&to_player, &D_00413F80.position, &effect->model->position);
    def = effect->def;
    if (effect->frame < def->radius_half) {
        to_player.y -= 220.0f;
        EffectSteer_Toward(&motion->velocity, &to_player, def->speed, def->unkDC);
        EffectAngle_FromVector(&effect->model->rotation, &motion->velocity);
        def = effect->def;
    }
    max_frames = def->max_frames;
    if (effect->frame < max_frames || max_frames == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    EffectHit_CheckAndEnd(effect, &motion->unk10);
    effect->frame += 1;
}

/* Kind 57 start: keeps its start point, with a random fall acceleration around def->gravity. */
void EffectTiltFall_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    TiltFallMotion *motion;
    f32 random_fraction;
    f32 gravity;

    motion = (TiltFallMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    Vector_Copy(&motion->velocity, &effect->model->position);
    motion->tilting_back = 0;
    motion->tilt_accel = 0.0f;
    motion->fall_speed = 0.0f;
    random_fraction = (rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f;
    gravity = effect->def->gravity;
    motion->tilt_speed = 0.0f;
    motion->fall_accel = random_fraction * 0.005f + gravity;
}

/* Kind 57 update: tilts to and fro between -def->unkE0 and def->unkE0 (speeding up by def->unkDC a
   frame) and, after def->radius_half frames, falls ever faster. */
void EffectTiltFall_Update(EffectActor *effect) {
    Vector4 forward;
    f32 unused_matrix[16]; // match: retail reserves 0x50 bytes of stack it never uses
    Vector4 unused_vector;
    TiltFallMotion *motion;
    f32 angle;

    memset(&forward, 0, 16);
    motion = (TiltFallMotion *)&effect->motion;
    forward.z = 1.0f;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    angle = effect->model->rotation.x + motion->tilt_speed;
    angle -= (float)(int)(angle / TWO_PI) * TWO_PI;
    if (PI < angle) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    effect->model->rotation.x = angle;
    motion->tilt_accel = effect->def->unkDC;
    if (effect->frame < effect->def->radius_half) {
        motion->tilt_accel *= 0.3f;
    }
    if (motion->tilting_back == 0) {
        if (effect->def->unkE0 < effect->model->rotation.x) {
            motion->tilt_speed -= motion->tilt_accel;
        } else {
            motion->tilt_speed += motion->tilt_accel;
            if (0.0f < motion->tilt_speed) {
                motion->tilting_back = 1;
                motion->tilt_speed = 0.0f;
                motion->tilt_accel = -motion->tilt_accel;
            }
        }
    } else {
        if (-effect->def->unkE0 < effect->model->rotation.x) {
            motion->tilt_speed -= motion->tilt_accel;
            if (motion->tilt_speed < 0.0f) {
                motion->tilting_back = 0;
                motion->tilt_speed = 0.0f;
                motion->tilt_accel = -motion->tilt_accel;
            }
        } else {
            motion->tilt_speed += motion->tilt_accel;
        }
    }
    if (effect->def->radius_half < effect->frame) {
        motion->fall_speed += motion->fall_accel;
        motion->velocity.y += motion->fall_speed * motion->fall_speed;
    }
    Vector_Add(&effect->model->position, &motion->velocity, &forward);
    effect->frame++;
}

/* Kind 59 start: sits def->unkDC off the floor (ends where there is none) and waits def->radius_half
   plus up to def->interval frames. */
void EffectFloorDelay_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    CountdownMotion *motion;

    motion = (CountdownMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    actor->model->position.y -= actor->def->unkDC;
    if (func_001C0E40(actor->def->unkDC * 2.0f, &actor->model->position) == 1) {
        actor->model->position.y = COLLISION_RESULT->point[1] - actor->def->unkDC;
    } else {
        Effect_Free(actor);
    }
    motion->countdown = actor->def->radius_half + rand() % actor->def->interval;
}

/* Kind 59 update: after the wait spawns def->unkF0 and ends. */
void EffectFloorDelay_Update(EffectActor *effect) {
    SpawnCountMotion *motion = (SpawnCountMotion *)&effect->motion;

    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (motion->spawn_count < effect->frame) {
        Effect_Spawn(effect->def->unkF0, &effect->model->position, &effect->model->rotation,
                      effect->owner_entity, effect->effect_param, effect->owner);
        Effect_Free(effect);
    }
    effect->frame += 1;
}

/* Kind 61 start: turns the heading by def->unkDC for each kind-61 effect started before it this frame,
   launches along it and lowers velocity.y by def->radius_half. */
void EffectTurnFan_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    SpinBurstMotion *motion;
    Vector4 angles;
    f32 heading;

    motion = (SpinBurstMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    Vector_Copy(&angles, &effect->model->rotation);
    angles.y += (f32)D_004468E8 * effect->def->unkDC;
    heading = angles.y;
    heading = heading - (f32)(int)(heading / 6.2831855f) * 6.2831855f;
    if (heading > 3.1415927f) {
        heading -= 6.2831855f;
    } else if (heading < -3.1415927f) {
        heading += 6.2831855f;
    }
    angles.y = heading;
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &angles);
    EffectAngle_FromVector(&effect->model->rotation, motion);
    motion->velocity.y -= effect->def->radius_half;
    motion->unk18 = 0;
    motion->start_frame = D_004468E8;
    motion->unk14 = 1.0f;
    D_004468E8 = motion->start_frame + 1;
}

/* Kind 61 update: moves, its x and z growing by def->scale_per_frame, until def->max_frames; checks for hits
   after frame def->unkF0. Resets the fan count for the next frame. */
void EffectTurnFan_Update(EffectActor *effect) {
    Vector4 velocity;
    EffectMotion *motion = &effect->motion;
    EffectDef *def;
    u16 max_frames;
    float scale;

    D_004468E8 = 0;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    max_frames = effect->def->max_frames;
    if (effect->frame < max_frames || max_frames == 0) {
        Vector_Copy(&velocity, &motion->velocity);
        def = effect->def;
        if (def->scale_per_frame != 0.0f) {
            scale = motion->unk10.y + def->scale_per_frame;
            motion->unk10.y = scale;
            velocity.x *= scale;
            velocity.z *= scale;
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, &velocity);
    }
    if (effect->def->unkF0 < effect->frame) {
        EffectHit_CheckAndEnd(effect, &motion->unk10.z);
    }
    effect->frame += 1;
}

/* Kind 62 start: placed; the effects aimed at it go with it. */
void EffectDelayedShot_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    BurstMotion *motion;

    motion = (BurstMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    motion->unk1C = 0;
    actor->flags |= EFFECT_FLAG_HAS_TARGETERS;
}

/* Kind 62 update: holds its start point until frame def->radius_half, launches along its start rotation
   then and moves on; checks for hits. */
void EffectDelayedShot_Update(EffectActor *effect) {
    EffectMotion *motion = &effect->motion;
    EffectDef *def;
    u16 frame;

    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    def = effect->def;
    frame = effect->frame;
    if (def->radius_half < frame) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    } else if (frame < def->radius_half) {
        Vector_Copy(&effect->model->position, effect->start_position);
    } else {
        EffectAngle_ToVector(def->speed, &motion->velocity, effect->start_rotation);
    }
    EffectHit_CheckAndEnd(effect, &motion->unk10.w);
    effect->frame += 1;
}

/* Kind 63 start: turned by def->unkDC and moved one step ahead for each level the spawner is below
   def->max_level; launches along the heading. */
void EffectLevelFan_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    LevelMotion *motion;
    Vector4 offset;
    f32 heading;

    motion = (LevelMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    heading = effect->model->rotation.y + (f32)(effect->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level) * effect->def->unkDC;
    heading = heading - (f32)(int)(heading / 6.2831855f) * 6.2831855f;
    if (heading > 3.1415927f) {
        heading -= 6.2831855f;
    } else if (heading < -3.1415927f) {
        heading += 6.2831855f;
    }
    effect->model->rotation.y = heading;
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    Vector_Scale(&offset.x, &motion->velocity.x, (f32)(effect->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level));
    Vector_Add(&effect->model->position, &effect->model->position, &offset);
    motion->scale = 1.0f;
    motion->unk14 = 0;
}

/* Kind 63 update: moves, its x and z growing by def->scale_per_frame, until def->max_frames; checks for hits. */
void EffectLevelFan_Update(EffectActor *effect) {
    GrowingMotion *motion;
    Vector4 offset;

    motion = (GrowingMotion *)&effect->motion;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        Vector_Copy(&offset, motion);
        if (effect->def->scale_per_frame != 0.0f) {
            motion->scale += effect->def->scale_per_frame;
            offset.x *= motion->scale;
            offset.z *= motion->scale;
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, &offset);
    }
    EffectHit_CheckAndEnd(effect, motion->unk14);
    effect->frame += 1;
}

/* Kind 74 start: sweeps 1000 along the heading and keeps the hit point (or the end) and its distance; steps
   of 100 along the heading. */
void EffectRay_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    TargetDistanceMotion *motion = (TargetDistanceMotion *)&effect->motion;
    Vector4 offset;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion->distance_to_target = 0;
    EffectAngle_ToVector(1000.0f, &offset, &effect->model->rotation);
    func_001C1430(1.0f, &effect->model->position, &offset);
    if (COLLISION_RESULT->hit != 0) {
        Vector_Copy(&motion->unk10, COLLISION_RESULT->point);
    } else {
        Vector_Add(&motion->unk10, &offset, &effect->model->position);
    }
    motion->distance_to_target = func_001B0178(&motion->unk10, &effect->model->position);
    EffectAngle_ToVector(100.0f, &motion->velocity, &effect->model->rotation);
}

/* Kind 64 start: sweeps toward the player and keeps the hit point (or 180 above the player) and its
   distance; turned toward it, steps of 100. */
void EffectRay_StartAtPlayer(EffectActor *effect, EffectSlot *spawn_slot) {
    TargetDistanceMotion *motion = (TargetDistanceMotion *)&effect->motion;
    Vector4 offset;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion->distance_to_target = 0;
    Vector_Subtract(&offset, &D_00413F80.position, &effect->model->position);
    func_001C1430(1.0f, &effect->model->position, &offset);
    if (COLLISION_RESULT->hit != 0) {
        Vector_Copy(&motion->unk10, COLLISION_RESULT->point);
    } else {
        Vector_Copy(&motion->unk10, &D_00413F80.position);
        motion->unk10.y -= 180.0f;
    }
    Vector_Subtract(&offset, &motion->unk10, &effect->model->position);
    EffectAngle_FromVector(&effect->model->rotation, &offset);
    motion->distance_to_target = func_001B0178(&motion->unk10, &effect->model->position);
    EffectAngle_ToVector(100.0f, &motion->velocity, &effect->model->rotation);
}

/* Kinds 64 and 74 update: at frame def->radius_half steps along the beam, one step per 100 of its length,
   with a hit check at each; runs no keys. */
void EffectRay_Update(EffectActor *effect) {
    ScalerMotion *motion = (ScalerMotion *)&effect->motion;
    s32 i;
    s32 steps;
    u8 flag;

    if (effect->frame == effect->def->radius_half) {
        steps = motion->scale_per_mille / 100.0f;
        for (i = 0; i < steps; i++) {
            flag = 0;
            sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
            EffectHit_CheckAndEnd(effect, &flag);
        }
    }
    effect->frame += 1;
}

/* Kind 65 start: keeps the beam length of the effect that spawned it. */
void EffectBeamStretch_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    LinkedMotion *motion;
    LinkedMotion *target_motion;

    target_motion = (LinkedMotion *)&effect->target->motion;
    motion = (LinkedMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion->unk20 = target_motion->unk20;
}

/* Kind 65 update: stretches the model's z scale by the beam length / 1000. */
void EffectBeamStretch_Update(EffectActor *effect) {
    ScalerMotion *motion;
    ModelInstance *model;

    EffectKeys_ApplyScale(effect);
    motion = (ScalerMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    model = effect->model;
    effect->frame += 1;
    model->unkE8 *= motion->scale_per_mille / 1000.0f;
}

/* Kind 66 start: the wobble angle starts at 0. */
void EffectWobble_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    ResetMotion *motion;

    motion = (ResetMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion->unk10 = 0;
}

/* Kind 66 update: turns the wobble angle by def->unkDC and pushes the model's x and y scale def->unkE0 along
   it; sits on the target. */
void EffectWobble_Update(EffectActor *effect) {
    HeadingAngleMotion *motion;
    DriftModel *model;
    Mat rotation_matrix;
    Vector4 forward;
    f32 heading;
    f32 zero;

    motion = (HeadingAngleMotion *)&effect->motion;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    zero = 0.0f;
    forward.x = 1.0f;
    forward.y = forward.z = forward.w = zero;
    heading = motion->heading + effect->def->unkDC;
    heading = heading - (f32)(int)(heading / 6.2831855f) * 6.2831855f;
    if (heading > 3.1415927f) {
        heading -= 6.2831855f;
    } else if (heading < -3.1415927f) {
        heading += 6.2831855f;
    }
    motion->heading = heading;
    sceVu0RotMatrixZ(rotation_matrix, D_00431AF0, heading);
    sceVu0ApplyMatrix(&forward.x, rotation_matrix, &forward.x);
    model = (DriftModel *)effect->model;
    model->drift_x += effect->def->unkE0 * forward.x;
    model->drift_y += effect->def->unkE0 * forward.y;
    Vector_Copy(&effect->model->position, &effect->target->model->position);
    effect->frame += 1;
}

/* Kind 73 start: launches along the heading; the effects aimed at it go with it. */
void EffectSeekPlayer_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    Vector4 *velocity;

    velocity = &effect->motion.velocity;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, velocity, &effect->model->rotation);
    effect->flags |= EFFECT_FLAG_HAS_TARGETERS;
}

/* Kind 73 update: for def->interval frames steers toward 110 above the player; on a hit spawns
   def->end_effect (not when the collision result's unk18 is 5) and ends. */
void EffectSeekPlayer_Update(EffectActor *actor) {
    Vector4 to_player;
    Vector4 *velocity;

    EffectKeys_ApplyScale(actor);
    velocity = &actor->motion.velocity;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    if (actor->frame < actor->def->interval) {
        Vector_Subtract(&to_player, &D_00413F80.position, &actor->model->position);
        to_player.y -= 110.0f;
        EffectSteer_Toward(velocity, &to_player, actor->def->speed, actor->def->unkDC);
    }
    sceVu0AddVector(&actor->model->position, &actor->model->position, velocity);
    if (func_001B4F90(actor) != 0 && actor->def->end_effect != 0xFFFF) {
        if (COLLISION_RESULT->unk18 != 5) {
            Effect_Spawn(actor->def->end_effect, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
        }
        Effect_Free(actor);
    }
    actor->frame++;
}

/* Kind 67 start: moves def->interval up and sweeps def->radius_half on up; starts at the hit with velocity.y
   def->speed, or ends where there is none. */
void EffectProbedFall_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectDef *def;
    Vector4 *velocity;
    Vector4 offset;

    velocity = &effect->motion.velocity;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    def = effect->def;
    offset.x = 0;
    offset.y = -def->radius_half;
    offset.z = 0;
    offset.w = 0;
    effect->model->position.y -= def->interval;
    func_001C1430(1.0f, &effect->model->position, &offset);
    if (COLLISION_RESULT->hit != 0) {
        Vector_Copy(&effect->model->position, COLLISION_RESULT->point);
    } else {
        Effect_Free(effect);
    }
    velocity->x = 0;
    velocity->y = effect->def->speed;
    velocity->z = 0;
    velocity->w = 0;
}

/* Kind 67 update: falls under def->gravity; checks for hits. */
void EffectProbedFall_Update(EffectActor *effect) {
    EffectMotion *motion;
    u8 hit_flag;

    hit_flag = 0;
    EffectKeys_ApplyScale(effect);
    motion = &effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    motion->velocity.y += effect->def->gravity;
    sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
    EffectHit_CheckAndEnd(effect, &hit_flag);
    effect->frame += 1;
}

/* Kind 68 start: placed def->radius_half from the player at the player's heading plus def->rotation_base[1]
   and def->unkDC per spawner level, def->interval up (with up to 50 at random), facing 110 above the
   player. It launches at frame 30 plus def->unkF0 per level and stops turning def->unkF2 frames before. */
void EffectSurroundPlayer_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    Mat rotation_matrix;
    Vector4 offset;
    Vector4 player_point;
    f32 heading;

    motion = (TurningMotion *)&effect->motion;
    offset.x = 0.0f;
    offset.y = ((f32)(rand() & 0xFFFF) * 0.000030517578f - 1.0f) * 50.0f;
    offset.y = (f32)(-effect->def->interval) + offset.y;
    offset.z = (f32)effect->def->radius_half;
    offset.w = 0.0f;
    heading = (D_00413F80.rotation.y + effect->def->rotation_base[1]) + (f32)(effect->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level) * effect->def->unkDC;
    heading = heading - (f32)(int)(heading / 6.2831855f) * 6.2831855f;
    if (heading > 3.1415927f) {
        heading -= 6.2831855f;
    } else if (heading < -3.1415927f) {
        heading += 6.2831855f;
    }
    sceVu0RotMatrixY(rotation_matrix, D_00431AF0, heading);
    sceVu0ApplyMatrix(&offset, rotation_matrix, &offset);
    Vector_Add(&effect->model->position, &D_00413F80.position, &offset);
    Vector_Copy(&player_point, &D_00413F80.position);
    player_point.y -= 110.0f;
    Vector_Subtract(&offset, &player_point, &effect->model->position);
    EffectAngle_FromVector(&effect->model->rotation, &offset);
    motion->unk20 = (s16)((effect->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level) * effect->def->unkF0 + 30);
    motion->unk22 = (s16)(motion->unk20 - effect->def->unkF2);
    motion->unk24 = 0;
}

/* Kind 68 update: keeps facing the player until its aim frame; at its launch frame launches along its
   heading and plays sound def->unkE0; moves and checks for hits; ends 60 frames after the launch. */
void EffectSurroundPlayer_Update(EffectActor *effect) {
    PlayerChaseMotion *motion;
    Vector4 offset;
    Vector4 player_offset;

    EffectKeys_ApplyScale(effect);
    motion = (PlayerChaseMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame >= motion->end_frame) {
        if (effect->frame == motion->end_frame) {
            EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
            Sound_PlayMapAt((u8)effect->def->unkE0, &effect->model->position.x);
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        EffectHit_CheckAndEnd(effect, motion->unk24);
        if (effect->frame > motion->end_frame + 0x3C) {
            Effect_Free(effect);
        }
    } else if (effect->frame < motion->player_frame) {
        Vector_Copy(&player_offset, &D_00413F80.position);
        player_offset.y -= 110.0f;
        Vector_Subtract(&offset, &player_offset, &effect->model->position);
        EffectAngle_FromVector(&effect->model->rotation, &offset);
    }
    effect->frame += 1;
}

/* Kind 75 start: one link of a chain: turns it a random amount and spawns the next link (def->radius_half) a
   random length further on, def->interval links in all. */
void EffectChain_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    Vector4 step;
    Vector4 saved_position;
    Vector4 saved_rotation;
    ChainMotion *motion;
    f32 angle;

    EffectStart_TurnModel(effect, spawn_slot);
    motion = (ChainMotion *)&effect->motion;
    EffectStart_PlaceModel(effect, spawn_slot);
    motion->length = ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * 100.0f + 100.0f;
    if (effect->target->def->unkF2 != 0) {
        motion->links_left = effect->def->interval;
    } else {
        motion->links_left = ((ChainMotion *)&effect->target->motion)->links_left - 1;
    }
    angle = effect->model->rotation.x + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->unkDC;
    angle -= (float)(int)(angle / TWO_PI) * TWO_PI;
    if (PI < angle) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    effect->model->rotation.x = angle;
    angle = effect->model->rotation.y + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->unkE0;
    angle -= (float)(int)(angle / TWO_PI) * TWO_PI;
    if (PI < angle) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    effect->model->rotation.y = angle;
    if (motion->links_left != 0) {
        Vector_Copy(&saved_position, &effect->model->position);
        Vector_Copy(&saved_rotation, &effect->model->rotation);
        EffectAngle_ToVector(motion->length, &step, &effect->model->rotation);
        Vector_Add(&effect->model->position, &effect->model->position, &step);
        Effect_SpawnFromEffect(effect->def->radius_half, effect);
        Vector_Copy(&effect->model->position, &saved_position);
        Vector_Copy(&effect->model->rotation, &saved_rotation);
    }
}

/* Kind 75 update: stretches the model's z scale by the link length / 1000. */
void EffectChain_Update(EffectActor *effect) {
    ScaleRateMotion *motion;
    ModelInstance *model;

    EffectKeys_ApplyScale(effect);
    motion = (ScaleRateMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    model = effect->model;
    effect->frame += 1;
    model->unkE8 *= motion->scale_per_mille / 1000.0f;
}

/* Kind 77 start: lifted by the owner's height, looks for a floor (the other strips, func_001C1138) within
   that reach and sits on it; marks it when there is none. */
void EffectOwnerFloor_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    GroundedMotion *motion;
    Entity *target;

    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    motion = (GroundedMotion *)&actor->motion;
    motion->no_floor = 0;
    target = actor->owner_entity;
    actor->model->position.y -= target->height;
    func_001C1138(target->height, &actor->model->position);
    if (COLLISION_RESULT->hit != 0) {
        actor->model->position.y = COLLISION_RESULT->point[1];
    } else {
        motion->no_floor = 1;
    }
}

/* Kind 77 update: ends when no floor was found, else runs its keys. */
void EffectOwnerFloor_Update(EffectActor *effect) {
    if (*(s8 *)&effect->motion != 0) { // match: velocity.x's first byte is read as a signed byte
        return Effect_Free(effect);
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    effect->frame += 1;
}

/* Kind 78 start: at its start pose; keeps the owner entity. */
void EffectLaunchFromOwner_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    SavedArgMotion *motion;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion = (SavedArgMotion *)&effect->motion;
    motion->owner_entity = effect->owner_entity;
    Vector_Copy(&effect->model->position, effect->start_position);
    Vector_Copy(&effect->model->rotation, effect->start_rotation);
    motion->unk14 = 0;
}

/* Kind 78 update: holds its start pose until frame def->unkF2, then flies backward along the owner model's
   heading at def->speed until def->max_frames; checks for hits. */
void EffectLaunchFromOwner_Update(EffectActor *effect) {
    AttachedMotion *motion;

    EffectKeys_ApplyScale(effect);
    motion = (AttachedMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame < effect->def->unkF2) {
        Vector_Copy(&effect->model->position, effect->start_position);
        Vector_Copy(&effect->model->rotation, effect->start_rotation);
    } else if (effect->def->unkF2 == effect->frame) {
        EffectAngle_ToVector(-effect->def->speed, &motion->velocity, &(*motion->anchor)->rotation);
    } else {
        if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
            sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        }
        EffectHit_CheckAndEnd(effect, motion->unk14);
    }
    effect->frame++;
}

/* Kind 80 start: clears the end countdown. */
void EffectBowDraw_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    StateFlagMotion *motion;

    EffectStart_TurnModel(effect, spawn_slot);
    motion = (StateFlagMotion *)&effect->motion;
    EffectStart_PlaceModel(effect, spawn_slot);
    motion->state = 0;
}

/* Kind 80 update: at its start pose while the attack button (D_0044693C) is held. Once it is let go (and
   the game status allows), spawns def->radius_half scaled by the draw strength (PlayerState.unkBC, up to
   5000) if the draw was full, and ends three frames later with its scale at 0. */
void EffectBowDraw_Update(EffectActor *actor) {
    ShockMotion *motion;

    motion = (ShockMotion *)&actor->motion;
    if (motion->countdown != 0) {
        actor->model->unkE0 = 0;
        actor->model->unkE4 = 0;
        actor->model->unkE8 = 0;
        motion->countdown--;
        if (motion->countdown <= 0) {
            Effect_Free(actor);
        }
        return;
    }
    EffectKeys_ApplyScale(actor);
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    Vector_Copy(&actor->model->position, actor->start_position);
    Vector_Copy(&actor->model->rotation, actor->start_rotation);
    if (D_0038CC50.unk00 != 0) {
        if (!(D_0038CC50.unk30 & 1)) {
            if ((D_00413DA8[0] & D_0044693C) && D_00413F80.unk263 == 0) {
                return;
            }
            if (D_00413F80.unk298 != 0 && D_00413F80.unk263 == 0) {
                Effect_SpawnFromEffect(actor->def->radius_half, actor);
                motion->scale = actor->def->speed * ((float)D_00413F80.unkBC / 5000.0f) + 1.0f;
            }
            motion->countdown = 3;
        }
    }
}

/* Kind 81 start: launches along the heading at the speed in the target effect's velocity.x. */
void EffectInheritSpeed_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectActor *target;
    TargetFollowerMotion *motion;

    target = effect->target;
    motion = (TargetFollowerMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion->speed = target->motion.velocity.x;
    EffectAngle_ToVector(motion->speed, &motion->velocity, &effect->model->rotation);
    motion->unk14 = 0;
    motion->unk18 = 1.0f;
    motion->unk1C = 0;
}

/* Kind 81 update: flies, falling under def->gravity after def->radius_half frames, faces its velocity;
   checks for hits. */
void EffectInheritSpeed_Update(EffectActor *effect) {
    FallingLateMotion *motion;

    EffectKeys_ApplyScale(effect);
    motion = (FallingLateMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        if (effect->def->radius_half < effect->frame) {
            motion->velocity.y += effect->def->gravity;
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        EffectAngle_FromVector(&effect->model->rotation, &motion->velocity);
    }
    EffectHit_CheckAndEnd(effect, motion->unk1C);
    effect->frame++;
}

/* Kind 82 start: picks the entity the player faces most nearly (EffectHit_FindAimedEntity) and launches at
   the speed in the target effect's velocity.x. */
void EffectHomeOnEntity_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    float speed;
    FollowerMotion *motion;
    EffectActor *target;

    target = actor->target;
    motion = (FollowerMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    actor->unk6C = EffectHit_FindAimedEntity(actor, &D_00413F80.position, &D_00413F80.rotation);
    speed = target->motion.velocity.x;
    motion->speed = speed;
    EffectAngle_ToVector(speed, &motion->velocity, &actor->model->rotation);
    motion->unk14 = 0;
    motion->unk18 = 1.0f;
    motion->unk20 = 0;
}

/* Kind 82 update: until frame def->unkF0 steers toward the middle of the picked entity; falls after
   def->radius_half frames; stops where its path meets a wall and ends the next frame; faces its velocity;
   checks for hits. */
void EffectHomeOnEntity_Update(EffectActor *actor) {
    FollowerMotion *motion;
    Vector4 delta;
    char unused[0x10]; /* unused, but retail reserves the bytes */

    motion = (FollowerMotion *)&actor->motion;
    if (motion->unk20 != 0) {
        Effect_Free(actor);
        return;
    }
    EffectKeys_ApplyScale(actor);
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    if (actor->def->unkF0 < actor->frame) {
        actor->unk6C = 0;
    }
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        if (actor->unk6C != 0) {
            Vector_Subtract(&delta, ((Entity *)actor->unk6C)->model->position, &actor->model->position);
            delta.y -= ((Entity *)actor->unk6C)->height * 0.5f;
            EffectSteer_Toward(&motion->velocity, &delta, motion->speed, actor->def->unkDC);
        }
        if (actor->def->radius_half < actor->frame) {
            motion->velocity.y += actor->def->gravity;
        }
        if (motion->unk20 == 0) {
            func_001C1430(1.0f, &actor->model->position, motion);
            if (COLLISION_RESULT->hit != 0) {
                motion->unk20 = 1;
                func_001B0178(COLLISION_RESULT->point, &actor->model->position);
                return;
            }
        }
        sceVu0AddVector(&actor->model->position, &actor->model->position, motion);
        EffectAngle_FromVector(&actor->model->rotation, motion);
    }
    EffectHit_CheckAndEnd(actor, &motion->unk20);
    actor->frame++;
}

/* Kind 90 start: 220 higher, launches along the heading and takes the frame and point it reaches what lies
   ahead (within def->radius_half). */
void EffectImpactShot_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    TurnMotion *motion;

    motion = (TurnMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    effect->model->position.y -= 220.0f;
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->end_frame = func_001B0208((float)effect->def->radius_half, effect, motion, motion->unk10);
    motion->done = 0;
}

/* Kind 90 update: moves; at the impact frame spawns def->end_effect at the impact point and ends; checks
   for hits. */
void EffectImpactShot_Update(EffectActor *effect) {
    EndFrameMotion *motion;

    EffectKeys_ApplyScale(effect);
    motion = (EndFrameMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
    if (motion->done) {
        Effect_Free(effect);
    } else if (effect->frame == motion->end_frame) {
        Effect_Spawn(effect->def->end_effect, &motion->unk10, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        motion->done = 1;
    } else {
        EffectHit_CheckAndEnd(effect, &motion->done);
    }
    effect->frame++;
}

/* Kind 91 start: starts a light point (LinkedPoint) at the model, its handle in the flag word, and keeps
   the target's position. */
void EffectFollowLight_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    s64 constant_copy[2];
    Vector4 *velocity;

    velocity = &effect->motion.velocity;
    constant_copy[0] = D_0043EF60[0];
    constant_copy[1] = D_0043EF60[1];
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EFFECT_FLAG_PARTS(effect)->unk44 = LINKED_POINT_START_S16(0.0f, &effect->model->position, constant_copy);
    Vector_Copy(velocity, &effect->target->model->position);
}

/* Kind 91 update: moves the light point to the effect over 100 x the z scale, and the model to where the
   target was a frame ago; runs no spin keys. */
void EffectFollowLight_Update(EffectActor *effect) {
    EffectMotion *motion;

    EffectKeys_ApplyScale(effect);
    motion = &effect->motion;
    EffectKeys_ApplyColour(effect);
    LinkedPoint_Retarget(100.0f * effect->model->unkE8, EFFECT_FLAG_PARTS(effect)->unk44, effect);
    Vector_Copy(&effect->model->position, motion);
    Vector_Copy(motion, &effect->target->model->position);
    effect->frame += 1;
}

/* Kind 92 start: starts a light point (LinkedPoint) at the model, its handle in the flag word. */
void EffectLight_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    s64 constant_copy[2];

    constant_copy[0] = D_0043EF60[0];
    constant_copy[1] = D_0043EF60[1];
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EFFECT_FLAG_PARTS(effect)->unk44 = LINKED_POINT_START_S16(0.0f, &effect->model->position, constant_copy);
}

/* Kind 92 update: moves the light point to the effect over 100 x the z scale; runs no spin keys. */
void EffectLight_Update(EffectActor *effect) {
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplyColour(effect);
    LinkedPoint_Retarget(100.0f * effect->model->unkE8, EFFECT_FLAG_PARTS(effect)->unk44, effect);
    effect->frame += 1;
}

/* Kind 85 start: keeps the centre, the start angle (heading plus def->unkE0), the rise per frame
   (def->interval over the life), the widening per frame (def->radius_half over half the life) and a
   random blink phase. */
void EffectSpiral_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    SpiralMotion *motion;
    EffectDef *def;
    ModelInstance *model;
    int random_step;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion = (SpiralMotion *)&effect->motion;
    Vector_Copy(motion, &effect->model->position);
    model = effect->model;
    def = effect->def;
    motion->unk14 = 0;
    motion->start_angle = model->rotation.y + def->unkE0;
    motion->unk18 = (f32)def->radius_half / ((f32)def->keyframe_end * 0.5f);
    motion->unk1C = (f32)def->interval / (f32)def->keyframe_end;
    random_step = rand() % 10;
    motion->unk21 = 0;
    motion->start_step = (u8)random_step;
}

/* Kind 85 update: circles the centre (def->unkDC a frame) while it rises and widens; blinks, hidden
   def->unkF2 frames and shown def->unkF0. */
void EffectSpiral_Update(EffectActor *effect) {
    SwingMotion *motion;
    EffectDef *def;
    Vector4 spin;
    f32 matrix[16];
    f32 angle;

    EffectKeys_ApplyScale(effect);
    motion = (SwingMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    motion->counter -= 1;
    if ((s8)motion->counter < 0) {
        if (motion->toggle == 1) {
            motion->toggle = 0;
            motion->counter = effect->def->unkF0;
        } else {
            motion->toggle = 1;
            motion->counter = effect->def->unkF2;
        }
    }
    if (motion->toggle != 0) {
        effect->scale = 0;
    }
    motion->phase = motion->phase + motion->phase_step;  // match: operand order picks retail registers
    spin.y = motion->spin_per_frame * (float)effect->frame;
    spin.z = motion->phase;
    spin.x = 0.0f;
    spin.w = 0.0f;
    angle = motion->angle + effect->def->unkDC;
    angle -= (float)(int)(angle / TWO_PI) * TWO_PI;
    if (PI < angle) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    motion->angle = angle;
    sceVu0RotMatrixY(matrix, D_00431AF0, angle);
    sceVu0ApplyMatrix(&spin, matrix, &spin);
    Vector_Add(&effect->model->position, &motion->velocity, &spin);
    effect->frame++;
}

/* Kind 86 start: at def->offset_base turned by the player's heading, plus a random spread, randomly turned,
   def->radius_half off the floor found within 1500 of it; ends where there is none. Velocity.y is
   def->speed. */
void EffectScatterAroundPlayer_Start(EffectActor *effect) {
    Vector4 offset;
    f32 matrix[16];
    EffectMotion *motion;

    sceVu0RotMatrixY(matrix, D_00431AF0, D_00413F80.rotation.y);
    sceVu0ApplyMatrix(&offset, matrix, effect->def->offset_base);
    motion = &effect->motion;
    Vector_Add(&effect->model->position, &D_00413F80, &offset);
    effect->model->position.x += ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->offset_spread[0];
    effect->model->position.z += ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->offset_spread[2];
    effect->model->rotation.x = ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * 3.1415927f;
    effect->model->rotation.y = ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * 3.1415927f;
    effect->model->rotation.z = 0.0f;
    effect->motion.velocity.x = 0.0f;
    motion->velocity.y = effect->def->speed;
    motion->velocity.z = 0.0f;
    effect->model->position.y -= 1500.0f;
    offset.y = 3000.0f;
    offset.x = 0.0f;
    offset.z = 0.0f;
    // match: argument order as (position, direction, scale) schedules the argument moves as retail does
    ((void (*)(void *, void *, f32))func_001C1430)(&effect->model->position, &offset, 1.0f);
    if (COLLISION_RESULT->hit != 0) {
        effect->model->position.y = COLLISION_RESULT->point[1] - (f32)effect->def->radius_half;
    } else {
        Effect_Free(effect);
    }
}

/* Kind 86 update: moves by the velocity. */
void EffectScatterAroundPlayer_Update(EffectActor *effect) {
    EffectMotion *motion = &effect->motion;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    Vector_Add(&effect->model->position, &effect->model->position, motion);
    effect->frame += 1;
}

/* Kind 87 start: finds the floor below and the frame it lands on it, falling from frame def->unkF0 under
   def->gravity; 0xFFFF: none. */
void EffectDrop_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    Vector4 probe;
    LandingFrameMotion *motion;
    f32 distance;
    f32 velocity;
    f32 fallen;
    int i;

    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    motion = (LandingFrameMotion *)&actor->motion;
    probe.x = 0.0f;
    probe.y = 3000.0f;
    probe.z = 0.0f;
    motion->landing_frame = 0xFFFF;
    func_001C1430(1.0f, &actor->model->position, &probe);
    if (COLLISION_RESULT->hit != 0) {
        Vector_Copy(&actor->motion, COLLISION_RESULT);
        motion->unk16 = *(u8 *)&COLLISION_RESULT->unk18; // match: a byte load of the word's low byte
        distance = func_001B0178(&actor->motion, &actor->model->position);
        velocity = 0.0f;
        fallen = 0.0f;
        for (i = 0; i < actor->def->keyframe_end; i++) {
            velocity += actor->def->gravity;
            fallen += velocity;
            if (distance < fallen) {
                motion->landing_frame = actor->def->unkF0 + i;
                break;
            }
        }
    }
}

/* Kind 87 update: from frame def->unkF0 falls under def->gravity; at the landing frame spawns
   def->radius_half (surface kind 5) or def->interval there and ends. */
void EffectDrop_Update(EffectActor *effect) {
    GravityMotion *motion;
    EffectDef *def;
    ModelInstance *model;
    u16 spawn_id;

    EffectKeys_ApplyScale(effect);
    motion = (GravityMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    def = effect->def;
    if (effect->frame >= def->unkF0) {
        model = effect->model;
        motion->vertical_speed += def->gravity;
        model->position.y += motion->vertical_speed;
        if (effect->frame >= motion->landing_frame) {
            if (motion->landing_mode == 5) {
                spawn_id = def->radius_half;
            } else {
                spawn_id = def->interval;
            }
            Effect_Spawn(spawn_id, &motion->velocity, &model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            Effect_Free(effect);
        }
    }
    effect->frame++;
}

/* Kind 88 start: only places and turns the model. */
void EffectPitchScale_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
}

/* Kind 88 update: scales the model's x and y scale by 10 x its pitch (rotation.x). */
void EffectPitchScale_Update(EffectActor *effect) {
    ModelInstance *model;

    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    model = effect->model;
    effect->frame++;
    model->unkE0 = model->unkE0 * model->rotation.x * 10.0f;
    model->unkE4 = model->unkE4 * model->rotation.x * 10.0f;
}

/* Kind 89 start: the pitch in degrees becomes the frames between two steps. */
void EffectRepeater_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    HeadingWordMotion *motion;
    s32 degrees;

    motion = (HeadingWordMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    degrees = (actor->model->rotation.x * 180.0f) / 3.1415f;
    motion->heading = degrees;
    motion->heading_word = motion->heading;
}

/* Kind 89 update: steps the frame every so many frames; at def->keyframe_end spawns def->radius_half and
   starts over. */
void EffectRepeater_Update(EffectActor *effect) {
    RepeaterMotion *motion;
    EffectDef *def;
    u16 reload_frames;
    int countdown;

    EffectKeys_ApplyScale(effect);
    motion = (RepeaterMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    countdown = motion->countdown - 1;
    motion->countdown = countdown;
    if (countdown <= 0) {
        def = effect->def;
        reload_frames = motion->reload_frames;
        effect->frame += 1;
        motion->countdown = reload_frames;
        if (effect->frame >= def->keyframe_end) {
            Effect_Spawn(def->radius_half, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            effect->frame = 0;
        }
    }
}

/* Kind 100 start: launches along the heading from its centre, at a random angle up to def->unkE4 and
   radius def->unkE8. */
void EffectCorkscrew_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    SpreadMotion *motion = (SpreadMotion *)&effect->motion;
    f32 random_turn;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &effect->motion.velocity, &effect->model->rotation);
    Vector_Copy(&motion->unk10, &effect->model->position);
    random_turn = (rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f;
    motion->unk20 = random_turn * effect->def->unkE4;
    motion->unk24 = effect->def->unkE8;
}

/* Kind 100 update: the centre moves by the velocity; the model circles it (def->unkDC a frame) at a radius
   that grows by def->unkE0. */
void EffectCorkscrew_Update(EffectActor *effect) {
    SpinAngleMotion *motion = (SpinAngleMotion *)&effect->motion;
    EffectDef *def;
    float spin_vector[4];
    float matrix[16];
    float angle;
    float spin_speed;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    def = effect->def;
    spin_speed = motion->unk24 + def->unkE0;
    spin_vector[1] = 0.0f;
    spin_vector[2] = 0.0f;
    spin_vector[3] = 0.0f;
    spin_vector[0] = motion->unk24 = spin_speed;
    angle = motion->unk20 + def->unkDC;
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    motion->unk20 = angle;
    sceVu0RotMatrixZ(matrix, D_00431AF0, angle);
    sceVu0RotMatrixX(matrix, matrix, effect->model->rotation.x);
    sceVu0RotMatrixY(matrix, matrix, effect->model->rotation.y);
    sceVu0ApplyMatrix(spin_vector, matrix, spin_vector);
    Vector_Add(&motion->unk10, &motion->unk10, motion);
    Vector_Add(&effect->model->position, &motion->unk10, spin_vector);
    effect->frame += 1;
}

/* Kind 101 start: launches along the heading; the effects aimed at it go with it. */
void EffectWander_Start(EffectActor *actor, EffectSlot *spawn_slot) {
    EffectMotion *motion;

    motion = &actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    actor->flags |= (s64)0x8000 << 0x2B;
}

/* Kind 101 update: turns its velocity a random amount up to def->unkDC; past y def->unkE0 its y speed
   flips back. */
void EffectWander_Update(EffectActor *effect) {
    EffectMotion *motion = &effect->motion;
    float matrix[16];
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    sceVu0RotMatrixX(matrix, D_00431AF0, ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->unkDC);
    sceVu0RotMatrixY(matrix, matrix, ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->unkDC);
    sceVu0ApplyMatrix(&motion->velocity.x, matrix, &motion->velocity.x);
    if (effect->def->unkE0 < effect->model->position.y && 0.0f < motion->velocity.y) {
        motion->velocity.y = -motion->velocity.y;
    }
    Vector_Add(&effect->model->position, &effect->model->position, motion);
    effect->frame += 1;
}

/* Kind 102 start: keeps the target's position and rotation. */
void EffectFollowPoseScaled_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectMotion *motion = &effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    Vector_Copy(motion, &effect->target->model->position);
    Vector_Copy(&effect->motion.unk10, &effect->target->model->rotation);
}

/* Kind 102 update: takes the pose the target had a frame ago, its alpha scaled by the target's / 128;
   loops. */
void EffectFollowPoseScaled_Update(EffectActor *effect) {
    EffectMotion *motion;
    Vector4 *motion_rotation;
    int next_frame;

    motion = &effect->motion;
    motion_rotation = &motion->unk10;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    effect->scale *= effect->target->scale * 0.0078125f;
    Vector_Copy(&effect->model->position, &motion->velocity);
    Vector_Copy(&effect->model->rotation, motion_rotation);
    Vector_Copy(&motion->velocity, &effect->target->model->position);
    Vector_Copy(motion_rotation, &effect->target->model->rotation);
    next_frame = effect->frame + 1;
    effect->frame = next_frame;
    if ((u16)next_frame >= effect->def->keyframe_end) { // match: compares the masked 16-bit frame
        effect->frame = 0;
    }
}

/* Kind 104 start: the wobble angle starts at 0. */
void EffectJitterWobble_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    ResetMotion *motion = (ResetMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion->unk10 = 0;
}

/* Kind 104 update: turns the wobble angle by def->unkDC plus up to def->unkE4 at random and pushes the
   model's x and y scale def->unkE0 along it; sits on the target. */
void EffectJitterWobble_Update(EffectActor *effect) {
    TiltMotion *motion = (TiltMotion *)&effect->motion;
    float matrix[16];
    float axis[4];
    float angle;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    axis[0] = 1.0f;
    axis[1] = axis[2] = axis[3] = 0.0f;
    angle = motion->spin_angle + effect->def->unkDC + ((rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->unkE4;
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (3.1415927f < angle) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    motion->spin_angle = angle;
    sceVu0RotMatrixZ(matrix, D_00431AF0, angle);
    sceVu0ApplyMatrix(axis, matrix, axis);
    effect->model->unkE0 = effect->model->unkE0 + effect->def->unkE0 * axis[0];
    ((TiltModel *)effect->model)->unkE4 = ((TiltModel *)effect->model)->unkE4 + effect->def->unkE0 * axis[1];
    Vector_Copy(&effect->model->position, &effect->target->model->position);
    effect->frame += 1;
}

/* Kind 105 start: keeps its centre; the offset is def->unkE0 along z, the turn steps 0.1. */
void EffectJitterOrbit_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    StartMotion *motion = (StartMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    Vector_Copy(&motion->unk10, &effect->model->position);
    motion->velocity.z = effect->def->unkE0;
    effect->motion.velocity.x = motion->velocity.y = motion->velocity.w = 0.0f;
    motion->unk24 = motion->unk20 = 0.1f;
}

/* Kind 105 update: turns its offset by a pitch and heading turn that drift at random (by up to def->unkDC,
   at most 0.15) and sits at the centre plus the offset; the frame does not advance. */
void EffectJitterOrbit_Update(EffectActor *effect) {
    YawPitchMotion *motion;
    f32 rotation_matrix[16];

    motion = (YawPitchMotion *)&effect->motion;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    motion->yaw = motion->yaw + ((float)(rand() & 0xFFFF) * 0.000030517578125f - 1.0f) * effect->def->unkDC;
    motion->pitch = motion->pitch + ((float)(rand() & 0xFFFF) * 0.000030517578125f - 1.0f) * effect->def->unkDC;
    if (motion->yaw > 0.15f) {
        motion->yaw = 0.15f;
    }
    if (motion->pitch > 0.15f) {
        motion->pitch = 0.15f;
    }
    if (motion->yaw < -0.15f) {
        motion->yaw = -0.15f;
    }
    if (motion->pitch < -0.15f) {
        motion->pitch = -0.15f;
    }
    sceVu0RotMatrixX(rotation_matrix, D_00431AF0, motion->yaw);
    sceVu0RotMatrixY(rotation_matrix, rotation_matrix, motion->pitch);
    sceVu0ApplyMatrix(motion, rotation_matrix, motion);
    Vector_Add(&effect->model->position, motion->unk10, motion);
}

/* Kind 106 start: launches along the heading. */
void EffectSeekPoint_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectMotion *motion;

    motion = &effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
}

/* Kind 106 update: after def->radius_half frames, while D_0034A019 is set, steers toward the world point
   (0, -270, 0); moves until def->max_frames. */
void EffectSeekPoint_Update(EffectActor *effect) {
    Vector4 ground;
    Vector4 to_ground;
    EffectMotion *motion;

    motion = &effect->motion;
    memset(&ground, 0, 16);
    ground.y = -270.0f;
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        if (D_0034A019[0] != 0 && effect->def->radius_half < effect->frame) {
            Vector_Subtract(&to_ground, &ground, &effect->model->position);
            EffectSteer_Toward(&motion->velocity, &to_ground, effect->def->speed, effect->def->unkDC);
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
    }
    effect->frame += 1;
}

/* Kind 107 start: drops the target and clears the stop flag. */
void EffectLoopUntilFlag_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    effect->target = NULL;
    *(int *)&effect->motion = 0; // match: an int store of the velocity's first word (sw $0)
}

/* Kind 107 update: loops its first def->radius_half frames until D_0034AFD6 is 3. */
void EffectLoopUntilFlag_Update(EffectActor *effect) {
    LoopMotion *motion;

    EffectKeys_ApplyScale(effect);
    motion = (LoopMotion *)&effect->motion;
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    effect->frame += 1;
    if (D_0034AFD6[0] == 3) {
        motion->stopped = 1;
    }
    if (motion->stopped == 0) {
        if (effect->frame >= effect->def->radius_half) {
            effect->frame = 0;
        }
    }
}

/* Kind 99 start: plays sound 2 of the weapon in hand (def->radius_half 2), or sound 3 when the weapon and
   the effect's param are a pair of the table D_00402D50 (Sound_PlaySw). */
void EffectWeaponSound_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectDef *def;
    u32 *key;
    u32 *param;
    int i;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    def = effect->def;
    if (def->radius_half == 2) {
        Sound_PlaySw(def->radius_half);
    } else if (def->radius_half == 3) {
        // match: the table walk is two pointers stepped together; this loop form keeps retail's registers
        for (i = 0, param = &D_00402D50[1], key = &D_00402D50[0]; i < 7; key += 2, i++) {
            if (D_00413F80.weapon == *key && effect->effect_param == *param) {
                Sound_PlaySw(def->radius_half);
                break;
            }
            param += 2;
        }
    }
}

/* Kind 98 start: plays sound def->radius_half at the model. */
void EffectSound_Start(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    Sound_PlayMapAt(*(u8 *)&effect->def->radius_half, &effect->model->position.x);
}
