#include "kf_entity.h"
#include "kf_sound.h"

extern char D_00249680[0x80000];
extern EntityTypeFileHeader D_00474B70;
extern SoundLoadQueue D_0038A130;
extern char *D_004467C0;
extern int D_004467C4;
extern char D_004467C8[];
extern EntityPoolSlot *D_00447098;
extern char D_0043E790[];

int sprintf(char *, char *, ...);
int func_00102790(int, int);
int func_001024B0(int);
void func_00102228(void);
int func_00102230(void);
int func_00102238(int, int, int);
int func_001025C0(int, void *, int);
void func_00102218(void);
void func_00124428(EntityPoolSlot *);
void func_001244C0(EntityPoolSlot *);
void func_00124B00(int);

/* Runs the entity pool's loads: steps the block being read or unpacked, ages the idle ones, and when
   nothing is busy starts reading the type file of the first queued block. */
void func_00123690(void) {
    char path[0x100];
    EntityPoolSlot *slot;
    EntityPoolSlot *queued;
    int busy;
    int i;

    queued = NULL;
    busy = FALSE;
    for (i = 0, slot = D_003D6DF0; i < ENTITY_POOL_SLOT_COUNT; i++, slot++) {
        switch (slot->head.fields.kind) {
        case ENTITY_POOL_FREE:
            break;
        case ENTITY_POOL_QUEUED:
            if (queued == NULL) {
                queued = slot;
            }
            break;
        case ENTITY_POOL_READING:
            if (func_00102790(D_004467C4, 1) == 0) {
                if (((EntityController *)(D_004467C0 + slot->head.fields.unk2 * 0x450))->flags & CONTROLLER_FLAG_TWO_BLOCKS) {
                    slot[1].head.fields.kind = ENTITY_POOL_EXTENSION;
                    slot[1].head.fields.unk2 = 0xFFFF;
                }
                func_001024B0(D_004467C4);
                func_00124428(slot);
                func_00124B00(i);
                D_00474B70 = *(EntityTypeFileHeader *)D_00249680;
                D_004467C4 = -1;
                if (D_00474B70.unk18 != 0 && D_00474B70.unk14 != 0) {
                    D_00474B70.unk18 += (int)D_00249680;
                    D_00474B70.unk14 += (int)D_00249680;
                    slot->head.fields.kind = ENTITY_POOL_UNPACKING;
                    D_0038A130.unk1C = (int)&D_00474B70;
                    D_0038A130.unk18 = 1;
                    D_0038A130.unk20 = i;
                    D_0038A130.unk24 = (int)slot;
                } else {
                    slot->head.fields.kind = ENTITY_POOL_READY;
                }
            }
            busy = TRUE;
            break;
        case ENTITY_POOL_UNPACKING:
            if (D_0038A130.unk18 == 0) {
                slot->head.fields.kind = ENTITY_POOL_READY;
                func_00102228();
                D_00447098 = NULL;
            }
            busy = TRUE;
            break;
        case ENTITY_POOL_READY:
            if (slot->unk4 > 0) {
                break;
            }
        case ENTITY_POOL_6:
            if (slot->unk6 != 0) {
                slot->unk6--;
            }
            break;
        case ENTITY_POOL_EXTENSION:
            break;
        }
    }
    if (queued == NULL) {
        return;
    }
    if (busy) {
        return;
    }
    if (D_0038A130.unk04 != 0) {
        return;
    }
    if (func_00102230() != 0) {
        return;
    }
    sprintf(path, D_0043E790, D_004467C8, ((EntityParams *)(D_004467C0 + queued->head.fields.unk2 * 0x450))->unk00_0);
    D_004467C4 = func_00102238((int)path, 1, 1);
    if (D_004467C4 < 0) {
        func_001244C0(queued);
        return;
    }
    queued->head.fields.kind = ENTITY_POOL_READING;
    func_001025C0(D_004467C4, D_00249680, -1);
    func_00102218();
    D_00447098 = queued;
}
