/* The area the game loads its files into, which is also its sound heap. */
#include "common.h"

/* 0x249680 the file-load buffer; the sound heap starts at its 64-byte boundary */
char file_load_buffer[0x100000] = { 0 };

/* sound_heap_base names the heap's base byte: ((u32)sound_heap_base >> 6) << 6 is file_load_buffer */
asm(".globl sound_heap_base\nsound_heap_base = file_load_buffer + 0x3F\n");
