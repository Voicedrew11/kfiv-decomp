/* Movie playback: FromSoftware's fslibmpg.c (the file name its error messages print), built after the SDK's
   MPEG streaming sample. The movie file is read into a ring (MovieReadBuf), libmpeg splits it into the video
   stream, which reaches the IPU through a ring of DMA blocks (MovieStream) and is decoded by a thread of its own
   (MovieVideo), and the sound stream, which goes to the SPU2 on the IOP (MovieAudio). Decoded pictures wait in a
   ring (MovieOut) for the vblank handler, which sends the GS packets that show them one field at a time. */
#include "gfx/movie.h"
#include "gfx/gs.h"
#include "core/file.h"
#include <libmpeg.h>
#include <eekernel.h>
#include <eeregs.h>
#include <libgraph.h>

void *memcpy(void *, const void *, u32);
void *memset(void *, int, u32);
char *index(const char *, int);
char *strncpy(char *, const char *, u32);
int strcmp(const char *, const char *);
u32 strlen(const char *);
int toupper(int);
int sprintf(char *, char *, ...);
s32 sceSdRemote(s32, ...);
int sceSifAllocIopHeap(int);
int sceSifFreeIopHeap(int);
int sceSifSetDma(void *, int);
int sceSifDmaStat(int);
int sceSifLoadModule(const char *, int, const char *);
int sceCdDiskReady(int);
int sceCdStInit(u32, u32, u32);
int sceCdStStop(void);
int sceCdStStart(u32, sceCdRMode *);
int sceCdStRead(u32, u32 *, u32, u32 *);
int sceCdStSeek(u32);
int sceOpen(const char *, int);
int sceRead(int, void *, int);
int sceLseek(int, int, int);
int sceClose(int);

/* The movie player's .data: the flags the vblank handler shares, the picture ring and the decoder state. */
#define MOVIE_DATA4 __attribute__((section(".data"), aligned(4)))

s32 movie_has_sound_stream MOVIE_DATA4 = { 1 };         /* 0x437820 nonzero: the movie has a sound stream */
volatile s32 movie_pictures_enabled MOVIE_DATA4 = { 0 }; /* 0x437824 the vblank handler shows pictures (Movie_StartDisplay) */
volatile s32 movie_picture_vblanks MOVIE_DATA4 = { 0 }; /* 0x437828 vblanks counted while it does */
volatile s32 movie_picture_shown MOVIE_DATA4 = { 0 }; /* 0x43782C a picture was shown completely: Movie_GifDmaHandler drops it */
volatile s32 movie_start_field MOVIE_DATA4 = { 0 }; /* 0x437830 field that starts, from GS CSR */
volatile s32 movie_path3_busy MOVIE_DATA4 = { 0 }; /* 0x437834 nonzero: path 3 still busy */
MovieOutBuffer movie_out_buffer = { 0 };          /* 0x437838 the ring of decoded pictures */
MovieThreadState movie_thread = { 0 };        /* 0x437850 the thread state, the heap and the file and display in it */
u32 movie_thread_pad[2] MOVIE_DATA4 = { 0 };      /* 0x437930 zero fill */
MovieVideoDecoder movie_decoder = { 0 };       /* 0x437938 the video decoder and its stream in it */
MovieAudio movie_audio = { 0 };              /* 0x4379F0 the sound decoder */
s32 movie_idle_vblanks MOVIE_DATA4 = { 0 };         /* 0x437A4C vblanks with no picture ready */

static char movie_audio_zero_buffer[0x800];               /* 0x1E4F7C0 0x800 zero bytes for the IOP */
static u32 movie_init_display_packet[0x50];                /* 0x1E4FFC0 GS packet of Movie_InitDisplay */
static u32 movie_clear_packet[0x70];                /* 0x1E50100 GS packet of MovieDisplay_Clear */
extern char _gp[];

/* an object of 8 bytes or less kept in .rodata, as the game keeps them, not in .sdata */
#define IN_RODATA __attribute__((section(".rodata")))

void sceMpegSetDecodeMode(void); // match: called with no arguments, the caller's own pass through

static void Movie_HeapInit(u32 start, u32 size);
static void *Movie_HeapAlloc(u32 size, u32 alignment);
static int Movie_PlayFile(char *path, int (*skip_requested)(void));
static int Movie_PlayLoop(MovieVideoDecoder *decoder, MovieReadBuffer *read_buffer, MovieFile *file,
                          int (*skip_requested)(void));
static void Movie_Yield(void);
static int Movie_IsAudioReady(void);
static void Movie_Setup(char *path);
static void Movie_Shutdown(void);
static void Movie_IdleThread(void *unused);
static void Movie_DebugPrint(char *message);
static int Movie_VideoCallback(sceMpeg *mpeg, MovieStreamCallbackData *stream_data, MovieReadBuffer *read_buffer);
static int Movie_AudioCallback(sceMpeg *mpeg, MovieStreamCallbackData *stream_data, MovieReadBuffer *read_buffer);
static int Movie_CopyTwoAreas(u8 *first_dest, int first_dest_size, u8 *second_dest, int second_dest_size,
                              u8 *first_src, int first_src_size, u8 *second_src, int second_src_size);
static int MovieReadBuf_Init(MovieReadBuffer *buffer);
static void MovieReadBuf_Delete(MovieReadBuffer *buffer);
static int MovieReadBuf_BeginPut(MovieReadBuffer *buffer, u8 **dest);
static int MovieReadBuf_EndPut(MovieReadBuffer *buffer, int size);
static int MovieReadBuf_BeginGet(MovieReadBuffer *buffer, u8 **src);
static int MovieReadBuf_EndGet(MovieReadBuffer *buffer, int size);
static int MovieAudio_Init(MovieAudio *audio, u8 *data, int size, int iop_buffer_size);
static int MovieAudio_Delete(MovieAudio *audio);
static int MovieAudio_Start(MovieAudio *audio);
static int MovieAudio_Reset(MovieAudio *audio);
static void MovieAudio_BeginPut(MovieAudio *audio, u8 **first, int *first_size, u8 **second, int *second_size);
static void MovieAudio_EndPut(MovieAudio *audio, int size);
static s32 MovieAudio_IsPreset(MovieAudio *audio);
static int MovieAudio_SendToIop(MovieAudio *audio);
static void MovieAudio_GetIopSpace(int *first, int *first_size, int *second, int *second_size, MovieAudio *audio,
                                   int play_pos);
static int MovieAudio_SendTwoAreas(u8 *first_dest, int first_dest_size, u8 *second_dest, int second_dest_size,
                                   u8 *first_src, int first_src_size, u8 *second_src, int second_src_size);
static int Movie_SendToIop(void *dest, void *src, int size);
static void MovieAudio_SetMasterVolume(int volume);
static void MovieAudio_SetInputVolume(int volume);
static int MovieStream_GetBlockIndex(MovieStreamBuffer *stream, u32 address);
static void Movie_SetD3Chcr(int chcr);
static void Movie_SetD4Chcr(int chcr);
static void Movie_SetDmaTag(MovieDmaTag *dma_tag, u32 address, u32 tag_id, u32 qwc);
static int MovieStream_Init(MovieStreamBuffer *stream, u8 *data, MovieDmaTag *tags, int block_count,
                            MovieTimeStamp *stamps, int stamp_capacity);
static int MovieStream_Reset(MovieStreamBuffer *stream);
static void MovieStream_BeginPut(MovieStreamBuffer *stream, u8 **first, int *first_size, u8 **second,
                                 int *second_size);
static void MovieStream_EndPut(MovieStreamBuffer *stream, int size);
static int MovieStream_AddDma(MovieStreamBuffer *stream);
static int MovieStream_StopDma(MovieStreamBuffer *stream);
static int MovieStream_RestartDma(MovieStreamBuffer *stream);
static int MovieStream_Delete(MovieStreamBuffer *stream);
static int MovieStream_GetCount(MovieStreamBuffer *stream);
static void MovieStream_Flush(MovieStreamBuffer *stream);
static int MovieStream_IsStampAt(MovieStreamBuffer *stream, s32 stop);
static int MovieStream_TrimStamps(MovieStreamBuffer *stream, MovieTimeStamp *new_stamp);
static int MovieStream_PutStamp(MovieStreamBuffer *stream, MovieTimeStamp *stamp);
static int MovieStream_GetStamp(MovieStreamBuffer *stream, MovieTimeStamp *stamp_out);
static int MovieVideo_Init(MovieVideoDecoder *decoder, u8 *work_area, int work_area_size, u8 *data,
                           MovieDmaTag *tags, int block_count, MovieTimeStamp *stamps, int stamp_capacity);
static int MovieVideo_SetStream(MovieVideoDecoder *decoder, int type, int channel, sceMpegCallback callback,
                                void *data);
static void MovieVideo_BeginPut(MovieVideoDecoder *decoder, u8 **first, s32 *first_size, u8 **second,
                                s32 *second_size);
static void MovieVideo_EndPut(MovieVideoDecoder *decoder, s32 size);
static s32 MovieVideo_ResetState(MovieVideoDecoder *decoder);
static int MovieVideo_Delete(MovieVideoDecoder *decoder);
static s32 MovieVideo_Abort(MovieVideoDecoder *decoder);
static int MovieVideo_GetState(MovieVideoDecoder *decoder);
static s32 MovieVideo_SetState(MovieVideoDecoder *decoder, s32 state);
static int MovieVideo_PutStamp(MovieVideoDecoder *decoder, s64 pts, s64 dts, u8 *start, s32 len);
static int MovieVideo_GetInputCount(MovieVideoDecoder *decoder);
static int MovieVideo_Flush(MovieVideoDecoder *decoder);
static int MovieVideo_IsFlushed(MovieVideoDecoder *decoder);
static void MovieVideo_Main(void *decoder_argument);
static int MovieVideo_DecodeAll(MovieVideoDecoder *decoder);
static int Movie_OnMpegError(sceMpeg *mpeg, void *callback_data, void *user_data);
static int Movie_OnMpegNoData(sceMpeg *mpeg, void *callback_data, void *user_data);
static int Movie_OnMpegStopDma(sceMpeg *mpeg, void *callback_data, void *user_data);
static int Movie_OnMpegRestartDma(sceMpeg *mpeg, void *callback_data, void *user_data);
static int Movie_OnMpegBackground(sceMpeg *mpeg, void *callback_data, void *user_data);
static int Movie_OnMpegTimeStamp(sceMpeg *mpeg, MovieStampCallbackData *callback_data, void *user_data);
static int MovieVideo_CopyTwoAreas(u8 *dest0, s32 dest0_size, u8 *dest1, s32 dest1_size, u8 *source0,
                                   s32 source0_size, u8 *source1, s32 source1_size);
static int Movie_PumpAudio(void);
static int MovieOut_Init(MovieOutBuffer *buffer, MovieOutPicture *data, MovieOutTag *tag, s32 size);
static void MovieOut_Delete(MovieOutBuffer *buffer);
static s32 MovieOut_Reset(MovieOutBuffer *buffer);
static s32 MovieOut_IsFull(MovieOutBuffer *buffer);
static void MovieOut_Commit(MovieOutBuffer *buffer);
static MovieOutPicture *MovieOut_GetWriteSlot(MovieOutBuffer *buffer);
static s32 MovieOut_IsEmpty(MovieOutBuffer *buffer);
static MovieOutTag *MovieOut_GetOldestTag(MovieOutBuffer *buffer);
static void MovieOut_DropOldest(MovieOutBuffer *buffer);
static void MovieDisplay_Init(MovieDisplay *display, s32 screen_x, s32 screen_y, s32 width, s32 height,
                              s32 picture_width, s32 picture_height);
static void MovieDisplay_Delete(MovieDisplay *display);
static void MovieDisplay_BuildFieldPackets(MovieDisplay *display, u32 packet_start, u32 picture, s32 field);
static void MovieDisplay_ShowField(MovieDisplay *display, s32 second);
static void MovieDisplay_Clear(MovieDisplay *display, u32 color);
static int GsPacket_AddTexturedSprite(int packet, s32 *screen_rect, s32 *texture_rect);
static int GsPacket_AddColorSprite(int packet, s32 *rect, u32 color);
static int GsPacket_AddDmaTag(int packet, u32 spr, u32 addr, u32 irq, u32 tag_id, u32 pce, u32 qwc);
static int GsPacket_AddGifTag(int packet, s64 regs, s32 nreg, s32 flg, s32 prim, s32 pre, s32 eop, s32 nloop);
static int GsPacket_AddReg(int packet, s32 reg, s64 value);
static int GsPacket_AddTexflush(int packet);
static int GsPacket_AddTex1(s64 packet, u32 lcm, u32 mxl, u32 mmag, u32 mmin, u32 mtba, u32 lod_l, u64 lod_k);
static int GsPacket_AddTex0(s64 packet, u32 tbp0, u32 tbw, u32 psm, u32 tex_width_log2, u32 tex_height_log2,
                            u32 tcc, u32 tfx, u32 cbp, u32 cpsm, u32 csm, u32 csa, u32 cld);
static int GsPacket_AddPrim(int packet, u32 prim, u32 iip, u32 tme, u32 fge, u32 abe, u32 aa1, u32 fst, u32 ctxt,
                            u32 fix);
