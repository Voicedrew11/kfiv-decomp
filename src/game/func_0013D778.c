#include "common.h"
#include "kf_entity.h"

int rand(void);
void func_00190A30(char *);
void func_00195898(char *, u8);
void func_0019A310(char *, void (*)(void), int);
void func_0013D820(void);

void func_0013D778(Entity *entity) {
    EntityUnkE0 *sub = &entity->unkE0;
    int rnd = rand();

    sub->unk68 = ENTITY_CONTROLLER(entity)->unk430 / 2 + rnd % ENTITY_CONTROLLER(entity)->unk430;
    ENTITY_FLAG_BITS(entity)->bit20 = 1;
    ENTITY_FLAG_BITS(entity)->bit18 = 1;
    ENTITY_FLAG_BITS(entity)->bit21 = 1;
    func_00190A30((char *)entity);
    entity->target = NULL;
    func_0019A310((char *)entity, func_0013D820, 0);
    func_00195898((char *)entity, 2);
}
