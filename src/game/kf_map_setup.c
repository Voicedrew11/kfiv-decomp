#include "kf_map.h"
#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_map_object.h"
#include "kf_model.h"
#include "kf_collision.h"
#include "kf_effect.h"

void func_00119590(int);
void func_00119730(int);
void func_00119770(int);
void func_001197B0(int);
void func_001197F0(int);
void func_00119A48(int);
void func_00119B20(int);
void func_00119D90(int);
void func_00119E18(int);
void func_00119E90(int);
void func_0011A048(int);
void func_0011A0B8(int);
void func_0011A138(int);
void func_0011A210(int);
void func_0011A2B8(int);
void func_0011A370(int);
void func_0011A3E8(int);
void func_00119E10(int);
int func_001C8C80(int, int);
void *func_00116160(int, int);
void func_00115550(MapRecordBounds *, f32 *);
void *func_001C8AF0(int);
extern MapSlot map_slots[];
extern u8 map_script_flags[];
extern u8 entity_group_state[];
extern u8 D_0034B69C[];
extern u8 D_0034B696[];
extern u8 D_00349D55[];
extern f32 map_record_height[];
extern u8 D_00349F56[];
extern u8 D_0034A017[];
extern u8 D_0034A0D5[];
extern u8 D_0034B098[];

void func_0011A428(int);

/* Runs the set-up function of the map in the slot, picked by the map's tag. */
void func_00119360(int slot) {
    switch (map_slots[slot].tag) {
    case 0xB:
        func_00119590(slot);
        break;
    case 0x11:
        func_00119730(slot);
        break;
    case 0x12:
        func_00119770(slot);
        break;
    case 0x13:
        func_001197B0(slot);
        break;
    case 0x14:
        func_001197F0(slot);
        break;
    case 0x18:
        func_00119A48(slot);
        break;
    case 0x19:
        func_00119B20(slot);
        break;
    case 0x1B:
        func_00119D90(slot);
        break;
    case 0x1D:
        func_00119E18(slot);
        break;
    case 0x23:
        func_00119E90(slot);
        break;
    case 0x26:
        func_0011A048(slot);
        break;
    case 0x29:
        func_0011A0B8(slot);
        break;
    case 0x2A:
        func_0011A138(slot);
        break;
    case 0x64:
        func_0011A210(slot);
        break;
    case 0x68:
        func_0011A2B8(slot);
        break;
    case 0x6B:
        func_0011A370(slot);
        break;
    case 0xFB:
        func_0011A3E8(slot);
        break;
    case 0xFC:
        func_0011A428(slot);
        break;
    }
}

/* Runs the every-frame function of map 0x1B in whichever slot holds it. */
void func_00119520(void) {
    int map;
    MapSlot *slot;

    for (map = 0; map < 2; map++) {
        slot = &map_slots[map];
        if (slot->unk00 != 0 && slot->tag == 0x1B) {
            func_00119E10(map);
        }
    }
}

/* Sets map objects 0xA, 0xB and 0x22 and entity 0x17 up from the stage flags. */
void func_00119590(int map) {
    MapObject *object;
    Entity *entity;

    if (map_script_flags[0x2C5] != 0) {
        func_001C8C80(map, 0x46);
    }
    if (entity_group_state[0x5C6] == 1 && map_script_flags[0x2C5] == 0) {
        object = MapObject_GetById(map, 0xA);
        if (object != NULL) {
            MapObject_SetScripted(object, 1);
        }
        object = MapObject_GetById(map, 0xB);
        if (object != NULL) {
            MapObject_SetScripted(object, 1);
        }
        ((int (*)(int, int))func_001C8C80)(map, 0x46); // match: called as returning a value, so the next test uses $3
        if (entity_group_state[0x5C0] == 0) {
            entity = Entity_GetBySpawnIdForMap(map, 0x17);
            if (entity != NULL) {
                entity->flags |= 2;
                entity->flags |= 0x80;
            }
        }
    } else {
        object = MapObject_GetById(map, 0xA);
        if (object != NULL) {
            MapObject_SetScripted(object, 0);
            object->unk10 |= 0x20000;
        }
        object = MapObject_GetById(map, 0xB);
        if (object != NULL) {
            MapObject_SetScripted(object, 0);
            object->unk10 |= 0x20000;
        }
        object = MapObject_GetById(map, 0x22);
        if (object != NULL) {
            MapObject_SetScripted(object, 1);
            MapObject_PlayScriptedAnim(object, 1, 0);
            MapObject_SetScriptedBit21(object, 1);
        }
    }
}

/* Sets flag 0x20000 of map object 0x65 once the stage flag is set. */
void func_00119730(int map) {
    MapObject *object;

    if (D_0034B69C[0] != 0) {
        object = MapObject_GetById(map, 0x65);
        if (object != NULL) {
            object->unk10 |= 0x20000;
        }
    }
}

