#include "kf_movie.h"

int sceSdRemote(int, ...);

/* Mutes and stops the sound stream and empties its buffers (audioDecReset). */
int func_0021C5A8(MovieBuffers *audio) {
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
    audio->unk00 = MOVIE_AUDIO_STATE_INIT;
    audio->header_count = 0;
    audio->put = 0;
    audio->count = 0;
    audio->total_bytes = 0;
    audio->total_bytes_sent = 0;
    audio->iop_last_pos = 0;
    audio->iop_pause_pos = 0;
    return 0;
}
