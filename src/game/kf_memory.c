#include "kf_memory.h"
#include "kf_map.h"

#define MEMORY_ARENA_SIZE 0x1600000

/* the sizes of the areas, in bytes */
extern int D_00446F0C;
extern int D_00446F10;
extern int D_00446F14;
extern int D_00446F18;
extern int D_00446F1C;
extern int D_00446F20;
extern int D_00446F24;
extern int D_00446F28;
extern int D_00446F2C;

/* the areas, as Memory_PlaceAreas lays them out */
extern MapSlotMemory D_004321F0[2];
extern MapSlotMemory *D_00446F08;
extern char *D_00446F50;
extern char *D_00446F54;
extern char *D_00446F58;            /* the packet area */
extern char *D_00446F5C;
extern char *D_00446F60;
extern char *D_00446F64;
extern char *D_00446F68;
extern char *D_00446F6C;
extern char *D_00446F70;
extern char *D_00446F74;

static char memory_arena[MEMORY_ARENA_SIZE]; /* 0x5FAF80 every area lies in it */

/* Clears the arena, sets the size of each area and lays them out. */
void Memory_Init(void) {
    Memory_ClearWords(memory_arena, sizeof(memory_arena) / 4);
    D_00446F0C = 0x280000;
    D_00446F10 = 0x1D0000;
    D_00446F18 = 0xA0000;
    D_00446F1C = 0x200000;
    D_00446F20 = 0xA80000;
    D_00446F14 = 0x300000;
    D_00446F24 = 0x20000;
    D_00446F28 = 0x80000;
    D_00446F2C = 0x60000;
    Memory_PlaceAreas();
}

/* Clears byte_count bytes, eight at a time. */
void Memory_Clear64(char *dest, u32 byte_count) {
    int remaining = byte_count >> 3;

    while (remaining--) {
        *(s64 *)dest = 0;
        dest += 8;
    }
    remaining = byte_count & 7;
    while (remaining--) {
        *dest = 0; // match: retail does not step dest here, so only the first leftover byte is cleared
    }
}

/* Clears word_count 32-bit words. */
void Memory_ClearWords(void *dest, u32 word_count) {
    s32 *words = dest;

    while (word_count--) {
        *words++ = 0;
    }
}

/* Copies word_count 32-bit words. */
void Memory_CopyWords(s32 *dest, s32 *src, u32 word_count) {
    while (word_count--) {
        *dest++ = *src++;
    }
}

/* Copies byte_count bytes, sixteen at a time; note src comes first. */
void Memory_Copy128(char *src, char *dest, u32 byte_count) {
    int remaining = byte_count >> 4;

    while (remaining--) {
        *(Uint128 *)dest = *(Uint128 *)src;
        src += 0x10;
        dest += 0x10;
    }
    remaining = byte_count & 0xF;
    while (remaining--) {
        *dest++ = *src++;
    }
}

/* Places the areas one after another in the arena, using the sizes Memory_Init set. */
void Memory_PlaceAreas(void) {
    char *next_area = memory_arena;

    D_00446F08 = D_004321F0;
    D_00446F58 = next_area;
    next_area += D_00446F0C;
    D_00446F68 = D_00446F58;
    D_00446F5C = next_area;
    next_area += D_00446F10;
    D_00446F60 = next_area;
    next_area += D_00446F14;
    D_00446F64 = next_area;
    next_area += D_00446F18;
    D_00446F50 = next_area;
    next_area += D_00446F20;
    D_00446F54 = next_area;
    next_area += D_00446F1C;
    D_00446F6C = next_area;
    next_area += D_00446F24;
    D_00446F70 = next_area;
    next_area += D_00446F28;
    D_00446F74 = next_area;
}
