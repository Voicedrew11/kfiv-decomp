#include "kf_model.h"

int func_00192DA8(void *, int, int);

/* Switches the actor to table entry idx and resets the owner's channel state; returns 1 if unchanged. */
int func_00192C98(ChannelActor *actor, int idx, int param, u32 amount, int force) {
    u8 *table;
    u8 *levels;
    int entry, current;

    table = actor->table;
    levels = table + 0x60;
    current = levels[actor->cur_index];
    entry = levels[idx];
    if (!force && current == entry) {
        return 1;
    }
    actor->old_index = entry;
    actor->cur_index = idx;
    actor->owner->channel->word = *(u16 *)(table + (idx << 1) + 0x80);
    if (entry >= actor->owner->channel->limit) {
        entry = 0;
    }
    actor->owner->channel->index = entry;
    actor->owner->channel->param = param;
    actor->owner->channel->param_shifted = param << 8;
    if (actor->enabled != 0 && (actor->flags & 0x46) == 0) {
        actor->owner->channel->value = (float)(int)amount + (float)(int)amount * 0.75f;
    } else {
        actor->owner->channel->value = amount;
    }
    actor->owner->vec->x = actor->owner->vec->y = actor->owner->vec->z = 0.0f;
    return func_00192DA8(actor->owner, entry, param);
}
