/* KFIV: not in ICO. The libdev.a calls the game makes (core/system.c); the VIF resets are asm. */
#ifndef SCE_LIBDEV_LIBDEV_H
#define SCE_LIBDEV_LIBDEV_H

void sceDevVif0Reset(void);
void sceDevVif1Reset(void);
void sceDevVu0Pause(void);     /* definition in sce/ */
void sceDevVu0SetTBit(void);   /* definition in sce/ */

#endif
