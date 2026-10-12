#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_001958B8(char *, void (*)(void), int);

void func_001807D0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityModelPart80 *part;
    s32 part_index;
    s32 first_part;
    s32 part_count;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            func_00192C98(entity, 0x12, 0, entity->params->unkC4 >> 1, 1);
            for (part_index = 0; part_index < 25; part_index++) {
                part = &((EntityModelPart80 *)entity->model->unkF8)[part_index + 8];
                ENTITY_MODEL_PART_FLAG_BITS(part)->bit40 = TRUE;
                ENTITY_MODEL_PART_FLAG_BITS(part)->bit41 = TRUE;
                unkE0->unk68 = 0;
                unkE0->unk6A = 0;
                unkE0->unk6C = 0;
            }
            switch (unkE0->unk7A) {
            case 0:
                first_part = 0x16;
                part_count = 10;
                unkE0->unk68 = entity->params->unk3D8[0].record.unk00;
                break;
            case 1:
                first_part = 0xB;
                part_count = 2;
                unkE0->unk6A = entity->params->unk240[1].unk00;
                break;
            case 2:
                first_part = 0xD;
                part_count = 9;
                unkE0->unk6C = (u8)entity->params->unk350[0];
                break;
            default:
                first_part = 8;
                part_count = 3;
                break;
            }
            for (part_index = 0; part_index < part_count; part_index++) {
                part = &((EntityModelPart80 *)entity->model->unkF8)[first_part + part_index];
                ENTITY_MODEL_PART_FLAG_BITS(part)->bit40 = FALSE;
                ENTITY_MODEL_PART_FLAG_BITS(part)->bit41 = FALSE;
            }
            entity->action_step++;
        }
        break;
    case 3:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 4:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk6E = entity->params->unk434;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
