#ifndef KF_MAP_SCROLL_H
#define KF_MAP_SCROLL_H

#include "common.h"

void MapScroll_ClearPairs(int map_slot);
void MapScroll_PrepareCopy(void);
void MapScroll_UpdateCopy(int map, int mode);
void MapScroll_PrepareDraw(void);
void MapScroll_UpdateDraw(int map, int mode);
void MapScroll_CarryPairs(int from_slot, int to_slot);

/* map/map_scroll.c */
u32 *MapScroll_AppendBitblt(u32 *packet, int end_of_packet, int format, int buffer_width, int width, int height, int source_base, int source_x, int source_y, int dest_base, int dest_x, int dest_y);

#endif
