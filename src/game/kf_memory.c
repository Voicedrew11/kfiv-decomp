#include "kf_memory.h"
#include "kf_map.h"

#define MEMORY_ARENA_SIZE 0x1600000

/* the sizes of the areas, in bytes */
extern int packet_area_size;        /* the packet buffers' area */
extern int effect_area_size;        /* the effects' textures and models (kf_effect.c) */
extern int entity_pool_area_size;   /* entity_pool_area */
extern int held_weapon_area_size;   /* the arm and held item files (kf_held_weapon.c) */
extern u32 map_work_area_size;
extern int map_file_area_size;
extern int hud_area_size;           /* hud_area */
extern int item_effect_area_size;
extern int D_00446F2C;              /* set, but no area is placed with it */

/* the areas, as Memory_PlaceAreas lays them out */
extern MapSlotMemory map_slot_memory[2];
extern MapSlotMemory *map_slot_area;  /* the two map slots' memory, the array above */
extern char *map_file_area;
extern char *map_work_area;
extern int packet_area;              /* the packet area */
extern char *effect_area;
extern char *entity_pool_area;      /* the entity pool blocks and work areas (kf_entity_pool.c) */
extern char *held_weapon_area;
extern int trade_menu_arena;
extern char *hud_area;              /* the HUD textures (kf_hud.c) */
extern char *item_effect_area;
extern char *sound_bank_area;

static char memory_arena[MEMORY_ARENA_SIZE]; /* 0x5FAF80 every area lies in it */

/* Clears the arena, sets the size of each area and lays them out. */
void Memory_Init(void) {
    Memory_ClearWords(memory_arena, sizeof(memory_arena) / 4);
    packet_area_size = 0x280000;
    effect_area_size = 0x1D0000;
    held_weapon_area_size = 0xA0000;
    map_work_area_size = 0x200000;
    map_file_area_size = 0xA80000;
    entity_pool_area_size = 0x300000;
    hud_area_size = 0x20000;
    item_effect_area_size = 0x80000;
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

    map_slot_area = map_slot_memory;
    packet_area = (int)next_area;
    next_area += packet_area_size;
    trade_menu_arena = packet_area;
    effect_area = next_area;
    next_area += effect_area_size;
    entity_pool_area = next_area;
    next_area += entity_pool_area_size;
    held_weapon_area = next_area;
    next_area += held_weapon_area_size;
    map_file_area = next_area;
    next_area += map_file_area_size;
    map_work_area = next_area;
    next_area += map_work_area_size;
    hud_area = next_area;
    next_area += hud_area_size;
    item_effect_area = next_area;
    next_area += item_effect_area_size;
    sound_bank_area = next_area;
}
