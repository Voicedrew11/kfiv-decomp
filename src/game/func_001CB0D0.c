#include "kf_map.h"
#include "kf_collision.h"

int func_001C0E40(f32, void *);
MapActor *func_001CB460(int);
MapActor *func_001CB4A8(int);
void func_001CB690(MapActor *);
void *func_001CB530(char *);
void func_00216778(void *, void *);
void func_001CB508(MapActor *);
void func_00216C48(void *, void *, void *);
void func_001CB588(MapActor *);
void KF_Vector_Subtract(void *, void *, void *);
extern MapLoadState D_0038D610;
extern int D_00446770;
extern f32 D_004469DC[];
extern s32 D_004471B8[2];

/* Drops gold in a map slot: takes a free map actor, or the oldest one, and puts it at position. */
int func_001CB0D0(Vector4 *position, Vector4 *rotation, int map, int amount, int object_id, int on_ground, int other_map) {
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
    actor = func_001CB460(map);
    if (actor == NULL) {
        actor = func_001CB4A8(map);
        if (actor == NULL) {
            return FALSE;
        }
        func_001CB690(actor);
        func_001CB530((char *)actor);
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
    func_001CB508(actor);
    if (map == D_00446770) {
        other_map = FALSE;
    }
    if (other_map) {
        func_00216C48(&actor->body->position, &actor->body->position, &D_0038D610);
    }
    func_001CB588(actor);
    if (other_map) {
        KF_Vector_Subtract(&actor->body->position, &actor->body->position, &D_0038D610);
    }
    return TRUE;
}
