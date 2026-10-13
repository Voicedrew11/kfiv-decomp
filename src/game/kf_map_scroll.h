#ifndef KF_MAP_SCROLL_H
#define KF_MAP_SCROLL_H

#include "common.h"

void KF_MapScroll_ClearPairs(int map_slot);
void KF_MapScroll_PrepareCopy(void);
void KF_MapScroll_UpdateCopy(int map, int mode);
void KF_MapScroll_PrepareDraw(void);
void KF_MapScroll_UpdateDraw(int map, int mode);
void KF_MapScroll_CarryPairs(int from_slot, int to_slot);

#endif
