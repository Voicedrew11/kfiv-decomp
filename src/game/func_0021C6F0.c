#include "kf_movie.h"

/* Gives the one or two stretches the next sound data goes in: the stream headers until they
   are complete, then the free part of the ring (audioDecBeginPut). */
void func_0021C6F0(MovieBuffers *audio, u8 **first, int *first_size, u8 **second, int *second_size) {
    int free_size;

    if (audio->unk00 == MOVIE_AUDIO_STATE_INIT) {
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
