#include "kf_gold.h"
#include "kf_collision.h"
#include "kf_game.h"
#include "kf_map_object.h"
#include "kf_model.h"
#include "kf_player.h"
#include "kf_screen.h"
#include "kf_message.h"

void *memset(void *, int, u32);
int func_00102238(char *, int, int);
int func_001025C0(int, void *, int);
int func_001024B0(int);
void func_0010B758(ModelInstance *, int *, int *);
void func_001CB710(OverlaySprite *);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0020C300(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void func_0010AEB8(void *);
void func_0010BA30(ModelInstance *);
void func_0010B018(ModelInstance *, s32, s32);
int func_001C0E40(f32, void *);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
int rand(void);
float cosf(float);
float sinf(float);

void KF_Vector_Subtract(void *, void *, void *);
extern MapDrawState D_0038A1B0;
extern MapSlot D_0038CD20[];
extern ObjectMarkRow D_00362928[];
extern MapLoadState D_0038D610;
extern GameStatus D_0038CC50;
extern f32 D_00431AF0[16];
extern u16 D_00413DAA[];
extern PlayerState D_00413F80;
extern u16 D_00446940;
extern int D_00446770;
extern f32 D_004469DC[];

MapActor *KF_Gold_FindFree(int slot);
MapActor *KF_Gold_FindOldest(int slot);
void KF_Gold_LinkInUse(MapActor *actor);
MapActor *KF_Gold_UnlinkFromList(MapActor *actor);
void KF_Gold_SaveMark(MapActor *actor);
void KF_Gold_ClearMark(MapActor *actor);

#define GOLD_MAX 999999

extern MapActor *D_004469E0;
extern s32 D_004471B8[2];
static MapActor map_actors[2][MAP_ACTOR_COUNT]; /* 0x5B6100 sixteen piles per loaded map */
static ModelInstance map_actor_models[2][MAP_ACTOR_COUNT]; /* 0x5B6680 */
static char map_gold_texture[0x2000];           /* 0x5B9E80 item/i0000.tx2 */
static char map_gold_model[0x2000];             /* 0x5BBE80 item/i0000.om2 */
static char map_actor_matrices[2][MAP_ACTOR_COUNT][0x80]; /* 0x5BDE80 */

/* Clears the map actors and loads their model (item/i0000.om2) and texture (item/i0000.tx2). */
void KF_Gold_Init(void) {
    int file;

    D_004469E0 = NULL;
    memset(D_004471B8, 0, sizeof(D_004471B8));
    memset(map_actors, 0, sizeof(map_actors));
    memset(map_actor_models, 0, sizeof(map_actor_models));
    memset(map_gold_texture, 0, sizeof(map_gold_texture));
    memset(map_gold_model, 0, sizeof(map_gold_model));
    memset(map_actor_matrices, 0, sizeof(map_actor_matrices));
    file = func_00102238("item/i0000.om2", 0, 1);
    func_001025C0(file, map_gold_model, -1);
    func_001024B0(file);
    file = func_00102238("item/i0000.tx2", 0, 1);
    func_001025C0(file, map_gold_texture, -1);
    func_001024B0(file);
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
void KF_Gold_InitMap(int map) {
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
    D_004471B8[map] = 0;
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
        func_0010B758(model, &source, &work);
        copy_matrix((ModelMatrixQwords *)matrix, (ModelMatrixQwords *)D_00431AF0);
        model->unk148 = (s32)&D_0038A1B0.unkC0;
        model->unk14C = &D_0038A1B0.unkA0;
        model->unk194 = (int)map_gold_texture;
        model->unk190 = 2;
        model->draw = (void (*)(ModelInstance *))func_001CB710;
    }
    tag = D_0038CD20[map].tag; // match: kept in a variable, retail does not read it again after the copy
    marks = D_00362928[tag];
    memset(&D_00362928[tag], 0, sizeof(ObjectMarkRow));
    for (i = 0; i < MAP_ACTOR_COUNT; i++) {
        if (marks.marks[i].unk08 != 0) {
            position[0] = marks.marks[i].x;
            position[1] = marks.marks[i].y;
            position[2] = marks.marks[i].z;
            rotation[1] = marks.marks[i].radius * 0.0625f;
            KF_Gold_Drop((Vector4 *)position, (Vector4 *)rotation, map, marks.marks[i].unk08, marks.marks[i].unk07,
                         FALSE, FALSE);
        }
    }
}

/* Takes every map actor of the slot off the list of the ones in use. */
void KF_Gold_UnlinkMap(int slot) {
    MapActor *actor = D_004469E0;

    while (actor != NULL) {
        if (((actor->flags >> 2) & 1) == slot) {
            actor = KF_Gold_UnlinkFromList(actor);
        } else {
            actor = actor->next;
        }
    }
}

/* Moves every map actor of the slot that is in use by offset (the maps' origins moved). */
void KF_Gold_MoveMap(int slot, f32 *offset) {
    MapActor *actor = map_actors[slot];
    int i;

    for (i = MAP_ACTOR_COUNT - 1; i >= 0; i--) {
        if (actor->flags & 1) {
            func_00216C48(&actor->body->position, &actor->body->position, offset);
            actor->unk14 += offset[1];
        }
        actor++;
    }
}

/* Steps the gold lying about: fades it in and out, finds the one nearest the player among those in reach and,
   when the button is pressed, gives its gold to the player. */
void KF_Gold_Update(void) {
    f32 delta[4];
    MapActor *nearest = NULL;
    MapActor *actor = D_004469E0;
    f32 nearest_distance = 40000.0f;
    f32 distance;
    int amount;

    while (actor != NULL) {
        switch (actor->step) {
        case 0:
            actor->fade_timer--;
            if (actor->fade_timer != 0) {
                actor->body->unk146 = ((15 - actor->fade_timer) << 7) / 15;
                MAP_ACTOR_BITS(actor)->in_reach = actor->body->unk146 > 0x10;
            } else {
                actor->body->unk146 = 0x80;
                actor->step = 1;
            }
            break;
        case 1:
            break;
        case 2:
            actor->fade_timer--;
            if (actor->fade_timer != 0) {
                actor->body->unk146 = (actor->fade_timer << 7) / 15;
            } else {
                if (MAP_ACTOR_WORDS(actor)->object_id != 0) {
                    KF_MapObject_MarkEmptiedById(MAP_ACTOR_BITS(actor)->map, MAP_ACTOR_WORDS(actor)->object_id);
                }
                KF_Gold_ClearMark(actor);
                actor = KF_Gold_UnlinkFromList(actor);
                continue;
            }
            break;
        }
        actor->body->unk145 = actor->body->unk146 < 0x80;
        if (actor->flags & 2) {
            KF_Vector_Subtract(delta, &actor->body->position, &D_00413F80);
            distance = delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2];
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = actor;
            }
        }
        actor = actor->next;
    }
    if (nearest != NULL && (D_00413DAA[0] & D_00446940) && !(D_0038CC50.unk30 & 1) && D_0038CC50.unk00 != 0) {
        MAP_ACTOR_BITS(nearest)->in_reach = FALSE;
        nearest->step = 2;
        nearest->fade_timer = 15;
        amount = MAP_ACTOR_WORDS(nearest)->amount;
        D_00413F80.unk4C += amount;
        if (D_00413F80.unk4C >= GOLD_MAX) {
            D_00413F80.unk4C = GOLD_MAX;
        }
        KF_Message_ShowNotice(0, 0, 0, 0, 0x2D, 0x14, amount);
    }
}

