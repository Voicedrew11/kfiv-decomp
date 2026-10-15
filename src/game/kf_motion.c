/* Motion: the keyed poses of a model's parts, played a step at a time, blended in, and the movement of the
   root part handed on to the entity. A part's slot (ModelPart.unk6C) lets two motions play on one model. */
#include "kf_motion.h"
#include "kf_entity_events.h"
#include "libvu0.h"

void Vector_Copy(void *, void *);
void sceVu0MulVector(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
void Vector_Scale(float *, float *, float);
float sinf(float);
float cosf(float);
extern f32 D_00431AF0[16];
void Vector_Hermite(void *, void *, void *, void *, void *, f32);

/* dst = a * (1 - t) + b * t across the xyz lanes, on the VU0 (EntityMath_LerpVector inlined) */
static inline void blend_vectors(Vector4 *dst, Vector4 *src_a, Vector4 *src_b, f32 ratio) {
    __asm__ volatile(
        "qmtc2.ni %3, $vf3\n"
        "vsubx.w $vf4, $vf0, $vf3x\n"
        "lqc2 $vf1, 0x0(%1)\n"
        "lqc2 $vf2, 0x0(%2)\n"
        "vmulax.xyz ACC, $vf2, $vf3x\n"
        "vmaddw.xyz $vf1, $vf1, $vf4w\n"
        "vnop\n"
        "vnop\n"
        "vnop\n"
        "sqc2 $vf1, 0x0(%0)\n"
        : : "r"(dst), "r"(src_a), "r"(src_b), "r"(ratio));
}

static s32 Motion_PoseSlot(ModelInstance *model, s32 slot, s32 table, s32 frame_fixed, s32 step, s32 backwards);

/* Puts every part of the model in motion slot 0 (ModelPart.unk6C). */
void Motion_ClearPartSlots(EntityModel *model) {
    EntityModelPart80Byte *part;
    ModelInstance *instance;
    s32 i;

    instance = (ModelInstance *)model;
    if (instance->part_count != 0) {
        i = 0;
        do {
            part = (EntityModelPart80Byte *)((i << 7) + (s32)model->unkF8);
            i += 1;
            part->unk6C = 0;
        } while (i < instance->part_count);
    }
}

/* Puts parts `first` to `last` of the model in motion slot `slot` (ModelPart.unk6C). */
void Motion_SetPartSlots(EntityModel *model, s32 first, s32 last, s32 value) {
    EntityModelPart80Byte *part;

    if (first <= last) {
        do {
            part = (EntityModelPart80Byte *)((first << 7) + (s32)model->unkF8);
            first += 1;
            part->unk6C = value;
        } while (first <= last);
    }
}

/* As Motion_SeekKeys, for the parts in motion slot `slot` only. */
int Motion_SeekSlotKeys(ModelInstance *model, int slot, int table_index, int value) {
    // match: indexing level_tables as an array swaps the addu operands
    ModelPartLevels **table = (ModelPartLevels **)((char *)model->level_tables + (table_index << 2));
    ModelPartLevels *levels;
    ModelPart *part;
    u16 *threshold;
    int count;
    int passed;
    int i;

    for (levels = *table, i = 0; i < model->part_count; i++, levels++) {
        part = &model->parts[i];
        if (part->unk6C == slot && levels->thresholds != NULL && levels->count != 0) {
            threshold = levels->thresholds;
            count = levels->count;
            for (passed = 0; passed < count; passed++) {
                if (value < *threshold) {
                    break;
                }
                threshold += 9;
            }
            if (passed == count) {
                part->level = 0;
            } else if (passed != 0) {
                part->level = passed - 1;
            }
        }
    }
    return 1;
}

/* Plays one of the entity's motion slots a step on and, for a slot that moves the entity, turns what the root
   part moved by into the entity's own movement; returns whether the motion ended. */
s32 Motion_StepSlot(Entity *entity, EntityUnkE0SlotBits *slot_bits, s32 type, s32 mode) {
    EntityMotionSlot *slot = (EntityMotionSlot *)slot_bits;
    f32 moved[4];
    f32 turned[4];
    EntityParams *params;
    MotionData *motion;
    ModelPart *root;
    s32 table;
    s32 step;
    f32 sine;
    f32 cosine;

    params = entity->params;
    table = params->unk60[slot->index];
    // match: indexing level_tables as an array swaps the addu operands
    motion = *(MotionData **)((char *)ENTITY_MODEL(entity)->level_tables + (table << 2));
    motion = (MotionData *)((ModelPartLevels *)motion - 1);
    if (entity->unk28 != 0 && (entity->unkC6 & 0x46) == 0) {
        step = (f32)params->unk80[entity->unkD1] * 0.75f;
    } else {
        step = params->unk80[entity->unkD1];
    }
    /* a motion standing on its first frame is stepped once more than the others */
    // match: one if with both tests joined by a ternary lays the branches out differently
    if (slot->backwards) {
        if (slot->frame == motion->frame_count) {
            slot->frame_fixed = Motion_PoseSlot(ENTITY_MODEL(entity), type, table, slot->frame_fixed, step, slot->backwards);
        }
    } else if (slot->frame == 0) {
        slot->frame_fixed = Motion_PoseSlot(ENTITY_MODEL(entity), type, table, slot->frame_fixed, step, slot->backwards);
    }
    slot->frame = slot->frame_fixed >> 8;
    if (!slot->moves_entity) {
        slot->frame_fixed = Motion_PoseSlot(ENTITY_MODEL(entity), type, table, slot->frame_fixed, step, slot->backwards);
        slot->frame = slot->frame_fixed >> 8;
        return slot->backwards ? slot->frame == motion->frame_count : slot->frame == 0;
    }
    root = ENTITY_MODEL(entity)->unk130;
    turned[0] = 0.0f;
    turned[1] = 0.0f;
    turned[2] = 0.0f;
    turned[3] = 0.0f;
    moved[0] = 0.0f;
    moved[1] = 0.0f;
    moved[2] = 0.0f;
    moved[3] = 0.0f;
    Vector_Copy(turned, &root->unk50);
    Vector_Copy(moved, root->unk40);
    slot->frame_fixed = Motion_PoseSlot(ENTITY_MODEL(entity), type, table, slot->frame_fixed, step, slot->backwards);
    slot->frame = slot->frame_fixed >> 8;
    Vector_Subtract(turned, &root->unk50, turned);
    Vector_Subtract(moved, root->unk40, moved);
    sceVu0MulVector(moved, moved, &ENTITY_MODEL(entity)->unkE0);
    Vector_Scale(moved, moved, ENTITY_PARAMS_MOTION_SCALE(entity->params));
    if (ENTITY_MODEL(entity)->unkE0 < 0.0f) {
        turned[1] = -turned[1];
    }
    if (mode == 1) {
        sceVu0RotMatrixY(MODEL_MATRIX_SCRATCH, D_00431AF0, -root->unk54);
        sceVu0RotMatrixX(MODEL_MATRIX_SCRATCH, MODEL_MATRIX_SCRATCH, -root->unk50);
        sceVu0RotMatrixZ(MODEL_MATRIX_SCRATCH, MODEL_MATRIX_SCRATCH, -root->unk58);
        sceVu0RotMatrixZ(MODEL_MATRIX_SCRATCH, MODEL_MATRIX_SCRATCH, ENTITY_MODEL(entity)->rotation.z);
        sceVu0RotMatrixX(MODEL_MATRIX_SCRATCH, MODEL_MATRIX_SCRATCH, ENTITY_MODEL(entity)->rotation.x);
        sceVu0RotMatrixY(MODEL_MATRIX_SCRATCH, MODEL_MATRIX_SCRATCH, ENTITY_MODEL(entity)->rotation.y);
        sceVu0ApplyMatrix(moved, MODEL_MATRIX_SCRATCH, moved);
        entity->unk84 += moved[0];
        entity->unk88 += moved[1];
        entity->unk8C += moved[2];
        entity->unk90 += turned[0];
        entity->unk94 += turned[1];
        entity->unk98 += turned[2];
    } else if (mode == 2) {
        sine = sinf(-ENTITY_MODEL(entity)->rotation.y);
        cosine = cosf(-ENTITY_MODEL(entity)->rotation.y);
        entity->unk84 += moved[0] * cosine - moved[2] * sine;
        entity->unk8C += moved[0] * sine + moved[2] * cosine;
    } else if (mode == 3) {
        entity->unk94 += turned[1];
    }
    return slot->backwards ? slot->frame == motion->frame_count : slot->frame == 0;
}

/* Poses the parts of one motion slot between the keys of a motion around a frame (times 256) and returns the
   frame a step on. */
static s32 Motion_PoseSlot(ModelInstance *model, s32 type, s32 table, s32 frame_fixed, s32 step, s32 backwards) {
    Vector4 *point0 = &MOTION_SCRATCH[0];
    Vector4 *point1 = &MOTION_SCRATCH[1];
    Vector4 *tangent0 = &MOTION_SCRATCH[2];
    Vector4 *tangent1 = &MOTION_SCRATCH[3];
    MotionData *motion;
    ModelPartLevels *record;
    MotionVector *vectors;
    ModelPart *part;
    MotionKey *key;
    MotionKey *next;
    s32 level;
    s32 i;
    f32 ratio;
    f32 angle;
    f32 tilt;
    f32 spin;
    u32 elapsed;
    u32 length;
    s32 frame;
    s32 target;

    // match: indexing level_tables as an array swaps the addu operands
    motion = *(MotionData **)((char *)model->level_tables + (table << 2));
    motion = (MotionData *)((ModelPartLevels *)motion - 1);
    record = *(ModelPartLevels **)((char *)model->level_tables + (table << 2));
    vectors = motion->vectors;
    if (backwards != 0) {
        frame_fixed -= step;
        target = frame_fixed;
        if (target < 0) {
            frame = motion->frame_count;
            frame_fixed = frame << 8;
        } else {
            frame = frame_fixed >> 8;
        }
    } else {
        frame = frame_fixed >> 8;
        target = frame_fixed + step;
    }
    for (i = 0; i < model->part_count; i++, record++) {
        part = &model->parts[i];
        if (part->unk6C != type) {
            continue;
        }
        if (target < 0) {
            part->level = record->count - 1;
        }
        if (record->thresholds != NULL) {
            if (record->count == 0) {
                continue;
            }
            key = (MotionKey *)record->thresholds + ((u32)(MODEL_PART_WORD(part) >> 39) & 0x7F);
            level = (u32)(MODEL_PART_WORD(part) >> 39) & 0x7F;
            while (level < record->count && frame >= key->frame) {
                level++;
                key++;
            }
            if (level == record->count) {
                part->level = 0;
                continue;
            }
            if (level == 0) {
                continue;
            }
            part->level = level - 1;
            next = key;
            key = next - 1;
            elapsed = key->frame;
            length = key->length;
            elapsed = frame_fixed - (elapsed << 8);
            length <<= 8;
            ratio = (f32)elapsed / (f32)length;

            *(MotionVector *)point0 = vectors[key->rotation];
            point0->w = 1.0f;
            *(MotionVector *)point1 = vectors[next->rotation];
            point1->w = 1.0f;
            *(MotionVector *)tangent0 = vectors[key->rotation_out];
            tangent0->w = 1.0f;
            *(MotionVector *)tangent1 = vectors[next->rotation_in];
            tangent1->w = 1.0f;
            Vector_Hermite(&part->unk50, point0, point1, tangent0, tangent1, ratio);
            angle = part->unk50;
            angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
            if (3.1415927f < angle) {
                angle -= 6.2831855f;
            } else if (angle < -3.1415927f) {
                angle += 6.2831855f;
            }
            part->unk50 = angle;
            tilt = part->unk54;
            tilt -= (f32)(s32)(tilt / 6.2831855f) * 6.2831855f;
            if (3.1415927f < tilt) {
                tilt -= 6.2831855f;
            } else if (tilt < -3.1415927f) {
                tilt += 6.2831855f;
            }
            part->unk54 = tilt;
            spin = part->unk58;
            spin -= (f32)(s32)(spin / 6.2831855f) * 6.2831855f;
            if (3.1415927f < spin) {
                spin -= 6.2831855f;
            } else if (spin < -3.1415927f) {
                spin += 6.2831855f;
            }
            part->unk58 = spin;

            *(MotionVector *)point0 = vectors[key->position];
            *(MotionVector *)point1 = vectors[next->position];
            *(MotionVector *)tangent0 = vectors[key->position_out];
            *(MotionVector *)tangent1 = vectors[next->position_in];
            Vector_Hermite(part->unk40, point0, point1, tangent0, tangent1, ratio);

            part->unk60[0] = vectors[key->scale].x * (1.0f - ratio) + vectors[next->scale].x * ratio;
            part->unk60[1] = vectors[key->scale].y * (1.0f - ratio) + vectors[next->scale].y * ratio;
            part->unk60[2] = vectors[key->scale].z * (1.0f - ratio) + vectors[next->scale].z * ratio;
        }
        if ((motion->frame_count << 8) < target) {
            part->level = 0;
        }
    }
    if (backwards == 0) {
        frame_fixed += step;
        frame = (u16)(frame_fixed >> 8);
        if (motion->frame_count < frame) {
            frame = 0; // match: never read, but without it the wrap becomes a movn
            frame_fixed = 0;
        }
    }
    return frame_fixed;
}

/* Eases the parts of one motion slot toward the pose between the motion keys around a frame; returns whether
   the weight reached 1. */
s32 Motion_BlendSlotToPose(ModelInstance *model, s32 type, s32 table, s16 frame, f32 weight) {
    Vector4 *point0 = &MOTION_SCRATCH[0];
    Vector4 *point1 = &MOTION_SCRATCH[1];
    Vector4 *tangent0 = &MOTION_SCRATCH[2];
    Vector4 *tangent1 = &MOTION_SCRATCH[3];
    Vector4 *result = &MOTION_SCRATCH[4];
    MotionData *motion;
    ModelPartLevels *record;
    MotionVector *vectors;
    ModelPart *part;
    MotionKey *key;
    MotionKey *next;
    s32 level;
    s32 i;
    f32 ratio;
    f32 diff;
    f32 angle;
    f32 tilt;
    f32 spin;

    // match: indexing level_tables as an array swaps the addu operands
    motion = *(MotionData **)((char *)model->level_tables + (table << 2));
    motion = (MotionData *)((ModelPartLevels *)motion - 1);
    record = *(ModelPartLevels **)((char *)model->level_tables + (table << 2));
    vectors = motion->vectors;
    for (i = 0; i < model->part_count; i++, record++) {
        part = &model->parts[i];
        if (part->unk6C == type) {
            if (record->thresholds != NULL) {
                if (record->count != 0) {
                    key = (MotionKey *)record->thresholds + ((u32)(MODEL_PART_WORD(part) >> 39) & 0x7F);
                    level = (u32)(MODEL_PART_WORD(part) >> 39) & 0x7F;
                    while (level < record->count && frame >= key->frame) {
                        level++;
                        key++;
                    }
                    if (level == record->count) {
                        part->level = 0;
                    } else if (level != 0) {
                        part->level = level - 1;
                        next = key;
                        key = next - 1;
                        ratio = (f32)(frame - key->frame) / (f32)key->length;

                        *(MotionVector *)point0 = vectors[key->rotation];
                        point0->w = 1.0f;
                        *(MotionVector *)point1 = vectors[next->rotation];
                        point1->w = 1.0f;
                        *(MotionVector *)tangent0 = vectors[key->rotation_out];
                        tangent0->w = 1.0f;
                        *(MotionVector *)tangent1 = vectors[next->rotation_in];
                        tangent1->w = 1.0f;
                        Vector_Hermite(result, point0, point1, tangent0, tangent1, ratio);
                        diff = result->x - part->unk50;
                        diff -= (f32)(s32)(diff / 6.2831855f) * 6.2831855f;
                        if (3.1415927f < diff) {
                            diff -= 6.2831855f;
                        } else if (diff < -3.1415927f) {
                            diff += 6.2831855f;
                        }
                        part->unk50 += diff * weight;
                        diff = result->y - part->unk54;
                        diff -= (f32)(s32)(diff / 6.2831855f) * 6.2831855f;
                        if (3.1415927f < diff) {
                            diff -= 6.2831855f;
                        } else if (diff < -3.1415927f) {
                            diff += 6.2831855f;
                        }
                        part->unk54 += diff * weight;
                        diff = result->z - part->unk58;
                        diff -= (f32)(s32)(diff / 6.2831855f) * 6.2831855f;
                        if (3.1415927f < diff) {
                            diff -= 6.2831855f;
                        } else if (diff < -3.1415927f) {
                            diff += 6.2831855f;
                        }
                        part->unk58 += diff * weight;
                        angle = part->unk50;
                        angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
                        if (3.1415927f < angle) {
                            angle -= 6.2831855f;
                        } else if (angle < -3.1415927f) {
                            angle += 6.2831855f;
                        }
                        part->unk50 = angle;
                        tilt = part->unk54;
                        tilt -= (f32)(s32)(tilt / 6.2831855f) * 6.2831855f;
                        if (3.1415927f < tilt) {
                            tilt -= 6.2831855f;
                        } else if (tilt < -3.1415927f) {
                            tilt += 6.2831855f;
                        }
                        part->unk54 = tilt;
                        spin = part->unk58;
                        spin -= (f32)(s32)(spin / 6.2831855f) * 6.2831855f;
                        if (3.1415927f < spin) {
                            spin -= 6.2831855f;
                        } else if (spin < -3.1415927f) {
                            spin += 6.2831855f;
                        }
                        part->unk58 = spin;

                        *(MotionVector *)point0 = vectors[key->position];
                        *(MotionVector *)point1 = vectors[next->position];
                        *(MotionVector *)tangent0 = vectors[key->position_out];
                        *(MotionVector *)tangent1 = vectors[next->position_in];
                        Vector_Hermite(result, point0, point1, tangent0, tangent1, ratio);
                        part->unk40[0] += (result->x - part->unk40[0]) * weight;
                        part->unk40[1] += (result->y - part->unk40[1]) * weight;
                        part->unk40[2] += (result->z - part->unk40[2]) * weight;

                        result->x = vectors[key->scale].x * (1.0f - ratio) + vectors[next->scale].x * ratio;
                        result->y = vectors[key->scale].y * (1.0f - ratio) + vectors[next->scale].y * ratio;
                        result->z = vectors[key->scale].z * (1.0f - ratio) + vectors[next->scale].z * ratio;
                        part->unk60[0] += (result->x - part->unk60[0]) * weight;
                        part->unk60[1] += (result->y - part->unk60[1]) * weight;
                        part->unk60[2] += (result->z - part->unk60[2]) * weight;
                    }
                }
            }
        }
    }
    return weight >= 1.0f;
}

/* Poses the parts of one motion slot between the first keys of two motions: ratio 0 gives the first motion's,
   1 the second's. Nothing calls it. */
s32 Motion_MixSlotFirstKeys(ModelInstance *model, s32 type, s32 table_a, s32 table_b, f32 ratio) {
    Vector4 *point0 = &MOTION_SCRATCH[0];
    Vector4 *point1 = &MOTION_SCRATCH[1];
    Vector4 *result = &MOTION_SCRATCH[2];
    MotionData *motion_a;
    MotionData *motion_b;
    ModelPartLevels *record_a;
    ModelPartLevels *record_b;
    MotionVector *vectors_a;
    MotionVector *vectors_b;
    ModelPart *part;
    MotionKey *key_a;
    MotionKey *key_b;
    s32 i;
    f32 angle;
    f32 angle_alt;

    motion_a = *(MotionData **)((char *)model->level_tables + (table_a << 2));
    motion_a = (MotionData *)((ModelPartLevels *)motion_a - 1);
    motion_b = *(MotionData **)((char *)model->level_tables + (table_b << 2));
    motion_b = (MotionData *)((ModelPartLevels *)motion_b - 1);
    record_a = *(ModelPartLevels **)((char *)model->level_tables + (table_a << 2));
    record_b = *(ModelPartLevels **)((char *)model->level_tables + (table_b << 2));
    vectors_a = motion_a->vectors;
    vectors_b = motion_b->vectors;
    part = model->parts;
    for (i = 0; i < model->part_count; i++, record_a++, record_b++, part++) {
        if (part->unk6C != type) {
            continue;
        }
        key_a = (MotionKey *)record_a->thresholds;
        if (key_a == NULL) {
            continue;
        }
        key_b = (MotionKey *)record_b->thresholds;
        if (key_b == NULL) {
            continue;
        }
        if (record_a->count == 0) {
            continue;
        }
        if (record_b->count == 0) {
            continue;
        }
        *(MotionVector *)point0 = vectors_a[key_a->rotation];
        *(MotionVector *)point1 = vectors_b[key_b->rotation];
        blend_vectors(result, point0, point1, ratio);
        angle = result->x;
        angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        part->unk50 = angle;
        angle_alt = result->y;
        angle_alt -= (f32)(s32)(angle_alt / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle_alt) {
            angle_alt -= 6.2831855f;
        } else if (angle_alt < -3.1415927f) {
            angle_alt += 6.2831855f;
        }
        part->unk54 = angle_alt;
        angle = result->z;
        angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        part->unk58 = angle;

        *(MotionVector *)point0 = vectors_a[key_a->position];
        point0->w = 1.0f;
        *(MotionVector *)point1 = vectors_b[key_b->position];
        point1->w = 1.0f;
        blend_vectors(result, point0, point1, ratio);
        Vector_Copy(part->unk40, result);

        part->unk60[0] = vectors_a[key_a->scale].x * (1.0f - ratio) + vectors_b[key_b->scale].x * ratio;
        part->unk60[1] = vectors_a[key_a->scale].y * (1.0f - ratio) + vectors_b[key_b->scale].y * ratio;
        part->unk60[2] = vectors_a[key_a->scale].z * (1.0f - ratio) + vectors_b[key_b->scale].z * ratio;
    }
    return 1;
}

/* Starts animation `animation` of the entity (EntityParams.unk60 gives its motion, unk80 its speed) at
   `frame`, blending in over `blend_frames` (three quarters more while the unk28 timer runs), and clears the
   root part's offset; returns 1 without doing anything when the motion is already playing and `force` is 0. */
int Motion_Start(Entity *entity, int animation, int frame, u32 blend_frames, int force) {
    EntityParams *params;
    u8 *motions;
    int motion, current;

    params = entity->params;
    motions = params->unk60;
    current = motions[entity->unkD1];
    motion = motions[animation];
    if (!force && current == motion) {
        return 1;
    }
    entity->unkD0 = motion;
    entity->unkD1 = animation;
    MODEL_CHANNEL(ENTITY_MODEL(entity))->word = params->unk80[animation];
    if (motion >= MODEL_CHANNEL(ENTITY_MODEL(entity))->limit) {
        motion = 0;
    }
    MODEL_CHANNEL(ENTITY_MODEL(entity))->index = motion;
    MODEL_CHANNEL(ENTITY_MODEL(entity))->param = frame;
    MODEL_CHANNEL(ENTITY_MODEL(entity))->param_shifted = frame << 8;
    if (entity->unk28 != 0 && (entity->unkC6 & 0x46) == 0) {
        MODEL_CHANNEL(ENTITY_MODEL(entity))->value = (f32)(s32)blend_frames + (f32)(s32)blend_frames * 0.75f;
    } else {
        MODEL_CHANNEL(ENTITY_MODEL(entity))->value = blend_frames;
    }
    ENTITY_MODEL(entity)->unk130->unk40[0] = ENTITY_MODEL(entity)->unk130->unk40[1] =
        ENTITY_MODEL(entity)->unk130->unk40[2] = 0.0f;
    return Motion_SeekKeys(ENTITY_MODEL(entity), motion, frame);
}

/* Puts every part on the key of motion `table_index` that frame `value` falls in (ModelPart.level). */
int Motion_SeekKeys(ModelInstance *model, int table_index, int value) {
    ModelPartLevels *levels;
    ModelPart *part;
    int i, passed;
    u16 *threshold;

    /* match: indexing level_tables as an array swaps the addu operands */
    levels = *(ModelPartLevels **)((char *)model->level_tables + (table_index << 2));
    for (i = 0; i < model->part_count; i++, levels++) {
        part = &model->parts[i];
        if (levels->thresholds == NULL || levels->count == 0) {
            continue;
        }
        threshold = levels->thresholds;
        for (passed = 0; passed < levels->count; passed++, threshold += 9) {
            if (value < *threshold) {
                break;
            }
        }
        if (passed == levels->count) {
            part->level = 0;
        } else if (passed != 0) {
            part->level = passed - 1;
        }
    }
    return 1;
}

/* Plays the entity's motion a step on, turns what the root part moved by into the entity's own movement (mode
   0: none of it) and starts what the motion sets off on the frames it passed; returns whether the motion
   ended. */
s32 Motion_Step(Entity *entity, s32 mode) {
    f32 moved[4];
    f32 turned[4];
    ModelChannel *channel;
    MotionData *motion;
    ModelPart *root;
    s32 step;
    s32 frame_from;
    s32 frame_to;
    s32 swapped;
    f32 sine;
    f32 cosine;

    channel = MODEL_CHANNEL(ENTITY_MODEL(entity));
    motion = (MotionData *)channel->tables[channel->index];
    motion = (MotionData *)((ModelPartLevels *)motion - 1);
    if (entity->unk28 != 0 && (entity->unkC6 & 0x46) == 0) {
        step = (f32)entity->params->unk80[entity->unkD1] * 0.75f;
    } else {
        step = entity->params->unk80[entity->unkD1];
    }
    frame_from = channel->param_shifted;
    /* a motion standing on its first frame is stepped once more than the others */
    // match: one if with both tests joined by a ternary lays the branches out differently
    if (ENTITY_FLAG_BITS(entity)->bit15) {
        if (channel->param == motion->frame_count) {
            Motion_StepModel(ENTITY_MODEL(entity), step, ENTITY_FLAG_BITS(entity)->bit15);
        }
    } else if (channel->param == 0) {
        Motion_StepModel(ENTITY_MODEL(entity), step, ENTITY_FLAG_BITS(entity)->bit15);
    }
    if (mode == 0) {
        Motion_StepModel(ENTITY_MODEL(entity), step, ENTITY_FLAG_BITS(entity)->bit15);
        frame_to = channel->param_shifted;
        swapped = frame_to;
        if (ENTITY_FLAG_BITS(entity)->bit15) {
            frame_to = frame_from;
            frame_from = swapped;
        }
        if (frame_from == frame_to) {
            frame_to = 0x100;
        }
        EntityEvent_StartFrameEffects(entity, entity->unkD1, frame_from, frame_to);
        EntityEvent_StartFrameCues(entity, entity->unkD1, frame_from, frame_to);
        return ENTITY_FLAG_BITS(entity)->bit15 ? channel->param == motion->frame_count : channel->param == 0;
    }
    root = ENTITY_MODEL(entity)->unk130;
    turned[0] = 0.0f;
    turned[1] = 0.0f;
    turned[2] = 0.0f;
    turned[3] = 0.0f;
    moved[0] = 0.0f;
    moved[1] = 0.0f;
    moved[2] = 0.0f;
    moved[3] = 0.0f;
    Vector_Copy(turned, &root->unk50);
    Vector_Copy(moved, root->unk40);
    Motion_StepModel(ENTITY_MODEL(entity), step, ENTITY_FLAG_BITS(entity)->bit15);
    Vector_Subtract(turned, &root->unk50, turned);
    Vector_Subtract(moved, root->unk40, moved);
    sceVu0MulVector(moved, moved, &ENTITY_MODEL(entity)->unkE0);
    Vector_Scale(moved, moved, ENTITY_PARAMS_MOTION_SCALE(entity->params));
    if (ENTITY_MODEL(entity)->unkE0 < 0.0f) {
        turned[1] = -turned[1];
    }
    if (mode == 1) {
        sceVu0RotMatrixY(MODEL_MATRIX_SCRATCH, D_00431AF0, -root->unk54);
        sceVu0RotMatrixX(MODEL_MATRIX_SCRATCH, MODEL_MATRIX_SCRATCH, -root->unk50);
        sceVu0RotMatrixZ(MODEL_MATRIX_SCRATCH, MODEL_MATRIX_SCRATCH, -root->unk58);
        sceVu0RotMatrixZ(MODEL_MATRIX_SCRATCH, MODEL_MATRIX_SCRATCH, ENTITY_MODEL(entity)->rotation.z);
        sceVu0RotMatrixX(MODEL_MATRIX_SCRATCH, MODEL_MATRIX_SCRATCH, ENTITY_MODEL(entity)->rotation.x);
        sceVu0RotMatrixY(MODEL_MATRIX_SCRATCH, MODEL_MATRIX_SCRATCH, ENTITY_MODEL(entity)->rotation.y);
        sceVu0ApplyMatrix(moved, MODEL_MATRIX_SCRATCH, moved);
        entity->unk84 += moved[0];
        entity->unk88 += moved[1];
        entity->unk8C += moved[2];
        entity->unk90 += turned[0];
        entity->unk94 += turned[1];
        entity->unk98 += turned[2];
    } else if (mode == 2) {
        sine = sinf(-ENTITY_MODEL(entity)->rotation.y);
        cosine = cosf(-ENTITY_MODEL(entity)->rotation.y);
        entity->unk84 += moved[0] * cosine - moved[2] * sine;
        entity->unk8C += moved[0] * sine + moved[2] * cosine;
    } else if (mode == 3) {
        entity->unk94 += turned[1];
    }
    frame_to = channel->param_shifted;
    swapped = frame_to;
    if (ENTITY_FLAG_BITS(entity)->bit15) {
        frame_to = frame_from;
        frame_from = swapped;
    }
    if (frame_from == frame_to) {
        frame_to = 0x100;
    }
    EntityEvent_StartFrameEffects(entity, entity->unkD1, frame_from, frame_to);
    EntityEvent_StartFrameCues(entity, entity->unkD1, frame_from, frame_to);
    return ENTITY_FLAG_BITS(entity)->bit15 ? channel->param == motion->frame_count : channel->param == 0;
}

/* Poses every part between the motion keys around the model's playback position, steps the position and
   reports whether the motion ended. */
s32 Motion_StepModel(ModelInstance *model, s32 step, s32 backwards) {
    Vector4 *point0 = &MOTION_SCRATCH[0];
    Vector4 *point1 = &MOTION_SCRATCH[1];
    Vector4 *tangent0 = &MOTION_SCRATCH[2];
    Vector4 *tangent1 = &MOTION_SCRATCH[3];
    ModelChannel *channel;
    MotionData *motion;
    ModelPartLevels *record;
    MotionVector *vectors;
    ModelPart *part;
    MotionKey *key;
    MotionKey *next;
    s32 level;
    s32 i;
    f32 ratio;
    f32 angle;
    f32 tilt;
    f32 spin;
    u32 elapsed;
    u32 length;
    u16 old_param;
    s32 target;

    channel = MODEL_CHANNEL(model);
    old_param = channel->param;
    motion = (MotionData *)channel->tables[channel->index];
    record = (ModelPartLevels *)motion;
    motion = (MotionData *)((ModelPartLevels *)motion - 1);
    vectors = motion->vectors;
    if (backwards != 0) {
        channel->param_shifted -= step;
        target = channel->param_shifted;
        if (target < 0) {
            channel->param = motion->frame_count;
            channel->param_shifted = motion->frame_count << 8;
        } else {
            channel->param = target >> 8;
        }
    } else {
        target = channel->param_shifted + step;
    }
    for (i = 0; i < model->part_count; i++, record++) {
        part = &model->parts[i];
        if (target < 0) {
            part->level = record->count - 1;
        }
        if (record->thresholds != NULL) {
            if (record->count == 0) {
                continue;
            }
            key = (MotionKey *)record->thresholds + ((u32)(MODEL_PART_WORD(part) >> 39) & 0x7F);
            level = (u32)(MODEL_PART_WORD(part) >> 39) & 0x7F;
            while (level < record->count && channel->param >= key->frame) {
                level++;
                key++;
            }
            if (level == record->count) {
                part->level = 0;
                continue;
            }
            if (level == 0) {
                continue;
            }
            part->level = level - 1;
            next = key;
            key = next - 1;
            elapsed = key->frame;
            length = key->length;
            elapsed = channel->param_shifted - (elapsed << 8);
            length <<= 8;
            ratio = (f32)elapsed / (f32)length;

            *(MotionVector *)point0 = vectors[key->rotation];
            point0->w = 1.0f;
            *(MotionVector *)point1 = vectors[next->rotation];
            point1->w = 1.0f;
            *(MotionVector *)tangent0 = vectors[key->rotation_out];
            tangent0->w = 1.0f;
            *(MotionVector *)tangent1 = vectors[next->rotation_in];
            tangent1->w = 1.0f;
            Vector_Hermite(&part->unk50, point0, point1, tangent0, tangent1, ratio);
            angle = part->unk50;
            angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
            if (3.1415927f < angle) {
                angle -= 6.2831855f;
            } else if (angle < -3.1415927f) {
                angle += 6.2831855f;
            }
            part->unk50 = angle;
            tilt = part->unk54;
            tilt -= (f32)(s32)(tilt / 6.2831855f) * 6.2831855f;
            if (3.1415927f < tilt) {
                tilt -= 6.2831855f;
            } else if (tilt < -3.1415927f) {
                tilt += 6.2831855f;
            }
            part->unk54 = tilt;
            spin = part->unk58;
            spin -= (f32)(s32)(spin / 6.2831855f) * 6.2831855f;
            if (3.1415927f < spin) {
                spin -= 6.2831855f;
            } else if (spin < -3.1415927f) {
                spin += 6.2831855f;
            }
            part->unk58 = spin;

            *(MotionVector *)point0 = vectors[key->position];
            *(MotionVector *)point1 = vectors[next->position];
            *(MotionVector *)tangent0 = vectors[key->position_out];
            *(MotionVector *)tangent1 = vectors[next->position_in];
            Vector_Hermite(part->unk40, point0, point1, tangent0, tangent1, ratio);

            part->unk60[0] = vectors[key->scale].x * (1.0f - ratio) + vectors[next->scale].x * ratio;
            part->unk60[1] = vectors[key->scale].y * (1.0f - ratio) + vectors[next->scale].y * ratio;
            part->unk60[2] = vectors[key->scale].z * (1.0f - ratio) + vectors[next->scale].z * ratio;
        }
        if ((motion->frame_count << 8) < target) {
            part->level = 0;
        }
    }
    if (backwards == 0) {
        channel->param_shifted += step;
        channel->param = channel->param_shifted >> 8;
        if (motion->frame_count < channel->param) {
            channel->param = 0;
            channel->param_shifted = 0;
        }
    }
    channel->unk8E = old_param != channel->param;
    if (backwards != 0) {
        return channel->param == motion->frame_count;
    }
    return channel->param == 0;
}

/* As Motion_StepModel, blending the pose with the current one by `weight`. Nothing calls it. */
s32 Motion_StepModelBlended(ModelInstance *model, s32 step, s32 backwards, f32 weight) {
    Vector4 *point0 = &MOTION_SCRATCH[0];
    Vector4 *point1 = &MOTION_SCRATCH[1];
    Vector4 *tangent0 = &MOTION_SCRATCH[2];
    Vector4 *tangent1 = &MOTION_SCRATCH[3];
    Vector4 *result = &MOTION_SCRATCH[4];
    ModelChannel *channel;
    MotionData *motion;
    ModelPartLevels *record;
    MotionVector *vectors;
    ModelPart *part;
    MotionKey *key;
    MotionKey *next;
    s32 level;
    s32 i;
    u16 old_param;
    s32 target;
    f32 ratio;
    f32 angle;
    f32 angle_alt;
    u32 elapsed;
    u32 length;

    channel = MODEL_CHANNEL(model);
    old_param = channel->param;
    motion = (MotionData *)channel->tables[channel->index];
    motion = (MotionData *)((ModelPartLevels *)motion - 1);
    record = channel->tables[channel->index];
    vectors = motion->vectors;
    if (backwards != 0) {
        channel->param_shifted -= step;
        target = channel->param_shifted;
        if (target < 0) {
            channel->param = motion->frame_count;
            channel->param_shifted = motion->frame_count << 8;
        } else {
            channel->param = target >> 8;
        }
    } else {
        target = channel->param_shifted + step;
    }
    for (i = 0; i < model->part_count; i++, record++) {
        part = &model->parts[i];
        if (target < 0) {
            part->level = record->count - 1;
        }
        if (record->thresholds != NULL) {
            if (record->count == 0) {
                continue;
            }
            key = (MotionKey *)record->thresholds + ((u32)(MODEL_PART_WORD(part) >> 39) & 0x7F);
            level = (u32)(MODEL_PART_WORD(part) >> 39) & 0x7F;
            while (level < record->count && channel->param >= key->frame) {
                level++;
                key++;
            }
            if (level == record->count) {
                part->level = 0;
                continue;
            }
            if (level == 0) {
                continue;
            }
            part->level = level - 1;
            next = key;
            key = next - 1;
            elapsed = key->frame;
            length = key->length;
            elapsed = channel->param_shifted - (elapsed << 8);
            length <<= 8;
            ratio = (f32)elapsed / (f32)length;

            *(MotionVector *)point0 = vectors[key->rotation];
            point0->w = 1.0f;
            *(MotionVector *)point1 = vectors[next->rotation];
            point1->w = 1.0f;
            *(MotionVector *)tangent0 = vectors[key->rotation_out];
            tangent0->w = 1.0f;
            *(MotionVector *)tangent1 = vectors[next->rotation_in];
            tangent1->w = 1.0f;
            Vector_Hermite(result, point0, point1, tangent0, tangent1, ratio);
            angle = result->x;
            angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
            if (3.1415927f < angle) {
                angle -= 6.2831855f;
            } else if (angle < -3.1415927f) {
                angle += 6.2831855f;
            }
            result->x = angle;
            angle_alt = result->y;
            angle_alt -= (f32)(s32)(angle_alt / 6.2831855f) * 6.2831855f;
            if (3.1415927f < angle_alt) {
                angle_alt -= 6.2831855f;
            } else if (angle_alt < -3.1415927f) {
                angle_alt += 6.2831855f;
            }
            result->y = angle_alt;
            angle = result->z;
            angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
            if (3.1415927f < angle) {
                angle -= 6.2831855f;
            } else if (angle < -3.1415927f) {
                angle += 6.2831855f;
            }
            result->z = angle;
            part->unk50 = part->unk50 * (1.0f - weight) + result->x * weight;
            part->unk54 = part->unk54 * (1.0f - weight) + result->y * weight;
            part->unk58 = part->unk58 * (1.0f - weight) + result->z * weight;
            angle_alt = part->unk50;
            angle_alt -= (f32)(s32)(angle_alt / 6.2831855f) * 6.2831855f;
            if (3.1415927f < angle_alt) {
                angle_alt -= 6.2831855f;
            } else if (angle_alt < -3.1415927f) {
                angle_alt += 6.2831855f;
            }
            part->unk50 = angle_alt;
            angle = part->unk54;
            angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
            if (3.1415927f < angle) {
                angle -= 6.2831855f;
            } else if (angle < -3.1415927f) {
                angle += 6.2831855f;
            }
            part->unk54 = angle;
            angle_alt = part->unk58;
            angle_alt -= (f32)(s32)(angle_alt / 6.2831855f) * 6.2831855f;
            if (3.1415927f < angle_alt) {
                angle_alt -= 6.2831855f;
            } else if (angle_alt < -3.1415927f) {
                angle_alt += 6.2831855f;
            }
            part->unk58 = angle_alt;

            *(MotionVector *)point0 = vectors[key->position];
            *(MotionVector *)point1 = vectors[next->position];
            *(MotionVector *)tangent0 = vectors[key->position_out];
            *(MotionVector *)tangent1 = vectors[next->position_in];
            Vector_Hermite(result, point0, point1, tangent0, tangent1, ratio);
            part->unk40[0] = part->unk40[0] * (1.0f - weight) + result->x * weight;
            part->unk40[1] = part->unk40[1] * (1.0f - weight) + result->y * weight;
            part->unk40[2] = part->unk40[2] * (1.0f - weight) + result->z * weight;

            result->x = vectors[key->scale].x * (1.0f - ratio) + vectors[next->scale].x * ratio;
            result->y = vectors[key->scale].y * (1.0f - ratio) + vectors[next->scale].y * ratio;
            result->z = vectors[key->scale].z * (1.0f - ratio) + vectors[next->scale].z * ratio;
            part->unk60[0] = part->unk60[0] * (1.0f - weight) + result->x * weight;
            part->unk60[1] = part->unk60[1] * (1.0f - weight) + result->y * weight;
            part->unk60[2] = part->unk60[2] * (1.0f - weight) + result->z * weight;
        }
        if ((motion->frame_count << 8) < target) {
            part->level = 0;
        }
    }
    if (backwards == 0) {
        channel->param_shifted += step;
        channel->param = channel->param_shifted >> 8;
        if (motion->frame_count < channel->param) {
            channel->param = 0;
            channel->param_shifted = 0;
        }
    }
    channel->unk8E = old_param != channel->param;
    return channel->param == (backwards == 0 ? 0 : motion->frame_count);
}

/* Eases every part toward the pose between the motion keys around the channel's frame, then counts the blend
   frames down. */
void Motion_BlendToPose(ModelInstance *model, f32 weight) {
    Vector4 *point0 = &MOTION_SCRATCH[0];
    Vector4 *point1 = &MOTION_SCRATCH[1];
    Vector4 *tangent0 = &MOTION_SCRATCH[2];
    Vector4 *tangent1 = &MOTION_SCRATCH[3];
    Vector4 *result = &MOTION_SCRATCH[4];
    ModelChannel *channel;
    MotionData *motion;
    ModelPartLevels *record;
    MotionVector *vectors;
    ModelPart *part;
    MotionKey *key;
    MotionKey *next;
    s32 level;
    s32 i;
    f32 ratio;
    f32 diff;
    f32 angle;
    f32 angle_y;

    channel = MODEL_CHANNEL(model);
    motion = (MotionData *)channel->tables[channel->index];
    motion = (MotionData *)((ModelPartLevels *)motion - 1);
    record = channel->tables[channel->index];
    vectors = motion->vectors;
    for (i = 0; i < model->part_count; i++, record++) {
        part = &model->parts[i];
        if (record->thresholds != NULL && record->count != 0) {
            key = (MotionKey *)record->thresholds + ((u32)(MODEL_PART_WORD(part) >> 39) & 0x7F);
            level = (u32)(MODEL_PART_WORD(part) >> 39) & 0x7F;
            while (level < record->count && channel->param >= key->frame) {
                level++;
                key++;
            }
            if (level == record->count) {
                part->level = 0;
            } else if (level != 0) {
                part->level = level - 1;
                next = key;
                key = next - 1;
                ratio = (f32)(channel->param - key->frame) / (f32)key->length;

                *(MotionVector *)point0 = vectors[key->rotation];
                point0->w = 1.0f;
                *(MotionVector *)point1 = vectors[next->rotation];
                point1->w = 1.0f;
                *(MotionVector *)tangent0 = vectors[key->rotation_out];
                tangent0->w = 1.0f;
                *(MotionVector *)tangent1 = vectors[next->rotation_in];
                tangent1->w = 1.0f;
                Vector_Hermite(result, point0, point1, tangent0, tangent1, ratio);
                diff = result->x - part->unk50;
                diff -= (f32)(s32)(diff / 6.2831855f) * 6.2831855f;
                if (3.1415927f < diff) {
                    diff -= 6.2831855f;
                } else if (diff < -3.1415927f) {
                    diff += 6.2831855f;
                }
                part->unk50 += diff * weight;
                diff = result->y - part->unk54;
                diff -= (f32)(s32)(diff / 6.2831855f) * 6.2831855f;
                if (3.1415927f < diff) {
                    diff -= 6.2831855f;
                } else if (diff < -3.1415927f) {
                    diff += 6.2831855f;
                }
                part->unk54 += diff * weight;
                diff = result->z - part->unk58;
                diff -= (f32)(s32)(diff / 6.2831855f) * 6.2831855f;
                if (3.1415927f < diff) {
                    diff -= 6.2831855f;
                } else if (diff < -3.1415927f) {
                    diff += 6.2831855f;
                }
                part->unk58 += diff * weight;
                angle = part->unk50;
                angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
                if (3.1415927f < angle) {
                    angle -= 6.2831855f;
                } else if (angle < -3.1415927f) {
                    angle += 6.2831855f;
                }
                part->unk50 = angle;
                angle_y = part->unk54;
                angle_y -= (f32)(s32)(angle_y / 6.2831855f) * 6.2831855f;
                if (3.1415927f < angle_y) {
                    angle_y -= 6.2831855f;
                } else if (angle_y < -3.1415927f) {
                    angle_y += 6.2831855f;
                }
                part->unk54 = angle_y;
                angle = part->unk58;
                angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
                if (3.1415927f < angle) {
                    angle -= 6.2831855f;
                } else if (angle < -3.1415927f) {
                    angle += 6.2831855f;
                }
                part->unk58 = angle;

                *(MotionVector *)point0 = vectors[key->position];
                *(MotionVector *)point1 = vectors[next->position];
                *(MotionVector *)tangent0 = vectors[key->position_out];
                *(MotionVector *)tangent1 = vectors[next->position_in];
                Vector_Hermite(result, point0, point1, tangent0, tangent1, ratio);
                part->unk40[0] += (result->x - part->unk40[0]) * weight;
                part->unk40[1] += (result->y - part->unk40[1]) * weight;
                part->unk40[2] += (result->z - part->unk40[2]) * weight;

                result->x = vectors[key->scale].x * (1.0f - ratio) + vectors[next->scale].x * ratio;
                result->y = vectors[key->scale].y * (1.0f - ratio) + vectors[next->scale].y * ratio;
                result->z = vectors[key->scale].z * (1.0f - ratio) + vectors[next->scale].z * ratio;
                part->unk60[0] += (result->x - part->unk60[0]) * weight;
                part->unk60[1] += (result->y - part->unk60[1]) * weight;
                part->unk60[2] += (result->z - part->unk60[2]) * weight;
            }
        }
    }
    channel->value--;
}

/* Poses every part between the first keys of two motions: ratio 0 gives the first motion's, 1 the second's.
   Nothing calls it. */
s32 Motion_MixFirstKeys(ModelInstance *model, s32 table_a, s32 table_b, f32 ratio) {
    Vector4 *point0 = &MOTION_SCRATCH[0];
    Vector4 *point1 = &MOTION_SCRATCH[1];
    Vector4 *result = &MOTION_SCRATCH[2];
    MotionData *motion_a;
    MotionData *motion_b;
    ModelPartLevels *record_a;
    ModelPartLevels *record_b;
    MotionVector *vectors_a;
    MotionVector *vectors_b;
    ModelPart *part;
    MotionKey *key_a;
    MotionKey *key_b;
    s32 i;
    f32 angle;
    f32 angle_alt;

    motion_a = *(MotionData **)((char *)model->level_tables + (table_a << 2));
    motion_a = (MotionData *)((ModelPartLevels *)motion_a - 1);
    motion_b = *(MotionData **)((char *)model->level_tables + (table_b << 2));
    motion_b = (MotionData *)((ModelPartLevels *)motion_b - 1);
    record_a = *(ModelPartLevels **)((char *)model->level_tables + (table_a << 2));
    record_b = *(ModelPartLevels **)((char *)model->level_tables + (table_b << 2));
    vectors_a = motion_a->vectors;
    vectors_b = motion_b->vectors;
    part = model->parts;
    for (i = 0; i < model->part_count; i++, record_a++, record_b++, part++) {
        key_a = (MotionKey *)record_a->thresholds;
        if (key_a == NULL) {
            continue;
        }
        key_b = (MotionKey *)record_b->thresholds;
        if (key_b == NULL) {
            continue;
        }
        if (record_a->count == 0) {
            continue;
        }
        if (record_b->count == 0) {
            continue;
        }
        *(MotionVector *)point0 = vectors_a[key_a->rotation];
        *(MotionVector *)point1 = vectors_b[key_b->rotation];
        blend_vectors(result, point0, point1, ratio);
        angle = result->x;
        angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        part->unk50 = angle;
        angle_alt = result->y;
        angle_alt -= (f32)(s32)(angle_alt / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle_alt) {
            angle_alt -= 6.2831855f;
        } else if (angle_alt < -3.1415927f) {
            angle_alt += 6.2831855f;
        }
        part->unk54 = angle_alt;
        angle = result->z;
        angle -= (f32)(s32)(angle / 6.2831855f) * 6.2831855f;
        if (3.1415927f < angle) {
            angle -= 6.2831855f;
        } else if (angle < -3.1415927f) {
            angle += 6.2831855f;
        }
        part->unk58 = angle;

        *(MotionVector *)point0 = vectors_a[key_a->position];
        point0->w = 1.0f;
        *(MotionVector *)point1 = vectors_b[key_b->position];
        point1->w = 1.0f;
        blend_vectors(result, point0, point1, ratio);
        Vector_Copy(part->unk40, result);

        part->unk60[0] = vectors_a[key_a->scale].x * (1.0f - ratio) + vectors_b[key_b->scale].x * ratio;
        part->unk60[1] = vectors_a[key_a->scale].y * (1.0f - ratio) + vectors_b[key_b->scale].y * ratio;
        part->unk60[2] = vectors_a[key_a->scale].z * (1.0f - ratio) + vectors_b[key_b->scale].z * ratio;
    }
    return 1;
}
