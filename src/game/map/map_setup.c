/* The per-map set-up run when a map is loaded. A map's tag is its file number (NNN.map); the place names
   are those docs/GAME_DATA.md gives the map numbers, taken from Prima's official strategy guide (2004). */
#include "map/map.h"
#include "entity/entity.h"
#include "entity/entity_world.h"
#include "map_object/map_object.h"
#include "model/model.h"
#include "collision/collision.h"
#include "effect/effect.h"
#include "item/item.h"
#include "save/save_state.h"

/* EntityGroupState.death_kind: no member of the group has died */
#define ENTITY_GROUP_DEATH_NONE             0

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
        MapSetup_Map252(slot);
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
            MapSetup_OceanShoreFrame(map);
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
    if (entity_group_state[23].unk06 == 1 && map_script_flags[0x2C5] == 0) {
        object = MapObject_GetById(map, 0xA);
        if (object != NULL) {
            MapObject_SetScripted(object, 1);
        }
        object = MapObject_GetById(map, 0xB);
        if (object != NULL) {
            MapObject_SetScripted(object, 1);
        }
        MapItem_PlaceFixedById(map, 0x46);
        if (entity_group_state[23].death_kind == ENTITY_GROUP_DEATH_NONE) {
            entity = Entity_GetBySpawnIdForMap(map, 0x17);
            if (entity != NULL) {
                entity->flags |= ENTITY_FLAG_LIVE;
                entity->flags |= ENTITY_FLAG_RESPAWNABLE;
            }
        }
    } else {
        object = MapObject_GetById(map, 0xA);
        if (object != NULL) {
            MapObject_SetScripted(object, 0);
            object->state_flags |= MAP_OBJECT_ON;
        }
        object = MapObject_GetById(map, 0xB);
        if (object != NULL) {
            MapObject_SetScripted(object, 0);
            object->state_flags |= MAP_OBJECT_ON;
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

    if (forge_door_open[0] != 0) {
        object = MapObject_GetById(map, 0x65);
        if (object != NULL) {
            object->state_flags |= MAP_OBJECT_ON;
        }
    }
}

/* The same for the Forge as map 018: door 0x65 starts open once the story flag is set. */
void MapSetup_Forge018(int map) {
    MapObject *object;

    if (forge_door_open[0] != 0) {
        object = MapObject_GetById(map, 0x65);
        if (object != NULL) {
            object->state_flags |= MAP_OBJECT_ON;
        }
    }
}

/* The same for the Forge as map 019: door 0x65 starts open once the story flag is set. */
void MapSetup_Forge019(int map) {
    MapObject *object;

    if (forge_door_open[0] != 0) {
        object = MapObject_GetById(map, 0x65);
        if (object != NULL) {
            object->state_flags |= MAP_OBJECT_ON;
        }
    }
}

static inline void hide_found_record(MapRecordBounds *record) {
    if (record != NULL) {
        record->record_flags |= MAP_RECORD_HIDDEN;
    }
}

static inline void show_found_record(MapRecordBounds *record) {
    if (record != NULL) {
        record->record_flags &= ~MAP_RECORD_HIDDEN;
    }
}

/* Set-up of the Earth Folks' Village (map 020): shows the map records that go with the story flag (0, 1 or
   more) and, past 0, changes fog block 2 and light block 4. */
void MapSetup_EarthFolksVillage(int map) {
    void *target; // match: one variable for every record and then the map slot; it gives the slot the records' register
    MapFogBlockValues *block;
    MapSlotUnk10 *light;

    switch (forge_stage[0]) {
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
    block = (MapFogBlockValues *)((MapSlot *)target)->fog_blocks;
    light = ((MapSlot *)target)->light_blocks;
    block[2].always_zero_a = block[2].always_zero_b = 0.0f;
    block[2].fog_base_value = -43.40425f;
    block[2].fog_inverse_depth_scale = 238723.4f;
    block[2].fog_colour = 0;
    light[4].light1_direction_y = 0.0f;
    light[4].light1_direction_z = 0.0f;
    light[4].light1_direction_x = -1.57f;
    light[4].light1_red = 130.0f / 255.0f;
    light[4].light1_green = 35.0f / 255.0f;
    light[4].light1_blue = 0.0f;
    light[4].ambient_red = 170.0f / 255.0f;
    light[4].ambient_green = 98.0f / 255.0f;
    light[4].ambient_blue = 70.0f / 255.0f;
}

/* Set-up of the Cavern of Lava (map 024): at story flag 0 a light grey fog in block 5, otherwise a thin black one,
   two light colours and all three light directions cleared in light block 9. */
void MapSetup_CavernOfLava(int map) {
    MapSlot *slot = &map_slots[map];
    MapFogBlockValues *fog;
    MapLightBlockLights *lights;

    if (forge_stage[0] == 0) {
        ((MapFogBlockValues *)slot->fog_blocks)[5].always_zero_a = ((MapFogBlockValues *)slot->fog_blocks)[5].always_zero_b = 0.0f;
        ((MapFogBlockValues *)slot->fog_blocks)[5].fog_base_value = -28.333328f;
        ((MapFogBlockValues *)slot->fog_blocks)[5].fog_inverse_depth_scale = 113333.336f;
        ((MapFogBlockValues *)slot->fog_blocks)[5].fog_colour = 0x8C8C8C;
    } else {
        fog = (MapFogBlockValues *)slot->fog_blocks;
        lights = slot->light_blocks;
        fog[5].always_zero_a = fog[5].always_zero_b = 0.0f;
        fog[5].fog_base_value = -318.75f;
        fog[5].fog_inverse_depth_scale = 1.1475e+06f;
        fog[5].fog_colour = 0;
        lights[9].colors[3].x = 169.0f / 255.0f;
        lights[9].colors[3].y = 86.0f / 255.0f;
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
        record->record_flags |= MAP_RECORD_HIDDEN;
    }
}

static inline void show_record(int map, int record_id) {
    MapRecordBounds *record = Map_GetRecordById(map, record_id);

    if (record != NULL) {
        record->record_flags &= ~MAP_RECORD_HIDDEN;
    }
}

/* Set-up of the Fire Lord's Domain (map 025): shows the map records that go with the story flag; at 2 also
   moves object 0x66 and starts effect 0x1DF. */
void MapSetup_FireLordsDomain(int map) {
    MapObject *object;
    f32 position[4];
    f32 rotation[4];

    if (forge_stage[0] != 2) {
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

/* Set-up of the Ocean Shore (map 027): once the Icon of Fire is on its altar (ocean_shore_icon_of_fire_placed
   1), the fog the map script fades to when it is placed, in blocks 0, 1 and 4. */
void MapSetup_OceanShore(int map) {
    MapSlot *slot = &map_slots[map];
    MapFogBlockValues *fog;

    if (ocean_shore_icon_of_fire_placed[0] == 1) {
        fog = (MapFogBlockValues *)slot->fog_blocks;
        fog[0].always_zero_a = fog[0].always_zero_b = 0.0f;
        fog[0].fog_base_value = -9.705887f;
        fog[0].fog_inverse_depth_scale = 476470.62f;
        fog[0].fog_colour = 0;
        fog[1].always_zero_a = fog[1].always_zero_b = 0.0f;
        fog[1].fog_base_value = 2.0588226f;
        fog[1].fog_inverse_depth_scale = 455294.1f;
        fog[1].fog_colour = 0;
        fog[4].always_zero_a = fog[4].always_zero_b = 0.0f;
        fog[4].fog_base_value = -9.705887f;
        fog[4].fog_inverse_depth_scale = 476470.62f;
        fog[4].fog_colour = 0;
    }
}

/* Every-frame function of the Ocean Shore: empty. */
void MapSetup_OceanShoreFrame(int map) {
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

/* Set-up of the Guardian's Room (map 035): once guardians_room_water_purified is set, swaps map records 8-12
   for 0x29-0x2D (the map script makes props 8-12 non-solid and 41-45 solid at the same time). */
void MapSetup_GuardiansRoom(int map) {
    if (guardians_room_water_purified[0] != 0) {
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

/* Set-up of the Dark Sanctuary (map 038): once dark_sanctuary_fog_lifted is 1, the fog the map script draws
   back to, in blocks 1 and 2. */
void MapSetup_DarkSanctuary(int map) {
    MapSlot *slot = &map_slots[map];

    if (dark_sanctuary_fog_lifted[0] == 1) {
        MapFogBlockValues *fog = (MapFogBlockValues *)slot->fog_blocks;

        fog[1].always_zero_a = fog[1].always_zero_b = 0.0f;
        fog[1].fog_base_value = 46.666664f;
        fog[1].fog_inverse_depth_scale = 166666.67f;
        fog[1].fog_colour = 0;
        fog[2].always_zero_a = fog[2].always_zero_b = 0.0f;
        fog[2].fog_base_value = 1.6666718f;
        fog[2].fog_inverse_depth_scale = 405333.34f;
        fog[2].fog_colour = 0;
    }
}

/* Set-up of the King's Throne, Alcoves (map 041): once its story flag is 1, new fog in blocks 2 and 3 and a dim light colour
   in light block 1. */
void MapSetup_KingsThroneAlcoves(int map) {
    MapSlot *slot = &map_slots[map];

    if (kings_throne_alcoves_story_flag[0] == 1) {
        MapFogBlockValues *fog = (MapFogBlockValues *)slot->fog_blocks;
        MapSlotUnk10 *light = slot->light_blocks;

        fog[2].always_zero_a = fog[2].always_zero_b = 0.0f;
        fog[2].fog_base_value = -127.5f;
        fog[2].fog_inverse_depth_scale = 382500.0f;
        fog[2].fog_colour = 0;
        fog[3].always_zero_a = fog[3].always_zero_b = 0.0f;
        fog[3].fog_base_value = -127.5f;
        fog[3].fog_inverse_depth_scale = 382500.0f;
        fog[3].fog_colour = 0;
        light[1].ambient_red = 76.0f / 255.0f;
        light[1].ambient_green = 76.0f / 255.0f;
        light[1].ambient_blue = 79.0f / 255.0f;
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
    if (entity_group_state[13].death_kind == ENTITY_GROUP_DEATH_NONE) {
        if (entity_group_state[13].unk05 != 0 && entity_group_state[13].unk06 == 0) {
            entity_group_state[13].unk06 = 1;
        }
        if (entity_group_state[13].unk06 == 1) {
            if (map_script_flags[0x1905] == 0) {
                map_script_flags[0x1905] = 1;
                entity_group_state[13].unk05 = 0x11;
            }
            return;
        }
    }
    if (entity != NULL) {
        ENTITY_FLAG_BITS(entity)->live = 0;
        ENTITY_FLAG_BITS(entity)->respawnable = 0;
    }
}

/* Set-up of the Craftsman's Room (map 104): until craftsmans_room_door_unlocked is set, takes map objects 0x77
   and 0x76 out of use, giving 0x77 (when it is a light, kind 9) the colour 0xFF, 0x80, 0x18 in its source (unk4A
   0); the map script turns 0x77 on when it sets the flag. */
void MapSetup_CraftsmansRoom(int map) {
    MapObject *object;

    if (craftsmans_room_door_unlocked[0] == 0) {
        object = MapObject_GetById(map, 0x77);
        if (object != NULL) {
            MapObject_Deactivate(object);
            if (object->model != NULL && object->definition->kind == MAP_OBJECT_KIND_LIGHT) {
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
void MapSetup_Map252(int map) {
}