/* Draws the map actors in use: solid ones at once, fading ones through the draw list given. */
void KF_Gold_Draw(s32 list) {
    MapActor *actor;

    func_00109A50();
    func_00109A80(0x42, 0x44);
    func_00109A80(0x43, 9);
    func_00109A80(0x47, 0x5000D);
    func_00109A80(0x48, 0x5000D);
    func_00109AD0();
    for (actor = D_004469E0; actor != NULL; actor = actor->next) {
        func_0020C300(actor->body, &actor->body->position, &actor->body->rotation, &actor->body->unkE0);
        if (actor->body->unk145 == 0) {
            func_0010AEB8(actor->body);
            func_0010BA30(actor->body);
        } else {
            func_0010B018(actor->body, 0, list);
        }
    }
}

/* Drops gold in a map slot: takes a free map actor, or the oldest one, and puts it at position. */
int KF_Gold_Drop(Vector4 *position, Vector4 *rotation, int map, int amount, int object_id, int on_ground,
                 int other_map) {
    MapActor *actor;

    if (amount < 0) {
        return FALSE;
    }
    if (amount > 0xFFFF) {
        amount = 0xFFFF;
    }
    if (on_ground && !func_001C0E40(1000.0f, position)) {
        return FALSE;
    }
    actor = KF_Gold_FindFree(map);
    if (actor == NULL) {
        actor = KF_Gold_FindOldest(map);
        if (actor == NULL) {
            return FALSE;
        }
        KF_Gold_ClearMark(actor);
        KF_Gold_UnlinkFromList(actor);
    }
    MAP_ACTOR_BITS(actor)->in_use = TRUE;
    MAP_ACTOR_BITS(actor)->map = map;
    MAP_ACTOR_BITS(actor)->in_reach = FALSE;
    MAP_ACTOR_WORDS(actor)->object_id = object_id;
    actor->serial = D_004471B8[map]++;
    actor->unk08 = D_004469DC[0];
    actor->fade_timer = 15;
    actor->step = 0;
    // match: read even when on_ground is not set and no test was made: then it is whatever the last test left
    actor->unk14 = *&COLLISION_RESULT->point[1];
    MAP_ACTOR_WORDS(actor)->amount = amount;
    func_00216778(&actor->body->position, position);
    func_00216778(&actor->body->rotation, rotation);
    actor->body->unkE8 = 1.5f; // match: z, y, x, w in this order gives the stores in retail order
    actor->body->unkE4 = 1.5f;
    actor->body->unkE0 = 1.5f;
    actor->body->unkEC = 1.0f;
    if (on_ground) {
        actor->body->position.y = actor->unk14;
    }
    KF_Gold_LinkInUse(actor);
    if (map == D_00446770) {
        other_map = FALSE;
    }
    if (other_map) {
        func_00216C48(&actor->body->position, &actor->body->position, &D_0038D610);
    }
    KF_Gold_SaveMark(actor);
    if (other_map) {
        KF_Vector_Subtract(&actor->body->position, &actor->body->position, &D_0038D610);
    }
    return TRUE;
}

