#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_0018DB58(void *, int, int);

void func_00140280(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (unkE0->unk7A != 0) {
        func_00192C98(entity, 2, 0, entity->params->unkC4, 0);
        if (unkE0->unk68 <= 0) {
            func_0018DB58(entity, 0, 0);
            unkE0->unk68 = entity->params->unk438;
        }
    } else {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
        unkE0->unk68 = 0;
    }
    unkE0->unk68--;
}
