#include "kf_movie.h"
#include <eekernel.h>
#include <eeregs.h>
#include <libmpeg.h>

extern s32 D_00437820[];
extern MovieOutBuffer D_00437838;
extern MovieThreadState D_00437850;
extern MovieFile D_0043789C;
extern MovieDisplay D_004378D0;
extern MovieVideoDecoder D_00437938;
extern MovieBuffers D_004379F0;
extern char D_004420C8[];
extern char _gp[];

int printf(char *, ...);
void func_0021BDC0(void *);
int func_0021BDE0(sceMpeg *, MovieStreamCallbackData *, MovieReadBuffer *);
int func_0021BF00(int, int *, u32);
int func_0021C110(MovieReadBuffer *);
int func_0021C220(MovieBuffers *, int, int, int);
int func_0021DE28(int, int, int, int, int, int, int, int);
int func_0021DF48(MovieVideoDecoder *, int, int, sceMpegCallback, void *);
void func_0021E198(MovieVideoDecoder *);
int func_0021E528(MovieOutBuffer *, MovieOutPicture *, MovieOutTag *, s32);
void func_0021E960(MovieDisplay *, u32, u32, s32);
int func_0021F650(int);
int func_0021F790(void);
s32 func_0021F998(MovieFile *, char *);

/* Sets up everything a movie needs: the DMA controller, the read ring, the video and sound
   decoders and their stream callbacks, the picture ring and its GS packets, the idle and
   decode threads, the file, and the vblank and GIF DMA handlers (the SDK sample's initAll). */
void func_0021BA20(char *path) {
    struct ThreadParam thread_param;
    int i;

    *D_CTRL |= 3;
    *D_STAT = 4;
    func_0021C110(MOVIE_READ_BUFFER(&D_00437850));
    func_0021DE28((int)&D_00437938, (int)D_00437850.unk18, 0x13B800, (int)D_00437850.unk20, (int)D_00437850.unk24,
                  0x80, (int)D_00437850.unk30, 0x280);
    func_0021C220(&D_004379F0, (int)D_00437850.unk1C, 0xC000, 0x6000);
    func_0021DF48(&D_00437938, 0, 0, (sceMpegCallback)func_0021BDE0, MOVIE_READ_BUFFER(&D_00437850));
    if (D_00437820[0] != 0) {
        func_0021DF48(&D_00437938, 2, 0, (sceMpegCallback)func_0021BF00, MOVIE_READ_BUFFER(&D_00437850));
    }
    func_0021E528(&D_00437838, (MovieOutPicture *)(((u32)D_00437850.out_pictures & 0x0FFFFFFF) | 0x20000000),
                  D_00437850.out_tags, 5);
    for (i = 0; i < D_00437838.size; i++) {
        func_0021E960(&D_004378D0, (u32)&D_00437838.tag[i] + 0x40, (u32)&D_00437838.data[i], 0);
        func_0021E960(&D_004378D0, (u32)&D_00437838.tag[i] + 0x1AB40, (u32)&D_00437838.data[i], 1);
    }
    thread_param.entry = func_0021BDC0;
    thread_param.stack = D_00437850.unk10;
    thread_param.stackSize = 0x800;
    thread_param.initPriority = 1;
    thread_param.gpReg = _gp;
    thread_param.option = 0;
    D_00437850.unk44 = CreateThread(&thread_param);
    StartThread(D_00437850.unk44, NULL);
    thread_param.entry = (void (*)(void *))func_0021E198;
    thread_param.stack = D_00437850.unk14;
    thread_param.stackSize = 0x4000;
    thread_param.initPriority = 1;
    thread_param.gpReg = _gp;
    thread_param.option = 0;
    D_00437850.unk40 = CreateThread(&thread_param);
    StartThread(D_00437850.unk40, &D_00437938);
    while (!func_0021F998(&D_0043789C, path)) {
        printf(D_004420C8, path);
    }
    D_00437938.intc_handler = AddIntcHandler(2, func_0021F650, 0);
    EnableIntc(2);
    D_00437938.dmac_handler = AddDmacHandler(2, (int (*)(int))func_0021F790, 0);
    EnableDmac(2);
}