/* Drops the gold of an entity near it: half of EntityParams.gold for sure, the rest by chance. */
int KF_Gold_DropFromEntity(Entity *entity) {
    Vector4 position;
    Vector4 rotation;
    u16 amount;
    s32 whole;
    s32 half;
    s32 sure;
    f32 distance;
    f32 angle;

    if (entity->params->gold == 0) {
        return; // match: no value, though the function returns what KF_Gold_Drop does (retail keeps $2 live here)
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
    return KF_Gold_Drop(&position, &rotation, entity->group_id, amount, 0, TRUE, TRUE);
}

/* Returns the first map actor of the slot that is not in use, or NULL. */
MapActor *KF_Gold_FindFree(int slot) {
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
MapActor *KF_Gold_FindOldest(int slot) {
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
void KF_Gold_LinkInUse(MapActor *actor) {
    MapActor *head = D_004469E0;

    actor->flags |= 1;
    actor->prev = NULL;
    actor->next = head;
    if (head != NULL) {
        head->prev = actor;
    }
    D_004469E0 = actor;
}

/* Removes a map actor from the in-use list and clears its in-use flag; returns the next actor in the list. */
MapActor *KF_Gold_UnlinkFromList(MapActor *actor) {
    MapActor *next = actor->next;
    MapActor *prev = actor->prev;

    *(s32 *)&actor->flags &= ~1; // match: cleared through a signed word
    if (prev != NULL) {
        prev->next = next;
    }
    if (actor->next != NULL) {
        actor->next->prev = actor->prev;
    }
    if (D_004469E0 == actor) {
        D_004469E0 = actor->next;
    }
    return next;
}

/* Records a dropped pile in the map's ObjectMark table so it can be restored when the map is loaded again. */
void KF_Gold_SaveMark(MapActor *actor) {
    u32 map_slot;
    MapSlot *entry;
    ObjectMark *mark;

    map_slot = (actor->flags >> 2) & 1;
    entry = &D_0038CD20[map_slot];
    mark = &D_00362928[entry->tag].marks[((char *)actor - (char *)map_actors[map_slot]) / sizeof(MapActor)];
    memset(mark, 0, sizeof(ObjectMark));
    mark->x = actor->body->position.x;
    mark->y = actor->body->position.y;
    mark->z = actor->body->position.z;
    mark->radius = actor->body->rotation.y * 16.0f;
    mark->unk08 = MAP_ACTOR_WORDS(actor)->amount;
    mark->unk07 = actor->unk24;
}

/* Clears the ObjectMark for a map actor slot. */
void KF_Gold_ClearMark(MapActor *actor) {
    u32 map_slot;
    u32 slot;
    MapSlot *entry;
    ObjectMark *mark;

    map_slot = (actor->flags >> 2) & 1;
    entry = &D_0038CD20[map_slot];
    slot = ((char *)actor - (char *)map_actors[map_slot]) / sizeof(MapActor);
    // match: retail adds the tag row and the slot before the table base
    mark = (ObjectMark *)((u8 *)D_00362928 + (entry->tag * sizeof(D_00362928[0]) + slot * sizeof(ObjectMark)));
    memset(mark, 0, sizeof(ObjectMark));
}