static int GsPacket_AddUv(int packet, u32 tex_u, u32 tex_v);
static int GsPacket_AddRgbaq(s64 packet, u32 red, u32 green, u32 blue, u32 alpha, u64 q_value);
static int GsPacket_AddXyz2(int packet, u32 pos_x, u32 pos_y, u32 depth);
static int GsPacket_AddFrame(s64 packet, u32 fbp, u32 fbw, u32 psm, u64 fbmsk);
static int GsPacket_AddTest(int packet, u32 ate, u32 atst, u32 aref, u32 afail, u32 date, u32 datm, u32 zte,
                            u32 ztst);
static int GsPacket_AddScissor(s64 packet, u32 scax0, u32 scax1, u32 scay0, u64 scay1);
static int GsPacket_AddXyOffset(int packet, u32 offset_x, u32 offset_y);
static int GsPacket_AddPrmodecont(int packet, u32 use_prim);
static int GsPacket_AddClamp(s64 packet, u64 wms, u64 wmt, u64 minu, u64 maxu, u64 minv, u64 maxv);
static int GsPacket_AddBitbltbuf(int packet, u32 dest_base, u32 dest_width, u32 dest_format);
static int GsPacket_AddTrxpos(int packet, u32 direction, u32 dest_x, u32 dest_y);
static int GsPacket_AddTrxreg(int packet, u32 width, u32 height);
static int GsPacket_AddTrxdir(int packet, u32 direction);
static void Movie_SendGifChain(u32 chain);
static int Movie_VblankHandler(int cause);
static int Movie_GifDmaHandler(int channel);
static void Movie_StartDisplay(s32 field);
static void Movie_StopDisplay(void);
static s32 MovieFile_Open(MovieFile *file, char *path);
static s32 MovieFile_Close(MovieFile *file);
static s32 MovieFile_Read(MovieFile *file, u32 *buffer, s32 size, s32 lsn);

/* GsPacket_AddReg as the register writers that take the packet as an s64 call it */
#define GS_PACKET_ADD_REG_64 ((int (*)(s64, s64, s64))GsPacket_AddReg) // match: no truncation of the packet

/* GsPacket_AddScissor as MovieDisplay_Clear calls it */
#define GS_PACKET_ADD_SCISSOR_32 ((int (*)(s64, u32, u32, u32, u32))GsPacket_AddScissor) // match: a 64-bit last parameter adds a move at each call

/* Gives the movie player the block of memory it allocates its buffers from. */
static void Movie_HeapInit(u32 start, u32 size) {
    movie_thread.heap_start = start;
    movie_thread.heap_next = start;
    movie_thread.heap_end = start + size - 1;
}

/* Takes size bytes at the next multiple of alignment from the movie player's block; NULL when it is full. */
static void *Movie_HeapAlloc(u32 size, u32 alignment) {
    u32 start = (movie_thread.heap_next + alignment - 1) / alignment * alignment;
    u32 end = start + size;

    if (movie_thread.heap_end < end) {
        return NULL;
    }
    movie_thread.heap_next = end;
    return (void *)start;
}

/* Plays a movie file: carves the player's buffers out of the block given, sets up the display and runs the
   movie to its end. Returns -1 when the block is too small. */
int Movie_Play(char *path, u32 heap, u32 heap_size, int (*skip_requested)(void), u32 volume, s16 video_mode,
               s16 display_x, s16 display_y) {
    Movie_HeapInit(heap, heap_size);
    if ((movie_thread.idle_stack = Movie_HeapAlloc(0x800, 0x40)) == NULL ||
        (movie_thread.decode_stack = Movie_HeapAlloc(0x4000, 0x40)) == NULL ||
        (movie_thread.mpeg_work = Movie_HeapAlloc(0x13B800, 0x40)) == NULL ||
        (movie_thread.audio_data = Movie_HeapAlloc(0xC000, 0x40)) == NULL ||
        (movie_thread.stream_data = Movie_HeapAlloc(0x40000, 0x40)) == NULL ||
        (movie_thread.stream_tags = Movie_HeapAlloc(0x810, 0x40)) == NULL ||
        (movie_thread.out_pictures = Movie_HeapAlloc(0x578000, 0x40)) == NULL ||
        (movie_thread.out_tags = Movie_HeapAlloc(0x10AF40, 0x40)) == NULL ||
        (movie_thread.time_stamps = Movie_HeapAlloc(0x3C00, 0x40)) == NULL ||
        (movie_thread.spare_buffer = Movie_HeapAlloc(0x100, 0x40)) == NULL ||
        (movie_thread.read_buffer = (s32)Movie_HeapAlloc(0x5000C, 0x40)) == 0) {
        return -1;
    }
    movie_thread.video_mode = video_mode;
    movie_thread.display_x = display_x;
    movie_thread.display_y = display_y;
    MovieDisplay_Init(&movie_thread.display, 0, 0, 0x280, 0x1C0, 0x280, 0x1C0);
    MovieDisplay_Clear(&movie_thread.display, 0);
    if (volume >= 0x4000) {
        volume = 0x3FFF;
    }
    movie_thread.volume = volume;
    Movie_PlayFile(path, skip_requested);
    MovieDisplay_Delete(&movie_thread.display);
    return 0;
}

/* Plays one movie file from start to end and frees what playing it needed. */
static int Movie_PlayFile(char *path, int (*skip_requested)(void)) {
    int result;

    Movie_Setup(path);
    result = Movie_PlayLoop(&movie_decoder, MOVIE_READ_BUFFER(&movie_thread), &movie_thread.file, skip_requested);
    Movie_Shutdown();
    return result;
}

/* Plays the movie file to its end: reads it into the read ring, has libmpeg split it into the video and sound
   streams, starts picture and sound once both are buffered, then waits for the decoder to run dry. From the
   eleventh picture on, skip_requested can end it early (the SDK sample's readMpeg). */
static int Movie_PlayLoop(MovieVideoDecoder *decoder, MovieReadBuffer *read_buffer, MovieFile *file,
                          int (*skip_requested)(void)) {
    u8 *put_dest;
    u8 *get_src;
    int put_size;
    int get_size;
    int read_rest = file->size;
    int write_rest = file->size;
    int count;
    int proceeded;
    int is_started = FALSE;
    int is_skipping = FALSE;

    /* the last 4 bytes are not played */
    while (write_rest > 4 && MovieVideo_GetState(decoder) != MOVIE_VIDEO_STATE_ENDED) {
        if (skip_requested != NULL && !is_skipping && ((sceMpeg *)decoder)->frameCount > 10) {
            is_skipping = skip_requested() != 0;
        }
        if (is_skipping && ((sceMpeg *)decoder)->frameCount > 10) {
            MovieVideo_Abort(&movie_decoder);
        }
        put_size = MovieReadBuf_BeginPut(read_buffer, &put_dest);
        if (read_rest > 0 && put_size >= 0x10000) {
            count = MovieFile_Read(file, (u32 *)put_dest, 0x10000, file->lsn + (file->size - read_rest) / 0x800);
            MovieReadBuf_EndPut(read_buffer, count);
            read_rest -= count;
        }
        Movie_Yield();
        get_size = MovieReadBuf_BeginGet(read_buffer, &get_src);
        if (get_size > 0) {
            proceeded =
                sceMpegDemuxPssRing((sceMpeg *)decoder, get_src, get_size, (int)read_buffer, read_buffer->size);
            MovieReadBuf_EndGet(read_buffer, proceeded);
            write_rest -= proceeded;
        }
        if (!is_started && MovieOut_IsFull(&movie_out_buffer) && Movie_IsAudioReady()) {
            Movie_StartDisplay(1);
            if (movie_has_sound_stream != 0) {
                MovieAudio_Start(&movie_audio);
            }
            is_started = TRUE;
        }
    }
    while (!MovieVideo_Flush(decoder)) {
        Movie_Yield();
    }
    while (!MovieVideo_IsFlushed(decoder) && MovieVideo_GetState(decoder) != MOVIE_VIDEO_STATE_ENDED) {
        Movie_Yield();
    }
    Movie_StopDisplay();
    if (movie_has_sound_stream != 0) {
        MovieAudio_Reset(&movie_audio);
    }
    return TRUE;
}

/* Lets the other movie threads run (the SDK sample's switchThread). */
static void Movie_Yield(void) {
    RotateThreadReadyQueue(1);
}

/* TRUE when enough sound is buffered to start, or the movie has no sound (isAudioOK). */
static int Movie_IsAudioReady(void) {
    return (movie_has_sound_stream != 0) ? MovieAudio_IsPreset(&movie_audio) : TRUE;
}

/* Sets up everything a movie needs: the DMA controller, the read ring, the video and sound decoders and their
   stream callbacks, the picture ring and its GS packets, the idle and decode threads, the file, and the vblank
   and GIF DMA handlers (the SDK sample's initAll). */
static void Movie_Setup(char *path) {
    struct ThreadParam thread_param;
    int i;

    *D_CTRL |= D_CTRL_DMAE | D_CTRL_RELE;
    *D_STAT = 4;
    MovieReadBuf_Init(MOVIE_READ_BUFFER(&movie_thread));
    MovieVideo_Init(&movie_decoder, movie_thread.mpeg_work, 0x13B800, movie_thread.stream_data, movie_thread.stream_tags,
                    0x80, movie_thread.time_stamps, 0x280);
    MovieAudio_Init(&movie_audio, movie_thread.audio_data, 0xC000, 0x6000);
    MovieVideo_SetStream(&movie_decoder, 0, 0, (sceMpegCallback)Movie_VideoCallback, MOVIE_READ_BUFFER(&movie_thread));
    if (movie_has_sound_stream != 0) {
        MovieVideo_SetStream(&movie_decoder, 2, 0, (sceMpegCallback)Movie_AudioCallback,
                             MOVIE_READ_BUFFER(&movie_thread));
    }
    MovieOut_Init(&movie_out_buffer, (MovieOutPicture *)(((u32)movie_thread.out_pictures & EE_PHYSICAL_ADDRESS_MASK) | EE_UNCACHED_ALIAS),
                  movie_thread.out_tags, 5);
    for (i = 0; i < movie_out_buffer.size; i++) {
        MovieDisplay_BuildFieldPackets(&movie_thread.display, (u32)&movie_out_buffer.tag[i] + 0x40, (u32)&movie_out_buffer.data[i], 0);
        MovieDisplay_BuildFieldPackets(&movie_thread.display, (u32)&movie_out_buffer.tag[i] + 0x1AB40, (u32)&movie_out_buffer.data[i],
                                       1);
    }
    thread_param.entry = Movie_IdleThread;
    thread_param.stack = movie_thread.idle_stack;
    thread_param.stackSize = 0x800;
    thread_param.initPriority = 1;
    thread_param.gpReg = _gp;
    thread_param.option = 0;
    movie_thread.idle_thread = CreateThread(&thread_param);
    StartThread(movie_thread.idle_thread, NULL);
    thread_param.entry = MovieVideo_Main;
    thread_param.stack = movie_thread.decode_stack;
    thread_param.stackSize = 0x4000;
    thread_param.initPriority = 1;
    thread_param.gpReg = _gp;
    thread_param.option = 0;
    movie_thread.decode_thread = CreateThread(&thread_param);
    StartThread(movie_thread.decode_thread, &movie_decoder);
    while (!MovieFile_Open(&movie_thread.file, path)) {
        printf("Can't Open file %s\n", path);
    }
    movie_decoder.intc_handler = AddIntcHandler(2, Movie_VblankHandler, 0);
    EnableIntc(2);
    movie_decoder.dmac_handler = AddDmacHandler(2, Movie_GifDmaHandler, 0);
    EnableDmac(2);
}

/* Frees what Movie_Setup set up: the threads, the handlers, the decoder, the sound buffers and the file. */
static void Movie_Shutdown(void) {
    MovieReadBuf_Delete((MovieReadBuffer *)movie_thread.read_buffer);
    MovieOut_Delete(&movie_out_buffer);
    TerminateThread(movie_thread.decode_thread);
    DeleteThread(movie_thread.decode_thread);
    TerminateThread(movie_thread.idle_thread);
    DeleteThread(movie_thread.idle_thread);
    DisableDmac(2);
    RemoveDmacHandler(2, movie_decoder.dmac_handler);
    DisableIntc(2);
    RemoveIntcHandler(2, movie_decoder.intc_handler);
    MovieVideo_Delete(&movie_decoder);
    MovieAudio_Delete(&movie_audio);
    MovieFile_Close(&movie_thread.file);
}

/* The idle thread of the movie player: yields forever (the SDK sample's defMain). */
static void Movie_IdleThread(void *unused) {
    while (TRUE) {
        Movie_Yield();
    }
}

/* The debug print of the original, empty in the game: the messages are still passed. */
static void Movie_DebugPrint(char *message) {
}

/* libmpeg calls this with a stretch of the video stream: copies it out of the read ring into the stream ring and
   queues its time stamp (the SDK sample's videoCallback). */
static int Movie_VideoCallback(sceMpeg *mpeg, MovieStreamCallbackData *stream_data, MovieReadBuffer *read_buffer) {
    u8 *first_src = stream_data->data;
    u8 *second_src = read_buffer->data;
    u32 first_src_size = read_buffer->data + read_buffer->size - stream_data->data;
    int second_src_size;
    u8 *first_dest;
    int first_dest_size;
    u8 *second_dest;
    int second_dest_size;
    int copied;

    if (stream_data->len < first_src_size) {
        first_src_size = stream_data->len;
    }
    second_src_size = stream_data->len - first_src_size;
    MovieVideo_BeginPut(&movie_decoder, &first_dest, &first_dest_size, &second_dest, &second_dest_size);
    copied = Movie_CopyTwoAreas((u8 *)(((u32)first_dest & EE_PHYSICAL_ADDRESS_MASK) | EE_UNCACHED_ALIAS), first_dest_size,
                                (u8 *)(((u32)second_dest & EE_PHYSICAL_ADDRESS_MASK) | EE_UNCACHED_ALIAS), second_dest_size, first_src,
                                first_src_size, second_src, second_src_size);
    if (copied > 0) {
        if (!MovieVideo_PutStamp(&movie_decoder, stream_data->pts, stream_data->dts, first_dest, copied)) {
            Movie_DebugPrint("pts buffer overflow\n");
        }
    }
    MovieVideo_EndPut(&movie_decoder, copied);
    return copied > 0;
}

