#ifndef CONSOLECONTROL
#define CONSOLECONTROL

#define UNICODE
#define CONSOLEWIDTH 80
#define CONSOLEHEIGHT 20

#include <windows.h>

__attribute__ ((always_inline))
inline void input(_In_ PINPUT_RECORD x);


#endif