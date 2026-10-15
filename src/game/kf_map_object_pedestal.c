#include "kf_map.h"
#include "kf_map_object.h"
#include "kf_model.h"
#include "kf_game.h"
#include "kf_message.h"

int Game_GetTriggerItem(int, int);
void func_00122600(MapObject *, int);
void func_0011F148(char *);
void Matrix_Multiply(void *, void *, void *);
void Vector_ApplyMatrix(void *, void *, void *);
void Vector_Copy(void *, void *);
void sceVu0InversMatrix(void *, void *);
void Vector_Subtract(void *, void *, void *);
float sqrtf(float);
void Vector_Scale(float *, float *, float);
void Vector_Add(void *, void *, void *);
int Game_ClearTriggerItem(int, int);
void Player_AddItem(int);
void func_00122530(char *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
int Game_AddTriggerItem(int, int, int);
extern MapSlot map_slots[];
extern GameStatus game_status;
extern void *D_00420D50[];
extern float player_state[];
extern EventRunner D_00420D00;
extern f32 D_00431AF0[16];
extern ItemCommonDef item_common_defs[];

/* Start handler: a pedestal that starts with an item on it stays so only while
   the map's trigger records still hold an item for it (Game_GetTriggerItem); its model
   shows that item. */
void func_00121DA8(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    int item;

    work->part = 2;
    work->fired = 0;
    item = 0;
    if (object->unk10 & 0x20000) {
        item = Game_GetTriggerItem(map_slots[MAP_OBJECT_BYTES(object)->unk00 & 1].tag, object->unk08->unk38.bytes[2]);
        if (item <= 0) {
            object->unk10 &= ~0x20000;
        }
    }
    func_00122600(object, item);
    if (!(object->unk10 & 0x20000)) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 4;
    }
}

/* Update handler, step 1: used by the player it only shows a message; once fired
   it puts the part at a point 220 below the player and works out the steps that
   carry it back (6.67 a frame). */
void func_00121E78(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    ModelPart *part = &object->model->parts[work->part];
    f32 matrix[16];
    f32 unused[32];
    Vector4 origin;
    Vector4 target;
    f32 distance;

    if (work->timer > 0) {
        work->timer--;
    }
    if (game_status.bit1 && D_00420D50[0] == object) {
        if (work->timer > 0) {
            return;
        }
        Message_ShowNotice(0, 0, 0, 0, 0x2D, (s16)object->unk04->unk68);
        work->timer = 0x2D;
    } else if (work->fired != 0) {
        work->fired = 0;
        object->unk10 |= 0x20000;
        MAP_OBJECT_BYTES(object)->step = 3;
        MapObject_StepAnim(object);
        func_0011F148((char *)object);
        Vector_Copy(&origin, part->unk40);
        origin.w = 1.0f;
        Vector_Copy(&target, player_state);
        target.w = 1.0f;
        target.y -= 220.0f;
        Matrix_Multiply(matrix, object->model->unk40, part);
        sceVu0InversMatrix(matrix, matrix);
        Vector_ApplyMatrix(&target, matrix, &target);
        Vector_Subtract(work, &origin, &target);
        distance = sqrtf(work->step.x * work->step.x + work->step.y * work->step.y + work->step.z * work->step.z);
        work->frames = distance / 6.6666665f;
        Vector_Copy(part->unk40, &target);
        Vector_Scale(&work->step.x, &work->step.x, 1.0f / work->frames);
        game_status.unk30 |= 1;
    }
}

/* Update handler, step 2: moves the part one step; after the last one it is at step 3. */
void func_001220B8(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    ModelPart *part = &object->model->parts[work->part];

    if (work->timer > 0) {
        work->timer--;
    }
    Vector_Add(part->unk40, part->unk40, work);
    if (--work->frames <= 0) {
        MAP_OBJECT_BYTES(object)->step = 3;
    }
}

/* Update handler: runs the animation; at its end the object is at step 4 and
   game_status.unk30 bit 0 is cleared. */
void func_00122138(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->param != 0) {
        MapObject_StepAnim(object);
    } else {
        MapObject_StartAnim(object, 5);
        MAP_OBJECT_BYTES(object)->step = 4;
        game_status.unk30 &= ~1U;
    }
}

/* Event handler: while an event runs on this object, moves it to step 5, takes
   back the item recorded for it in the map's trigger records and gives it to the
   player, runs the animation and clears bit 17. */
