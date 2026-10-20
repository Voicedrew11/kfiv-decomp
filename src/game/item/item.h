/* Item numbers: the rows of param/item_nam.prm. The counts the player carries, the map files, the scripts and the
   item_NNN keys of the ini presets all use them. Only the items the code names are listed. */
#ifndef KF_ITEM_H
#define KF_ITEM_H

#include "common.h"
#include "core/game.h"
#include "entity/entity.h"
#include "map/map.h"
#include "model/model.h"

/* Item and gold placement records store rotation, scale and radius in 1/16 units. */
#define ITEM_RECORD_FIXED_SCALE     (1.0f / 16.0f)

/* 0 to 149: things carried; param/item_cmn.prm has a row for each */
#define ITEM_HERB                   6
#define ITEM_HERBAL_LIQUID          7
#define ITEM_FOREST_FRUIT           8
#define ITEM_DRAGON_FLOWER          13
#define ITEM_POWER_SEED             21
#define ITEM_WIDDA_NEEDLE           22
#define ITEM_WISE_MANS_FRUIT        27
#define ITEM_FOOLS_FRUIT            28
#define ITEM_BONE                   31
#define ITEM_SCALE                  32
#define ITEM_CLAW                   33
#define ITEM_EARTH_STONE            34
#define ITEM_KEY_FIRST              47      /* Zastari's Key; the keys run to ITEM_KEY_LAST, 54 is unused */
#define ITEM_KEY_UNUSED             54
#define ITEM_KEY_LAST               59      /* King's Key */
#define ITEM_MAGIC_CRYSTAL          70
#define ITEM_FIRE_CRYSTAL           71      /* 71 to 76: the crystals of fire, earth, wind, water, light and dark */
#define ITEM_EARTH_CRYSTAL          72
#define ITEM_WIND_CRYSTAL           73
#define ITEM_WATER_CRYSTAL          74
#define ITEM_LIGHT_CRYSTAL          75
#define ITEM_DARK_CRYSTAL           76
#define ITEM_FIRE_CRYSTAL_2         77      /* 77 to 82: six more crystals with the same names */
#define ITEM_EARTH_CRYSTAL_2        78
#define ITEM_WIND_CRYSTAL_2         79
#define ITEM_WATER_CRYSTAL_2        80
#define ITEM_LIGHT_CRYSTAL_2        81
#define ITEM_DARK_CRYSTAL_2         82
#define ITEM_ARROW                  145
#define ITEM_WIDDAS_ARROW           147

/* 150 to 219: weapons; PlayerState.weapon is the number less ITEM_WEAPON_FIRST */
#define ITEM_WEAPON_FIRST           150     /* "None": the empty hand */
#define ITEM_BROKEN_KATANA          183
#define ITEM_ANCIENT_SWORD          193

/* 220 to 379: what is worn; PlayerState.equipment holds the number less ITEM_WORN_FIRST. Each group starts
   with its "None". */
#define ITEM_WORN_FIRST             220
#define ITEM_SHIELD_NONE            220
#define ITEM_ANCIENT_SHIELD         245
#define ITEM_HELM_NONE              250
#define ITEM_ANGELIC_CROWN          268
#define ITEM_SHADOW_MASK            269
#define ITEM_SACRIFICIAL_MASK       270
#define ITEM_ANCIENT_HELMET         273
#define ITEM_ARMOR_NONE             275
#define ITEM_ICE_ARMOR              293
#define ITEM_DEMON_ARMOR            295
#define ITEM_ANCIENT_ARMOR          297
#define ITEM_GAUNTLET_NONE          300
#define ITEM_SACRED_GLOVES          313
#define ITEM_BONE_HANDS             314
#define ITEM_SILVERY_GLOVES         319
#define ITEM_VENOM_GLOVES           320
#define ITEM_ANCIENT_GLOVES         322
#define ITEM_LORD_MYUS_GLOVES       323
#define ITEM_BOOT_NONE              325
#define ITEM_ABSORPTION_BOOTS       345
#define ITEM_DEMON_LEGS             346
#define ITEM_ANCIENT_GREAVES        348
#define ITEM_RING_NONE              350
#define ITEM_RING_OF_SIGHT          351
#define ITEM_WIDDAS_RING            353
#define ITEM_RING_OF_FIRE           355
#define ITEM_RING_OF_EARTH          356
#define ITEM_RING_OF_WIND           357
#define ITEM_RING_OF_WATER          358
#define ITEM_BRACELET_NONE          360
#define ITEM_CLARITY_BRACELET       361
#define ITEM_POWER_BRACELET         363
#define ITEM_FEATHER_BRACELET       364
#define ITEM_FORTUNE                368
#define ITEM_AMULET_NONE            370
#define ITEM_PURITY_AMULET          375
#define ITEM_THIEVES_NECKLACE       376
#define ITEM_MYUS_NECKLACE          377
#define ITEM_COUNT                  380

