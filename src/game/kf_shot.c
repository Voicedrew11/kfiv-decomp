#include "kf_shot.h"

void *memset(void *, s32, u32);

extern s32 D_00446928;

#define SHOT_RING_SLOTS 0x100
#define SHOT_SLOT_SIZE 0x30

static s32 shot_ring_next;                                  /* 0x44710C next slot of shot_ring */
static s32 second_ring_next;                                /* 0x447110 next slot of second_ring */
static char shot_ring[SHOT_RING_SLOTS * SHOT_SLOT_SIZE];   /* 0x574348 the shots the player and entities launch */
static char second_ring[SHOT_RING_SLOTS * SHOT_SLOT_SIZE]; /* 0x577348 the same, nothing claims from it */

/* Empties both rings of shot slots. */
void Shot_ClearAll(void) {
    shot_ring_next = 0;
    memset(shot_ring, 0, sizeof(shot_ring));
    second_ring_next = 0;
    memset(second_ring, 0, sizeof(second_ring));
    D_00446928 = 3;
}

/* Claims the next shot slot, cleared; the oldest is reused once all are taken. */
void *Shot_Alloc(void) {
    void *slot = shot_ring + shot_ring_next * SHOT_SLOT_SIZE;

    memset(slot, 0, SHOT_SLOT_SIZE);
    shot_ring_next = (shot_ring_next + 1) % SHOT_RING_SLOTS;
    return slot;
}

/* Shot_Alloc for second_ring. */
void *Shot_AllocSecondRing(void) {
    void *slot = second_ring + second_ring_next * SHOT_SLOT_SIZE;

    memset(slot, 0, SHOT_SLOT_SIZE);
    second_ring_next = (second_ring_next + 1) % SHOT_RING_SLOTS;
    return slot;
}
