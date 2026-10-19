#include "common.h"

/* Empty in retail. The map unloaders call it last (MapLoad_FinishAndEnter, MapLoad_StartRequest, MapChange_UnloadAll). */
void MapLoad_AfterUnloadHook(void) {
}
