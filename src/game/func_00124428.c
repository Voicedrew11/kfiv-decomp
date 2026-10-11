#include "kf_entity.h"

extern char D_00249680[0x80000];
extern char *D_004467C0;

/* Copies the loaded type file into the slot's block; hangs if it is empty or too big for the block. */
void func_00124428(EntityPoolSlot *slot) {
    EntityController *controller;
    int *file;
    int table_offset;
    int size;

    controller = (EntityController *)(D_004467C0 + slot->head.fields.unk2 * 0x450);
    file = (int *)D_00249680;
    size = 0;
    // match: header words of the loaded file, no named struct yet
    if ((table_offset = file[4]) != 0 || (table_offset = file[3]) != 0) {
        size = table_offset + *(int *)(table_offset + (char *)file);
    }
    if (size == 0) {
        goto hang;
    }
    if (controller->flags & CONTROLLER_FLAG_TWO_BLOCKS) {
        if (size > 0x100000) {
            goto hang;
        }
    } else if (size > 0x80000) {
        goto hang;
    }
    memcpy((void *)slot->base_address, D_00249680, size);
    return;
hang:
    goto hang;
}
