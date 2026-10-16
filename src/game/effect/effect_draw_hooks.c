/* Draw hooks of the effect models, picked by placement and blend mode. */
#include "model/model.h"

void Effect_PrepareAdditiveDraw(ModelInstance *model);
void Effect_PrepareSubtractiveDraw(ModelInstance *model);
void Effect_PrepareBlendedDraw(ModelInstance *model);
void Effect_PrepareCutoutDraw(ModelInstance *model);
void Effect_PrepareAdditiveDrawOnTop(ModelInstance *model);
void Effect_PrepareSubtractiveDrawOnTop(ModelInstance *model);
void Effect_PrepareBlendedDrawOnTop(ModelInstance *model);
void Effect_PrepareCutoutDrawOnTop(ModelInstance *model);

void (*effect_draw_hooks[4][4])(ModelInstance *) = { /* 0x3F1710 [placement][blend mode] */
    Effect_PrepareAdditiveDraw, Effect_PrepareSubtractiveDraw, Effect_PrepareBlendedDraw, Effect_PrepareCutoutDraw,
    Effect_PrepareAdditiveDrawOnTop, Effect_PrepareSubtractiveDrawOnTop, Effect_PrepareBlendedDrawOnTop, Effect_PrepareCutoutDrawOnTop,
    Effect_PrepareAdditiveDrawOnTop, Effect_PrepareSubtractiveDrawOnTop, Effect_PrepareBlendedDrawOnTop, Effect_PrepareCutoutDrawOnTop,
    Effect_PrepareAdditiveDraw, Effect_PrepareSubtractiveDraw, Effect_PrepareBlendedDraw, Effect_PrepareCutoutDraw,
};
