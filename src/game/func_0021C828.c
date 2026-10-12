#include "kf_movie.h"

int sceSdRemote(int, ...);
void func_0021C9E0(int *, int *, int *, int *, MovieBuffers *, int);
int func_0021CA88(u8 *, int, u8 *, int, u8 *, int, u8 *, int);

/* Sends whole 1024-byte blocks of waiting sound data to the free part of the IOP buffer;
   returns the bytes sent (audioDecSendToIOP). */
int func_0021C828(MovieBuffers *audio) {
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

    switch (audio->unk00) {
        case MOVIE_AUDIO_STATE_INIT:
            return 0;
        case MOVIE_AUDIO_STATE_PRESET:
            first_dest = audio->iop_buffer0 + audio->total_bytes_sent % audio->iop_buffer_size;
            first_dest_size = audio->iop_buffer_size - audio->total_bytes_sent;
            second_dest = 0;
            second_dest_size = 0;
            break;
        case MOVIE_AUDIO_STATE_PLAY:
            play_pos = sceSdRemote(1, 0x8100, 0, 0) & 0xFFFFFF;
            func_0021C9E0(&first_dest, &first_dest_size, &second_dest, &second_dest_size, audio,
                          play_pos - audio->iop_buffer0);
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
        sent = func_0021CA88((u8 *)first_dest, first_dest_size, (u8 *)second_dest, second_dest_size, first_src,
                             first_src_size, second_src, second_src_size);
    }
    audio->count -= sent;
    audio->total_bytes_sent += sent;
    audio->iop_last_pos = (audio->iop_last_pos + sent) % audio->iop_buffer_size;
    return sent;
}
