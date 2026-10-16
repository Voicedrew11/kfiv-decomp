/* The item tables Map_LoadParamFiles loads from the param files: a row per item, per weapon and per worn item. */
#include "common.h"
#include "kf_game.h"
#include "kf_map.h"
#include "kf_player.h"

GameDefinition item_name_defs[380] = { 0 }; /* 0x4147F8 0x40 bytes per item, param/item_nam.prm */
ItemCommonDef item_common_defs[150] = { 0 };      /* 0x41A6F8 the common block, one per item */
PlayerItemDef weapon_defs[70] = { 0 };      /* 0x41B9B8 param/item_wpn.prm */
LevelRecordDef worn_defs[160] = { 0 };      /* 0x41E118 param/item_pct.prm */

/* The weapons and the worn items inside item_name_defs (items 150 and 220 on). */
asm(".globl weapon_name_defs\nweapon_name_defs = item_name_defs + 0x2580\n");
asm(".globl worn_name_defs\nworn_name_defs = item_name_defs + 0x3700\n");