/* what PlayerState.weapon and PlayerState.equipment hold for an item */
#define ITEM_WEAPON_INDEX(item)     ((item) - ITEM_WEAPON_FIRST)
#define ITEM_WORN_INDEX(item)       ((item) - ITEM_WORN_FIRST)

/* item/item_defs.c */
extern ItemCommonDef item_common_defs[];

/* item/map_item.c */
void MapItem_UpdatePickup(void);
void MapItem_Init(void);
void MapItem_InitMap(int map);
void MapItem_RollFixedItems(int map);
void MapItem_UnloadMap(int map);
void MapItem_MoveMap(int map, f32 *offset);
int MapItem_PlaceFixedById(int map, int item_id);
s32 MapItem_DropFromEnemy(Entity *entity);
ItemNode *MapItem_PutOutForObject(int map, int item, void *position, void *rotation, void *scale, void *object);
ItemNode *MapItem_FindForObject(int object);
void MapItem_UpdateFalling(void);
void MapItem_DrawAll(int with_picked);
void MapItem_DrawPicked(int hidden);
int MapItem_StartPickup(ItemNode *node);
int MapItem_StartPickupById(int item_id, int may_put_back);
void MapItem_ClearMap(int map);
s32 MapItem_Drop(s32 map, s32 item, s32 priority, f32 *position, f32 floor_y, f32 *offset, f32 *rotation, f32 *scale, u32 order);
ItemModelSource *MapItem_FindModelSource(int slot, int key);
void MapItem_PrepareDraw(ModelInstance *model);
void MapItem_PickupAim(void);
void MapItem_PickupBringUp(void);
void MapItem_PickupHold(void);
void MapItem_PickupTake(void);
void MapItem_PickupPutBack(void);
int MapItem_PlaceFixed(int map, int index);
ItemNode *MapItem_Unlink(ItemNode *node);
int MapItem_GetFixedIndex(int slot, int object_id);
void MapItem_Link(ListNode *node);
int MapItem_UniqueNotCarried(int item);
int MapItem_UniqueNotInTradeStock(int item);
int MapItem_UniqueNotDropped(int item);

/* data still in asm */
extern const Vector4Arg enemy_drop_scale;
extern MapItemDef *map_item_defs[2];
extern s16 *map_item_model_kinds[2];
extern u8 *map_item_group_thresholds[2];
extern ItemNode *lying_items;  /* the items lying in the maps, a list */
extern ItemNode *pickup_item;  /* the one being picked up */
extern s32 map_fixed_item_counts[2];
extern s32 pickup_no_put_back;
extern f32 pickup_pose_scale;
extern s32 pickup_step;
extern f32 pickup_view_pitch;
extern int pickup_move_frames;
extern int pickup_frames_left;

/* item/item_defs.c */
extern GameDefinition item_name_defs[380];
extern GameDefinition weapon_name_defs[70];

/* item/map_item.c */
void *MapItem_GetFixedById(int slot);
int MapItem_IsPickable(ItemNode *node);

#endif
