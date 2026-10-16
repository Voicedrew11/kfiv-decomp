/* The per-map set-up run when a map is loaded. A map's tag is its file number (NNN.map); the place names
   are those docs/GAME_DATA.md gives the map numbers, taken from Prima's official strategy guide (2004). */
#include "kf_map.h"
#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_map_object.h"
#include "kf_model.h"
#include "kf_collision.h"
#include "kf_effect.h"

void MapSetup_Armory(int);
void MapSetup_Forge017(int);
void MapSetup_Forge018(int);
void MapSetup_Forge019(int);
void MapSetup_EarthFolksVillage(int);
void MapSetup_CavernOfLava(int);
void MapSetup_FireLordsDomain(int);
void MapSetup_OceanShore(int);
void MapSetup_UndergroundCanal(int);
void MapSetup_GuardiansRoom(int);
void MapSetup_DarkSanctuary(int);
void MapSetup_KingsThroneAlcoves(int);
void MapSetup_SanctuaryOfLight(int);
void MapSetup_CentralTowerLevel3(int);
void MapSetup_CraftsmansRoom(int);
void MapSetup_CentralTowerLevel2(int);
void MapSetup_PlaceItemLowered(int);
void func_00119E10(int);
int MapItem_PlaceFixedById(int, int);
void *Map_GetRecordById(int, int);
void Map_OffsetRecord(MapRecordBounds *, f32 *);
void *MapItem_GetFixedById(int);
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

/* Runs the set-up function of the map in the slot, picked by the map's number (its tag): it puts the map's
   records, objects, fog and lights in the state the story flags call for. */
void MapSetup_RunForMap(int slot) {
    switch (map_slots[slot].tag) {
    case 0xB:
        MapSetup_Armory(slot);
        break;
    case 0x11:
        MapSetup_Forge017(slot);
        break;
    case 0x12:
        MapSetup_Forge018(slot);
        break;
    case 0x13:
        MapSetup_Forge019(slot);
        break;
    case 0x14:
        MapSetup_EarthFolksVillage(slot);
        break;
    case 0x18:
        MapSetup_CavernOfLava(slot);
        break;
    case 0x19:
        MapSetup_FireLordsDomain(slot);
        break;
    case 0x1B:
        MapSetup_OceanShore(slot);
        break;
    case 0x1D:
        MapSetup_UndergroundCanal(slot);
        break;
    case 0x23:
        MapSetup_GuardiansRoom(slot);
        break;
    case 0x26:
        MapSetup_DarkSanctuary(slot);
        break;
    case 0x29:
        MapSetup_KingsThroneAlcoves(slot);
        break;
    case 0x2A:
        MapSetup_SanctuaryOfLight(slot);
        break;
    case 0x64:
        MapSetup_CentralTowerLevel3(slot);
        break;
    case 0x68:
        MapSetup_CraftsmansRoom(slot);
        break;
    case 0x6B:
        MapSetup_CentralTowerLevel2(slot);
        break;
    case 0xFB:
        MapSetup_PlaceItemLowered(slot);
        break;
    case 0xFC:
        func_0011A428(slot);
        break;
    }
}

/* Runs the every-frame function of the Ocean Shore (map 027) in whichever slot holds it. Nothing calls it. */
void MapSetup_UpdateOceanShore(void) {
    int map;
    MapSlot *slot;

    for (map = 0; map < 2; map++) {
        slot = &map_slots[map];
        if (slot->loaded != 0 && slot->tag == 0x1B) {
            func_00119E10(map);
        }
    }
}

/* Set-up of the Armory (map 011): fixed item 0x46, map objects 0xA, 0xB and 0x22 and entity 0x17 from the
   story flags. */
void MapSetup_Armory(int map) {
    MapObject *object;
    Entity *entity;

    if (map_script_flags[0x2C5] != 0) {
        MapItem_PlaceFixedById(map, 0x46);
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
        ((int (*)(int, int))MapItem_PlaceFixedById)(map, 0x46); // match: called as returning a value, so the next test uses $3
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
            object->state_flags |= 0x20000;
        }
        object = MapObject_GetById(map, 0xB);
        if (object != NULL) {
            MapObject_SetScripted(object, 0);
            object->state_flags |= 0x20000;
        }
        object = MapObject_GetById(map, 0x22);
        if (object != NULL) {
            MapObject_SetScripted(object, 1);
            MapObject_PlayScriptedAnim(object, 1, 0);
            MapObject_SetScriptedBit21(object, 1);
        }
    }
}

/* Set-up of the Forge as map 017: once the story flag is set, map object 0x65 starts open (bit 17; in all
   three Forge maps it is a door of kind 0x11 at the same spot, one that does not shut by itself). The Forge
   is three map files (017, 018, 019) with the same links and three identical set-up functions; the file
   number the switch in MapSetup_RunForMap picks each by is the only thing that tells them apart. */
void MapSetup_Forge017(int map) {
    MapObject *object;

    if (D_0034B69C[0] != 0) {
        object = MapObject_GetById(map, 0x65);
        if (object != NULL) {
            object->state_flags |= 0x20000;
        }
    }
}

