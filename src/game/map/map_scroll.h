#ifndef KF_MAP_SCROLL_H
#define KF_MAP_SCROLL_H

#include "common.h"

void MapScroll_ClearPairs(int map_slot);
void MapScroll_PrepareCopy(void);
void MapScroll_UpdateCopy(int map, int mode);
void MapScroll_PrepareDraw(void);
void MapScroll_UpdateDraw(int map, int mode);
void MapScroll_CarryPairs(int from_slot, int to_slot);

#endif
