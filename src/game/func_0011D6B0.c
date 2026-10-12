#include "kf_entity.h"
#include "kf_map.h"
#include "kf_gs.h"
#include "kf_player.h"

extern MapSlot D_0038CD20[];
extern MapDrawState D_0038A1B0;
extern Vector4 D_0038D9F0[4];
extern PlayerState D_00413F80;
extern s32 D_00447078;
extern s32 D_0044707C;
extern s32 D_00447080;
extern MapObject *D_00447088[2];
extern MapObject *D_00447090[2];
extern MapSlotUnk10Vectors D_00472A40;
extern MapSlotUnk10Vectors D_00472AC0[2][0x20];

void KF_Vector_Scale(float *, float *, float);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109EB8(void);
void func_0010AEB8(void *);
void func_0010B018(ModelInstance *, s32, s32);
void func_0019AC88(ModelInstance *, int);
void func_0019ADB8(ModelInstance *, int);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);

/* the flags of the map record an object stands in, and which of the two maps that record is of */
#define RECORD_FLAGS(object) (((MapDrawRecord *)(object)->unk0C)->flags)
#define RECORD_MAP(object) ((u8)(((MapRecord *)(object)->unk0C)->unk38 >> 27) & 1)

/* Draws the map objects of both maps (or of one, when only_one is set): first
   the jointed ones (D_00447090), then the plain ones (D_00447088). While the
   player wears item 0x9D the objects that allow it are lit with their light
   blocks blended towards D_0038D9F0 and back, over 210 frames (fully at frame 105). */
