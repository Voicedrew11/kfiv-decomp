/* Model and part matrices (kf_model_matrix.c). */
#ifndef KF_MODEL_MATRIX_H
#define KF_MODEL_MATRIX_H

#include "kf_model.h"

/* Builds a part's local matrix in dest from its scale, its three angles (applied x, z, y) and its offset. */
void ModelMatrix_BuildPartLocal(ModelInstance *model, ModelPart *part, ModelPart *dest);

/* Builds the model's matrix (ModelInstance.unk40) from a position, a rotation and a scale. */
void ModelMatrix_Build(ModelInstance *model, Vector4 *position, Vector4 *rotation, f32 *scale);

/* Makes the two matrices a model is drawn with from the two sources. */
void ModelMatrix_BuildDrawMatrices(ModelInstance *model, s32 normal_src, s32 light_src, void *normal_dest,
                                   void *light_dest);

#endif