/* Sets flag 0x20000 of map object 0x65 once the stage flag is set. */
void func_00119770(int map) {
    MapObject *object;

    if (D_0034B69C[0] != 0) {
        object = MapObject_GetById(map, 0x65);
        if (object != NULL) {
            object->unk10 |= 0x20000;
        }
    }
}

/* Sets flag 0x20000 of map object 0x65 once the stage flag is set. */
void func_001197B0(int map) {
    MapObject *object;

    if (D_0034B69C[0] != 0) {
        object = MapObject_GetById(map, 0x65);
        if (object != NULL) {
            object->unk10 |= 0x20000;
        }
    }
}

static inline void hide_found_record(MapRecordBounds *record) {
    if (record != NULL) {
        record->unk3C |= 0x40000000;
    }
}

static inline void show_found_record(MapRecordBounds *record) {
    if (record != NULL) {
        record->unk3C &= ~0x40000000;
    }
}

/* Shows the map records that go with the stage flag (0, 1 or more) and, past 0, changes block 2 and 4 of the map. */
void func_001197F0(int map) {
    void *target; // match: one variable for every record and then the map slot; it gives the slot the records' register
    MapSlotUnk0CValues *block;
    MapSlotUnk10 *light;

    switch (D_0034B696[0]) {
    case 0:
        target = func_00116160(map, 0x33);
        show_found_record(target);
        target = func_00116160(map, 0x6D);
        hide_found_record(target);
        target = func_00116160(map, 0x6E);
        hide_found_record(target);
        return;
    case 1:
        target = func_00116160(map, 2);
        hide_found_record(target);
        target = func_00116160(map, 0x33);
        hide_found_record(target);
        target = func_00116160(map, 0x6D);
        show_found_record(target);
        target = func_00116160(map, 0x6E);
        hide_found_record(target);
        break;
    default:
        target = func_00116160(map, 2);
        hide_found_record(target);
        target = func_00116160(map, 0x33);
        hide_found_record(target);
        target = func_00116160(map, 0x6D);
        hide_found_record(target);
        target = func_00116160(map, 0x6E);
        show_found_record(target);
        break;
    }
    target = &map_slots[map];
    block = (MapSlotUnk0CValues *)((MapSlot *)target)->unk0C;
    light = ((MapSlot *)target)->unk10;
    block[2].unk00 = block[2].unk04 = 0.0f;
    block[2].unk08 = -43.40425f;
    block[2].unk0C = 238723.4f;
    block[2].unk10 = 0;
    light[4].unk14 = 0.0f;
    light[4].unk24 = 0.0f;
    light[4].unk04 = -1.57f;
    light[4].unk50 = 0.50980395f;
    light[4].unk54 = 0.13725491f;
    light[4].unk58 = 0.0f;
    light[4].unk70 = 0.6666667f;
    light[4].unk74 = 0.38431373f;
    light[4].unk78 = 0.27450982f;
}

/* Set-up of map 0x18: at stage flag 0 a light grey fog in block 5, otherwise a thin black one,
   two light colours and all three light directions cleared in light block 9. */
void func_00119A48(int map) {
    MapSlot *slot = &map_slots[map];
    MapSlotUnk0CValues *fog;
    MapSlotUnk10Lights *lights;

    if (D_0034B696[0] == 0) {
        ((MapSlotUnk0CValues *)slot->unk0C)[5].unk00 = ((MapSlotUnk0CValues *)slot->unk0C)[5].unk04 = 0.0f;
        ((MapSlotUnk0CValues *)slot->unk0C)[5].unk08 = -28.333328f;
        ((MapSlotUnk0CValues *)slot->unk0C)[5].unk0C = 113333.336f;
        ((MapSlotUnk0CValues *)slot->unk0C)[5].unk10 = 0x8C8C8C;
    } else {
        fog = (MapSlotUnk0CValues *)slot->unk0C;
        lights = slot->unk10;
        fog[5].unk00 = fog[5].unk04 = 0.0f;
        fog[5].unk08 = -318.75f;
        fog[5].unk0C = 1.1475e+06f;
        fog[5].unk10 = 0;
        lights[9].colors[3].x = 0.6627451f;
        lights[9].colors[3].y = 0.3372549f;
        lights[9].directions[0][0] = 0.0f;
        lights[9].directions[1][0] = 0.0f;
        lights[9].directions[2][0] = 0.0f;
        lights[9].colors[0].x = 0.0f;
        lights[9].colors[0].y = 0.0f;
        lights[9].colors[0].z = 0.0f;
        lights[9].directions[0][1] = 0.0f;
        lights[9].directions[1][1] = 0.0f;
        lights[9].directions[2][1] = 0.0f;
        lights[9].colors[1].x = 0.0f;
        lights[9].colors[1].y = 0.0f;
        lights[9].colors[1].z = 0.0f;
        lights[9].directions[0][2] = 0.0f;
        lights[9].directions[1][2] = 0.0f;
        lights[9].directions[2][2] = 0.0f;
        lights[9].colors[2].x = 0.0f;
        lights[9].colors[2].y = 0.0f;
        lights[9].colors[2].z = 0.0f;
        lights[9].colors[3].z = 0.0f;
    }
}

