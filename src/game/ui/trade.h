/* The trade, smith and save screens (ui/trade.c). */
#ifndef KF_TRADE_H
#define KF_TRADE_H

#include "common.h"

/* the smith material of an item: the low two bits of upgrade_material_kind (1 bone, 2 scale, 3 claw) */
#define UPGRADE_MATERIAL_MASK 3

/* Runs the trade screens of trade table `value` until they are left. */
void Trade_Run(s32 value);

/* Runs the smith's screens until they are left. */
void Smith_Run(void);

/* Runs the save screen until it is left. */
void SaveScreen_Run(void);

#endif
