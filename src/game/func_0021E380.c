#include "kf_movie.h"
#include <libmpeg.h>

extern MovieStreamBuffer D_00437980;

int func_0021DC90(MovieStreamBuffer *, MovieTimeStamp *);

/* libmpeg callback 5: give the time stamp of the picture being decoded. */
int func_0021E380(sceMpeg *mpeg, MovieStampCallbackData *callback_data, void *user_data) {
    MovieTimeStamp stamp;

    func_0021DC90(&D_00437980, &stamp);
    callback_data->pts = stamp.pts;
    callback_data->dts = stamp.dts;
    return 1;
}
