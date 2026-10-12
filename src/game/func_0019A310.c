#include "kf_entity.h"

int func_0019A310(char *entity, void (*callback)(void), int run) {
    Entity *slot = (Entity *)entity;

    slot->unkE0.unk0C = callback;
    slot->unkCB = 0;
    if (run != 0) {
        if (callback != NULL) {
            callback();
        }
    }
    return 1;
}
