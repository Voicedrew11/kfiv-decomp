/* The start-up settings file ini/NNNN.ini (core/ini.c). */
#ifndef KF_INI_H
#define KF_INI_H

#include "common.h"
#include "effect/item_effect.h"

/* Reads ini/NNNN.ini for `number` (ini/9999.ini when it is missing) and applies every key it knows. */
void Ini_Load(int number);

/* core/ini.c */
extern ItemUseEffectDef item_effect_defs_by_kind[];

/* data still in asm */
extern char ini_path_format[];  /* "ini/%4.4d.ini" */

#endif
