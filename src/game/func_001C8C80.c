#include "common.h"

int func_001CA5B0(int, int);
int func_001C8B58(int, int);

/* Puts the fixed item with this id of a map in place (func_001C8B58). */
int func_001C8C80(int map, int item_id) {
    return func_001C8B58(map, func_001CA5B0(map, item_id));
}
