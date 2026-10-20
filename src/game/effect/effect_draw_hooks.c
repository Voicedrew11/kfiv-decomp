/* Draw hooks of the effect models, picked by placement and blend mode. */
#include "model/model.h"
#include "effect/effect.h"

void (*effect_draw_hooks[4][4])(ModelInstance *) = { /* 0x3F1710 [placement][blend mode] */
    Effect_PrepareAdditiveDraw, Effect_PrepareSubtractiveDraw, Effect_PrepareBlendedDraw, Effect_PrepareCutoutDraw,
    Effect_PrepareAdditiveDrawOnTop, Effect_PrepareSubtractiveDrawOnTop, Effect_PrepareBlendedDrawOnTop, Effect_PrepareCutoutDrawOnTop,
    Effect_PrepareAdditiveDrawOnTop, Effect_PrepareSubtractiveDrawOnTop, Effect_PrepareBlendedDrawOnTop, Effect_PrepareCutoutDrawOnTop,
    Effect_PrepareAdditiveDraw, Effect_PrepareSubtractiveDraw, Effect_PrepareBlendedDraw, Effect_PrepareCutoutDraw,
};
