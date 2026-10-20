#include "item/gold.h"
#include "collision/collision.h"
#include "core/game.h"
#include "map_object/map_object.h"
#include "model/model.h"
#include "player/player.h"
#include "gfx/screen.h"
#include "ui/message.h"
#include "gfx/gs.h"

void *memset(void *, int, u32);
int File_Open(char *, int, int);
int File_Read(int, void *, int);
int File_Close(int);
void Model_SetUpSkinned(ModelInstance *, int *, int *);
void Gold_PrepareFadedDraw(OverlaySprite *);
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int, s64);
void Packet_EndGsRegisters(void);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void ModelTexture_SendForModel(void *);
void Model_DrawSkinned(ModelInstance *);
void DrawList_Add(ModelInstance *, s32, s32);
int Collision_FindFloorBelow(f32, void *);
void Vector_Copy(void *, void *);
void Vector_Add(void *, void *, void *);
int rand(void);
float cosf(float);
float sinf(float);

void Vector_Subtract(void *, void *, void *);
extern MapDrawState map_draw_state;
extern MapSlot map_slots[];
extern ObjectMarkRow map_object_marks[];
extern MapLoadState map_load_state;
extern GameStatus game_status;
extern f32 identity_matrix[16];
extern u16 pad_pressed[];
extern PlayerState player_state;
extern u16 action_button;
extern int current_map_slot;
extern f32 gold_drop_unmeasured_distance[];

MapActor *Gold_FindFree(int slot);
MapActor *Gold_FindOldest(int slot);
void Gold_LinkInUse(MapActor *actor);
MapActor *Gold_UnlinkFromList(MapActor *actor);
void Gold_SaveMark(MapActor *actor);
void Gold_ClearMark(MapActor *actor);

#define GOLD_MAX 999999

extern MapActor *map_actors_in_use;
extern s32 map_actor_drop_counts[2];
static MapActor map_actors[2][MAP_ACTOR_COUNT]; /* 0x5B6100 sixteen piles per loaded map */
static ModelInstance map_actor_models[2][MAP_ACTOR_COUNT]; /* 0x5B6680 */
static char map_gold_texture[0x2000];           /* 0x5B9E80 item/i0000.tx2 */
static char map_gold_model[0x2000];             /* 0x5BBE80 item/i0000.om2 */
static char map_actor_matrices[2][MAP_ACTOR_COUNT][0x80]; /* 0x5BDE80 */

/* Clears the map actors and loads their model (item/i0000.om2) and texture (item/i0000.tx2). */
void Gold_Init(void) {
    int file;

    map_actors_in_use = NULL;
    memset(map_actor_drop_counts, 0, sizeof(map_actor_drop_counts));
    memset(map_actors, 0, sizeof(map_actors));
    memset(map_actor_models, 0, sizeof(map_actor_models));
    memset(map_gold_texture, 0, sizeof(map_gold_texture));
    memset(map_gold_model, 0, sizeof(map_gold_model));
    memset(map_actor_matrices, 0, sizeof(map_actor_matrices));
    file = File_Open("item/i0000.om2", 0, 1);
    File_Read(file, map_gold_model, -1);
    File_Close(file);
    file = File_Open("item/i0000.tx2", 0, 1);
    File_Read(file, map_gold_texture, -1);
    File_Close(file);
}

static inline void copy_matrix(ModelMatrixQwords *target, ModelMatrixQwords *origin) {
    Uint128 row0 = origin->rows[0];
    Uint128 row1 = origin->rows[1];
    Uint128 row2 = origin->rows[2];
    Uint128 row3 = origin->rows[3];

    target->rows[0] = row0;
    target->rows[1] = row1;
    target->rows[2] = row2;
    target->rows[3] = row3;
}

/* Sets up a map slot's gold: gives each of the 16 map actors its model, then drops again what the map's marks
   say was lying there when it was left, and clears the marks. */
