#ifndef MAIN
#define MAIN

#define UNICODE

#define VERSION "alpha 0.1"
#include "errorwrapper.h"
#include "fuwautils.h"
#include "consolecontrol.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

BOOL WINAPI restoreHandler();
void setUpConsole();

typedef struct
{
        HANDLE outbuffer;
        DWORD mode;
        UINT incodepage;
        UINT outcodepage;
        CONSOLE_CURSOR_INFO cursor;
        CONSOLE_FONT_INFOEX font;
        CONSOLE_SCREEN_BUFFER_INFOEX info;
        LONG_PTR winlongstyle;
} restore_t;

typedef enum
{
        WIDE_CHAR = 0,
        NARROW_CHAR
} chartype_t;

#endif