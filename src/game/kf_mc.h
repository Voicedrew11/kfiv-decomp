/* Memory card: the request state block at D_01E4F278. */
#ifndef KF_MC_H
#define KF_MC_H

#include "common.h"

typedef struct {
    int unk00[32];                  /* 0x00 */
    int unk80;                      /* 0x80 0..2 */
    int unk84;                      /* 0x84 0..4 */
    int state;                      /* 0x88 the request in progress, 0 when idle */
    int unk8C;                      /* 0x8C */
    int unk90;                      /* 0x90 */
    int unk94;                      /* 0x94 */
    int timeout;                    /* 0x98 frames */
    int unk9C;                      /* 0x9C */
    int unkA0;                      /* 0xA0 */
    int unkA4;                      /* 0xA4 */
    int unkA8;                      /* 0xA8 */
    int unkAC;                      /* 0xAC */
    int unkB0;                      /* 0xB0 */
    int unkB4;                      /* 0xB4 */
    int unkB8;                      /* 0xB8 */
} McState;

extern McState D_01E4F278;

#endif
