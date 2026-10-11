#include "kf_entity.h"

extern u32 D_00446ED4;
extern s32 D_00446770;
extern PlayerData D_00413F80;
extern MapSlot D_0038CD20[];
extern KfVector D_004FF390[];
extern f32 D_00446824[];

void func_00196870(void *, int);
void func_0018B778(float, void *, void *, void *);
f32 func_0018A9F0(void *, void *);
void func_00196CC0(void *);
void func_0018E9C0(void *);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_001686B0(Entity *, s32);

void func_00168368(Entity *entity, s32 unknown) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    MapSlot *slot;
    KfVector *point;
    f32 height;
    f32 no_floor;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (D_00413F80.unk259 != 0) {
        unkE0->unk7C = 30;
    }
    if (unkE0->unk7C > 0) {
        unkE0->unk7C--;
    }
    if (entity->distance_to_player < entity->unk38 + 500.0f + 100.0f && unkE0->unk7A == 1 && unkE0->unk7B == 0) {
        unkE0->unk7A = 0;
    } else if (entity->unk38 + 1000.0f + 100.0f < entity->distance_to_player && unkE0->unk7A == 0) {
        unkE0->unk7A = 1;
    }
    if (unkE0->unk6A < 5) {
        func_0018B778(500.0f, &D_004FF390[unkE0->unk6A], &D_00413F80, &D_00413F80);
        slot = &D_0038CD20[D_00446770];
        height = func_0018A9F0(slot->unk14 + D_00413F80.unk252, &D_004FF390[unkE0->unk6A]);
        no_floor = D_00446824[0];
        if (height == no_floor) {
            height = func_0018A9F0(entity->unk14, &D_004FF390[unkE0->unk6A]);
            if (height != no_floor) {
                point = &D_004FF390[unkE0->unk6A];
                point->y += height;
                unkE0->unk6A++;
            }
        } else {
            point = &D_004FF390[unkE0->unk6A];
            point->y += height;
            unkE0->unk6A++;
        }
    }
    if (entity->unkC6 == 0) {
        func_00196CC0(entity);
        func_0018E9C0(entity);
        if ((s32)(entity->flags >> 53) & 1) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->unkCC) {
            case 0:
                func_00182FD8(entity, unknown);
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                func_001686B0(entity, unknown);
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
}