void func_001221C0(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    u8 timer = work->timer;
    u8 tag = map_slots[MAP_OBJECT_BYTES(object)->unk00 & 1].tag;
    int item;

    if (work->timer > 0) {
        work->timer = timer - 1;
    }
    if (game_status.bit1 && D_00420D00.current == object) {
        MAP_OBJECT_BYTES(object)->step = 5;
        // match: two leftover arguments are passed
        item = ((int (*)(int, int, void *, int))Game_GetTriggerItem)(tag, object->unk08->unk38.bytes[2], work, timer);
        Game_ClearTriggerItem(tag, object->unk08->unk38.bytes[2]);
        MapObject_StepAnim(object);
        func_0011F148((char *)object);
        Player_AddItem(item);
        object->unk10 &= ~0x20000;
        game_status.unk30 |= 1;
    }
}

/* Update handler, step 5: once the animation has ended, works out the steps that
   carry the part from point 0 of the model to a point 115 below the player. */
void func_001222C0(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    f32 matrix[16];
    Vector4 origin;
    Vector4 target;
    f32 distance;
    int frames;
    void *point;

    if (work->timer > 0) {
        work->timer--;
    }
    if (MODEL_CHANNEL(object->model)->param != 0) {
        MapObject_StepAnim(object);
        return;
    }
    MAP_OBJECT_BYTES(object)->step = 6;
    func_00122530((char *)object);
    object->model->unk1B4(object->model);
    point = (void *)object->model->get_point(object->model, 0);
    Vector_Copy(&origin, point);
    Vector_Copy(&target, player_state);
    target.y -= 115.0f;
    Vector_Subtract(work, &target, &origin);
    distance = sqrtf(((MapObjectWorkKind121DA8 *)object->work)->step.x * ((MapObjectWorkKind121DA8 *)object->work)->step.x + work->step.y * work->step.y + work->step.z * work->step.z);
    sceVu0RotMatrixX(matrix, D_00431AF0, object->model->rotation.x);
    sceVu0RotMatrixY(matrix, D_00431AF0, -object->model->rotation.y);
    Vector_ApplyMatrix(work, matrix, work);
    frames = distance / 6.6666665f;
    work->frames = frames;
    Vector_Scale(&work->step.x, &work->step.x, 1.0f / frames);
}

/* The same for the kind of func_00121DA8, which also hides the item on its model (func_00122600). */
void func_00122480(MapObject *object) {
    MapObjectWorkKind121DA8 *work = (MapObjectWorkKind121DA8 *)object->work;
    ModelPart *part = &object->model->parts[work->part];

    if (work->timer > 0) {
        work->timer--;
    }
    Vector_Add(part->unk40, part->unk40, work);
    if (--work->frames <= 0) {
        func_00122600(object, 0);
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        game_status.unk30 &= ~1U;
    }
}

void func_00122530(char *object) {
    func_0011F148(object);
}

/* Fires the trigger's one-shot when its type matches; returns whether it fired. */
int func_00122538(MapTrigger *trigger, int type_index) {
    u8 type;
    MapSlot *slot;
    int tag;
    MapTriggerShot *shot = &trigger->shot;

    type = item_common_defs[type_index].type;
    slot = &map_slots[trigger->unk10 & 1];
    tag = slot->tag;
    if (trigger->type == 1 && shot->fired == 0 && type == trigger->type &&
        Game_AddTriggerItem(tag, (u8)trigger->unk08[0x3A], type_index)) {
        func_00122600((MapObject *)trigger, type_index);
        shot->fired = type;
        return TRUE;
    }
    return FALSE;
}

/* Shows the model chunk that goes with the item on the pedestal: items 0x86..0x8A
   show chunks 0..4 (a set bit of ModelInstance.unk178 skips that chunk); any other
   item, or none, hides all five. */
void func_00122600(MapObject *object, int item) {
    u32 *skip_mask;
    u32 chunk;
    int shown;

    skip_mask = (u32 *)&object->model->unk178;
    chunk = item - 0x86;
    shown = (chunk <= 4U) ? (int)chunk : -1;

    *skip_mask &= ~1u;
    if (shown != 0) {
        *skip_mask |= 1;
    }
    *skip_mask &= ~2u;
    if (shown != 1) {
        *skip_mask |= 2;
    }
    *skip_mask &= ~4u;
    if (shown != 2) {
        *skip_mask |= 4;
    }
    *skip_mask &= ~8u;
    if (shown != 3) {
        *skip_mask |= 8;
    }
    *(u32 *)&object->model->unk178 &= ~0x10u;
    if (shown != 4) {
        *(u32 *)&object->model->unk178 |= 0x10;
    }
}
