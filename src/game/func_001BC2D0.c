#include "kf_game.h"
#include "kf_map.h"
#include "kf_entity.h"
#include "kf_player.h"

extern GameStatus D_0038CC50;
extern TriggerTypeDef D_0041A6F8[];
extern u8 D_00369830[];
extern EventRunner D_00420D00;
extern PlayerState D_00413F80;
extern void *D_00414188[];
extern int D_00446770;

int func_00210C48(int);
int func_002109C8(int, int);
void func_001BC6E8(int);
void func_001BC968(int);
int func_0011D600(void *, int);
int func_001188E0(int);
void func_0011A438(int);
void func_001CFA70(int, int, int, int, int, int, ...);

/* Uses the item GameStatus.unk20 names once GameStatus.bit6 asks for it: an entity or a map object in front of
   the player may take it for a script of its own; otherwise it does what its type says and one is used up. */
void func_001BC2D0(void) {
    int item = D_0038CC50.unk20;
    TriggerTypeDef *def = &D_0041A6F8[item];
    Entity *entity;
    MapObject *object;
    MapObject *stood_on;
    u16 timed;

    if (D_0038CC50.unk44 > 0) {
        D_0038CC50.unk44--;
    }
    if (D_0038CC50.bit6) {
        D_0038CC50.bit6 = FALSE;
        D_0038CC50.unk44 = 1;
        if (D_00369830[item] != 0) {
            entity = D_00420D00.unk54;
            if (entity != NULL && entity->spawn_info->unk58 != 0 && entity->group_id == D_00446770) {
                if (entity->spawn_info->unk4E != 0 && func_00210C48(entity->spawn_info->unk4E) != 0) {
                    return;
                }
                func_002109C8(((Entity *)D_00420D00.unk54)->spawn_info->unk58, 0);
                return;
            }
            object = D_00420D00.current;
            if (object != NULL && object->unk08->unk58 != 0 && (MAP_OBJECT_BYTES(object)->unk00 & 1) == D_00446770) {
                if (object->unk08->unk40 != 0 && func_00210C48(object->unk08->unk40) != 0) {
                    return;
                }
                func_002109C8(((MapObject *)D_00420D00.current)->unk08->unk58, 0);
                return;
            }
            func_001BC6E8(item);
            switch (def->type) {
            case 0:
                D_00413F80.unkB4 += def->unk04;
                if (D_00413F80.unkB4 > D_00413F80.unk58) {
                    D_00413F80.unkB4 = D_00413F80.unk58;
                }
                D_00413F80.unkB8 += def->unk06;
                if (D_00413F80.unkB8 > D_00413F80.unk5C) {
                    D_00413F80.unkB8 = D_00413F80.unk5C;
                }
                timed = def->unk08;
                if (timed & 1) {
                    D_00413F80.timed_flags[0] = 0;
                    D_00413F80.flag_timers[0] = 60;
                }
                if (timed & 2) {
                    D_00413F80.timed_flags[1] = 0;
                    D_00413F80.flag_timers[1] = 60;
                }
                if (timed & 4) {
                    D_00413F80.timed_flags[2] = 0;
                    D_00413F80.flag_timers[2] = 60;
                }
                if (timed & 8) {
                    D_00413F80.timed_flags[3] = 0;
                    D_00413F80.flag_timers[3] = 60;
                }
                if (timed & 0x10) {
                    D_00413F80.timed_flags[4] = 0;
                    D_00413F80.flag_timers[4] = 60;
                }
                if (timed & 0x20) {
                    D_00413F80.timed_flags[5] = 0;
                    D_00413F80.flag_timers[5] = 60;
                }
                break;
            case 1:
            case 4:
            case 5:
                if (D_00420D00.current == NULL || !func_0011D600(D_00420D00.current, item)) {
                    func_001CFA70(0, 0, 0, 1, 0x2D, 0xA);
                    return;
                }
                break;
            case 2:
                stood_on = D_00414188[0];
                if (stood_on == NULL || stood_on->unk04->kind != 12 || !func_001188E0(item)) {
                    func_001CFA70(0, 0, 0, 1, 0x2D, 0xA);
                    return;
                }
                break;
            case 3:
                return;
            case 6:
                func_0011A438(item);
                break;
            case 7:
                break;
            case 8:
                func_001CFA70(0, 0, 0, 1, 0x2D, 0xA);
                return;
            default:
                return;
            }
            func_001BC968(item);
            switch (def->use_mode) {
            case 0:
                D_00369830[item]--;
                break;
            case 1:
                break;
            case 2:
                D_00369830[item]--;
                D_00369830[def->unk02]++;
                break;
            }
        }
    }
}