void Gold_InitMap(int map) {
    f32 position[4];
    f32 rotation[4];
    ObjectMarkRow marks;
    int source; // match: two variables, an array of two would be padded to 16 bytes
    int work;
    MapActor *actor;
    ModelInstance *model;
    char *matrix;
    s32 i;
    s32 tag;

    memset(position, 0, sizeof(position));
    position[3] = 1.0f;
    memset(rotation, 0, sizeof(rotation));
    rotation[3] = 1.0f;
    map_actor_drop_counts[map] = 0;
    memset(map_actors[map], 0, sizeof(map_actors[0]));
    memset(map_actor_models[map], 0, sizeof(map_actor_models[0]));
    memset(map_actor_matrices[map], 0, sizeof(map_actor_matrices[0]));
    actor = map_actors[map];
    model = map_actor_models[map];
    matrix = map_actor_matrices[map][0];
    for (i = 0; i < MAP_ACTOR_COUNT; i++, actor++, model++, matrix += 0x80) {
        actor->body = model;
        work = (int)matrix;
        source = (int)map_gold_model;
        Model_SetUpSkinned(model, &source, &work);
        copy_matrix((ModelMatrixQwords *)matrix, (ModelMatrixQwords *)identity_matrix);
        model->light_block = (s32)&map_draw_state.current_light_block;
        model->fog_block = &map_draw_state.current_fog_block;
        model->texture_data = (int)map_gold_texture;
        model->texture_slot = 2;
        model->draw = Gold_PrepareFadedDraw;
    }
    tag = map_slots[map].tag; // match: kept in a variable, retail does not read it again after the copy
    marks = map_object_marks[tag];
    memset(&map_object_marks[tag], 0, sizeof(ObjectMarkRow));
    for (i = 0; i < MAP_ACTOR_COUNT; i++) {
        if (marks.marks[i].amount != 0) {
            position[0] = marks.marks[i].x;
            position[1] = marks.marks[i].y;
            position[2] = marks.marks[i].z;
            rotation[1] = marks.marks[i].radius * 0.0625f;
            Gold_Drop((Vector4 *)position, (Vector4 *)rotation, map, marks.marks[i].amount, marks.marks[i].object_id,
                         FALSE, FALSE);
        }
    }
}

/* Takes every map actor of the slot off the list of the ones in use. */
void Gold_UnlinkMap(int slot) {
    MapActor *actor = map_actors_in_use;

    while (actor != NULL) {
        if (((actor->flags >> 2) & 1) == slot) {
            actor = Gold_UnlinkFromList(actor);
        } else {
            actor = actor->next;
        }
    }
}

/* Moves every map actor of the slot that is in use by offset (the maps' origins moved). */
void Gold_MoveMap(int slot, f32 *offset) {
    MapActor *actor = map_actors[slot];
    int i;

    for (i = MAP_ACTOR_COUNT - 1; i >= 0; i--) {
        if (actor->flags & 1) {
            Vector_Add(&actor->body->position, &actor->body->position, offset);
            actor->position_y += offset[1];
        }
        actor++;
    }
}

/* Steps the gold lying about: fades it in and out, finds the one nearest the player among those in reach and,
   when the button is pressed, gives its gold to the player. */
void Gold_Update(void) {
    f32 delta[4];
    MapActor *nearest = NULL;
    MapActor *actor = map_actors_in_use;
    f32 nearest_distance = 40000.0f;
    f32 distance;
    int amount;

    while (actor != NULL) {
        switch (actor->step) {
        case 0:
            actor->fade_timer--;
            if (actor->fade_timer != 0) {
                actor->body->fade_alpha = ((15 - actor->fade_timer) << 7) / 15;
                MAP_ACTOR_BITS(actor)->in_reach = actor->body->fade_alpha > 0x10;
            } else {
                actor->body->fade_alpha = 0x80;
                actor->step = 1;
            }
            break;
        case 1:
            break;
        case 2:
            actor->fade_timer--;
            if (actor->fade_timer != 0) {
                actor->body->fade_alpha = (actor->fade_timer << 7) / 15;
            } else {
                if (MAP_ACTOR_WORDS(actor)->object_id != 0) {
                    MapObject_MarkEmptiedById(MAP_ACTOR_BITS(actor)->map, MAP_ACTOR_WORDS(actor)->object_id);
                }
                Gold_ClearMark(actor);
                actor = Gold_UnlinkFromList(actor);
                continue;
            }
            break;
        }
        actor->body->fade_enabled = actor->body->fade_alpha < 0x80;
        if (actor->flags & MAP_ACTOR_FLAG_IN_REACH) {
            Vector_Subtract(delta, &actor->body->position, &player_state);
            distance = delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2];
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = actor;
            }
        }
        actor = actor->next;
    }
    if (nearest != NULL && (pad_pressed[0] & action_button) && !(game_status.update_hold_flags & 1) && game_status.player_control != 0) {
        MAP_ACTOR_BITS(nearest)->in_reach = FALSE;
        nearest->step = 2;
        nearest->fade_timer = 15;
        amount = MAP_ACTOR_WORDS(nearest)->amount;
        player_state.gold += amount;
        if (player_state.gold >= GOLD_MAX) {
            player_state.gold = GOLD_MAX;
        }
        Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_GOLD, amount);
    }
}

