#include "kf_movie.h"

extern MovieOutBuffer D_00437838;

int func_0021CFD0(MovieStreamBuffer *);
s32 func_0021E578(MovieOutBuffer *);
int func_0021E208(int);
s32 func_0021DFD8(char *, s32);

/* Empties the stream buffer, decodes what is left, waits for the last picture to be shown and ends the decoder. */
void func_0021E198(MovieVideoDecoder *decoder) {
    func_0021CFD0(&decoder->stream);
    func_0021E578(&D_00437838);
    func_0021E208((int)decoder); // match: func_0021E208 takes the decoder as an int
    while (D_00437838.count != 0) {
    }
    func_0021DFD8((char *)decoder, 3);
}
