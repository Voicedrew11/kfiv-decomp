/* The map viewer screen (ui/map_viewer.c) and its tables. */
#ifndef KF_MAP_VIEWER_H
#define KF_MAP_VIEWER_H

#include "common.h"
#include "core/game.h"
#include "gfx/packet.h"
#include "map/map.h"
#include "model/model.h"

/* ui/map_viewer.c */
void MapViewer_Init(void);
void MapViewer_Run(void);
void MapViewer_OpenForItem(int viewer_number);
/* the viewers and the functions that find where the marker goes (ui/map_viewer.c) */
void MapViewer_RunPictureMap(void);
int MapViewer_LocateOnCaretakersMap(int *screen_x, int *screen_y);
int MapViewer_LocateOnZastarisMap(int *screen_x, int *screen_y);
int MapViewer_LocateOnEarthFolksMap(int *map_x, int *map_y);
int MapViewer_LocateOnWiddasMap(int *screen_x, int *screen_y);
void MapViewer_RunKingsMap(void);
int MapViewer_IsKingsMapUsable(int kind);
void MapViewer_Reset(void);
void MapViewer_DrawPictureMap(void);

/* ui/map_viewer_tables.c */
extern ViewerEntry viewer_entries[];
extern ScreenQuad viewer_marker_rect;
extern ScreenQuad viewer_picture_rect;
extern ScreenQuad viewer_arrow_rect_1;
extern ScreenQuad viewer_arrow_rect_2;
extern ScreenQuad viewer_picture_label_rect;
extern ScreenQuad kings_map_overlay_rect;
extern ScreenQuad kings_map_corner_rect;
extern MapLightBlockLights viewer_default_lights;
extern Vector4 map_object_glow_colors[4];

/* data still in asm */
extern char viewer_texture_file_name_format[];
extern char viewer_pointer_model_path[];
extern ViewerCornerTable viewer_marker_corner_us;
extern ViewerCornerTable viewer_marker_corner_vs;
extern f32 float_largest_value[];
extern f32 float_lowest_value[];

#endif
