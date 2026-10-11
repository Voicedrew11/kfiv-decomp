#include "kf_entity.h"

void *memset(void *, int, u32);
extern int D_00446F14;
extern int D_00446F60;
extern int D_0044709C;
extern int D_004467E4;
extern int D_004467E8;
extern int D_004470AC;
extern int D_004470B4;
extern int D_004470B8;
extern int D_004470BC;
extern int D_004470C0;
extern char D_003BE530[];
extern int D_004470A0[2];

#define ENTITY_POOL_BLOCK_SIZE   0x80000
#define ENTITY_POOL_BLOCK_COUNT  5

/* one block of the entity pool */
typedef struct {
    char unk0;                  /* 0x0 */
    u16 unk2;        /* 0x2 */
    s16 unk4;                 /* 0x4 */
    char unk6;                  /* 0x6 */
    int unk8;                   /* 0x8 */
    int base_address;           /* 0xC */
} EntityPoolBlock;

extern EntityPoolBlock D_003D6DF0[ENTITY_POOL_BLOCK_COUNT];

/* Clear the entity pool and empty every entity list. */
void KF_Entity_ResetAllLists(void) {
    EntityPoolBlock *block;
    int block_address;
    int blocks_left;

    memset((void *)D_00446F60, 0, D_00446F14);

    memset(D_003D6DF0, 0, sizeof(D_003D6DF0));
    D_004470A8 = NULL;
    D_004467E8 = -1;
    D_004470B0 = NULL;
    D_004467E0 = NULL;
    D_004470AC = 0;
    D_004470B4 = 0;
    D_004467E4 = 0;
    memset(D_004470A0, 0, 8);
    D_004470B8 = 0;
    D_004470BC = 0;
    D_004470C0 = 0;
    memset(D_003BE530, 0, 0x8C0);
    block_address = D_00446F60;
    block = D_003D6DF0; blocks_left = ENTITY_POOL_BLOCK_COUNT - 1;
    do {
        block->base_address = block_address;
        blocks_left -= 1;
        block->unk0 = 0;
        block->unk2 = 0xFFFF;
        block_address += ENTITY_POOL_BLOCK_SIZE;
        block->unk4 = 0;
        block->unk6 = 0;
        block->unk8 = 0;
        block++;
    } while (blocks_left >= 0);
    D_0044709C = block_address;
}