/* libmpeg calls this with a stretch of the sound stream: copies it, less its 4-byte header, out of the read ring
   into the sound ring (the SDK sample's pcmCallback). */
static int Movie_AudioCallback(sceMpeg *mpeg, MovieStreamCallbackData *stream_data, MovieReadBuffer *read_buffer) {
    u8 *first_dest;
    int first_dest_size;
    u8 *second_dest;
    int second_dest_size;
    u8 *first_src;
    int second_src_size;
    int remaining;
    int first_src_size;
    u8 *end;
    int ring_size;
    int copied;

    ring_size = read_buffer->size;
    remaining = stream_data->len - 4;
    end = read_buffer->data + ring_size;
    first_src = stream_data->data + 4;
    if (first_src >= end) {
        first_src -= ring_size;
    }
    first_src_size = end - first_src;
    first_src_size = (remaining < first_src_size) ? remaining : first_src_size;
    second_src_size = remaining - first_src_size;
    MovieAudio_BeginPut(&movie_audio, &first_dest, &first_dest_size, &second_dest, &second_dest_size);
    copied = Movie_CopyTwoAreas(first_dest, first_dest_size, second_dest, second_dest_size, first_src,
                                first_src_size, read_buffer->data, second_src_size);
    MovieAudio_EndPut(&movie_audio, copied);
    return copied > 0;
}

/* Copies two source stretches into two destination stretches, as one stream; returns the bytes copied, or 0 when
   they do not fit (the SDK sample's copy2area). */
static int Movie_CopyTwoAreas(u8 *first_dest, int first_dest_size, u8 *second_dest, int second_dest_size,
                              u8 *first_src, int first_src_size, u8 *second_src, int second_src_size) {
    if (first_dest_size + second_dest_size < first_src_size + second_src_size) {
        return 0;
    }
    if (first_src_size >= first_dest_size) {
        memcpy(first_dest, first_src, first_dest_size);
        memcpy(second_dest, first_src + first_dest_size, first_src_size - first_dest_size);
        memcpy(second_dest + first_src_size - first_dest_size, second_src, second_src_size);
    } else if (second_src_size >= first_dest_size - first_src_size) {
        memcpy(first_dest, first_src, first_src_size);
        memcpy(first_dest + first_src_size, second_src, first_dest_size - first_src_size);
        memcpy(second_dest, second_src + first_dest_size - first_src_size,
               second_src_size - (first_dest_size - first_src_size));
    } else {
        memcpy(first_dest, first_src, first_src_size);
        memcpy(first_dest + first_src_size, second_src, second_src_size);
    }
    return first_src_size + second_src_size;
}

/* Empties the read ring (readBufCreate). */
static int MovieReadBuf_Init(MovieReadBuffer *buffer) {
    buffer->put = buffer->count = 0;
    buffer->size = sizeof(buffer->data);
    return 0;
}

/* Frees the read ring: nothing to do (readBufDelete). */
static void MovieReadBuf_Delete(MovieReadBuffer *buffer) {
}

/* Gives the place the next data goes and returns the free bytes (readBufBeginPut). */
static int MovieReadBuf_BeginPut(MovieReadBuffer *buffer, u8 **dest) {
    int free_size = buffer->size - buffer->count;

    if (free_size != 0) {
        *dest = buffer->data + buffer->put;
    }
    return free_size;
}

/* Accounts for size bytes written at the put position (readBufEndPut). */
static int MovieReadBuf_EndPut(MovieReadBuffer *buffer, int size) {
    int accepted = buffer->size - buffer->count;

    if (size < accepted) {
        accepted = size;
    }
    buffer->put = (buffer->put + accepted) % buffer->size;
    buffer->count += accepted;
    return accepted;
}

/* Gives the oldest waiting data and returns how many bytes wait (readBufBeginGet). */
static int MovieReadBuf_BeginGet(MovieReadBuffer *buffer, u8 **src) {
    if (buffer->count != 0) {
        *src = buffer->data + (buffer->put - buffer->count + buffer->size) % buffer->size;
    }
    return buffer->count;
}

/* Accounts for size bytes taken out (readBufEndGet). */
static int MovieReadBuf_EndGet(MovieReadBuffer *buffer, int size) {
    int taken = buffer->count;

    if (size < taken) {
        taken = size;
    }
    buffer->count -= taken;
    return taken;
}

/* Sets up the sound ring over size bytes at data and takes two IOP heap blocks: iop_buffer_size bytes the SPU2
   plays from, and 0x800 zero bytes. Returns 0 when the IOP heap is full (audioDecCreate). */
static int MovieAudio_Init(MovieAudio *audio, u8 *data, int size, int iop_buffer_size) {
    int iop_block;

    audio->state = MOVIE_AUDIO_STATE_INIT;
    audio->header_count = 0;
    audio->put = 0;
    audio->count = 0;
    audio->total_bytes = 0;
    audio->total_bytes_sent = 0;
    audio->iop_last_pos = 0;
    audio->iop_pause_pos = 0;
    audio->data = data;
    audio->size = size;
    audio->iop_buffer_size = iop_buffer_size;
    iop_block = sceSifAllocIopHeap(iop_buffer_size);
    audio->iop_buffer = iop_block;
    if (iop_block < 0) {
        return 0;
    }
    iop_block = sceSifAllocIopHeap(0x800);
    audio->iop_zero_buffer = iop_block;
    if (iop_block < 0) {
        return 0;
    }
    memset(movie_audio_zero_buffer, 0, 0x800);
    Movie_SendToIop((void *)audio->iop_zero_buffer, movie_audio_zero_buffer, 0x800);
    MovieAudio_SetMasterVolume(movie_thread.volume);
    return 1;
}

/* Frees the two IOP heap blocks and turns the master volume down (audioDecDelete). */
static int MovieAudio_Delete(MovieAudio *audio) {
    if (sceSifFreeIopHeap(audio->iop_buffer) < 0) {
        printf("sceSifFreeIopHeap Error %s %d\n", "fslibmpg.c", 0x3AE);
    }
    if (sceSifFreeIopHeap(audio->iop_zero_buffer) < 0) {
        printf("sceSifFreeIopHeap Error %s %d\n", "fslibmpg.c", 0x3B0);
    }
    MovieAudio_SetMasterVolume(0);
    return 1;
}

/* Stops the sound where it is: mutes the input, keeps the play position and has the SPU2 play the zero block
   (audioDecPause); nothing calls it. */
int MovieAudio_Pause(MovieAudio *audio) {
    audio->state = MOVIE_AUDIO_STATE_PAUSE;
    MovieAudio_SetInputVolume(0);
    audio->iop_pause_pos = (sceSdRemote(1, 0x80E0, 0, 2, 0, 0) & 0xFFFFFF) - audio->iop_buffer;
    sceSdRemote(1, 0x80D0, 0, 0, audio->iop_zero_buffer, 0x4000, 0x800);
    return 1;
}

/* Starts the sound again from where MovieAudio_Pause left it (audioDecResume); nothing calls it. */
int MovieAudio_Resume(MovieAudio *audio) {
    MovieAudio_SetInputVolume(0x7FFF);
    sceSdRemote(1, 0x80E0, 0, 0x13, audio->iop_buffer, (audio->iop_buffer_size / 0x400) * 0x400,
                audio->iop_buffer + audio->iop_pause_pos);
    audio->state = MOVIE_AUDIO_STATE_PLAY;
    return 1;
}

/* Starts the block transfer of the sound stream on the IOP and turns the volumes up (audioDecStart). */
static int MovieAudio_Start(MovieAudio *audio) {
    sceSdRemote(1, 0x8010, 0x800, 0xC0);
    sceSdRemote(1, 0x8010, 0x801, 0xC);
    sceSdRemote(1, 0x80E0, 0, 0x10, audio->iop_buffer, (audio->iop_buffer_size / 1024) * 1024);
    sceSdRemote(1, 0x8010, 0xF80, 0x7FFF);
    sceSdRemote(1, 0x8010, 0x1080, 0x7FFF);
    sceSdRemote(1, 0x8010, 0xF81, 0x7FFF);
    sceSdRemote(1, 0x8010, 0x1081, 0x7FFF);
    sceSdRemote(1, 0x8010, 0xD81, 0x7FFF);
    sceSdRemote(1, 0x8010, 0xE81, 0x7FFF);
    sceSdRemote(1, 0x8010, 0x980, movie_thread.volume);
    sceSdRemote(1, 0x8010, 0xA80, movie_thread.volume);
    sceSdRemote(1, 0x8010, 0x981, movie_thread.volume);
    sceSdRemote(1, 0x8010, 0xA81, movie_thread.volume);
    audio->state = MOVIE_AUDIO_STATE_PLAY;
    return 0;
}

/* Mutes and stops the sound stream and empties its buffers (audioDecReset). */
static int MovieAudio_Reset(MovieAudio *audio) {
    sceSdRemote(1, 0x8010, 0xF80, 0);
    sceSdRemote(1, 0x8010, 0x1080, 0);
    sceSdRemote(1, 0x8010, 0xF81, 0);
    sceSdRemote(1, 0x8010, 0x1081, 0);
    sceSdRemote(1, 0x8010, 0xD81, 0);
    sceSdRemote(1, 0x8010, 0xE81, 0);
    sceSdRemote(1, 0x8010, 0x980, 0);
    sceSdRemote(1, 0x8010, 0xA80, 0);
    sceSdRemote(1, 0x8010, 0x981, 0);
    sceSdRemote(1, 0x8010, 0xA81, 0);
    sceSdRemote(1, 0x80E0, 0, 2, 0, 0);
    sceSdRemote(1, 0x80E0, 1, 2, 0, 0);
    audio->state = MOVIE_AUDIO_STATE_INIT;
    audio->header_count = 0;
    audio->put = 0;
    audio->count = 0;
    audio->total_bytes = 0;
    audio->total_bytes_sent = 0;
    audio->iop_last_pos = 0;
    audio->iop_pause_pos = 0;
    return 0;
}

/* Gives the one or two stretches the next sound data goes in: the stream headers until they are complete, then
   the free part of the ring (audioDecBeginPut). */
static void MovieAudio_BeginPut(MovieAudio *audio, u8 **first, int *first_size, u8 **second, int *second_size) {
    int free_size;

    if (audio->state == MOVIE_AUDIO_STATE_INIT) {
        *first = audio->header + audio->header_count;
        *first_size = sizeof(audio->header) - audio->header_count;
        *second = audio->data;
        *second_size = audio->size;
        return;
    }
    free_size = audio->size - audio->count;
    if (audio->size - audio->put >= free_size) {
        *first = audio->data + audio->put;
        *first_size = free_size;
        *second = NULL;
        *second_size = 0;
    } else {
        *first = audio->data + audio->put;
        *first_size = audio->size - audio->put;
        *second = audio->data;
        *second_size = free_size - (audio->size - audio->put);
    }
}

/* Accounts for size bytes put: first the rest of the headers (which ends the INIT state), then sound data
   (audioDecEndPut). */
static void MovieAudio_EndPut(MovieAudio *audio, int size) {
    u32 header_count;
    u32 header_left;
    u32 header_taken;

    if (audio->state == MOVIE_AUDIO_STATE_INIT) {
        header_count = audio->header_count;
        header_left = sizeof(audio->header) - header_count;
        header_taken = (header_left >= size) ? size : header_left;
        header_count += header_taken;
        audio->header_count = header_count;
        if (header_count >= sizeof(audio->header)) {
            audio->state = MOVIE_AUDIO_STATE_PRESET;
        }
        size -= header_taken;
    }
    header_left = (audio->put + size) % audio->size;
    audio->count += size;
    audio->total_bytes += size;
    audio->put = header_left;
}

/* Whether the whole IOP buffer has been filled once, so the sound can start (isPreset). */
static s32 MovieAudio_IsPreset(MovieAudio *audio) {
    return audio->total_bytes_sent >= audio->iop_buffer_size;
}

/* Sends whole 1024-byte blocks of waiting sound data to the free part of the IOP buffer; returns the bytes sent
   (audioDecSendToIOP). */
