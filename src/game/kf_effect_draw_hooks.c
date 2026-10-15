/* The draw hook of each effect model: an effect definition's placement and its blend mode
   (unkF4) pick which of the sixteen Effect_SetupModel installs. */
#include "kf_model.h"

void Effect_PrepareAdditiveDraw(ModelInstance *model);
void Effect_PrepareSubtractiveDraw(ModelInstance *model);
void Effect_PrepareBlendedDraw(ModelInstance *model);
void Effect_PrepareCutoutDraw(ModelInstance *model);
void Effect_PrepareAdditiveDrawOnTop(ModelInstance *model);
void Effect_PrepareSubtractiveDrawOnTop(ModelInstance *model);
void Effect_PrepareBlendedDrawOnTop(ModelInstance *model);
void Effect_PrepareCutoutDrawOnTop(ModelInstance *model);

void (*D_003F1710[4][4])(ModelInstance *) = { /* 0x3F1710 [placement][blend mode] */
    Effect_PrepareAdditiveDraw, Effect_PrepareSubtractiveDraw, Effect_PrepareBlendedDraw, Effect_PrepareCutoutDraw,
    Effect_PrepareAdditiveDrawOnTop, Effect_PrepareSubtractiveDrawOnTop, Effect_PrepareBlendedDrawOnTop, Effect_PrepareCutoutDrawOnTop,
    Effect_PrepareAdditiveDrawOnTop, Effect_PrepareSubtractiveDrawOnTop, Effect_PrepareBlendedDrawOnTop, Effect_PrepareCutoutDrawOnTop,
    Effect_PrepareAdditiveDraw, Effect_PrepareSubtractiveDraw, Effect_PrepareBlendedDraw, Effect_PrepareCutoutDraw,
};
