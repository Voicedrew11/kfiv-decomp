#include "kf_map.h"
#include "kf_game.h"
#include "kf_player.h"

void func_0011E678(int, int);
void func_001CB690(MapActor *);
void *func_001CB530(char *);
void KF_Vector_Subtract(void *, void *, void *);
void func_001CFA70(int, int, int, int, int, int, ...);
extern GameStatus D_0038CC50;
extern u16 D_00413DAA[];
extern PlayerState D_00413F80;
extern u16 D_00446940;
extern MapActor *D_004469E0;

#define GOLD_MAX 999999

/* Steps the gold lying about: fades it in and out, finds the one nearest the player among those in reach and,
   when the button is pressed, gives its gold to the player. */
void func_001CAD80(void) {
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
                    func_0011E678(MAP_ACTOR_BITS(actor)->map, MAP_ACTOR_WORDS(actor)->object_id);
                }
                func_001CB690(actor);
                actor = func_001CB530((char *)actor);
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
        func_001CFA70(0, 0, 0, 0, 0x2D, 0x14, amount);
    }
}
