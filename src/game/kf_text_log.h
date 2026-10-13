#ifndef KF_TEXT_LOG_H
#define KF_TEXT_LOG_H

#include "common.h"

void KF_TextLog_LoadFont(int font_base, int palette_base);
void KF_TextLog_SetWindow(int window_x, int window_y, int width, int height, int offset_x, int offset_y,
                          int screen_width, int screen_height, int depth);
void KF_TextLog_PutChar(char character, int pos_x, int pos_y, int pos_z);
void KF_TextLog_SendGsRegs(void);
void KF_TextLog_AppendV(char *format, char *args);
void KF_TextLog_Printf(char *format, ...);
void KF_TextLog_Flush(void);
void KF_TextLog_SetColor(int *colour);

#endif