static inline void hide_record(int map, int record_id) {
    MapRecordBounds *record = func_00116160(map, record_id);

    if (record != NULL) {
        record->unk3C |= 0x40000000;
    }
}

static inline void show_record(int map, int record_id) {
    MapRecordBounds *record = func_00116160(map, record_id);

    if (record != NULL) {
        record->unk3C &= ~0x40000000;
    }
}

/* Shows the map records that go with the stage flag; at 2 also moves object 0x66 and spawns effect 0x1DF. */
void func_00119B20(int map) {
    MapObject *object;
    f32 position[4];
    f32 rotation[4];

    if (D_0034B696[0] != 2) {
        hide_record(map, 0x42);
        hide_record(map, 0x43);
        hide_record(map, 0x44);
        show_record(map, 0x3F);
        show_record(map, 0x41);
    } else {
        show_record(map, 0x42);
        show_record(map, 0x43);
        show_record(map, 0x44);
        hide_record(map, 0x3F);
        hide_record(map, 0x41);
        object = MapObject_GetById(map, 0x66);
        if (object != NULL) {
            object->model->position.x = -5887.0f;
            object->model->position.y = -790.0f;
            object->model->position.z = -1157.0f;
        }
        position[0] = -5887.0f;
        position[1] = -910.0f;
        position[2] = -1157.0f;
        position[3] = 0.0f;
        rotation[1] = rotation[0] = 3.14f;
        rotation[2] = 0.0f;
        rotation[3] = 0.0f;
        Effect_StartSpawner(0x1DF, position, rotation, &map_slots[map]);
    }
    object = MapObject_GetById(map, 0x67);
    if (object != NULL) {
        object->model->unk150 = object->model->unk154 = object->model->unk158 = 255.0f;
    }
}

/* Set-up of map 0x1B: once its stage flag is 1, new fog in blocks 0, 1 and 4. */
void func_00119D90(int map) {
    MapSlot *slot = &map_slots[map];
    MapSlotUnk0CValues *fog;

    if (D_00349D55[0] == 1) {
        fog = (MapSlotUnk0CValues *)slot->unk0C;
        fog[0].unk00 = fog[0].unk04 = 0.0f;
        fog[0].unk08 = -9.705887f;
        fog[0].unk0C = 476470.62f;
        fog[0].unk10 = 0;
        fog[1].unk00 = fog[1].unk04 = 0.0f;
        fog[1].unk08 = 2.0588226f;
        fog[1].unk0C = 455294.1f;
        fog[1].unk10 = 0;
        fog[4].unk00 = fog[4].unk04 = 0.0f;
        fog[4].unk08 = -9.705887f;
        fog[4].unk0C = 476470.62f;
        fog[4].unk10 = 0;
    }
}

void func_00119E10(int map) {
}

/* Puts map record 5 at the height the stage keeps for it, 590 the first time. */
void func_00119E18(int map) {
    MapRecordBounds *record;
    f32 offset[4];

    if (map_script_flags[0x740] == 0) {
        map_record_height[0] = 590.0f;
        map_script_flags[0x747] = 0;
        map_script_flags[0x746] = 1;
    }
    record = func_00116160(map, 5);
    if (record != NULL) {
        offset[0] = 0.0f;
        offset[1] = map_record_height[0] - 590.0f;
        offset[2] = 0.0f;
        offset[3] = 0.0f;
        func_00115550(record, offset);
    }
}

/* Once the stage flag is set, swaps map records 8-12 for 0x29-0x2D. */
void func_00119E90(int map) {
    if (D_00349F56[0] != 0) {
        hide_record(map, 8);
        hide_record(map, 9);
        hide_record(map, 0xA);
        hide_record(map, 0xB);
        hide_record(map, 0xC);
        show_record(map, 0x29);
        show_record(map, 0x2A);
        show_record(map, 0x2B);
        show_record(map, 0x2C);
        show_record(map, 0x2D);
    }
}

/* Set-up of map 0x26: once its stage flag is 1, new fog in blocks 1 and 2. */
void func_0011A048(int map) {
    MapSlot *slot = &map_slots[map];

    if (D_0034A017[0] == 1) {
        MapSlotUnk0CValues *fog = (MapSlotUnk0CValues *)slot->unk0C;

        fog[1].unk00 = fog[1].unk04 = 0.0f;
        fog[1].unk08 = 46.666664f;
        fog[1].unk0C = 166666.67f;
        fog[1].unk10 = 0;
        fog[2].unk00 = fog[2].unk04 = 0.0f;
        fog[2].unk08 = 1.6666718f;
        fog[2].unk0C = 405333.34f;
        fog[2].unk10 = 0;
    }
}

