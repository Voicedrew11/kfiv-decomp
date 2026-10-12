#include "kf_splash.h"

void func_00209A28(int texture, int pos_x, int pos_y, int width, int height, void *sprite_arg, void *track) {
    BootSprite *sprite = sprite_arg;

    sprite->track = track;
    sprite->texture = texture;
    sprite->pos_x = pos_x;
    sprite->pos_y = pos_y;
    sprite->width = width;
    sprite->height = height;
    sprite->frame = 0;
}
