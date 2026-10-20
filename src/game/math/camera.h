/* The camera and its view, projection and screen matrices (math/camera_matrices.c). */
#ifndef KF_CAMERA_H
#define KF_CAMERA_H

#include "common.h"

/* math/camera_matrices.c */
extern f32 view_screen_matrix[0x10];
extern f32 model_extra_transform_matrix[0x10];
extern f32 world_view_matrix[0x10];
extern f32 inverse_camera_rotation[0x10];
extern f32 world_view_spare_matrix[0x10];
extern f32 world_screen_matrix[0x60];  /* the world-to-screen matrix */
extern f32 projection_matrix[0x10];
extern f32 camera_state[];
extern f32 camera_state_tail[];  /* camera_state from CameraState.screen_scale (0x48) on */
extern f32 flagged_model_transform_matrix[0x80];

#endif