/* Set-up of map 0x29: once its stage flag is 1, new fog in blocks 2 and 3 and a dim light colour
   in light block 1. */
void func_0011A0B8(int map) {
    MapSlot *slot = &map_slots[map];

    if (D_0034A0D5[0] == 1) {
        MapSlotUnk0CValues *fog = (MapSlotUnk0CValues *)slot->unk0C;
        MapSlotUnk10 *light = slot->unk10;

        fog[2].unk00 = fog[2].unk04 = 0.0f;
        fog[2].unk08 = -127.5f;
        fog[2].unk0C = 382500.0f;
        fog[2].unk10 = 0;
        fog[3].unk00 = fog[3].unk04 = 0.0f;
        fog[3].unk08 = -127.5f;
        fog[3].unk0C = 382500.0f;
        fog[3].unk10 = 0;
        light[1].unk70 = 0.29803923f;
        light[1].unk74 = 0.29803923f;
        light[1].unk78 = 0.30980393f;
    }
}

/* Parks or reveals the pair of scene objects 0x66/0x67 depending on the stage flags. */
void func_0011A138(int map) {
    MapObject *object;

    if (map_script_flags[0xA85] != 0) {
        if (map_script_flags[0xA86] == 0) {
            func_001C8C80(map, 0xC9);
            object = MapObject_GetById(map, 0x67);
            if (object != NULL) {
                MapObject_Activate(object);
                object->model->position.x = -19150.0f;
                object->model->position.y = -2030.0f;
                object->model->position.z = 3450.0f;
            }
        }
    } else {
        object = MapObject_GetById(map, 0x66);
        if (object != NULL) {
            MapObject_Deactivate(object);
        }
        object = MapObject_GetById(map, 0x67);
        if (object != NULL) {
            MapObject_Deactivate(object);
        }
    }
}

/* Map tag 0x64: clears two flag bits on spawn id 1 while a cutscene gate runs. */
void func_0011A210(int map_slot) {
    Entity *entity;

    entity = Entity_GetBySpawnIdForMap(map_slot, 1);
    if (entity_group_state[0x340] == 0) {
        if (entity_group_state[0x345] != 0 && entity_group_state[0x346] == 0) {
            entity_group_state[0x346] = 1;
        }
        if (entity_group_state[0x346] == 1) {
            if (map_script_flags[0x1905] == 0) {
                map_script_flags[0x1905] = 1;
                entity_group_state[0x345] = 0x11;
            }
            return;
        }
    }
    if (entity != NULL) {
        ENTITY_FLAG_BITS(entity)->bit1 = 0;
        ENTITY_FLAG_BITS(entity)->bit7 = 0;
    }
}

/* Map script: until flag D_0034B098[0] is set, takes map objects 0x77 and 0x76 out of
   use, giving 0x77 (when it is of kind 9) the values 0xFF, 0x80, 0x18, 0. */
void func_0011A2B8(int map) {
    MapObject *object;

    if (D_0034B098[0] == 0) {
        object = MapObject_GetById(map, 0x77);
        if (object != NULL) {
            MapObject_Deactivate(object);
            if (object->model != NULL && object->unk04->kind == 9) {
                MAP_OBJECT_WORDS(object)->unk44 = 0xFF;
                MAP_OBJECT_WORDS(object)->unk46 = 0x80;
                MAP_OBJECT_WORDS(object)->unk48 = 0x18;
                MAP_OBJECT_WORDS(object)->unk4A = 0;
            }
        }
        object = MapObject_GetById(map, 0x76);
        if (object != NULL) {
            MapObject_Deactivate(object);
        }
    }
}

/* Map script: once flag map_script_flags[0x1C0A] is set, sets 0x1ADB and swaps map object 0x75 for 0x76. */
void func_0011A370(int map) {
    MapObject *object;

    if (map_script_flags[0x1C0A] != 0) {
        map_script_flags[0x1ADB] = 1;
        object = MapObject_GetById(map, 0x75);
        if (object != NULL) {
            MapObject_Deactivate(object);
        }
        object = MapObject_GetById(map, 0x76);
        if (object != NULL) {
            MapObject_Activate(object);
        }
    }
}

/* Lowers the model of node 0x37 by 400 after func_001C8C80 has set it up. */
void func_0011A3E8(int map) {
    ModelNode *node;
    Vector4 unused;

    func_001C8C80(map, 0x37);
    node = func_001C8AF0(0x37);
    if (node != NULL) {
        node->model->position.y -= 400.0f;
    }
}

void func_0011A428(int map) {
}