static int MovieAudio_SendToIop(MovieAudio *audio) {
    int first_dest;
    int first_dest_size;
    int second_dest;
    int second_dest_size;
    u8 *first_src;
    u8 *second_src;
    int first_src_size;
    int second_src_size;
    int sent = 0;
    int whole_count;
    int play_pos;

    switch (audio->state) {
        case MOVIE_AUDIO_STATE_INIT:
            return 0;
        case MOVIE_AUDIO_STATE_PRESET:
            first_dest = audio->iop_buffer + audio->total_bytes_sent % audio->iop_buffer_size;
            first_dest_size = audio->iop_buffer_size - audio->total_bytes_sent;
            second_dest = 0;
            second_dest_size = 0;
            break;
        case MOVIE_AUDIO_STATE_PLAY:
            play_pos = sceSdRemote(1, 0x8100, 0, 0) & 0xFFFFFF;
            MovieAudio_GetIopSpace(&first_dest, &first_dest_size, &second_dest, &second_dest_size, audio,
                                   play_pos - audio->iop_buffer);
            break;
        case MOVIE_AUDIO_STATE_PAUSE:
            return 0;
    }
    first_src = audio->data + (audio->put - audio->count + audio->size) % audio->size;
    second_src = audio->data;
    whole_count = (audio->count / 1024) * 1024;
    first_src_size = audio->data + audio->size - first_src;
    if (whole_count < first_src_size) {
        first_src_size = whole_count;
    }
    second_src_size = whole_count - first_src_size;
    if (first_dest_size + second_dest_size >= 1024 && first_src_size + second_src_size >= 1024) {
        sent = MovieAudio_SendTwoAreas((u8 *)first_dest, first_dest_size, (u8 *)second_dest, second_dest_size,
                                       first_src, first_src_size, second_src, second_src_size);
    }
    audio->count -= sent;
    audio->total_bytes_sent += sent;
    audio->iop_last_pos = (audio->iop_last_pos + sent) % audio->iop_buffer_size;
    return sent;
}

/* Gives the one or two stretches of the IOP buffer between the last block sent and the one before the play
   position, in whole 1024-byte blocks (the SDK sample's getAreaIOP). */
static void MovieAudio_GetIopSpace(int *first, int *first_size, int *second, int *second_size, MovieAudio *audio,
                                   int play_pos) {
    int buffer_size = audio->iop_buffer_size;
    int last_pos = audio->iop_last_pos;
    int free_size;

    play_pos = (((play_pos + buffer_size) - last_pos) - 0x400) % buffer_size;
    free_size = play_pos;
    free_size = (free_size / 1024) * 1024;
    if ((buffer_size - last_pos) >= free_size) {
        *first = audio->iop_buffer + audio->iop_last_pos;
        *first_size = free_size;
        *second = 0;
        *second_size = 0;
        return;
    }
    *first = audio->iop_buffer + audio->iop_last_pos;
    *first_size = audio->iop_buffer_size - audio->iop_last_pos;
    *second = audio->iop_buffer;
    *second_size = free_size - (audio->iop_buffer_size - audio->iop_last_pos);
}

/* Sends two source stretches to two stretches of IOP memory, as one stream, dropping what does not fit; returns
   the bytes sent (the SDK sample's sendToIOP2area). */
static int MovieAudio_SendTwoAreas(u8 *first_dest, int first_dest_size, u8 *second_dest, int second_dest_size,
                                   u8 *first_src, int first_src_size, u8 *second_src, int second_src_size) {
    if (first_dest_size + second_dest_size < first_src_size + second_src_size) {
        int excess = (first_src_size + second_src_size) - (first_dest_size + second_dest_size);

        if (excess >= second_src_size) {
            first_src_size -= excess - second_src_size;
            second_src_size = 0;
        } else {
            second_src_size -= excess;
        }
    }
    if (first_src_size >= first_dest_size) {
        Movie_SendToIop(first_dest, first_src, first_dest_size);
        Movie_SendToIop(second_dest, first_src + first_dest_size, first_src_size - first_dest_size);
        Movie_SendToIop(second_dest + first_src_size - first_dest_size, second_src, second_src_size);
    } else if (second_src_size >= first_dest_size - first_src_size) {
        Movie_SendToIop(first_dest, first_src, first_src_size);
        Movie_SendToIop(first_dest + first_src_size, second_src, first_dest_size - first_src_size);
        Movie_SendToIop(second_dest, second_src + first_dest_size - first_src_size,
                        second_src_size - (first_dest_size - first_src_size));
    } else {
        Movie_SendToIop(first_dest, first_src, first_src_size);
        Movie_SendToIop(first_dest + first_src_size, second_src, second_src_size);
    }
    return first_src_size + second_src_size;
}

/* Copies size bytes to IOP memory with sceSifSetDma and waits for the transfer (the SDK sample's sendToIOP);
   returns size. */
static int Movie_SendToIop(void *dest, void *src, int size) {
    MovieSifDmaData dma;
    int dma_id;

    if (size <= 0) {
        return 0;
    }
    dma.data = src;
    dma.addr = dest;
    dma.size = size;
    dma.attr = 0;
    FlushCache(0);
    dma_id = sceSifSetDma(&dma, 1);
    do {
    } while (sceSifDmaStat(dma_id) >= 0);
    return size;
}

/* Sets the master volume of both SPU2 cores, left and right (changeMasterVolume). */
static void MovieAudio_SetMasterVolume(int volume) {
    int core;

    for (core = 0; core < 2; core++) {
        sceSdRemote(1, 0x8010, core | 0x980, volume);
        sceSdRemote(1, 0x8010, core | 0xA80, volume);
    }
}

/* Sets the volume of the sound stream's input, left and right (changeInputVolume). */
static void MovieAudio_SetInputVolume(int volume) {
    sceSdRemote(1, 0x8010, 0xF80, volume);
    sceSdRemote(1, 0x8010, 0x1080, volume);
}

/* Block index of a DMA address in the stream ring; the tag after the last block counts as 0 (getFIFOindex). */
static int MovieStream_GetBlockIndex(MovieStreamBuffer *stream, u32 address) {
    if (address == ((u32)(stream->block_count * sizeof(MovieDmaTag) + (u32)stream->tags + sizeof(MovieDmaTag)) & EE_PHYSICAL_ADDRESS_MASK)) {
        return 0;
    }
    return (address - (u32)stream->data) / 0x800;
}

/* Writes D3_CHCR with the DMA controller held (setD3_CHCR). */
static void Movie_SetD3Chcr(int chcr) {
    DIntr();
    *D_ENABLEW = *D_ENABLER | D_ENABLE_HOLD;
    *D3_CHCR = chcr;
    *D_ENABLEW = *D_ENABLER & ~D_ENABLE_HOLD;
    EIntr();
}

/* Writes D4_CHCR with the DMA controller held (setD4_CHCR). */
static void Movie_SetD4Chcr(int chcr) {
    DIntr();
    *D_ENABLEW = *D_ENABLER | D_ENABLE_HOLD;
    *D4_CHCR = chcr;
    *D_ENABLEW = *D_ENABLER & ~D_ENABLE_HOLD;
    EIntr();
}

/* Builds a source-chain DMA tag (scTag2). */
static void Movie_SetDmaTag(MovieDmaTag *dma_tag, u32 address, u32 tag_id, u32 qwc) {
    dma_tag->tag = ((u64)address << 32) | ((u64)tag_id << 28) | (u64)qwc;
}

/* Sets up the stream ring over block_count 0x800-byte blocks and their DMA tags, and points DMA channel 4 (to
   the IPU) at it (viBufCreate, with viBufReset written out). */
static int MovieStream_Init(MovieStreamBuffer *stream, u8 *data, MovieDmaTag *tags, int block_count,
                            MovieTimeStamp *stamps, int stamp_capacity) {
    struct SemaParam sema_param;
    int i;

    stream->data = data;
    stream->tags = (void *)(((u32)tags & EE_PHYSICAL_ADDRESS_MASK) | EE_UNCACHED_ALIAS);
    stream->block_count = block_count;
    stream->data_size = block_count * 0x800;
    stream->dma_start = 0;
    stream->dma_count = 0;
    stream->read_bytes = 0;
    stream->is_active = TRUE;
    stream->total_bytes = 0;
    stream->stamps = stamps;
    stream->stamp_capacity = stamp_capacity;
    stream->stamp_count = 0;
    stream->stamp_write = 0;
    for (i = 0; i < stamp_capacity; i++) {
        stream->stamps[i].pts = -1;
        stream->stamps[i].dts = -1;
        stream->stamps[i].pos = 0;
        stream->stamps[i].len = 0;
    }
    sema_param.initCount = 1;
    sema_param.maxCount = 1;
    stream->sema = CreateSema(&sema_param);
    for (i = 0; i < stream->block_count; i++) {
        Movie_SetDmaTag(&tags[i], (u32)(data + i * 0x800), 3, 0x80);
    }
    Movie_SetDmaTag(&tags[i], (u32)tags & EE_PHYSICAL_ADDRESS_MASK, 2, 0);
    *D4_QWC = 0;
    *D4_MADR = (u32)stream->data & EE_PHYSICAL_ADDRESS_MASK;
    *D4_TADR = (u32)stream->tags & EE_PHYSICAL_ADDRESS_MASK;
    Movie_SetD4Chcr(D_CHCR_DIR_FROM_MEMORY | D_CHCR_MOD_CHAIN);
    return TRUE;
}

/* Empties the stream ring, rebuilds its DMA tags and points DMA channel 4 at it again (viBufReset). */
static int MovieStream_Reset(MovieStreamBuffer *stream) {
    int i;

    stream->is_active = TRUE;
    stream->dma_start = 0;
    stream->dma_count = 0;
    stream->read_bytes = 0;
    stream->total_bytes = 0;
    stream->saved.d4_madr = 0;
    stream->saved.d4_tadr = 0;
    stream->saved.d4_qwc = 0;
    stream->saved.d4_chcr = 0;
    for (i = 0; i < stream->block_count; i++) {
        Movie_SetDmaTag((MovieDmaTag *)stream->tags + i, (u32)(stream->data + i * 0x800), 3, 0x80);
    }
    Movie_SetDmaTag((MovieDmaTag *)stream->tags + i, (u32)stream->tags & EE_PHYSICAL_ADDRESS_MASK, 2, 0);
    *D4_QWC = 0;
    *D4_MADR = (u32)stream->data & EE_PHYSICAL_ADDRESS_MASK;
    *D4_TADR = (u32)stream->tags & EE_PHYSICAL_ADDRESS_MASK;
    Movie_SetD4Chcr(D_CHCR_DIR_FROM_MEMORY | D_CHCR_MOD_CHAIN);
    return TRUE;
}

/* Gives the one or two free stretches of the stream ring; two blocks stay unused (viBufBeginPut). */
static void MovieStream_BeginPut(MovieStreamBuffer *stream, u8 **first, int *first_size, u8 **second,
                                 int *second_size) {
    int write_pos;
    int until_end;
    int free_size;
    int reserved;

    WaitSema(stream->sema);
    reserved = stream->dma_count + 2;
    free_size = (stream->block_count - reserved) * 0x800 - stream->read_bytes;
    write_pos = ((stream->dma_start + stream->dma_count) * 0x800 + stream->read_bytes) % stream->data_size;
    until_end = stream->data_size - write_pos;
    if (until_end >= free_size) {
        *first = (u8 *)(MOVIE_STREAM_DATA_ADDRESS(stream) + write_pos);
        *first_size = free_size;
        *second = NULL;
        *second_size = 0;
    } else {
        *first_size = until_end;
        *first = (u8 *)(MOVIE_STREAM_DATA_ADDRESS(stream) + write_pos);
        *second = (u8 *)MOVIE_STREAM_DATA_ADDRESS(stream);
        *second_size = free_size - (stream->data_size - write_pos);
    }
    SignalSema(stream->sema);
}

/* Accounts for size bytes written to the stream ring (viBufEndPut). */
static void MovieStream_EndPut(MovieStreamBuffer *stream, int size) {
    WaitSema(stream->sema);
    stream->read_bytes += size;
    stream->total_bytes = size + stream->total_bytes;
    SignalSema(stream->sema);
}

/* Queues the whole blocks written since the last call for DMA to the IPU and restarts the channel (viBufAddDMA). */
static int MovieStream_AddDma(MovieStreamBuffer *stream) {
    int i;
    int index;
    int tag_id;
    int last;
    u32 d4_chcr;
    int is_new_data = FALSE;
    int consumed;
    int read_start;
    int read_count;

    WaitSema(stream->sema);
    if (!stream->is_active) {
        Movie_DebugPrint("DMA ADD not active\n");
        return FALSE; // match: leaves without SignalSema, as the SDK sample does
    }
    /* stop the channel; D4_CHCR then holds the tag it stopped on */
    Movie_SetD4Chcr(D_CHCR_DIR_FROM_MEMORY | D_CHCR_MOD_CHAIN);
    d4_chcr = *D4_CHCR;
    index = MovieStream_GetBlockIndex(stream, *D4_MADR);
    consumed = (index + stream->block_count - stream->dma_start) % stream->block_count;
    stream->dma_start = (stream->dma_start + consumed) % stream->block_count;
    stream->dma_count -= consumed;
    read_start = (stream->dma_start + stream->dma_count) % stream->block_count;
    read_count = stream->read_bytes / 0x800;
    stream->read_bytes %= 0x800;
    if (read_count > 0) {
        /* the tag that ended the chain becomes an ordinary one */
        last = (stream->dma_start + stream->dma_count - 1 + stream->block_count) % stream->block_count;
        Movie_SetDmaTag((MovieDmaTag *)stream->tags + last, (u32)(stream->data + last * 0x800), 3, 0x80);
        is_new_data = TRUE;
    }
    index = read_start;
    for (i = 0; i < read_count; i++) {
        tag_id = (i == read_count - 1) ? 0 : 3;
        Movie_SetDmaTag((MovieDmaTag *)stream->tags + index, (u32)(stream->data + index * 0x800), tag_id, 0x80);
        index = (index + 1) % stream->block_count;
    }
    stream->dma_count += read_count;
    if (stream->dma_count != 0) {
        if (is_new_data) {
            d4_chcr = (d4_chcr & ~D_CHCR_TAG_ID_BITS) | EE_UNCACHED_ACCELERATED_ALIAS;
        }
        Movie_SetD4Chcr(d4_chcr | D_CHCR_STR);
    }
    SignalSema(stream->sema);
    return TRUE;
}

