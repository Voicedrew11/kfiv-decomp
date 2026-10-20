/* KFIV: not in ICO. The libsdr.a calls the game makes (core/system.c, gfx/movie.c). */
#ifndef SCE_LIBSDR_LIBSDR_H
#define SCE_LIBSDR_LIBSDR_H

int sceSdRemoteInit(void);       /* definition in sce/ */
int sceSdRemote(int mode, ...);  /* definition in sce/ */

#endif
