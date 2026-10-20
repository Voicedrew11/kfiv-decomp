#ifndef KF_MEMORY_H
#define KF_MEMORY_H

#include "common.h"
#include "map/map.h"

void Memory_Init(void);
void Memory_Clear64(char *dest, u32 byte_count);
void Memory_ClearWords(void *dest, u32 word_count);
void Memory_CopyWords(s32 *dest, s32 *src, u32 word_count);
void Memory_Copy128(char *src, char *dest, u32 byte_count);
void Memory_PlaceAreas(void);

/* data still in asm */
extern MapSlotMemory *map_slot_area;  /* the two map slots' memory */
/* the sizes of the areas, in bytes */
extern int packet_area_size;  /* the packet buffers' area */
extern int effect_area_size;  /* the effects' textures and models (effect/effect.c) */
extern int entity_pool_area_size;  /* the entity pool's area */
extern int held_weapon_area_size;  /* the arm and held item files (player/held_weapon.c) */
extern int hud_area_size;  /* the HUD's area */
extern int item_effect_area_size;
extern int memory_unplaced_area_size;  /* set, but no area is placed with it */
extern int packet_area;  /* the packet area */
extern char *effect_area;
extern char *entity_pool_area;  /* the entity pool blocks and work areas (entity/entity_pool.c) */
extern char *held_weapon_area;
extern int trade_menu_arena;
extern char *hud_area;  /* the HUD textures (ui/hud.c) */
extern char *item_effect_area;
extern char *sound_bank_area;

#endif