/* Draws the map actors in use: solid ones at once, fading ones through the draw list given. */
void Gold_Draw(s32 list) {
    MapActor *actor;

    Packet_BeginGsRegisters();
    Packet_AddGsRegister(0x42, 0x44);
    Packet_AddGsRegister(0x43, 9);
    Packet_AddGsRegister(0x47, 0x5000D);
    Packet_AddGsRegister(0x48, 0x5000D);
    Packet_EndGsRegisters();
    for (actor = map_actors_in_use; actor != NULL; actor = actor->next) {
        ModelMatrix_Build(actor->body, &actor->body->position, &actor->body->rotation, &actor->body->scale_x);
        if (actor->body->fade_enabled == 0) {
            ModelTexture_SendForModel(actor->body);
            Model_DrawSkinned(actor->body);
        } else {
            DrawList_Add(actor->body, 0, list);
        }
    }
}

/* Drops gold in a map slot: takes a free map actor, or the oldest one, and puts it at position. */
int Gold_Drop(Vector4 *position, Vector4 *rotation, int map, int amount, int object_id, int on_ground,
                 int other_map) {
    MapActor *actor;

    if (amount < 0) {
        return FALSE;
    }
    if (amount > 0xFFFF) {
        amount = 0xFFFF;
    }
    if (on_ground && !Collision_FindFloorBelow(1000.0f, position)) {
        return FALSE;
    }
    actor = Gold_FindFree(map);
    if (actor == NULL) {
        actor = Gold_FindOldest(map);
        if (actor == NULL) {
            return FALSE;
        }
        Gold_ClearMark(actor);
        Gold_UnlinkFromList(actor);
    }
    MAP_ACTOR_BITS(actor)->in_use = TRUE;
    MAP_ACTOR_BITS(actor)->map = map;
    MAP_ACTOR_BITS(actor)->in_reach = FALSE;
    MAP_ACTOR_WORDS(actor)->object_id = object_id;
    actor->serial = map_actor_drop_counts[map]++;
    actor->drop_unmeasured_distance = gold_drop_unmeasured_distance[0];
    actor->fade_timer = 15;
    actor->step = 0;
    // match: read even when on_ground is not set and no test was made: then it is whatever the last test left
    actor->position_y = *&COLLISION_RESULT->point[1];
    MAP_ACTOR_WORDS(actor)->amount = amount;
    Vector_Copy(&actor->body->position, position);
    Vector_Copy(&actor->body->rotation, rotation);
    actor->body->scale_z = 1.5f; // match: z, y, x, w in this order gives the stores in retail order
    actor->body->scale_y = 1.5f;
    actor->body->scale_x = 1.5f;
    actor->body->scale_w = 1.0f;
    if (on_ground) {
        actor->body->position.y = actor->position_y;
    }
    Gold_LinkInUse(actor);
    if (map == current_map_slot) {
        other_map = FALSE;
    }
    if (other_map) {
        Vector_Add(&actor->body->position, &actor->body->position, &map_load_state);
    }
    Gold_SaveMark(actor);
    if (other_map) {
        Vector_Subtract(&actor->body->position, &actor->body->position, &map_load_state);
    }
    return TRUE;
}

/* Drops the gold of an entity near it: half of EntityParams.gold for sure, the rest by chance. */
int Gold_DropFromEntity(Entity *entity) {
    Vector4 position;
    Vector4 rotation;
    u16 amount;
    s32 whole;
    s32 half;
    s32 sure;
    f32 distance;
    f32 angle;

    if (entity->params->gold == 0) {
        return; // match: no value, though the function returns what Gold_Drop does (retail keeps $2 live here)
    }
    amount = 1;
    if (entity->params->gold >= 2) {
        whole = entity->params->gold;
        half = whole / 2;
        sure = whole - half;
        amount = (s32)((half + 1) * (f32)rand() * 4.656613e-10f) + sure;
    }
    distance = entity->radius * rand() * 4.656613e-10f;
    angle = rand() * 6.2831855f * 4.656613e-10f - 3.1415927f;
    position.x = entity->model->position[0] + cosf(angle) * distance;
    position.y = entity->model->position[1] - 20.0f;
    position.z = entity->model->position[2] + sinf(angle) * distance;
    rotation.x = 0.0f;
    rotation.y = rand() * 6.2831855f * 4.656613e-10f - 3.1415927f;
    rotation.z = 0.0f;
    return Gold_Drop(&position, &rotation, entity->group_id, amount, 0, TRUE, TRUE);
}

