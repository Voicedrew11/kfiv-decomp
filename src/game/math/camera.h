/* The camera and its view, projection and screen matrices (math/camera_matrices.c). */
#ifndef KF_CAMERA_H
#define KF_CAMERA_H

#include "common.h"

/* camera_state: the camera */
typedef struct {
    f32 position[4];                /* 0x00 */
    f32 pitch;                      /* 0x10 rotation about x */
    f32 yaw;                        /* 0x14 rotation about y */
    f32 roll;                       /* 0x18 rotation about z */
    char pad1C[0x04];
    f32 look_at_point[4];           /* 0x20 the point to look at (GameStatus.camera_look_mode) */
    f32 move_to_point[4];           /* 0x30 the point to move to (GameStatus.camera_move_enabled) */
    f32 turn_speed;                 /* 0x40 how fast it turns to unk20 */
    f32 move_speed;                 /* 0x44 how far it moves to unk30 in a frame */
    f32 screen_scale;               /* 0x48 512.0 after Map_ResetGameState */
    f32 height_offset;              /* 0x4C added to the height above the player */
    f32 script_shake_offset[4];     /* 0x50 the first shake's offset (GameLoop_UpdateCameraShake) */
    f32 entity_shake_offset[4];     /* 0x60 the second shake's offset */
    f32 script_shake_reach;         /* 0x70 how far the first shake reaches */
    s32 script_shake_frames;        /* 0x74 frames of it left */
    f32 entity_shake_reach;         /* 0x78 how far the second shake reaches (EntityAi_ShakeCamera) */
    s32 entity_shake_frames;        /* 0x7C frames of it left */
} CameraState;

/* math/camera_matrices.c */
extern f32 view_screen_matrix[0x10];
extern f32 model_extra_transform_matrix[0x10];
extern f32 world_view_matrix[0x10];
extern f32 inverse_camera_rotation[0x10];
extern f32 world_view_spare_matrix[0x10];
extern f32 world_screen_matrix[0x60];  /* the world-to-screen matrix */
extern f32 projection_matrix[4][4];
extern CameraState camera_state;  /* defined as two arrays split at screen_scale, camera_state_tail the second */
extern f32 camera_state_tail[];  /* camera_state from CameraState.screen_scale (0x48) on */
extern f32 flagged_model_transform_matrix[0x80];

#endif
