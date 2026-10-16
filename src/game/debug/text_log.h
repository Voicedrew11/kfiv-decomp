#ifndef KF_TEXT_LOG_H
#define KF_TEXT_LOG_H

#include "common.h"

void TextLog_LoadFont(int font_base, int palette_base);
void TextLog_SetWindow(int window_x, int window_y, int width, int height, int offset_x, int offset_y,
                          int screen_width, int screen_height, int depth);
void TextLog_PutChar(char character, int pos_x, int pos_y, int pos_z);
void TextLog_SendGsRegs(void);
void TextLog_AppendV(char *format, char *args);
void TextLog_Printf(char *format, ...);
void TextLog_Flush(void);
void TextLog_SetColor(int *colour);

#endif
