#define __STDC_WANT_LIB_EXT1__
#define UNICODE
#include <stdio.h>
#include "errorwrapper.h"
#include "fuwawinutils.h"
#include "main.h"
#include <windows.h>

restore_t restore;
HANDLE testinbuffer;
HANDLE testbuffer;

int main(){
    restore = (restore_t){E(GetStdHandle(STD_OUTPUT_HANDLE)), 0, E(GetConsoleCP()), E(GetConsoleOutputCP()), {0, 0}, {sizeof(CONSOLE_FONT_INFOEX), 0, {0, 0}, 0, 0, {0}}};
    E(GetConsoleMode(restore.outbuffer, &restore.mode));
    E(GetConsoleCursorInfo(restore.outbuffer, &restore.cursor));
    E(GetCurrentConsoleFontEx(restore.outbuffer, FALSE, &restore.font));
    E(SetConsoleTitle(TEXT("Learnscript Shell -- ") TEXT(VERSION)));
    E(SetConsoleCP(65001));
    E(SetConsoleOutputCP(65001));
    E(SetConsoleCtrlHandler(&restoreHandler, TRUE));

    testinbuffer = E(GetStdHandle(STD_INPUT_HANDLE));
    testbuffer = E(CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL));
    CONSOLE_SCREEN_BUFFER_INFOEX x = { //initialize struct for window info
        sizeof(CONSOLE_SCREEN_BUFFER_INFOEX),
        {CONSOLEWIDTH, CONSOLEHEIGHT},
        {0, 0},
        0,
        {0, 0, 100, 50},
        {CONSOLEWIDTH, CONSOLEHEIGHT},
        0,
        FALSE,
        {0}
    };
    {
        CONSOLE_SCREEN_BUFFER_INFOEX y; //discard this struct
        E(GetConsoleScreenBufferInfoEx(testbuffer, &y)); //keep default formatting maybe?
        for(int i = 0; i < 16; i++)
            x.ColorTable[i] = y.ColorTable[i];
    }
    E(SetConsoleScreenBufferInfoEx(testbuffer, &x));
    E(SetConsoleActiveScreenBuffer(testbuffer));
    E(SetConsoleMode(testbuffer, ENABLE_WINDOW_INPUT | ENABLE_WINDOW_INPUT));

    //waitForKey();
    return 0;
}

BOOL WINAPI restoreHandler(DWORD dwCtrlType){
    if(dwCtrlType != CTRL_C_EVENT && dwCtrlType != CTRL_BREAK_EVENT){
        return FALSE;
    }
    E(SetConsoleActiveScreenBuffer(restore.outbuffer));
    E(SetConsoleCP(restore.incodepage));
    E(SetConsoleOutputCP(restore.outcodepage));
    E(SetConsoleCursorInfo(restore.outbuffer, &restore.cursor));
    E(SetCurrentConsoleFontEx(restore.outbuffer, FALSE, &restore.font));
    E(SetConsoleMode(restore.outbuffer, restore.mode));
    return TRUE;
}