/* Returns the first map actor of the slot that is not in use, or NULL. */
MapActor *Gold_FindFree(int slot) {
    MapActor *actor = map_actors[slot];
    int i;

    for (i = 0; i < MAP_ACTOR_COUNT; i++) {
        if (!(actor->flags & 1)) {
            return actor;
        }
        actor++;
    }
    return NULL;
}

/* Returns the oldest map actor of the slot that no map object dropped, or NULL. */
MapActor *Gold_FindOldest(int slot) {
    MapActor *actor = map_actors[slot];
    MapActor *oldest = NULL;
    int i;

    for (i = MAP_ACTOR_COUNT - 1; i >= 0; i--) {
        if (MAP_ACTOR_WORDS(actor)->object_id == 0) {
            if (oldest == NULL) {
                oldest = actor;
            } else if (actor->serial < oldest->serial) {
                oldest = actor;
            }
        }
        actor++;
    }
    return oldest;
}

/* Marks a map actor as in use and pushes it onto the front of the in-use list. */
void Gold_LinkInUse(MapActor *actor) {
    MapActor *head = map_actors_in_use;

    actor->flags |= 1;
    actor->prev = NULL;
    actor->next = head;
    if (head != NULL) {
        head->prev = actor;
    }
    map_actors_in_use = actor;
}

/* Removes a map actor from the in-use list and clears its in-use flag; returns the next actor in the list. */
MapActor *Gold_UnlinkFromList(MapActor *actor) {
    MapActor *next = actor->next;
    MapActor *prev = actor->prev;

    *(s32 *)&actor->flags &= ~1; // match: cleared through a signed word
    if (prev != NULL) {
        prev->next = next;
    }
    if (actor->next != NULL) {
        actor->next->prev = actor->prev;
    }
    if (map_actors_in_use == actor) {
        map_actors_in_use = actor->next;
    }
    return next;
}

/* Records a dropped pile in the map's ObjectMark table so it can be restored when the map is loaded again. */
void Gold_SaveMark(MapActor *actor) {
    u32 map_slot;
    MapSlot *entry;
    ObjectMark *mark;

    map_slot = (actor->flags >> 2) & 1;
    entry = &map_slots[map_slot];
    mark = &map_object_marks[entry->tag].marks[((char *)actor - (char *)map_actors[map_slot]) / sizeof(MapActor)];
    memset(mark, 0, sizeof(ObjectMark));
    mark->x = actor->body->position.x;
    mark->y = actor->body->position.y;
    mark->z = actor->body->position.z;
    mark->radius = actor->body->rotation.y * 16.0f;
    mark->amount = MAP_ACTOR_WORDS(actor)->amount;
    mark->object_id = actor->object_id;
}

/* Clears the ObjectMark for a map actor slot. */
void Gold_ClearMark(MapActor *actor) {
    u32 map_slot;
    u32 slot;
    MapSlot *entry;
    ObjectMark *mark;

    map_slot = (actor->flags >> 2) & 1;
    entry = &map_slots[map_slot];
    slot = ((char *)actor - (char *)map_actors[map_slot]) / sizeof(MapActor);
    // match: retail adds the tag row and the slot before the table base
    mark = (ObjectMark *)((u8 *)map_object_marks + (entry->tag * sizeof(map_object_marks[0]) + slot * sizeof(ObjectMark)));
    memset(mark, 0, sizeof(ObjectMark));
}

/* Gold pile model's ModelInstance.draw, which DrawList_Draw runs before the model is drawn: sets the blend to
   its fade alpha (unk146) and the tests, then sends its textures. */
void Gold_PrepareFadedDraw(OverlaySprite *sprite) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, ((s64)sprite->fade_alpha << 32) | 0x64);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x51001);
    Packet_AddGsRegister(0x48, 0x30001);
    Packet_EndGsRegisters();
    ModelTexture_SendForModel(sprite);
}
