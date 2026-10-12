#include "kf_packet.h"
#include "kf_texture.h"

void func_0010B238(TextureTableHeader *);
void func_0010A100(void *);

extern TextureTableHeader *D_004471F0;
extern ScreenSprite D_005CE098;

/* Select the texture table at D_004471F0 and queue the sprite at D_005CE098. */
void func_001D2020(void) {
    func_0010B238(D_004471F0);
    func_0010A100(&D_005CE098);
}