/* The same for the Forge as map 018: door 0x65 starts open once the story flag is set. */
void MapSetup_Forge018(int map) {
    MapObject *object;

    if (D_0034B69C[0] != 0) {
        object = MapObject_GetById(map, 0x65);
        if (object != NULL) {
            object->state_flags |= 0x20000;
        }
    }
}

/* The same for the Forge as map 019: door 0x65 starts open once the story flag is set. */
void MapSetup_Forge019(int map) {
    MapObject *object;

    if (D_0034B69C[0] != 0) {
        object = MapObject_GetById(map, 0x65);
        if (object != NULL) {
            object->state_flags |= 0x20000;
        }
    }
}

static inline void hide_found_record(MapRecordBounds *record) {
    if (record != NULL) {
        record->record_flags |= 0x40000000;
    }
}

static inline void show_found_record(MapRecordBounds *record) {
    if (record != NULL) {
        record->record_flags &= ~0x40000000;
    }
}

/* Set-up of the Earth Folks' Village (map 020): shows the map records that go with the story flag (0, 1 or
   more) and, past 0, changes fog block 2 and light block 4. */
void MapSetup_EarthFolksVillage(int map) {
    void *target; // match: one variable for every record and then the map slot; it gives the slot the records' register
    MapSlotUnk0CValues *block;
    MapSlotUnk10 *light;

    switch (D_0034B696[0]) {
    case 0:
        target = Map_GetRecordById(map, 0x33);
        show_found_record(target);
        target = Map_GetRecordById(map, 0x6D);
        hide_found_record(target);
        target = Map_GetRecordById(map, 0x6E);
        hide_found_record(target);
        return;
    case 1:
        target = Map_GetRecordById(map, 2);
        hide_found_record(target);
        target = Map_GetRecordById(map, 0x33);
        hide_found_record(target);
        target = Map_GetRecordById(map, 0x6D);
        show_found_record(target);
        target = Map_GetRecordById(map, 0x6E);
        hide_found_record(target);
        break;
    default:
        target = Map_GetRecordById(map, 2);
        hide_found_record(target);
        target = Map_GetRecordById(map, 0x33);
        hide_found_record(target);
        target = Map_GetRecordById(map, 0x6D);
        hide_found_record(target);
        target = Map_GetRecordById(map, 0x6E);
        show_found_record(target);
        break;
    }
    target = &map_slots[map];
    block = (MapSlotUnk0CValues *)((MapSlot *)target)->fog_blocks;
    light = ((MapSlot *)target)->light_blocks;
    block[2].unk00 = block[2].unk04 = 0.0f;
    block[2].unk08 = -43.40425f;
    block[2].unk0C = 238723.4f;
    block[2].unk10 = 0;
    light[4].light1_direction_y = 0.0f;
    light[4].light1_direction_z = 0.0f;
    light[4].light1_direction_x = -1.57f;
    light[4].light1_red = 0.50980395f;
    light[4].light1_green = 0.13725491f;
    light[4].light1_blue = 0.0f;
    light[4].ambient_red = 0.6666667f;
    light[4].ambient_green = 0.38431373f;
    light[4].ambient_blue = 0.27450982f;
}

/* Set-up of the Cavern of Lava (map 024): at story flag 0 a light grey fog in block 5, otherwise a thin black one,
   two light colours and all three light directions cleared in light block 9. */
void MapSetup_CavernOfLava(int map) {
    MapSlot *slot = &map_slots[map];
    MapSlotUnk0CValues *fog;
    MapSlotUnk10Lights *lights;

    if (D_0034B696[0] == 0) {
        ((MapSlotUnk0CValues *)slot->fog_blocks)[5].unk00 = ((MapSlotUnk0CValues *)slot->fog_blocks)[5].unk04 = 0.0f;
        ((MapSlotUnk0CValues *)slot->fog_blocks)[5].unk08 = -28.333328f;
        ((MapSlotUnk0CValues *)slot->fog_blocks)[5].unk0C = 113333.336f;
        ((MapSlotUnk0CValues *)slot->fog_blocks)[5].unk10 = 0x8C8C8C;
    } else {
        fog = (MapSlotUnk0CValues *)slot->fog_blocks;
        lights = slot->light_blocks;
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
    MapRecordBounds *record = Map_GetRecordById(map, record_id);

    if (record != NULL) {
        record->record_flags |= 0x40000000;
    }
}

static inline void show_record(int map, int record_id) {
    MapRecordBounds *record = Map_GetRecordById(map, record_id);

    if (record != NULL) {
        record->record_flags &= ~0x40000000;
    }
}

/* Set-up of the Fire Lord's Domain (map 025): shows the map records that go with the story flag; at 2 also
   moves object 0x66 and starts effect 0x1DF. */
void MapSetup_FireLordsDomain(int map) {
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
        object->model->colour_red = object->model->colour_green = object->model->colour_blue = 255.0f;
    }
}