/* Stops the stream DMA and the IPU input and keeps their registers for MovieStream_RestartDma (viBufStopDMA). */
static int MovieStream_StopDma(MovieStreamBuffer *stream) {
    WaitSema(stream->sema);
    stream->is_active = FALSE;
    Movie_SetD4Chcr(D_CHCR_DIR_FROM_MEMORY | D_CHCR_MOD_CHAIN);
    stream->saved.d4_madr = *D4_MADR;
    stream->saved.d4_tadr = *D4_TADR;
    stream->saved.d4_qwc = *D4_QWC;
    stream->saved.d4_chcr = *D4_CHCR;
    while (*IPU_CTRL & IPU_CTRL_OFC_MASK) {
    }
    Movie_SetD3Chcr(0);
    stream->saved.d3_madr = *D3_MADR;
    stream->saved.d3_qwc = *D3_QWC;
    stream->saved.d3_chcr = *D3_CHCR;
    stream->saved.ipu_bp = *IPU_BP;
    stream->saved.ipu_ctrl = *IPU_CTRL;
    SignalSema(stream->sema);
    return TRUE;
}

/* whether block index is one of count blocks from start in a ring of size */
#define IS_IN_REGION(index, start, count, size)     (((index) + (size) - (start)) % (size) >= 0 && ((index) + (size) - (start)) % (size) < (count))

/* Restarts the stream DMA and the IPU from the registers saved when they were stopped (viBufRestartDMA). */
static int MovieStream_RestartDma(MovieStreamBuffer *stream) {
    s32 bit_position = stream->saved.ipu_bp & IPU_BP_BIT_POSITION_MASK;
    s32 fifo_pending = (stream->saved.ipu_bp >> IPU_BP_FP_SHIFT) & IPU_BP_FP_MASK;
    s32 fifo_count = (stream->saved.ipu_bp >> IPU_BP_IFC_SHIFT) & IPU_BP_IFC_MASK;
    u32 d4_madr_next = stream->saved.d4_madr - ((fifo_pending + fifo_count) << 4);
    u32 d4_qwc_next = stream->saved.d4_qwc + (fifo_pending + fifo_count);
    u32 d4_tadr_next = stream->saved.d4_tadr;
    u32 d4_chcr_next = stream->saved.d4_chcr | D_CHCR_STR;
    s32 tag_id;
    s32 index;
    s32 index_next;

    WaitSema(stream->sema);
    if (d4_madr_next < (u32)stream->data) {
        d4_qwc_next = ((u32)stream->data - d4_madr_next) >> 4;
        d4_madr_next += stream->block_count << 11;
        d4_tadr_next = (u32)stream->tags & EE_PHYSICAL_ADDRESS_MASK;
        tag_id = (stream->saved.d4_madr == (u32)stream->data ||
                  stream->saved.d4_madr == (u32)stream->data + (stream->block_count << 11))
                     ? 0
                     : 3;
        d4_chcr_next = (stream->saved.d4_chcr & ~D_CHCR_TAG_ID_BITS) | (tag_id << 28) | D_CHCR_STR;
        if (!IS_IN_REGION(0, stream->dma_start, stream->dma_count, stream->block_count)) {
            stream->dma_start = stream->block_count - 1;
            stream->dma_count++;
        }
    } else {
        index = MovieStream_GetBlockIndex(stream, stream->saved.d4_madr);
        index_next = MovieStream_GetBlockIndex(stream, d4_madr_next);
        if (index != index_next) {
            d4_qwc_next = ((u32)stream->data + (index << 11) - d4_madr_next) >> 4;
            d4_tadr_next = ((index << 4) + (u32)stream->tags) & EE_PHYSICAL_ADDRESS_MASK;
            tag_id = ((u32)stream->data + (stream->saved.d4_madr - (u32)stream->data) % (stream->block_count << 11) ==
                      (u32)stream->data + (((stream->dma_start + stream->dma_count) % stream->block_count) << 11))
                         ? 0
                         : 3;
            d4_chcr_next = (stream->saved.d4_chcr & ~D_CHCR_TAG_ID_BITS) | (tag_id << 28) | D_CHCR_STR;
            if (!IS_IN_REGION(index_next, stream->dma_start, stream->dma_count, stream->block_count)) {
                stream->dma_start = index_next;
                stream->dma_count++;
            }
        }
    }
    if (stream->saved.d3_madr != 0 && stream->saved.d3_qwc != 0) {
        *D3_MADR = stream->saved.d3_madr;
        *D3_QWC = stream->saved.d3_qwc;
        Movie_SetD3Chcr(stream->saved.d3_chcr | D_CHCR_STR);
    }
    if (stream->dma_count != 0) {
        while (*IPU_CTRL < 0) {
        }
        *IPU_CMD = bit_position;
        while (*IPU_CTRL < 0) {
        }
    }
    *D4_MADR = d4_madr_next;
    *D4_TADR = d4_tadr_next;
    *D4_QWC = d4_qwc_next;
    if (stream->dma_count != 0) {
        Movie_SetD4Chcr(d4_chcr_next);
    }
    *IPU_CTRL = stream->saved.ipu_ctrl;
    stream->is_active = TRUE;
    SignalSema(stream->sema);
    return 1;
}

/* Stops DMA channel 4, clears its registers and deletes the stream's semaphore (viBufDelete). */
static int MovieStream_Delete(MovieStreamBuffer *stream) {
    Movie_SetD4Chcr(D_CHCR_DIR_FROM_MEMORY | D_CHCR_MOD_CHAIN);
    *D4_QWC = 0;
    *D4_MADR = 0;
    *D4_TADR = 0;
    DeleteSema(stream->sema);
    return 1;
}

/* Whether the stream DMA is running (viBufIsActive); nothing calls it. */
int MovieStream_IsActive(MovieStreamBuffer *stream) {
    s32 is_active;

    WaitSema(stream->sema);
    is_active = stream->is_active;
    SignalSema(stream->sema);
    return is_active;
}

/* Bytes in the stream ring not yet taken by the IPU: the queued blocks and what was written after them
   (viBufCount). */
static int MovieStream_GetCount(MovieStreamBuffer *stream) {
    int count;

    WaitSema(stream->sema);
    count = (stream->dma_count << 11) + stream->read_bytes;
    SignalSema(stream->sema);
    return count;
}

/* Pads the bytes not queued yet to a whole block, so the last of the stream gets sent (viBufFlush). */
static void MovieStream_Flush(MovieStreamBuffer *stream) {
    WaitSema(stream->sema);
    stream->read_bytes = (stream->read_bytes + 0x7FF) / 0x800 * 0x800;
    SignalSema(stream->sema);
}

/* Whether one of the queued time stamps covers the stream position stop. */
static int MovieStream_IsStampAt(MovieStreamBuffer *stream, s32 stop) {
    s32 data_size = stream->block_count << 11;
    s32 is_end = FALSE;
    s32 stamp_count = stream->stamp_count;
    s32 found = FALSE;
    s32 stamp_write = stream->stamp_write;
    s32 index;

    if (stamp_count > 0) {
        for (index = 0; index < stamp_count && !is_end; index++) {
            s32 read = (stamp_write - stamp_count + stream->stamp_capacity + index) % stream->stamp_capacity;

            if ((stop + data_size - stream->stamps[read].pos) % data_size < stream->stamps[read].len) {
                is_end = TRUE;
                found = TRUE;
            }
        }
    }
    return found;
}

/* Shortens or drops the queued time stamps that the data of a new one overwrites (viBufModifyPts). */
static int MovieStream_TrimStamps(MovieStreamBuffer *stream, MovieTimeStamp *new_stamp) {
    MovieTimeStamp *stamp;
    s32 read = (stream->stamp_write - stream->stamp_count + stream->stamp_capacity) % stream->stamp_capacity;
    s32 data_size = stream->block_count << 11;
    s32 keep_going = TRUE;

    if (stream->stamp_count > 0) {
        while (keep_going) {
            stamp = stream->stamps + read;
            if (stamp->len == 0 || new_stamp->len == 0) {
                break;
            }
            if (MovieStream_IsStampAt(stream, new_stamp->pos)) {
                if (stamp->pts >= 0) {
                    Movie_DebugPrint("pts is not used(a)\n");
                    stamp->pts = -1;
                    stamp->dts = -1;
                    stamp->pos = 0;
                    stamp->len = 0;
                }
                stream->stamp_count = (stream->stamp_count - 1 >= 0) ? stream->stamp_count - 1 : 0;
            } else if ((stamp->pos + data_size - new_stamp->pos) % data_size < new_stamp->len) {
                s32 overlap = new_stamp->pos + new_stamp->len - stamp->pos;

                stamp->pos = new_stamp->pos + new_stamp->len;
                stamp->len -= overlap;
                if (stamp->len == 0) {
                    if (stamp->pts >= 0) {
                        Movie_DebugPrint("pts is not used(b)\n");
                        stamp->pts = -1;
                        stamp->dts = -1;
                        stamp->pos = 0;
                        stamp->len = 0;
                    }
                    stream->stamp_count = (stream->stamp_count - 1 >= 0) ? stream->stamp_count - 1 : 0;
                }
            } else {
                keep_going = FALSE;
            }
            read = (read + 1) % stream->stamp_capacity;
        }
    }
    return 0;
}

/* Queues a time stamp, after trimming those its data overwrites; FALSE when the queue is full (viBufPutTs). */
static int MovieStream_PutStamp(MovieStreamBuffer *stream, MovieTimeStamp *stamp) {
    int is_queued = FALSE;
    MovieTimeStamp *slot;

    WaitSema(stream->sema);
    if (stream->stamp_count < stream->stamp_capacity) {
        MovieStream_TrimStamps(stream, stamp);
        is_queued = TRUE;
        slot = (MovieTimeStamp *)(stream->stamp_write * sizeof(MovieTimeStamp) + (int)stream->stamps);
        slot->pos = stamp->pos;
        slot->pts = stamp->pts;
        stream->stamps[stream->stamp_write].len = stamp->len;
        slot->dts = stamp->dts;
        stream->stamp_count++;
        stream->stamp_write = (stream->stamp_write + 1) % stream->stamp_capacity;
    }
    SignalSema(stream->sema);
    return is_queued;
}

/* Takes the time stamp of the stream data the IPU is decoding now out of the queue (viBufGetTs). */
static int MovieStream_GetStamp(MovieStreamBuffer *stream, MovieTimeStamp *stamp_out) {
    u32 d4_madr = *D4_MADR;
    u32 ipu_bp = *IPU_BP;
    s32 bit_position = stream->saved.ipu_bp & IPU_BP_BIT_POSITION_MASK;
    s32 fifo_pending = (ipu_bp >> IPU_BP_FP_SHIFT) & IPU_BP_FP_MASK;
    s32 fifo_count = (ipu_bp >> IPU_BP_IFC_SHIFT) & IPU_BP_IFC_MASK;
    u32 d4_madr_next = d4_madr - ((fifo_pending + fifo_count) << 4);
    s32 data_size = stream->block_count << 11;
    s32 is_end = FALSE;
    s32 skipped = 0;
    s32 stop;
    s32 stamp_count;
    s32 stamp_write;
    s32 index;

    WaitSema(stream->sema);
    stamp_out->pts = -1;
    stamp_out->dts = -1;
    stop = (d4_madr_next + (bit_position >> 3) + data_size - (u32)stream->data) % data_size;
    stamp_count = stream->stamp_count;
    stamp_write = stream->stamp_write;
    if (stamp_count > 0) {
        for (index = 0; index < stamp_count && !is_end; index++) {
            s32 read = (stamp_write - stamp_count + stream->stamp_capacity + index) % stream->stamp_capacity;

            if ((stop + data_size - stream->stamps[read].pos) % data_size < stream->stamps[read].len) {
                if (stream->stamps[read].pts >= 0) {
                    stamp_out->pts = stream->stamps[read].pts;
                    stamp_out->dts = stream->stamps[read].dts;
                    stream->stamps[read].pts = -1;
                    stream->stamps[read].dts = -1;
                }
                is_end = TRUE;
            } else {
                skipped++;
            }
        }
        stream->stamp_count = stamp_count - (stamp_count < skipped ? stamp_count : skipped);
    }
    SignalSema(stream->sema);
    return 1;
}

/* Creates the libmpeg decoder in its work area with the player's callbacks, and the stream ring over block_count
   blocks at data (videoDecCreate). */
static int MovieVideo_Init(MovieVideoDecoder *decoder, u8 *work_area, int work_area_size, u8 *data,
                           MovieDmaTag *tags, int block_count, MovieTimeStamp *stamps, int stamp_capacity) {
    sceMpegCreate((sceMpeg *)decoder, work_area, work_area_size);
    sceMpegAddCallback((sceMpeg *)decoder, 0, Movie_OnMpegError, NULL);
    sceMpegAddCallback((sceMpeg *)decoder, 1, Movie_OnMpegNoData, NULL);
    sceMpegAddCallback((sceMpeg *)decoder, 2, Movie_OnMpegStopDma, NULL);
    sceMpegAddCallback((sceMpeg *)decoder, 3, Movie_OnMpegRestartDma, NULL);
    sceMpegAddCallback((sceMpeg *)decoder, 4, Movie_OnMpegBackground, NULL);
    sceMpegAddCallback((sceMpeg *)decoder, 5, (sceMpegCallback)Movie_OnMpegTimeStamp, NULL);
    MovieVideo_ResetState(decoder);
    MovieStream_Init(&decoder->stream, data, tags, block_count, stamps, stamp_capacity);
    return 1;
}

