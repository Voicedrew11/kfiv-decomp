#include "kf_movie.h"

extern MovieThreadState D_00437850;

int sceSdRemote(int, ...);

/* Starts the block transfer of the sound stream on the IOP and turns the volumes up (audioDecStart). */
int func_0021C448(MovieBuffers *audio) {
    sceSdRemote(1, 0x8010, 0x800, 0xC0);
    sceSdRemote(1, 0x8010, 0x801, 0xC);
    sceSdRemote(1, 0x80E0, 0, 0x10, audio->iop_buffer0, (audio->iop_buffer_size / 1024) * 1024);
    sceSdRemote(1, 0x8010, 0xF80, 0x7FFF);
    sceSdRemote(1, 0x8010, 0x1080, 0x7FFF);
    sceSdRemote(1, 0x8010, 0xF81, 0x7FFF);
    sceSdRemote(1, 0x8010, 0x1081, 0x7FFF);
    sceSdRemote(1, 0x8010, 0xD81, 0x7FFF);
    sceSdRemote(1, 0x8010, 0xE81, 0x7FFF);
    sceSdRemote(1, 0x8010, 0x980, D_00437850.unkD0);
    sceSdRemote(1, 0x8010, 0xA80, D_00437850.unkD0);
    sceSdRemote(1, 0x8010, 0x981, D_00437850.unkD0);
    sceSdRemote(1, 0x8010, 0xA81, D_00437850.unkD0);
    audio->unk00 = MOVIE_AUDIO_STATE_PLAY;
    return 0;
}
