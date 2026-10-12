#include "kf_entity.h"
#include "kf_gs.h"

extern Entity *D_004467E0;
extern u32 D_00446ED4;

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0019AB60(ModelInstance *);
void func_0010B018(ModelInstance *, s32, s32);
void func_0010AEB8(void *);
void func_0010BA30(ModelInstance *);
void func_00133708(Entity *, s32);

/* Sets the colour the entity's model is drawn in, and its alpha from the entity's own. */
static inline void func_00125BD8_set_tint(Entity *entity, f32 red, f32 green, f32 blue) {
    ModelInstance *model = ENTITY_MODEL(entity);

    model->unk150 = red;
    model->unk154 = green;
    model->unk158 = blue;
    model->alpha = (f32)entity->unkD6;
}

/* Draws the entities of both maps, pool block by pool block: red while one is hit, pulsing in the colour of
   the timer that is running (unk20, unk24, unk28, unk2A), plain otherwise. */
void func_00125BD8(int list) {
    Entity *entity;
    s32 map;
    s32 block;
    s32 period;
    s32 half;
    s32 phase;
    s32 falling;

    period = 40;
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_ALPHA_2, 9);
    func_00109A80(GS_REG_TEST_1, 0x5000D);
    func_00109A80(GS_REG_TEST_2, 0x5000D);
    func_00109AD0();
    for (map = 0; map < 2; map++) {
        for (block = 0; block < 5; block++) {
            for (entity = D_004467E0; entity != NULL; entity = entity->next) {
                if (!ENTITY_FLAG_BITS(entity)->bit1) {
                    continue;
                }
                if (ENTITY_FLAG_BITS(entity)->bits2 >= 3) {
                    continue;
                }
                if (entity->group_id != map) {
                    continue;
                }
                if (entity->unkD3 != block) {
                    continue;
                }
                if (entity->unk14 == NULL) {
                    continue;
                }
                if (ENTITY_FLAG_BITS(entity)->bit30) {
                    continue;
                }
                if (entity->unkA3 != 0 && entity->unk82 == 0) {
                    ENTITY_MODEL(entity)->unk150 = 255.0f;
                    ENTITY_MODEL(entity)->unk154 = 64.0f;
                    ENTITY_MODEL(entity)->unk158 = 64.0f;
                    ENTITY_MODEL(entity)->alpha = (f32)entity->unkD6;
                } else if (ENTITY_FLAG_BITS(entity)->bits2 < 2) {
                    half = 20;
                    phase = D_00446ED4 % period;
                    falling = period - phase;
                    if (phase >= half) {
                        phase = falling;
                    }
                    if (entity->unk20 != 0 && entity->unk26 == 1) {
                        func_00125BD8_set_tint(entity, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half);
                    } else if (entity->unk24 != 0 && entity->unk26 == 2) {
                        func_00125BD8_set_tint(entity, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f);
                    } else if (entity->unk28 != 0 && entity->unk26 == 3) {
                        func_00125BD8_set_tint(entity, phase * 127.0f / half + 128.0f, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half);
                    } else if (entity->unk2A != 0 && entity->unk26 == 4) {
                        func_00125BD8_set_tint(entity, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f, phase * 127.0f / half + 128.0f);
                    } else if (entity->unk20 != 0) {
                        func_00125BD8_set_tint(entity, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half);
                    } else if (entity->unk24 != 0) {
                        func_00125BD8_set_tint(entity, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f);
                    } else if (entity->unk28 != 0) {
                        func_00125BD8_set_tint(entity, phase * 127.0f / half + 128.0f, phase * 127.0f / half + 128.0f, 128.0f - phase * 64.0f / half);
                    } else if (entity->unk2A != 0) {
                        func_00125BD8_set_tint(entity, 128.0f - phase * 64.0f / half, phase * 127.0f / half + 128.0f, phase * 127.0f / half + 128.0f);
                    } else {
                        func_00125BD8_set_tint(entity, 128.0f, 128.0f, 128.0f);
                    }
                }
                func_0019AB60(ENTITY_MODEL(entity));
                if (ENTITY_MODEL(entity)->alpha < 128.0f || (ENTITY_MODEL(entity)->unk100 & 0x200) || entity->spawn_info->type_id == 0x10) {
                    func_0010B018(ENTITY_MODEL(entity), 0, list);
                } else {
                    func_0010AEB8(ENTITY_MODEL(entity));
                    func_0010BA30(ENTITY_MODEL(entity));
                }
                if (entity->spawn_info->type_id == 0xF) {
                    func_00133708(entity, list);
                }
            }
        }
    }
}
