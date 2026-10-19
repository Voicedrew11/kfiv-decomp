/* The sound driver's RPC buffers: the argument block and the file name the stream calls send. */
#include "common.h"

u32 sound_rpc_args_pad[10] = { 0 };     /* 0x420918 the ten words before the argument block */
int sound_rpc_args[0x40] = { 0 };   /* 0x420940 the arguments of the sound RPC */
char sound_rpc_file_name[0x100] = { 0 }; /* 0x420A40 the file name the sound RPC sends */
