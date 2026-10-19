#include "common.h"

/* Empty in retail. The map loaders call it between clearing the map scroll pairs and setting up
   the map objects (MapChange_SetUpMap, Map_LoadAtOnce, MapLoad_SetUpMap). */
void MapLoad_BeforeObjectInitHook(void) {
}
