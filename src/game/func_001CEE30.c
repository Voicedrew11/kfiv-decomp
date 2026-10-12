#include "common.h"

int func_001CBAE0();

/* Sends driver command 0x30C0 for a stream with two values; returns the stream, or -1 when the driver refuses. */
int func_001CEE30(int stream, int first_value, int second_value) {
    return (func_001CBAE0(1, 0x30C0, stream, first_value, second_value) == 0) ? stream : -1;
}
