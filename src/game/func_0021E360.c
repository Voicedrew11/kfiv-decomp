#include "kf_movie.h"
#include <libmpeg.h>

int func_0021E500(void);

/* libmpeg callback 4: read more of the movie file. */
int func_0021E360(sceMpeg *mpeg, void *callback_data, void *user_data) {
    func_0021E500();
    return 1;
}