void func_0011D6B0(int mode, int only_one, int which) {
    f32 scaled[4];
    f32 blend;
    f32 rest;
    int map;
    int i;
    MapSlotUnk10Vectors *dest;
    MapSlotUnk10Vectors *source;
    MapObject *object;

    D_0044707C = D_00447078;
    D_00447078 = D_00413F80.equipment[7] == 0x9D;
    if (D_0044707C != D_00447078) {
        D_00447080 = 0;
    } else if (D_0044707C != 0) {
        D_00447080 = (D_00447080 + 1) % 210;
    }
    if (D_00447080 < 105) {
        blend = D_00447080 / 105.0f;
    } else {
        blend = (210 - D_00447080) / 105.0f;
    }
    rest = 1.0f - blend;
    for (map = 0; map < 2; map++) {
        if (D_0038CD20[map].unk00 != 0) {
            for (i = 0; i < 0x20; i++) {
                dest = &D_00472AC0[map][i];
                source = (MapSlotUnk10Vectors *)&D_0038A1B0.unkA90[map][i];
                func_00216778(&dest->unk00[0], &source->unk00[0]);
                func_00216778(&dest->unk00[1], &source->unk00[1]);
                func_00216778(&dest->unk00[2], &source->unk00[2]);
                func_00216778(&dest->unk00[3], &source->unk00[3]);
                KF_Vector_Scale(scaled, &D_0038D9F0[0].x, blend);
                KF_Vector_Scale(&dest->unk40[0].x, &source->unk40[0].x, rest);
                func_00216C48(&dest->unk40[0], &dest->unk40[0], scaled);
                KF_Vector_Scale(scaled, &D_0038D9F0[1].x, blend);
                KF_Vector_Scale(&dest->unk40[1].x, &source->unk40[1].x, rest);
                func_00216C48(&dest->unk40[1], &dest->unk40[1], scaled);
                KF_Vector_Scale(scaled, &D_0038D9F0[2].x, blend);
                KF_Vector_Scale(&dest->unk40[2].x, &source->unk40[2].x, rest);
                func_00216C48(&dest->unk40[2], &dest->unk40[2], scaled);
                KF_Vector_Scale(scaled, &D_0038D9F0[3].x, blend);
                KF_Vector_Scale(&dest->unk40[3].x, &source->unk40[3].x, rest);
                func_00216C48(&dest->unk40[3], &dest->unk40[3], scaled);
            }
        }
    }
    func_00216778(&D_00472A40.unk00[0], &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk00[0]);
    func_00216778(&D_00472A40.unk00[1], &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk00[1]);
    func_00216778(&D_00472A40.unk00[2], &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk00[2]);
    func_00216778(&D_00472A40.unk00[3], &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk00[3]);
    KF_Vector_Scale(scaled, &D_0038D9F0[0].x, blend);
    KF_Vector_Scale(&D_00472A40.unk40[0].x, &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk40[0].x, rest);
    func_00216C48(&D_00472A40.unk40[0], &D_00472A40.unk40[0], scaled);
    KF_Vector_Scale(scaled, &D_0038D9F0[1].x, blend);
    KF_Vector_Scale(&D_00472A40.unk40[1].x, &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk40[1].x, rest);
    func_00216C48(&D_00472A40.unk40[1], &D_00472A40.unk40[1], scaled);
    KF_Vector_Scale(scaled, &D_0038D9F0[2].x, blend);
    KF_Vector_Scale(&D_00472A40.unk40[2].x, &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk40[2].x, rest);
    func_00216C48(&D_00472A40.unk40[2], &D_00472A40.unk40[2], scaled);
    KF_Vector_Scale(scaled, &D_0038D9F0[3].x, blend);
    KF_Vector_Scale(&D_00472A40.unk40[3].x, &((MapSlotUnk10Vectors *)&D_0038A1B0.unkC0)->unk40[3].x, rest);
    func_00216C48(&D_00472A40.unk40[3], &D_00472A40.unk40[3], scaled);
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_ALPHA_2, 9);
    func_00109A80(GS_REG_TEST_1, 0x5000D);
    func_00109A80(GS_REG_TEST_2, 0x5000D);
    func_00109AD0();
    for (map = 0; map < 2; map++) {
        if (only_one != 0 && which != map) {
            continue;
        }
        for (object = D_00447090[map]; object != NULL; object = object->unk1C) {
            if (!(object->unk10 & 2)) {
                continue;
            }
            if (mode != 0 && !(RECORD_FLAGS(object) & 0x20)) {
                continue;
            }
            if (only_one != 0) {
                if (!(object->unk04->unk68 & 0x10000)) {
                    continue;
                }
            } else {
                if (object->unk04->unk68 & 0x10000) {
                    continue;
                }
            }
            if ((MAP_OBJECT_DEF_WORD(object) & 0x4000) && D_00447078 != 0) {
                if ((s8)object->unk08->unk38.bytes[5] != -1) {
                    object->model->unk148 = (s32)&D_00472AC0[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->unk08->unk38.bytes[5]];
                } else if (RECORD_FLAGS(object) & 0x10) {
                    object->model->unk148 = (s32)&D_00472A40;
                } else {
                    object->model->unk148 = (s32)&D_00472AC0[RECORD_MAP(object)][((MapDrawRecord *)object->unk0C)->unk35];
                }
            } else {
                if ((s8)object->unk08->unk38.bytes[5] != -1) {
                    object->model->unk148 = (s32)&D_0038A1B0.unkA90[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->unk08->unk38.bytes[5]];
                } else if (RECORD_FLAGS(object) & 0x10) {
                    object->model->unk148 = (s32)&D_0038A1B0.unkC0;
                } else {
                    object->model->unk148 = (s32)&D_0038A1B0.unkA90[RECORD_MAP(object)][((MapDrawRecord *)object->unk0C)->unk35];
                }
            }
            if ((s8)object->unk08->unk38.bytes[4] != -1) {
                object->model->unk14C = &D_0038A1B0.unk290[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->unk08->unk38.bytes[4]];
            } else if (RECORD_FLAGS(object) & 0x10) {
                object->model->unk14C = &D_0038A1B0.unkA0;
            } else {
                object->model->unk14C = &D_0038A1B0.unk290[RECORD_MAP(object)][((MapDrawRecord *)object->unk0C)->unk34];
            }
            if (object->unk04->kind == 9) {
                func_0019AC88(object->model, *(s16 *)object->work);
            } else if (object->unk08->unk5A != 0 && object->unk08->unk5A != 0xFF) {
                func_0019ADB8(object->model, object->unk08->unk5A);
            } else {
                func_0019ADB8(object->model, ((MapRecordInfo *)object->unk0C)->unk38);
            }
            if ((object->unk08->unk38.flags & 0x1000000000000L) || (object->model->alpha < 128.0f && !(object->unk04->unk68 & 0x10000))) {
                func_0010B018(object->model, 0, mode);
            } else {
                func_0010AEB8(object->model);
                object->model->unk1A8(object->model);
            }
        }
    }
    for (map = 0; map < 2; map++) {
        if (only_one != 0 && which != map) {
            continue;
        }
        for (object = D_00447088[map]; object != NULL; object = object->unk1C) {
            if (!(object->unk10 & 2)) {
                continue;
            }
            if (mode != 0 && !(RECORD_FLAGS(object) & 0x20)) {
                continue;
            }
            if (only_one != 0) {
                if (!(object->unk04->unk68 & 0x10000)) {
                    continue;
                }
            } else {
                if (object->unk04->unk68 & 0x10000) {
                    continue;
                }
            }
            if ((MAP_OBJECT_DEF_WORD(object) & 0x4000) && D_00447078 != 0) {
                if ((s8)object->unk08->unk38.bytes[5] != -1) {
                    object->model->unk148 = (s32)&D_00472AC0[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->unk08->unk38.bytes[5]];
                } else if (RECORD_FLAGS(object) & 0x10) {
                    object->model->unk148 = (s32)&D_00472A40;
                } else {
                    object->model->unk148 = (s32)&D_00472AC0[RECORD_MAP(object)][((MapDrawRecord *)object->unk0C)->unk35];
                }
            } else {
                if ((s8)object->unk08->unk38.bytes[5] != -1) {
                    object->model->unk148 = (s32)&D_0038A1B0.unkA90[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->unk08->unk38.bytes[5]];
                } else if (RECORD_FLAGS(object) & 0x10) {
                    object->model->unk148 = (s32)&D_0038A1B0.unkC0;
                } else {
                    object->model->unk148 = (s32)&D_0038A1B0.unkA90[RECORD_MAP(object)][((MapDrawRecord *)object->unk0C)->unk35];
                }
            }
            if ((s8)object->unk08->unk38.bytes[4] == -1) {
                if (RECORD_FLAGS(object) & 0x10) {
                    object->model->unk14C = &D_0038A1B0.unkA0;
                } else {
                    object->model->unk14C = &D_0038A1B0.unk290[RECORD_MAP(object)][((MapDrawRecord *)object->unk0C)->unk34];
                }
            } else {
                object->model->unk14C = &D_0038A1B0.unk290[MAP_OBJECT_BYTES(object)->unk00 & 1][(s8)object->unk08->unk38.bytes[4]];
            }
            if (object->unk04->kind == 9) {
                func_0019AC88(object->model, *(s16 *)object->work);
            } else if (object->unk08->unk5A != 0 && object->unk08->unk5A != 0xFF) {
                func_0019ADB8(object->model, object->unk08->unk5A);
            } else {
                func_0019ADB8(object->model, ((MapRecordInfo *)object->unk0C)->unk38);
            }
            if ((object->unk08->unk38.flags & 0x1000000000000L) || (object->model->unk145 != 0 && (s16)object->model->unk146 < 0x80 && !(object->unk04->unk68 & 0x10000))) {
                func_0010B018(object->model, 0, mode);
            } else {
                func_0010AEB8(object->model);
                object->model->unk1A8(object->model);
            }
        }
    }
    func_00109EB8();
}