/* Set-up of the Ocean Shore (map 027): once its story flag is 1, new fog in blocks 0, 1 and 4. */
void MapSetup_OceanShore(int map) {
    MapSlot *slot = &map_slots[map];
    MapSlotUnk0CValues *fog;

    if (D_00349D55[0] == 1) {
        fog = (MapSlotUnk0CValues *)slot->fog_blocks;
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

/* Every-frame function of the Ocean Shore: empty. */
void func_00119E10(int map) {
}

/* Set-up of the Underground Canal (map 029): puts map record 5 at the height the story state keeps for it,
   590 the first time. */
void MapSetup_UndergroundCanal(int map) {
    MapRecordBounds *record;
    f32 offset[4];

    if (map_script_flags[0x740] == 0) {
        map_record_height[0] = 590.0f;
        map_script_flags[0x747] = 0;
        map_script_flags[0x746] = 1;
    }
    record = Map_GetRecordById(map, 5);
    if (record != NULL) {
        offset[0] = 0.0f;
        offset[1] = map_record_height[0] - 590.0f;
        offset[2] = 0.0f;
        offset[3] = 0.0f;
        Map_OffsetRecord(record, offset);
    }
}

/* Set-up of the Guardian's Room (map 035): once the story flag is set, swaps map records 8-12 for
   0x29-0x2D. */
void MapSetup_GuardiansRoom(int map) {
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

/* Set-up of the Dark Sanctuary (map 038): once its story flag is 1, new fog in blocks 1 and 2. */
void MapSetup_DarkSanctuary(int map) {
    MapSlot *slot = &map_slots[map];

    if (D_0034A017[0] == 1) {
        MapSlotUnk0CValues *fog = (MapSlotUnk0CValues *)slot->fog_blocks;

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

/* Set-up of the King's Throne, Alcoves (map 041): once its story flag is 1, new fog in blocks 2 and 3 and a dim light colour
   in light block 1. */
void MapSetup_KingsThroneAlcoves(int map) {
    MapSlot *slot = &map_slots[map];

    if (D_0034A0D5[0] == 1) {
        MapSlotUnk0CValues *fog = (MapSlotUnk0CValues *)slot->fog_blocks;
        MapSlotUnk10 *light = slot->light_blocks;

        fog[2].unk00 = fog[2].unk04 = 0.0f;
        fog[2].unk08 = -127.5f;
        fog[2].unk0C = 382500.0f;
        fog[2].unk10 = 0;
        fog[3].unk00 = fog[3].unk04 = 0.0f;
        fog[3].unk08 = -127.5f;
        fog[3].unk0C = 382500.0f;
        fog[3].unk10 = 0;
        light[1].ambient_red = 0.29803923f;
        light[1].ambient_green = 0.29803923f;
        light[1].ambient_blue = 0.30980393f;
    }
}

/* Set-up of the Sanctuary of Light (map 042): takes map objects 0x66 and 0x67 out of use, or once the
   story flag is set (and until the next one is) puts fixed item 0xC9 out and moves 0x67 into place. */
void MapSetup_SanctuaryOfLight(int map) {
    MapObject *object;

    if (map_script_flags[0xA85] != 0) {
        if (map_script_flags[0xA86] == 0) {
            MapItem_PlaceFixedById(map, 0xC9);
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

/* Set-up of the Ancient City, Level 3, Central Tower (map 100): steps the story flags around entity
   spawn id 1 and, unless that stops it, clears its flag bits 1 and 7. */
void MapSetup_CentralTowerLevel3(int map_slot) {
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

/* Set-up of the Craftsman's Room (map 104): until flag D_0034B098[0] is set, takes map objects 0x77 and 0x76 out of
   use, giving 0x77 (when it is a light, kind 9) the colour 0xFF, 0x80, 0x18 in its source (unk4A 0). */
void MapSetup_CraftsmansRoom(int map) {
    MapObject *object;

    if (D_0034B098[0] == 0) {
        object = MapObject_GetById(map, 0x77);
        if (object != NULL) {
            MapObject_Deactivate(object);
            if (object->model != NULL && object->definition->kind == 9) {
                MAP_OBJECT_WORDS(object)->kind_param_0 = 0xFF;
                MAP_OBJECT_WORDS(object)->kind_param_1 = 0x80;
                MAP_OBJECT_WORDS(object)->kind_param_2 = 0x18;
                MAP_OBJECT_WORDS(object)->kind_param_3 = 0;
            }
        }
        object = MapObject_GetById(map, 0x76);
        if (object != NULL) {
            MapObject_Deactivate(object);
        }
    }
}

/* Set-up of the Ancient City, Level 2, Central Tower (map 107): once flag map_script_flags[0x1C0A] is set, sets 0x1ADB and swaps map object 0x75 for 0x76. */
void MapSetup_CentralTowerLevel2(int map) {
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

/* Set-up of map 251, which is not on the disc: puts fixed item 0x37 out (MapItem_PlaceFixedById) and lowers
   its model by 400. */
void MapSetup_PlaceItemLowered(int map) {
    ModelNode *node;
    Vector4 unused;

    MapItem_PlaceFixedById(map, 0x37);
    node = MapItem_GetFixedById(0x37);
    if (node != NULL) {
        node->model->position.y -= 400.0f;
    }
}

/* Set-up of map 252, which is not on the disc: empty. */
void func_0011A428(int map) {
}
