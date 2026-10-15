#include "kf_entity.h"

void func_00183FA8(Entity *, int);
void func_00183FC8(char *, int);
void func_001840B8(char *, int);
void func_001841B0(Entity *, int);
void func_001841E0(Entity *, int);
void func_00184240(char *, int);
void EntityAi_SetState(char *, u8);

void func_00183E98(Entity *entity, s32 unknown) {
    if (entity->unkC6 == 0) {
        switch (entity->state) {
        case 4:
            if (entity->params->flags & 0x80) {
                func_001840B8((char *)entity, unknown);
            } else {
                func_00183FC8((char *)entity, unknown);
            }
            break;
        case 5:
            func_001841B0(entity, unknown);
            if (entity->params->unk18 < entity->distance_to_player) {
                EntityAi_SetState((char *)entity, 6);
            }
            break;
        case 6:
            if (entity->params->flags & 0x80) {
                func_00184240((char *)entity, unknown);
            } else {
                func_001841E0(entity, unknown);
            }
            break;
        case 0:
        case 1:
        case 2:
        case 3:
        default:
            func_00183FA8(entity, unknown);
            break;
        }
    }
}