/* Calls sceMpegSetDecodeMode with whatever the argument registers hold (videoDecSetDecodeMode); nothing calls it. */
void MovieVideo_SetDecodeMode(void) {
    sceMpegSetDecodeMode();
}

/* Registers the callback that receives one elementary stream of the movie (videoDecSetStream). */
static int MovieVideo_SetStream(MovieVideoDecoder *decoder, int type, int channel, sceMpegCallback callback,
                                void *data) {
    sceMpegAddStrCallback((sceMpeg *)decoder, type, channel, callback, data);
    return 1;
}

/* Gives the two free stretches of the decoder's stream buffer (videoDecBeginPut). */
static void MovieVideo_BeginPut(MovieVideoDecoder *decoder, u8 **first, s32 *first_size, u8 **second,
                                s32 *second_size) {
    MovieStream_BeginPut(&decoder->stream, first, first_size, second, second_size);
}

/* Tells the decoder's stream buffer how many bytes were written to it (videoDecEndPut). */
static void MovieVideo_EndPut(MovieVideoDecoder *decoder, s32 size) {
    MovieStream_EndPut(&decoder->stream, size);
}

/* Sets the decoder's state to running. */
static s32 MovieVideo_ResetState(MovieVideoDecoder *decoder) {
    decoder->state = MOVIE_VIDEO_STATE_RUNNING;
    return 0;
}

/* Shuts the video decoder down: its stream buffer, then the libmpeg handle (videoDecDelete). */
static int MovieVideo_Delete(MovieVideoDecoder *decoder) {
    MovieStream_Delete(&decoder->stream);
    sceMpegDelete((sceMpeg *)decoder);
    return 1;
}

/* Has the decode loop stop (videoDecAbort). */
static s32 MovieVideo_Abort(MovieVideoDecoder *decoder) {
    decoder->state = MOVIE_VIDEO_STATE_ABORT;
    return 0;
}

/* The decoder's state (videoDecGetState). */
static int MovieVideo_GetState(MovieVideoDecoder *decoder) {
    return decoder->state;
}

/* Sets the decoder's state and returns the one before (videoDecSetState). */
static s32 MovieVideo_SetState(MovieVideoDecoder *decoder, s32 state) {
    s32 old_state;

    old_state = decoder->state;
    decoder->state = state;
    return old_state;
}

/* Queues the time stamp of the stream data written at start (videoDecPutTs). */
static int MovieVideo_PutStamp(MovieVideoDecoder *decoder, s64 pts, s64 dts, u8 *start, s32 len) {
    MovieTimeStamp stamp;

    stamp.pts = pts;
    stamp.dts = dts;
    stamp.pos = start - decoder->stream.data;
    stamp.len = len;
    return MovieStream_PutStamp(&movie_decoder.stream, &stamp); // match: the global buffer, not the decoder's own (the same one)
}

/* Bytes of stream data the decoder has not consumed yet (videoDecInputCount). */
static int MovieVideo_GetInputCount(MovieVideoDecoder *decoder) {
    return MovieStream_GetCount(&decoder->stream);
}

/* Free bytes in the decoder's stream buffer (videoDecInputSpaceCount); nothing calls it. */
int MovieVideo_GetInputSpace(MovieVideoDecoder *decoder) {
    u8 *first;
    s32 first_size;
    u8 *second;
    s32 second_size;

    MovieVideo_BeginPut(decoder, &first, &first_size, &second, &second_size);
    return first_size + second_size;
}

/* the MPEG sequence end code MovieVideo_Flush puts after the stream */
static const MovieEndCode sequence_end_code IN_RODATA = { { 0x00, 0x00, 0x01, 0xB7 } };

/* Ends the stream: puts a sequence end code after the data, pads it to a whole block and marks the decoder
   flushed. Returns 0 while there is no room for the end code (videoDecFlush). */
static int MovieVideo_Flush(MovieVideoDecoder *decoder) {
    MovieEndCode end_code = sequence_end_code;
    u8 *first;
    s32 first_size;
    u8 *second;
    s32 second_size;
    int copied;

    MovieVideo_BeginPut(decoder, &first, &first_size, &second, &second_size);
    if (first_size + second_size < 4) {
        return 0;
    }
    copied = MovieVideo_CopyTwoAreas((u8 *)(((u32)first & EE_PHYSICAL_ADDRESS_MASK) | EE_UNCACHED_ALIAS), first_size,
                                     (u8 *)(((u32)second & EE_PHYSICAL_ADDRESS_MASK) | EE_UNCACHED_ALIAS), second_size, end_code.bytes, 4,
                                     NULL, 0);
    MovieVideo_EndPut(&movie_decoder, copied);
    MovieStream_Flush(&decoder->stream);
    if (decoder->state == MOVIE_VIDEO_STATE_RUNNING) {
        decoder->state = MOVIE_VIDEO_STATE_FLUSHED;
    }
    return 1;
}

/* Whether the decoder has used up everything it was given (videoDecIsFlushed). */
static int MovieVideo_IsFlushed(MovieVideoDecoder *decoder) {
    return MovieVideo_GetInputCount(decoder) == 0 && sceMpegIsRefBuffEmpty((sceMpeg *)decoder);
}

/* The decode thread: empties the stream buffer and the picture ring, decodes the stream, waits for the last
   picture to be shown and marks the decoder ended (videoDecMain). */
static void MovieVideo_Main(void *decoder_argument) {
    MovieVideoDecoder *decoder = decoder_argument;

    MovieStream_Reset(&decoder->stream);
    MovieOut_Reset(&movie_out_buffer);
    MovieVideo_DecodeAll(decoder);
    while (movie_out_buffer.count != 0) {
    }
    MovieVideo_SetState(decoder, MOVIE_VIDEO_STATE_ENDED);
}

/* Decodes pictures into the picture ring until the stream ends; -1 when the decoder was told to stop
   (decodeOneFrame, looped). */
static int MovieVideo_DecodeAll(MovieVideoDecoder *decoder) {
    int result = 1;
    MovieOutPicture *picture;

    while (!sceMpegIsEnd((sceMpeg *)decoder)) {
        if (MovieVideo_GetState(decoder) == MOVIE_VIDEO_STATE_ABORT) {
            result = -1;
            break;
        }
        while ((picture = MovieOut_GetWriteSlot(&movie_out_buffer)) == NULL) {
            Movie_Yield();
        }
        if (sceMpegGetPicture((sceMpeg *)decoder, (u32)picture, 0x460) < 0) {
            Movie_DebugPrint("sceMpegGetPicture() decode error");
        }
        MovieOut_Commit(&movie_out_buffer);
        Movie_Yield();
    }
    sceMpegReset((sceMpeg *)decoder);
    return result;
}

/* libmpeg callback 0 (an error): nothing to do. */
static int Movie_OnMpegError(sceMpeg *mpeg, void *callback_data, void *user_data) {
    return 1;
}

/* libmpeg callback 1 (the decoder ran out of data): send sound, yield, queue what the stream buffer holds for
   DMA (mpegNodata). */
static int Movie_OnMpegNoData(sceMpeg *mpeg, void *callback_data, void *user_data) {
    Movie_PumpAudio();
    Movie_Yield();
    MovieStream_AddDma(&movie_decoder.stream);
    return 1;
}

/* libmpeg callback 2: stop the stream DMA (mpegStopDMA). */
static int Movie_OnMpegStopDma(sceMpeg *mpeg, void *callback_data, void *user_data) {
    MovieStream_StopDma(&movie_decoder.stream);
    return 1;
}

/* libmpeg callback 3: restart the stream DMA (mpegRestartDMA). */
static int Movie_OnMpegRestartDma(sceMpeg *mpeg, void *callback_data, void *user_data) {
    MovieStream_RestartDma(&movie_decoder.stream);
    return 1;
}

/* libmpeg callback 4 (background work while decoding): send sound (mpegBackground). */
static int Movie_OnMpegBackground(sceMpeg *mpeg, void *callback_data, void *user_data) {
    Movie_PumpAudio();
    return 1;
}

/* libmpeg callback 5: give the time stamp of the picture being decoded (mpegTS). */
static int Movie_OnMpegTimeStamp(sceMpeg *mpeg, MovieStampCallbackData *callback_data, void *user_data) {
    MovieTimeStamp stamp;

    MovieStream_GetStamp(&movie_decoder.stream, &stamp);
    callback_data->pts = stamp.pts;
    callback_data->dts = stamp.dts;
    return 1;
}

/* Copies a source in two stretches into a destination in two stretches; returns the bytes copied, 0 if they do
   not fit. The same code as Movie_CopyTwoAreas. */
static int MovieVideo_CopyTwoAreas(u8 *dest0, s32 dest0_size, u8 *dest1, s32 dest1_size, u8 *source0,
                                   s32 source0_size, u8 *source1, s32 source1_size) {
    if (dest0_size + dest1_size < source0_size + source1_size) {
        return 0;
    }
    if (source0_size >= dest0_size) {
        memcpy(dest0, source0, dest0_size);
        memcpy(dest1, source0 + dest0_size, source0_size - dest0_size);
        memcpy(dest1 + source0_size - dest0_size, source1, source1_size);
    } else if (source1_size >= dest0_size - source0_size) {
        memcpy(dest0, source0, source0_size);
        memcpy(dest0 + source0_size, source1, dest0_size - source0_size);
        memcpy(dest1, source1 + dest0_size - source0_size, source1_size - (dest0_size - source0_size));
    } else {
        memcpy(dest0, source0, source0_size);
        memcpy(dest0 + source0_size, source1, source1_size);
    }
    return source0_size + source1_size;
}

/* Sends waiting sound to the IOP; returns 0. */
static int Movie_PumpAudio(void) {
    MovieAudio_SendToIop(&movie_audio);
    return 0;
}

/* Sets up the ring of decoded pictures over size picture and tag slots, all empty (voBufCreate). */
static int MovieOut_Init(MovieOutBuffer *buffer, MovieOutPicture *data, MovieOutTag *tag, s32 size) {
    s32 index;

    buffer->data = data;
    buffer->tag = tag;
    buffer->size = size;
    buffer->count = 0;
    buffer->write = 0;
    for (index = 0; index < size; index++) {
        buffer->tag[index].status = MOVIE_OUT_STATUS_EMPTY;
    }
    return 0;
}

/* Frees the picture ring: nothing to do (voBufDelete). */
static void MovieOut_Delete(MovieOutBuffer *buffer) {
}

/* Empties the ring of decoded pictures (voBufReset). */
static s32 MovieOut_Reset(MovieOutBuffer *buffer) {
    buffer->count = 0;
    buffer->write = 0;
    return 0;
}

/* Whether every slot holds a picture (voBufIsFull). */
static s32 MovieOut_IsFull(MovieOutBuffer *buffer) {
    return buffer->count == buffer->size;
}

/* Marks the slot being written as holding a picture and moves on to the next slot (voBufIncCount). */
static void MovieOut_Commit(MovieOutBuffer *buffer) {
    DIntr();
    buffer->tag[buffer->write].status = MOVIE_OUT_STATUS_FULL;
    buffer->count++;
    buffer->write = (buffer->write + 1) % buffer->size;
    EIntr();
}

/* Returns the slot the next picture is decoded into, or NULL when the ring is full (voBufGetData). */
static MovieOutPicture *MovieOut_GetWriteSlot(MovieOutBuffer *buffer) {
    return MovieOut_IsFull(buffer) ? NULL : buffer->data + buffer->write;
}

/* Whether no picture waits (voBufIsEmpty). */
static s32 MovieOut_IsEmpty(MovieOutBuffer *buffer) {
    return buffer->count == 0;
}

/* Returns the tag of the oldest picture waiting, or NULL when the ring is empty (voBufGetTag). */
static MovieOutTag *MovieOut_GetOldestTag(MovieOutBuffer *buffer) {
    return MovieOut_IsEmpty(buffer) ? NULL : buffer->tag + (buffer->write - buffer->count + buffer->size) % buffer->size;
}

/* Drops the oldest picture waiting (voBufDecCount). */
static void MovieOut_DropOldest(MovieOutBuffer *buffer) {
    if (buffer->count > 0) {
        buffer->count--;
    }
}

#define ROUND_UP(value, unit) ((((value) + (unit) - 1) / (unit)) * (unit))

