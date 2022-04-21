#include "main.h"

restore_t restore;
HANDLE in;
HANDLE out = 0;
TCHAR testBuf[10];
#define testBufSize sizeof(testBuf)/sizeof(TCHAR)
HWND hconsole;

int main()
{
    setUpConsole();
    DWORD read;
    INPUT_RECORD ir;
    memset(&ir, 0, sizeof(ir));
    int meh = 0;
    while(1){
        ReadConsoleInput(in, &ir, 1, &read);
        switch(ir.EventType){
            case KEY_EVENT:
                break;
        }
    }
    restoreHandler(CTRL_C_EVENT);
}

void setUpConsole(){
    hconsole = GetConsoleWindow();
    restore = (restore_t){E(GetStdHandle(STD_OUTPUT_HANDLE)), 0, E(GetConsoleCP()), E(GetConsoleOutputCP()), {0, 0}, {sizeof(CONSOLE_FONT_INFOEX), 0, {0, 0}, 0, 0, {0}}, E(GetWindowLong(hconsole, GWL_STYLE))};
    E(GetConsoleMode(restore.outbuffer, &restore.mode));
    E(GetConsoleCursorInfo(restore.outbuffer, &restore.cursor));
    E(GetCurrentConsoleFontEx(restore.outbuffer, FALSE, &restore.font));
    E(SetWindowLong(hconsole, GWL_STYLE, restore.winlongstyle & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX));
    E(SetConsoleTitle(TEXT("Learnscript Shell -- ") TEXT(VERSION)));
    E(SetConsoleCP(65001));
    E(SetConsoleOutputCP(65001));
    E(SetConsoleCtrlHandler(&restoreHandler, TRUE));
    E(SetConsoleMode(in, ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT));
    in = E(GetStdHandle(STD_INPUT_HANDLE));
    out = E(CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL));
    CONSOLE_SCREEN_BUFFER_INFOEX x = {// initialize struct for window info
                                      sizeof(CONSOLE_SCREEN_BUFFER_INFOEX),
                                      {CONSOLEWIDTH, CONSOLEHEIGHT},
                                      {0, 0},
                                      FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
                                      {0, 0, 100, 50},
                                      {CONSOLEWIDTH, CONSOLEHEIGHT},
                                      0,
                                      FALSE,
                                      {-288680048, 32758, 16, 0, 0, 0, 0, 0, -1205726256, 603, -288680711, 32758, 1, 0, 65, 0,}};
    E(SetConsoleActiveScreenBuffer(out));
    E(SetConsoleScreenBufferInfoEx(out, &x));
}

BOOL WINAPI restoreHandler(DWORD dwCtrlType)
{
    if (dwCtrlType != CTRL_C_EVENT && dwCtrlType != CTRL_BREAK_EVENT)
    {
        return FALSE;
    }
    E(SetConsoleActiveScreenBuffer(restore.outbuffer));
    E(SetConsoleCP(restore.incodepage));
    E(SetConsoleOutputCP(restore.outcodepage));
    E(SetConsoleCursorInfo(restore.outbuffer, &restore.cursor));
    E(SetCurrentConsoleFontEx(restore.outbuffer, FALSE, &restore.font));
    E(SetConsoleMode(restore.outbuffer, restore.mode));
    ExitProcess(0);
}