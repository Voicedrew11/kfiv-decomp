#include "common.h"

/* The camera, view and projection matrices. */

f32 view_screen_matrix[0x10] = { 0 };      /* 0x4322A0 view to screen (sceVu0ViewScreenMatrix) */

f32 model_extra_transform_matrix[0x10] = { 0 }; /* 0x4322E0 used only by ModelMatrix_BuildFlattenedTilt, which nothing calls */

f32 world_view_matrix[0x10] = { 0 };       /* 0x432320 world to view: the camera's position and turn undone */

f32 inverse_camera_rotation[0x10] = { 0 }; /* 0x432360 the camera rotation, inverted */

f32 world_view_spare_matrix[0x10] = { 0 }; /* 0x4323A0 set to identity by Display_Init; nothing else uses it */

f32 world_screen_matrix[0x60] = { 0 };     /* 0x4323E0 world to screen: view_screen_matrix times world_view_matrix */

f32 projection_matrix[4][4] = { 0 };       /* 0x432560 a perspective projection (Display_BuildProjection) */

f32 camera_state[0x12] = { 0 };            /* 0x4325A0 the camera (CameraState) up to its word at 0x48 */

f32 camera_state_tail[0xE] = { 0 };        /* 0x4325E8 camera_state from screen_scale (0x48) on */

f32 flagged_model_transform_matrix[0x80] = { 0 };              /* 0x432620 */