/* Sets the movie's display up: two fields of half the height, one after the other in GS memory. */
static void MovieDisplay_Init(MovieDisplay *display, s32 screen_x, s32 screen_y, s32 width, s32 height,
                              s32 picture_width, s32 picture_height) {
    s32 field_height = height / 2;
    s32 buffer_width = (width + 63) / 64;
    s32 packet_start;
    s32 packet;

    movie_pictures_enabled = 0;
    movie_picture_vblanks = 0;
    movie_picture_shown = 0;
    movie_start_field = 0;
    movie_path3_busy = 0;
    display->screen_x = screen_x;
    display->width = width;
    display->frame_base[1] = (buffer_width * 64 * ROUND_UP(field_height, 32)) >> 11;
    display->height = height;
    display->picture_width = picture_width;
    display->picture_height = picture_height;
    display->frame_base[0] = 0;
    display->screen_y = screen_y;
    sceGsResetGraph(0, 1, movie_thread.video_mode, 1);
    sceGsSetDefDispEnv((sceGsDispEnv *)display, 0, width, field_height, movie_thread.display_x, movie_thread.display_y);
    display->dispfb = (display->dispfb & ~GS_DISPFB_FBW_MASK) | ((buffer_width & GS_FBW_MASK) << GS_DISPFB_FBW_SHIFT);
    FlushCache(0);
    sceGsPutDispEnv(display);
    packet_start = (u32)movie_init_display_packet & EE_PHYSICAL_ADDRESS_MASK;
    packet = GsPacket_AddGifTag(packet_start | EE_UNCACHED_ALIAS, 0xE, 1, 0, 0, 0, 1, 6);
    packet = GsPacket_AddPrmodecont(packet, 1);
    packet = GsPacket_AddFrame(packet, display->frame_base[0], ROUND_UP(display->width, 64) / 64, 0, 0);
    packet = GsPacket_AddTest(packet, 0, 0, 0, 0, 0, 0, 0, 0);
    packet = GsPacket_AddXyOffset(packet, 0, 0);
    packet = GsPacket_AddScissor(packet, 0, display->width - 1, 0, display->height - 1);
    packet = GsPacket_AddClamp(packet, 1, 1, 0, 0, 0, 0);
    *D2_MADR = packet_start;
    *D2_QWC = 7;
    *D2_CHCR = D_CHCR_STR | D_CHCR_DIR_FROM_MEMORY;
    sceGsSyncPath(0, 0);
}

/* Frees the display: nothing to do. */
static void MovieDisplay_Delete(MovieDisplay *display) {
}

/* Builds the GIF chain that shows a decoded picture on one field: for the first field it uploads the picture as
   a texture, one 16x16 macroblock at a time, and for both it draws the texture over the field's frame buffer. */
static void MovieDisplay_BuildFieldPackets(MovieDisplay *display, u32 packet_start, u32 picture, s32 field) {
    s32 screen_rect[4];
    s32 texture_rect[4];
    s32 picture_width = display->picture_width;
    s32 picture_height = display->picture_height;
    s32 width = display->width;
    s32 field_height = display->height / 2;
    s32 rows = picture_height >> 4;
    s32 columns = picture_width >> 4;
    s32 packet = (packet_start & EE_PHYSICAL_ADDRESS_MASK) | EE_UNCACHED_ALIAS;
    s32 field_pages = (field_height + 31) / 32;      /* a field's frame buffer, in rows of GS pages */
    s32 buffer_width = ROUND_UP(width, 64) / 64;
    s32 buffer_pixels = buffer_width * 64;
    s32 texture_base = field_pages * buffer_pixels;  /* the texture comes after the two frame buffers */
    s32 column;
    s32 row;

    if (field == 0) {
        screen_rect[0] = display->screen_x << 4;
        screen_rect[1] = display->screen_y << 4;
        screen_rect[2] = width << 4;
        screen_rect[3] = field_height << 4;
        texture_rect[0] = 8;
        texture_rect[1] = 0x18;
        texture_rect[2] = picture_width << 4;
        texture_rect[3] = picture_height << 4;
    } else {
        screen_rect[0] = display->screen_x << 4;
        screen_rect[1] = (display->screen_y + field_pages * 32) << 4;
        screen_rect[2] = width << 4;
        screen_rect[3] = field_height << 4;
        texture_rect[1] = texture_rect[0] = 8;
        texture_rect[2] = picture_width << 4;
        texture_rect[3] = picture_height << 4;
    }
    if (field == 0) {
        packet = GsPacket_AddDmaTag(packet, 0, 0, 0, 1, 0, 3);
        packet = GsPacket_AddGifTag(packet, 0xE, 1, 0, 0, 0, 0, 2);
        packet = GsPacket_AddBitbltbuf(packet, texture_base, buffer_width, 0);
        packet = GsPacket_AddTrxreg(packet, 16, 16);
        for (column = 0; column < columns; column++) {
            for (row = 0; row < rows; row++) {
                packet = GsPacket_AddDmaTag(packet, 0, 0, 0, 1, 0, 4);
                packet = GsPacket_AddGifTag(packet, 0xE, 1, 0, 0, 0, 0, 2);
                packet = GsPacket_AddTrxpos(packet, 0, column << 4, row << 4);
                packet = GsPacket_AddTrxdir(packet, 0);
                packet = GsPacket_AddGifTag(packet, 0, 0, 2, 0, 0, 0, 0x40);
                packet = GsPacket_AddDmaTag(packet, 0, picture & EE_PHYSICAL_ADDRESS_MASK, 0, 3, 0, 0x40);
                picture += 0x400;
            }
        }
    }
    packet = GsPacket_AddDmaTag(packet, 0, 0, 0, 7, 0, 9);
    packet = GsPacket_AddGifTag(packet, 0xE, 1, 0, 0, 0, 1, 8);
    packet = GsPacket_AddTexflush(packet);
    packet = GsPacket_AddTex0(packet, texture_base, buffer_width, 0, 10, 10, 0, 1, 0, 0, 0, 0, 0);
    packet = GsPacket_AddTex1(packet, 0, 0, 1, 1, 0, 0, 0);
    GsPacket_AddTexturedSprite(packet, screen_rect, texture_rect);
}

/* Shows one of the two fields' frame buffers. */
static void MovieDisplay_ShowField(MovieDisplay *display, s32 second) {
    s32 frame_base;

    if (second) {
        frame_base = display->frame_base[1];
    } else {
        frame_base = display->frame_base[0];
    }
    display->dispfb = (display->dispfb & ~GS_DISPFB_FBP_MASK) | (frame_base & GS_DISPFB_FBP_MASK);
    sceGsPutDispEnv(display);
}

/* Fills both fields' frame buffers with one color. */
static void MovieDisplay_Clear(MovieDisplay *display, u32 color) {
    s32 rect[4];
    s32 packet_start = (u32)movie_clear_packet & EE_PHYSICAL_ADDRESS_MASK;
    s32 packet;

    rect[0] = 0;
    rect[1] = 0;
    rect[2] = display->width << 4;
    rect[3] = (((display->height + 15) / 16) * 16 * 2) << 4;
    packet = GsPacket_AddGifTag(packet_start | EE_UNCACHED_ALIAS, 0xE, 1, 0, 0, 0, 1, 6);
    packet = GS_PACKET_ADD_SCISSOR_32(packet, 0, display->width, 0, display->height << 1);
    packet = GsPacket_AddColorSprite(packet, rect, color);
    GS_PACKET_ADD_SCISSOR_32(packet, 0, display->width - 1, 0, display->height - 1);
    *D2_MADR = packet_start;
    *D2_QWC = 7;
    *D2_CHCR = D_CHCR_STR | D_CHCR_DIR_FROM_MEMORY;
    sceGsSyncPath(0, 0);
}

/* Adds a textured sprite: screen_rect and texture_rect are x, y, width, height. */
static int GsPacket_AddTexturedSprite(int packet, s32 *screen_rect, s32 *texture_rect) {
    return GsPacket_AddXyz2(
        GsPacket_AddUv(
            GsPacket_AddXyz2(GsPacket_AddUv(GsPacket_AddPrim(packet, 6, 0, 1, 0, 0, 0, 1, 0, 0), texture_rect[0],
                                            texture_rect[1]),
                             screen_rect[0], screen_rect[1], 0),
            texture_rect[0] + texture_rect[2], texture_rect[1] + texture_rect[3]),
        screen_rect[0] + screen_rect[2], screen_rect[1] + screen_rect[3], 0);
}

/* Adds a sprite of one color over rect (x, y, width, height), color as RGBA bytes. */
static int GsPacket_AddColorSprite(int packet, s32 *rect, u32 color) {
    return GsPacket_AddXyz2(
        GsPacket_AddRgbaq(GsPacket_AddXyz2(GsPacket_AddPrim(packet, 6, 0, 0, 0, 0, 0, 1, 0, 0), rect[0], rect[1], 0),
                          color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF, color >> 24, 0),
        rect[0] + rect[2], rect[1] + rect[3], 0);
}

/* Writes a DMA tag at address packet (one doubleword; the other half of its quadword is left alone). Returns the
   address after the quadword. */
static int GsPacket_AddDmaTag(int packet, u32 spr, u32 addr, u32 irq, u32 tag_id, u32 pce, u32 qwc) {
    u64 *tag = (u64 *)packet;

    *tag = ((u64)spr << 63) | ((u64)(addr & ~0xF) << 32) | ((u64)irq << 31) | ((u64)tag_id << 28) | ((u64)pce << 26) | qwc;
    return (int)(tag + 2);
}

/* Writes a GIF tag at address packet: the register list, then NLOOP, EOP, PRE, PRIM, FLG and NREG. Returns the
   address after it. */
static int GsPacket_AddGifTag(int packet, s64 regs, s32 nreg, s32 flg, s32 prim, s32 pre, s32 eop, s32 nloop) {
    u32 *words = (u32 *)packet;

    words[0] = (eop << 15) | nloop;
    words[1] = (pre << 14) | (prim << 15) | (flg << 26) | (nreg << 28);
    words[2] = regs & 0xFFFFFFFF;
    words[3] = regs >> 32;
    return (int)(words + 4);
}

/* Writes one GS register setting at address packet (64-bit value, then register number, as four words) and
   returns the address for the next one. */
static int GsPacket_AddReg(int packet, s32 reg, s64 value) {
    u32 *words = (u32 *)packet;

    words[0] = value & 0xFFFFFFFF;
    words[1] = value >> 32;
    words[2] = reg;
    words[3] = 0;
    return (int)(words + 4);
}

/* Adds a TEXFLUSH. */
static int GsPacket_AddTexflush(int packet) {
    return GsPacket_AddReg(packet, GS_REG_TEXFLUSH, 0);
}

/* Adds a TEX1_1: texture filtering and mipmap settings. */
static int GsPacket_AddTex1(s64 packet, u32 lcm, u32 mxl, u32 mmag, u32 mmin, u32 mtba, u32 lod_l, u64 lod_k) {
    u64 low = ((u64)lcm) | (lod_k << 32);
    u64 middle = (((u64)mxl) << 2) | (((u64)mtba) << 9);
    u64 high = ((((u64)mmin) << 6) | (((u64)mmag) << 5)) | (((u64)lod_l) << 19);

    return GS_PACKET_ADD_REG_64(packet, GS_REG_TEX1_1, (low | middle) | high);
}

/* Adds a TEX0_1: the texture's place, size and format. */
static int GsPacket_AddTex0(s64 packet, u32 tbp0, u32 tbw, u32 psm, u32 tex_width_log2, u32 tex_height_log2,
                            u32 tcc, u32 tfx, u32 cbp, u32 cpsm, u32 csm, u32 csa, u32 cld) {
    u64 value;

    value = tbp0 | ((u64)tbw << 14) | ((u64)psm << 20) | ((u64)tex_width_log2 << 26) | ((u64)tex_height_log2 << 30) | ((u64)tcc << 34) | ((u64)tfx << 35) | ((u64)cbp << 37) | ((u64)cpsm << 51) | ((u64)csm << 55) | ((u64)csa << 56) | ((u64)cld << 61);
    return GS_PACKET_ADD_REG_64(packet, GS_REG_TEX0_1, value);
}

/* Adds a PRIM. */
static int GsPacket_AddPrim(int packet, u32 prim, u32 iip, u32 tme, u32 fge, u32 abe, u32 aa1, u32 fst, u32 ctxt,
                            u32 fix) {
    return GsPacket_AddReg(packet, GS_REG_PRIM, prim | ((u64)iip << 3) | ((u64)tme << 4) | ((u64)fge << 5) | ((u64)abe << 6) | ((u64)aa1 << 7) | ((u64)fst << 8) | ((u64)ctxt << 9) | ((u64)fix << 10));
}

/* Adds a UV: texel coordinates. */
static int GsPacket_AddUv(int packet, u32 tex_u, u32 tex_v) {
    return GsPacket_AddReg(packet, GS_REG_UV, tex_u | ((u64)tex_v << 16));
}

/* Adds an RGBAQ: the vertex color and q. */
static int GsPacket_AddRgbaq(s64 packet, u32 red, u32 green, u32 blue, u32 alpha, u64 q_value) {
    u64 low = ((u64)blue << 16) | ((u64)green << 8);
    u64 high = red | (q_value << 32);

    return GS_PACKET_ADD_REG_64(packet, GS_REG_RGBAQ, high | low | ((u64)alpha << 24));
}

/* Adds an XYZ2: a vertex, which draws. */
static int GsPacket_AddXyz2(int packet, u32 pos_x, u32 pos_y, u32 depth) {
    return GsPacket_AddReg(packet, GS_REG_XYZ2, pos_x | ((u64)pos_y << 16) | ((u64)depth << 32));
}

/* Adds a FRAME_1: the frame buffer's base, width, format and write mask. */
static int GsPacket_AddFrame(s64 packet, u32 fbp, u32 fbw, u32 psm, u64 fbmsk) {
    u64 low = fbp | (fbmsk << 32);
    u64 high = ((u64)psm << 24) | ((u64)fbw << 16);

    return GS_PACKET_ADD_REG_64(packet, GS_REG_FRAME_1, low | high);
}

