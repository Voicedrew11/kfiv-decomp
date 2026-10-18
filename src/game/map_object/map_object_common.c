#include "model/model.h"
#include "map_object/map_object.h"
#include "map/map.h"

void Vector_Add(void *, void *, void *);
s32 Sound_StartTaggedAt(s8, u8, u8, f32, Vector4Arg *);
u16 ModelMotion_GetLastFrame(char *);
int Sound_StopTagged(char);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void Model_MultiplyChildMatrices(ModelPart *);
void sceVu0CopyMatrix(void *, void *);
extern int current_map_slot;
extern int sound_load_queue[0x28 / 4];

/* Offset hook of every kind: moves the model and the object's source position by offset. */
void MapObject_MoveByOffset(MapObject *object, void *offset) {
    Vector_Add(&object->model->position, &object->model->position, offset);
    Vector_Add(object->source, object->source, offset);
}

/* Step 0 of the scenery kinds (0 and 0x16 to 0x1C): restarts the animation the object is on (kind 0 also
   forgets its ambient sound) and goes to step 1. */
void MapObject_StartScenery(MapObject *object) {
    if (object->definition->kind == 0) {
        object->work[0] = 0;
    }
    if (object->model->model_flags & 1) {
        MapObject_StartAnim(object, MAP_OBJECT_BYTES(object)->anim);
        MapObject_StepAnim(object);
    }
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Step 1 of the scenery kinds: a kind 0 object whose source names an ambient sound channel (0x40 to 0x7F)
   starts that sound while its map is the current one and no sound is loading, and forgets it when its map is
   not; the animation runs while a script lets it or while the source says it runs by itself. */
void MapObject_UpdateScenery(MapObject *object) {
    MapObjectUnk08 *source;

    if (object->definition->kind == 0) {
        source = object->source;
        if ((u32)((u16)source->kind_param_0 - 0x40) < 0x40) {
            if (object->work[0] != 0) {
                if ((MAP_OBJECT_BYTES(object)->unk00 & 1) != current_map_slot) {
                    object->work[0] = 0;
                }
            } else if ((MAP_OBJECT_BYTES(object)->unk00 & 1) == current_map_slot && sound_load_queue[4] == 0 && sound_load_queue[5] == 0) {
                Sound_StartTaggedAt(source->kind_param_0, source->kind_param_2, source->kind_param_1, source->kind_param_3, (Vector4Arg *)&object->model->position);
                object->work[0] = 1;
            }
        }
    }
    if (object->model->model_flags & 1) {
        if (object->state_flags & MAP_OBJECT_SCRIPTED) {
            if (object->state_flags & 0x200000) {
                if ((object->state_flags & MAP_OBJECT_FLAG_400000) || MODEL_CHANNEL(object->model)->frame != 0) {
                    MapObject_StepAnim(object);
                }
            }
        } else if (object->source->spawn_flags.flags & (0x8000L << 35)) {
            MapObject_StepAnim(object);
        }
    }
}

/* Step 0xF of most kinds, after MapObject_Break: runs the animation, fading the model out over it when the
   definition asks for that (unk68 bit 0x20000); once it has ended the object goes out of use and, when it did
   not fade, the object named by its source comes into use. */
void MapObject_UpdateBreaking(MapObject *object) {
    int total;
    int alpha;
    MapObject *other;

    if (MODEL_CHANNEL(object->model) != NULL && MODEL_CHANNEL(object->model)->frame != 0) {
        if (object->definition->definition_flags & 0x20000) {
            total = ModelMotion_GetLastFrame((char *)object->model);
            if (total != 0) {
                alpha = ((total - MODEL_CHANNEL(object->model)->frame) << 7) / total;
            } else {
                alpha = total; // match: 0, through the variable
            }
            if (object->model->model_flags & 0x4000) {
                object->model->alpha = alpha;
            } else {
                object->model->fade_enabled = 1;
                object->model->fade_alpha = alpha;
            }
        }
        MapObject_StepAnim(object);
    } else {
        if (!(object->definition->definition_flags & 0x20000)) {
            other = MapObject_GetById(MAP_OBJECT_BYTES(object)->unk00 & 1, object->source->spawn_flags.bytes[3]);
            if (other != NULL) {
                MapObject_Activate(other);
            }
        }
        MapObject_Deactivate(object);
    }
}

/* End hook of every kind but the light (map_object_end_hooks, run by MapObject_Deactivate): a kind 0 object
   that started its ambient sound stops it, if its map is still the current one, and forgets it. */
void MapObject_StopAmbientSound(MapTrigger *trigger) {
    TriggerSource *source;

    if (*trigger->unk04 == 0) {
        source = MAP_TRIGGER_SOURCE(trigger);
        if (((u32)(source->unk44 - 0x40) < 0x40) && (trigger->shot.unk00[0] != 0)) {
            if ((trigger->unk10 & 1) != current_map_slot) {
                trigger->shot.unk00[0] = 0;
                return;
            }
            Sound_StopTagged((char)source->unk44);
            trigger->shot.unk00[0] = 0;
        }
    }
}

/* Every-frame hook of most kinds: rebuilds the model matrices from the model's position and rotation (and the
   parts' animation), then updates the model's points. */
void MapObject_RebuildMatrices(char *arg) {
    MapObject *object = (MapObject *)arg;
    int i;
    ModelPart *part;
    int part_count;

    ModelMatrix_Build(object->model, &object->model->position, &object->model->rotation, &object->model->scale_x);
    part_count = object->model->part_count;
    part = object->model->parts;
    for (i = 0; i < part_count; i++) {
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
    sceVu0CopyMatrix(object->model->collision_matrix, object->model->model_matrix);
    object->model->update_bone_points(object->model);
}

/* Item hook of the kinds that take no item: returns FALSE (the item is not used). */
int MapObject_RefuseItem(void) {
    return 0;
}

/* Does nothing: the signal hook of the kinds that take no signal. */
void MapObject_IgnoreSignal(void) {
}

/* Does nothing: the set-up hook of the kinds that need none. */
void MapObject_SkipSetUp(void) {
}
