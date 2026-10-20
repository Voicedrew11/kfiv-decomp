/* The start-up settings file ini/NNNN.ini (core/ini.c). */
#ifndef KF_INI_H
#define KF_INI_H

#include "common.h"
#include "effect/item_effect.h"

/* Reads ini/NNNN.ini for `number` (ini/9999.ini when it is missing) and applies every key it knows. */
void Ini_Load(int number);

/* core/ini.c */
extern ItemUseEffectDef item_effect_defs_by_kind[];
void Ini_SetHp(char *text);
void Ini_SetMp(char *text);
void Ini_SetPower(char *text);
void Ini_SetMagic(char *text);
void Ini_SetLevel(char *text);
void Ini_SetExp(char *text);
void Ini_SetGold(char *text);
void Ini_SetEquipWeapon(char *text);
void Ini_SetEquipMagic(char *text);
void Ini_SetEquipShield(char *text);
void Ini_SetEquipHelm(char *text);
void Ini_SetEquipArmor(char *text);
void Ini_SetEquipGauntlet(char *text);
void Ini_SetEquipBoot(char *text);
void Ini_SetEquipRing(char *text);
void Ini_SetEquipBracelet(char *text);
void Ini_SetEquipAmulet(char *text);
void Ini_SetShortcutType(char *text);
void Ini_SetShortcut(char *text);
void Ini_SetItem(char *text);
void Ini_SetMagicFire(char *text);
void Ini_SetMagicEarth(char *text);
void Ini_SetMagicWind(char *text);
void Ini_SetMagicWater(char *text);
void Ini_SetMagicHoly(char *text);
void Ini_SetMagicDark(char *text);
void Ini_SetDebugAutoRecv(char *text);
void Ini_SetDebugDash(char *text);
void Ini_SetDebugMagicRapidFire(char *text);

/* data still in asm */
extern char ini_path_format[];  /* "ini/%4.4d.ini" */

#endif