/* Adds a TEST_1: alpha, destination alpha and depth tests. */
static int GsPacket_AddTest(int packet, u32 ate, u32 atst, u32 aref, u32 afail, u32 date, u32 datm, u32 zte,
                            u32 ztst) {
    return GsPacket_AddReg(packet, GS_REG_TEST_1, ate | ((u64)atst << 1) | ((u64)aref << 4) | ((u64)afail << 12) | ((u64)date << 14) | ((u64)datm << 15) | ((u64)zte << 16) | ((u64)ztst << 17));
}

/* Adds a SCISSOR_1: the drawing area, x from scax0 to scax1 and y from scay0 to scay1. */
static int GsPacket_AddScissor(s64 packet, u32 scax0, u32 scax1, u32 scay0, u64 scay1) {
    u64 low = scax0 | (scay1 << 48);
    u64 high = ((u64)scay0 << 32) | ((u64)scax1 << 16);

    return GS_PACKET_ADD_REG_64(packet, GS_REG_SCISSOR_1, low | high);
}

/* Adds an XYOFFSET_1. */
static int GsPacket_AddXyOffset(int packet, u32 offset_x, u32 offset_y) {
    return GsPacket_AddReg(packet, GS_REG_XYOFFSET_1, offset_x | ((u64)offset_y << 32));
}

/* Adds a PRMODECONT. */
static int GsPacket_AddPrmodecont(int packet, u32 use_prim) {
    return GsPacket_AddReg(packet, GS_REG_PRMODECONT, (u64)use_prim);
}

/* Adds a PRMODE; nothing calls it. */
int GsPacket_AddPrmode(int packet, u32 iip, u32 tme, u32 fge, u32 abe, u32 aa1, u32 fst, u32 ctxt, u32 fix) {
    return GsPacket_AddReg(packet, GS_REG_PRMODE, ((u64)iip << 3) | ((u64)tme << 4) | ((u64)fge << 5) | ((u64)abe << 6) | ((u64)aa1 << 7) | ((u64)fst << 8) | ((u64)ctxt << 9) | ((u64)fix << 10));
}

/* Adds a CLAMP_1: the texture wrap modes and the clamp area. */
static int GsPacket_AddClamp(s64 packet, u64 wms, u64 wmt, u64 minu, u64 maxu, u64 minv, u64 maxv) {
    u64 low = ((wms << 32) >> 32) | (maxv << 34);
    u64 middle = ((maxu << 32) >> 18) | ((minu << 32) >> 28);
    u64 high = ((wmt << 32) >> 30) | ((minv << 32) >> 8);

    return GS_PACKET_ADD_REG_64(packet, GS_REG_CLAMP_1, low | middle | high);
}

/* Adds a BITBLTBUF: destination buffer of a transfer. */
static int GsPacket_AddBitbltbuf(int packet, u32 dest_base, u32 dest_width, u32 dest_format) {
    return GsPacket_AddReg(packet, GS_REG_BITBLTBUF, ((u64)dest_format << 56) | ((u64)dest_width << 48) | ((u64)dest_base << 32));
}

/* Adds a TRXPOS: destination corner of a transfer. */
static int GsPacket_AddTrxpos(int packet, u32 direction, u32 dest_x, u32 dest_y) {
    return GsPacket_AddReg(packet, GS_REG_TRXPOS, ((u64)direction << 59) | ((u64)dest_y << 48) | ((u64)dest_x << 32));
}

/* Adds a TRXREG: size of the rectangle to transfer. */
static int GsPacket_AddTrxreg(int packet, u32 width, u32 height) {
    return GsPacket_AddReg(packet, GS_REG_TRXREG, ((u64)height << 32) | width);
}

/* Adds a TRXDIR: starts a transfer. */
static int GsPacket_AddTrxdir(int packet, u32 direction) {
    return GsPacket_AddReg(packet, GS_REG_TRXDIR, (u64)direction);
}

/* the vblank handler's debug output in the original: compiled out, but its text is still in the file */
static const char field_names[] = "EVEN\0ODD ";
static const char vblank_format[] = "GS_CSR=%s vbct=%d isvb%d\n";

/* Starts the GIF DMA channel on a chain of packets. */
static void Movie_SendGifChain(u32 chain) {
    *D2_TADR = chain & EE_PHYSICAL_ADDRESS_MASK;
    *D2_QWC = 0;
    *D2_CHCR = D_CHCR_STR | D_CHCR_DIR_FROM_MEMORY | D_CHCR_MOD_CHAIN;
}

/* Vblank handler of the movie player: sends the waiting picture's packets for the field that starts. */
static int Movie_VblankHandler(int cause) {
    MovieOutTag *tag;
    s32 unused[4]; // match: retail keeps 0x10 bytes of stack it never touches

    movie_start_field = (*GS_CSR >> 13) & 1;
    if (movie_pictures_enabled != 0) {
        movie_picture_vblanks++;
        movie_path3_busy = sceGsSyncPath(1, 0);
        if (movie_path3_busy == 0) {
            tag = MovieOut_GetOldestTag(&movie_out_buffer);
            if (tag == NULL) {
                movie_idle_vblanks++;
                __asm__ volatile("sync");
                __asm__ volatile("ei");
                return 0;
            }
            if (movie_start_field == 0 && tag->status == MOVIE_OUT_STATUS_FULL) {
                MovieDisplay_ShowField(&movie_thread.display, 1);
                Movie_SendGifChain((u32)tag->field_packets[0]);
                tag->status = MOVIE_OUT_STATUS_SHOWN_EVEN;
            } else if (movie_start_field != 0 && tag->status == MOVIE_OUT_STATUS_SHOWN_EVEN) {
                MovieDisplay_ShowField(&movie_thread.display, 0);
                Movie_SendGifChain((u32)tag->field_packets[1]);
                tag->status = MOVIE_OUT_STATUS_EMPTY;
                movie_picture_shown = 1;
            }
        }
    }
    __asm__ volatile("sync");
    __asm__ volatile("ei");
    return 0;
}

/* GIF DMA end handler: once a picture has been shown on both fields, drops it from the ring. */
static int Movie_GifDmaHandler(int channel) {
    if (movie_pictures_enabled != 0 && movie_picture_shown != 0) {
        MovieOut_DropOldest(&movie_out_buffer);
        movie_picture_shown = 0;
    }
    __asm__ volatile("sync");
    __asm__ volatile("ei");
    return 0;
}

/* Waits for a vertical blank that starts the other field, then turns the vblank handler's showing on
   (startDisplay). */
static void Movie_StartDisplay(s32 field) {
    while (sceGsSyncV(0) == field) {
    }
    movie_idle_vblanks = 0;
    movie_pictures_enabled = 1;
    movie_picture_vblanks = 0;
}

/* Turns the vblank handler's showing off (endDisplay). */
static void Movie_StopDisplay(void) {
    movie_pictures_enabled = 0;
    movie_idle_vblanks = 0;
}

/* the short strings of the file functions: the game keeps them with the others in .rodata, where a literal of 8
   bytes or less would not go */
static const char empty_string[] IN_RODATA = "";
static const char cdrom_device[] IN_RODATA = "cdrom0";
static const char cd_version_suffix[] IN_RODATA = ";1";
static const char cd_path_format[] IN_RODATA = "%s%s";
static const char device_path_format[] IN_RODATA = "%s:%s";
static const char default_device[] IN_RODATA = "host0";

/* Loads an IOP module with no arguments; a failure ends the program (loadModule); nothing calls it. */
void Movie_LoadModule(const char *path) {
    if (sceSifLoadModule(path, 0, empty_string) < 0) {
        Exit(1);
    }
}

/* Opens a file, from the disc when the path starts with "cdrom" (any case), and gives its size; returns the
   descriptor, or a negative value when it cannot be opened or sized. Nothing calls it. */
int Movie_OpenFileWithSize(const char *path, int *size) {
    int file_fd;

    if ((path[0] == 'c' || path[0] == 'C') &&
        (path[1] == 'd' || path[1] == 'D') &&
        (path[2] == 'r' || path[2] == 'R') &&
        (path[3] == 'o' || path[3] == 'O') &&
        (path[4] == 'm' || path[4] == 'M')) {
        sceCdDiskReady(0);
        file_fd = sceOpen(path, 0);
    } else {
        file_fd = sceOpen(path, 1);
    }
    if (file_fd >= 0) {
        *size = sceLseek(file_fd, 0, 2);
        if (*size < 0 || sceLseek(file_fd, 0, 0) < 0) {
            sceClose(file_fd);
            return -1;
        }
        return file_fd;
    }
    return file_fd;
}

/* Opens the movie file: "cdrom0:" paths are streamed from the disc, anything else is read from its device
   (host0 by default) (strFileOpen). */
static s32 MovieFile_Open(MovieFile *file, char *path) {
    char full_path[0x100];
    char device[0x40];
    sceCdRMode mode;
    char *name;
    s32 device_length;
    s32 name_length;
    s32 i;
    char *suffix;

    name = index(path, ':');
    if (name != NULL) {
        device_length = name - path;
        strncpy(device, path, device_length);
        name++;
        device[device_length] = '\0';
        if (strcmp(device, cdrom_device) == 0) {
            name_length = strlen(name);
            file->is_cd_stream = TRUE;
            for (i = 0; i < name_length; i++) {
                if (name[i] == '/') {
                    name[i] = '\\';
                }
                name[i] = toupper(name[i]);
            }
            suffix = index(path, ';') ? (char *)empty_string : (char *)cd_version_suffix;
            sprintf(full_path, (char *)cd_path_format, name, suffix);
        } else {
            file->is_cd_stream = FALSE;
            sprintf(full_path, (char *)device_path_format, device, name);
        }
    } else {
        memcpy(device, default_device, 6);
        file->is_cd_stream = FALSE;
        sprintf(full_path, (char *)device_path_format, device, path);
    }
    if (file->is_cd_stream) {
        sceCdDiskReady(0);
        file->iop_buffer = sceSifAllocIopHeap(0x28010);
        if (file->iop_buffer == 0) {
            printf("SifAllocIopHeap Error\n");
            return 0;
        }
        if (sceCdStInit(0x50, 5, ((u32)(file->iop_buffer + 0xF) >> 4) << 4) != 1) {
            printf("sceCdStInit Error \n");
            if (sceSifFreeIopHeap(file->iop_buffer) < 0) {
                printf("sceSifFreeIopHeap Error %s %d\n", "fslibmpg.c", 0xE5B);
            }
            return 0;
        }
        if (!sceCdSearchFile((sceCdlFILE *)&file->lsn, full_path)) {
            printf("Cannot open '%s'(sceCdSearchFile)\n", full_path);
            while (!sceCdStStop()) {
            }
            if (sceSifFreeIopHeap(file->iop_buffer) < 0) {
                printf("sceSifFreeIopHeap Error %s %d\n", "fslibmpg.c", 0xE63);
            }
            return 0;
        }
        file->size = file->cd_size;
        mode.trycount = 0;
        mode.spindlctrl = 0;
        mode.datapattern = 0;
        while (!sceCdStStart(file->lsn, &mode)) {
        }
    } else {
        file->file_fd = sceOpen(full_path, 1);
        if (file->file_fd < 0) {
            printf("Cannot open '%s'(sceOpen)\n", full_path);
            return 0;
        }
        file->size = sceLseek(file->file_fd, 0, 2);
        if (file->size < 0) {
            printf("sceLseek() fails (%s): %d\n", full_path, file->size);
            sceClose(file->file_fd);
            return 0;
        }
        if (sceLseek(file->file_fd, 0, 0) < 0) {
            printf("sceLseek() fails (%s)\n", full_path);
            sceClose(file->file_fd);
            return 0;
        }
    }
    return 1;
}

/* Closes the movie file: stops the disc stream and frees its buffer, or closes the host file (strFileClose). */
static s32 MovieFile_Close(MovieFile *file) {
    if (file->is_cd_stream) {
        while (!sceCdStStop()) {
        }
        if (sceSifFreeIopHeap(file->iop_buffer) < 0) {
            printf("sceSifFreeIopHeap Error %s %d\n", "fslibmpg.c", 0xEB0);
        }
    } else {
        sceClose(file->file_fd);
    }
    return 1;
}

/* Reads the next stretch of the movie file; a disc stream that reports an error is restarted or sought back to
   lsn (strFileRead). Returns the bytes read. */
static s32 MovieFile_Read(MovieFile *file, u32 *buffer, s32 size, s32 lsn) {
    sceCdRMode mode;
    u32 error;
    s32 count;

    if (file->is_cd_stream) {
        do {
            count = sceCdStRead(size >> 11, buffer, 1, &error);
            if (error == 0) {
                break;
            }
            printf("sceCdStRead Error count=%d err=%d\n", count, error);
            if (error == 1) {
                printf("sceCdStStop\n");
                while (!sceCdStStop()) {
                }
                mode.trycount = 0;
                mode.spindlctrl = 0;
                mode.datapattern = 0;
                while (!sceCdStStart(lsn, &mode)) {
                }
            } else {
                printf("sceCdDiskReady...\n");
                sceCdDiskReady(0);
                printf("sceCdStSeek... lsn=%d\n", lsn);
                while (!sceCdStSeek(lsn)) {
                }
                printf("retry...\n");
            }
        } while (error != 0);
        count <<= 11;
    } else {
        do {
            count = sceRead(file->file_fd, buffer, size);
            if (count < 0) {
                printf("read error retry %d\n", count);
            }
        } while (count < 0);
    }
    return count;
}
