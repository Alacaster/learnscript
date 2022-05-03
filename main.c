#include "main.h"

restore_t restore;
HANDLE in;
HANDLE out = 0;
HWND hconsole;

int main()
{
    setUpConsole();
    // DWORD read;
    INPUT_RECORD ir;
    memset(&ir, 0, sizeof(ir));
    /* while(1){
        ReadConsoleInput(in, &ir, 1, &read);
        switch(ir.EventType){
            case KEY_EVENT:
                break;
        }
    } */waitForKey();
    restoreHandler(CTRL_C_EVENT);
}

void setUpConsole(){
    hconsole = GetConsoleWindow();
    in = E(GetStdHandle(STD_INPUT_HANDLE));
    out = E(CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL));
    restore = (restore_t){
        E(GetStdHandle(STD_OUTPUT_HANDLE)), //outbuffer
        0, //mode
        E(GetConsoleCP()), //incodepage
        E(GetConsoleOutputCP()), //outcodepage
        {0, 0}, //cursor
        {sizeof(CONSOLE_FONT_INFOEX), 0, {0, 0}, 0, 0, {0}}, //font
        {sizeof(CONSOLE_SCREEN_BUFFER_INFOEX), {0, 0}, {0, 0}, 0, {0, 0, 0, 0}, {0, 0}, 0, 0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}, //info
        E(GetWindowLong(hconsole, GWL_STYLE))}; //winlongstyle
    CONSOLE_SCREEN_BUFFER_INFOEX x = {
        sizeof(CONSOLE_SCREEN_BUFFER_INFOEX),
        {80, 20},
        {0, 0},
        FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
        {0, 0, 100, 50},
        {80, 20},
        0,
        FALSE,
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
    E(GetConsoleMode(restore.outbuffer, &restore.mode));
    E(GetConsoleCursorInfo(restore.outbuffer, &restore.cursor));
    E(GetCurrentConsoleFontEx(restore.outbuffer, FALSE, &restore.font));
    E(GetConsoleScreenBufferInfoEx(restore.outbuffer, &restore.info));

    E(SetConsoleMode(in, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT));
    E(SetConsoleCP(65001));
    E(SetConsoleOutputCP(65001));
    E(SetWindowLong(hconsole, GWL_STYLE, restore.winlongstyle & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX));

    E(SetConsoleTitle(TEXT("Learnscript Shell -- ") TEXT(VERSION))); //title is automatically restored
    E(SetConsoleCtrlHandler(&restoreHandler, TRUE));
    x.wAttributes = restore.info.wAttributes;
    for(int i = 0; i<16; i++){
        x.ColorTable[i] = restore.info.ColorTable[i];
    }
    E(SetConsoleActiveScreenBuffer(out));
    E(SetConsoleScreenBufferInfoEx(out, &x));
}

BOOL WINAPI restoreHandler(DWORD dwCtrlType)
{
    if (dwCtrlType != CTRL_C_EVENT && dwCtrlType != CTRL_BREAK_EVENT)
    {
        return FALSE;
    }
    E(SetConsoleCP(restore.incodepage));
    E(SetConsoleOutputCP(restore.outcodepage));
    E(SetConsoleCursorInfo(restore.outbuffer, &restore.cursor));
    E(SetCurrentConsoleFontEx(restore.outbuffer, FALSE, &restore.font));
    E(SetConsoleMode(restore.outbuffer, restore.mode));
    E(SetWindowLong(hconsole, GWL_STYLE, restore.winlongstyle));
    E(SetConsoleScreenBufferInfoEx(restore.outbuffer, &restore.info));
    //E(SetConsoleActiveScreenBuffer(restore.outbuffer)); //idk i think whatever program should handle this themselves also it makes the window jitter because it forces an update and changes the console appearance
    ExitProcess(0);
}
