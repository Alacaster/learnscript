#ifndef SWITCHES
#define SWITCHES
#define winErrorCheck 1
#define getFunctionID 2
#endif
#if SWITCHSELECTION == 1
switch (funcid) // use test() macro to use default windows error handler
{
case 0:
    test(value == 0);
    break;
case 1:
    test(*((HANDLE *)((void *)&value)) == INVALID_HANDLE_VALUE);
    break;
default:
    break;
}
#endif
#if SWITCHSELECTION == 2
switch (sum)
{
case 0:
    SETH(GetConsoleMode, 0) //added a comment
    SETH(GetWindowLong, 0)
    break;
case 1:
    break;
case 2:
    break;
case 3:
    SETH(ReadConsole, 0)
    break;
case 4:
    break;
case 5:
    SETH(GetConsoleCursorInfo, 0)
    break;
case 6:
    SETH(WriteConsole, 0)
    SETH(GetCurrentConsoleFontEx, 0)
    SETH(GetConsoleScreenBufferInfoEx, 0)
    break;
case 7:
    SETH(SetConsoleOutputCP, 0)
    break;
case 8:
    break;
case 9:
    SETH(CreateConsoleScreenBuffer, 1)
    break;
case 10:
    SETH(SetConsoleCP, 0)
    SETH(SetConsoleCtrlHandler, 0)
    break;
case 11:
    SETH(ReadConsoleInput, 0) 
    break;
case 12:
    SETH(SetConsoleMode, 0)
    SETH(SetWindowLong, 0)
    break;
case 13:
    break;
case 14:
    break;
case 15:
    SETH(GetStdHandle, 1)
    SETH(GetConsoleOutputCP, 0)
    break;
case 16:
    break;
case 17:
    SETH(GetConsoleScreenBufferInfo, 0)
    SETH(SetConsoleTitle, 0)
    SETH(SetConsoleActiveScreenBuffer, 0)
    SETH(SetConsoleCursorInfo, 0)
    SETH(FindWindow, 0)
    break;
case 18:
    SETH(GetConsoleCP, 0)
    SETH(SetConsoleScreenBufferInfoEx, 0)
    SETH(SetCurrentConsoleFontEx, 0)
    break;
case 19:
    break;
default:
    break;
}
#endif

// Language: c