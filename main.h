#ifndef MAIN
#define MAIN

#define CONSOLEWIDTH 140
#define CONSOLEHEIGHT 140
#define VERSION "alpha 0.1"
#include <windows.h>

BOOL WINAPI restoreHandler();
typedef struct{
        HANDLE outbuffer;
        DWORD mode;
        UINT incodepage;
        UINT outcodepage;
        CONSOLE_CURSOR_INFO cursor;
        CONSOLE_FONT_INFOEX font;
} restore_t;

#endif