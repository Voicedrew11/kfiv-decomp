/* The pool of linked points the effect and model lights live in. */
#include "kf_linked_point.h"

/* 0x3E7500 the pool of influence points used as model lights */
LinkedPoint linked_point_pool[LINKED_POINT_COUNT] = { 0 };
