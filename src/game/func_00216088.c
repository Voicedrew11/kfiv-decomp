#include "kf_script.h"
#include "kf_entity.h"

Entity *func_001267B0(int);

/* Script command: for each bit set in `mask`, copy that bit of `value` into one of the entity's flags. */
void func_00216088(ScriptSetFlagsCommand *command) {
    Entity *entity;

    entity = func_001267B0(command->target_id.value);
    if (entity == NULL) {
        command->active = 0;
        return;
    }
    if (command->mask.value & 1) {
        ENTITY_FLAG_BITS(entity)->bit1 = command->value.value & 1;
    }
    if (command->mask.value & 2) {
        ENTITY_FLAG_BITS(entity)->bits2 = (command->value.value & 2) ? 3 : 0;
    }
    if (command->mask.value & 0x4) {
        ENTITY_FLAG_BITS(entity)->bit5 = command->value.value >> 2;
    }
    if (command->mask.value & 0x8) {
        ENTITY_FLAG_BITS(entity)->bit6 = command->value.value >> 3;
    }
    if (command->mask.value & 0x10) {
        ENTITY_FLAG_BITS(entity)->bit7 = command->value.value >> 4;
    }
    if (command->mask.value & 0x20) {
        ENTITY_FLAG_BITS(entity)->bit18 = command->value.value >> 5;
    }
    if (command->mask.value & 0x40) {
        ENTITY_FLAG_BITS(entity)->bit22 = command->value.value >> 6;
    }
    if (command->mask.value & 0x80) {
        ENTITY_FLAG_BITS(entity)->bit23 = command->value.value >> 7;
    }
    if (command->mask.value & 0x100) {
        ENTITY_FLAG_BITS(entity)->bit24 = command->value.value >> 8;
    }
    if (command->mask.value & 0x200) {
        ENTITY_FLAG_BITS(entity)->bit25 = command->value.value >> 9;
    }
    if (command->mask.value & 0x400) {
        ENTITY_FLAG_BITS(entity)->bit26 = command->value.value >> 10;
    }
    if (command->mask.value & 0x800) {
        ENTITY_FLAG_BITS(entity)->bit27 = command->value.value >> 11;
    }
    if (command->mask.value & 0x1000) {
        ENTITY_FLAG_BITS(entity)->bit28 = command->value.value >> 12;
    }
    if (command->mask.value & 0x2000) {
        ENTITY_FLAG_BITS(entity)->bit14 = command->value.value >> 13;
    }
    if (command->mask.value & 0x4000) {
        ENTITY_FLAG_BITS(entity)->bit9 = command->value.value >> 14;
    }
    if (command->mask.value & 0x8000) {
        ENTITY_FLAG_BITS(entity)->bit29 = command->value.value >> 15;
    }
    command->active = 0;
}
