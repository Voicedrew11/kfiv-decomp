/* What a save file holds: every block func_00103C18 writes and func_00104EC8 reads back. */
#include "kf_game.h"
#include "kf_map.h"
#include "kf_entity.h"
#include "kf_player.h"

/* 0x349680 the options; func_00102930 sets their defaults, the code names single bytes of them */
u8 D_00349680[0x10] = { 0 };
asm(".globl D_00349681\nD_00349681 = D_00349680 + 1\n"
    ".globl D_00349682\nD_00349682 = D_00349680 + 2\n"
    ".globl D_00349683\nD_00349683 = D_00349680 + 3\n"
    ".globl D_00349685\nD_00349685 = D_00349680 + 5\n"
    ".globl D_00349686\nD_00349686 = D_00349680 + 6\n"
    ".globl D_00349688\nD_00349688 = D_00349680 + 8\n"
    ".globl D_00349689\nD_00349689 = D_00349680 + 9\n"
    ".globl D_0034968A\nD_0034968A = D_00349680 + 0xA\n");

/* 0x349690 a 0x80-by-0x40 byte grid the map scripts, NPCs and set-ups record their state in */
u8 D_00349690[0x2000] = { 0 };
asm(".globl D_00349D55\nD_00349D55 = D_00349690 + 0x6C5\n"
    ".globl D_00349F56\nD_00349F56 = D_00349690 + 0x8C6\n"
    ".globl D_0034A017\nD_0034A017 = D_00349690 + 0x987\n"
    ".globl D_0034A018\nD_0034A018 = D_00349690 + 0x988\n"
    ".globl D_0034A019\nD_0034A019 = D_00349690 + 0x989\n"
    ".globl D_0034A0D5\nD_0034A0D5 = D_00349690 + 0xA45\n"
    ".globl D_0034AFD6\nD_0034AFD6 = D_00349690 + 0x1946\n"
    ".globl D_0034B098\nD_0034B098 = D_00349690 + 0x1A08\n"
    ".globl D_0034B217\nD_0034B217 = D_00349690 + 0x1B87\n"
    ".globl D_0034B21C\nD_0034B21C = D_00349690 + 0x1B8C\n");

/* 0x34B690 a 0x40-byte block of state a new game clears and a save keeps */
u8 D_0034B690[0x40] = { 0 };
asm(".globl D_0034B696\nD_0034B696 = D_0034B690 + 6\n"
    ".globl D_0034B69C\nD_0034B69C = D_0034B690 + 0xC\n");

/* 0x34B6D0 the state a group of entities shares (EntityController.unk43A picks one) */
EntityGroupRecord D_0034B6D0[0x32] = { { 0 } };

/* 0x34C350 a Recollection page: bits 0-2 the description shown, then a bit per message heard */
RecollectionFlags recollection_flags[50] = { { 0 } };

/* 0x34C4E0 per Recollection page: 0 not met yet, else which description it shows */
u8 recollection_pages[50] = { 0 };

/* 0x34C518 0x400 bytes a save keeps whole; the f32 D_0034C524 at +0xC is a record's height */
u8 D_0034C518[0x400] = { 0 };
asm(".globl D_0034C524\nD_0034C524 = D_0034C518 + 0xC\n");

/* 0x34C918 a bit per map tag: the maps visited (func_001034A8, func_001034F8) */
u8 D_0034C918[0x10] = { 0 };

/* 0x34C928 four flag bits per nibble, 0x100 bytes per map */
u8 D_0034C928[0x8000] = { 0 };

/* 0x354928 a 4-bit counter per map entry: how often the entity there has died */
u8 D_00354928[0x80][0x40] = { { 0 } };

/* 0x356928 a byte per map object counting its item's pickups (kf_model.h's required_flag) */
u8 D_00356928[0x2000] = { 0 };

/* 0x358928 the items lying on the maps: a row of 16 per map tag (kf_map_item.c) */
u8 D_00358928[0xA000] = { 0 };

/* 0x362928 where the map objects are marked, an ObjectMarkRow per map tag (kf_gold.c) */
u8 D_00362928[0x5000] = { 0 };

/* 0x367928[5] which item each trigger holds */
TriggerItemRecord D_00367928[TRIGGER_ITEM_RECORD_COUNT] = { { 0 } };

/* 0x367940 the experience of each of the six schools' five spells */
u16 spell_experience[6][5] = { { 0 } };

/* 0x367980 the level of each of them */
u8 spell_levels[6][5] = { { 0 } };

/* 0x3679A0 what has been gained per weapon, a LevelRecord each */
LevelRecord D_003679A0[70] = { { 0 } };

/* 0x3682F0 what has been gained per worn item, a LevelRecord each */
LevelRecord D_003682F0[160] = { { 0 } };

/* 0x369830 how many of each item the player has, one byte per item definition */
DefinitionByteSet item_counts = { { 0 } };

/* 0x3699B0 the five trade tables' stock, copied from 0x0043D9B0 on a new game */
DefinitionByteSet D_003699B0 = { { 0 } };
DefinitionByteSet D_00369B30 = { { 0 } };
DefinitionByteSet D_00369CB0 = { { 0 } };
DefinitionByteSet D_00369E30 = { { 0 } };
DefinitionByteSet D_00369FB0 = { { 0 } };

/* 0x36A130 the save image; the play time D_0036A138 is at +8 */
u8 D_0036A130[0x20000] = { 0 };
asm(".globl D_0036A138\nD_0036A138 = D_0036A130 + 8\n");
