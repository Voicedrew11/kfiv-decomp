#include "kf_model.h"
#include "kf_map_object.h"
#include "kf_map.h"

void Vector_Add(void *, void *, void *);
s32 Sound_StartTaggedAt(s8, u8, u8, f32, Vector4Arg *);
u16 func_001090E0(char *);
int Sound_StopTagged(char);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void func_0010B5D0(ModelPart *);
void sceVu0CopyMatrix(void *, void *);
extern int D_00446770;
extern int sound_load_queue[0x28 / 4];

/* Moves a map object and what its unk08 points at by an offset. */
void func_0011EDC8(MapObject *object, void *offset) {
    Vector_Add(&object->model->position, &object->model->position, offset);
    Vector_Add(object->unk08, object->unk08, offset);
}

/* Start handler shared by several kinds: restarts the animation it is on. */
void func_0011EE18(MapObject *object) {
    if (object->unk04->kind == 0) {
        object->work[0] = 0;
    }
    if (object->model->unk100 & 1) {
        MapObject_StartAnim(object, MAP_OBJECT_BYTES(object)->anim);
        MapObject_StepAnim(object);
    }
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Update handler: keeps a kind 0 object's sound playing while its map is the
   current one, and runs its animation when it is allowed to. */
void func_0011EE78(MapObject *object) {
    MapObjectUnk08 *source;

    if (object->unk04->kind == 0) {
        source = object->unk08;
        if ((u32)((u16)source->unk44 - 0x40) < 0x40) {
            if (object->work[0] != 0) {
                if ((MAP_OBJECT_BYTES(object)->unk00 & 1) != D_00446770) {
                    object->work[0] = 0;
                }
            } else if ((MAP_OBJECT_BYTES(object)->unk00 & 1) == D_00446770 && sound_load_queue[4] == 0 && sound_load_queue[5] == 0) {
                Sound_StartTaggedAt(source->unk44, source->unk48, source->unk46, source->unk4A, (Vector4Arg *)&object->model->position);
                object->work[0] = 1;
            }
        }
    }
    if (object->model->unk100 & 1) {
        if (object->unk10 & 0x100000) {
            if (object->unk10 & 0x200000) {
                if ((object->unk10 & 0x400000) || MODEL_CHANNEL(object->model)->param != 0) {
                    MapObject_StepAnim(object);
                }
            }
        } else if (object->unk08->unk38.flags & (0x8000L << 35)) {
            MapObject_StepAnim(object);
        }
    }
}

/* Update handler: runs the animation, fading the model out over it when the
   definition asks for that; once it has ended the object goes out of use and
   the object named by its source comes into use. */
void func_0011EFC8(MapObject *object) {
    int total;
    int alpha;
    MapObject *other;

    if (MODEL_CHANNEL(object->model) != NULL && MODEL_CHANNEL(object->model)->param != 0) {
        if (object->unk04->unk68 & 0x20000) {
            total = func_001090E0((char *)object->model);
            if (total != 0) {
                alpha = ((total - MODEL_CHANNEL(object->model)->param) << 7) / total;
            } else {
                alpha = total; // match: 0, through the variable
            }
            if (object->model->unk100 & 0x4000) {
                object->model->alpha = alpha;
            } else {
                object->model->unk145 = 1;
                object->model->unk146 = alpha;
            }
        }
        MapObject_StepAnim(object);
    } else {
        if (!(object->unk04->unk68 & 0x20000)) {
            other = MapObject_GetById(MAP_OBJECT_BYTES(object)->unk00 & 1, object->unk08->unk38.bytes[3]);
            if (other != NULL) {
                MapObject_Activate(other);
            }
        }
        MapObject_Deactivate(object);
    }
}

void func_0011F0D8(MapTrigger *trigger) {
    TriggerSource *source;

    if (*trigger->unk04 == 0) {
        source = MAP_TRIGGER_SOURCE(trigger);
        if (((u32)(source->unk44 - 0x40) < 0x40) && (trigger->shot.unk00[0] != 0)) {
            if ((trigger->unk10 & 1) != D_00446770) {
                trigger->shot.unk00[0] = 0;
                return;
            }
            Sound_StopTagged((char)source->unk44);
            trigger->shot.unk00[0] = 0;
        }
    }
}

/* Rebuilds a map object's model matrices from its position and rotation. */
void func_0011F148(char *arg) {
    MapObject *object = (MapObject *)arg;
    int i;
    ModelPart *part;
    int part_count;

    ModelMatrix_Build(object->model, &object->model->position, &object->model->rotation, &object->model->unkE0);
    part_count = object->model->part_count;
    part = object->model->parts;
    for (i = 0; i < part_count; i++) {
        ModelMatrix_BuildPartLocal(object->model, part, part);
        part++;
    }
    if (object->model->unk100 & 1) {
        part = object->model->parts;
        for (i = 0; i < part_count; i++) {
            func_0010B5D0(part);
            part++;
        }
    }
    sceVu0CopyMatrix(object->model->unk80, object->model->unk40);
    object->model->unk1B4(object->model);
}

int func_0011F220(void) {
    return 0;
}

void func_0011F228(void) {
}

void func_0011F230(void) {
}
