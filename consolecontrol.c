#include "consolecontrol.h"
#include "fuwautils.h"
#include "errorwrapper.h"

static COORD virtualcursor = {0, 0};
typedef wchar_t bufferlayer_t[CONSOLEWIDTH][CONSOLEHEIGHT];
bufferlayer_t inputbuf;
bufferlayer_t outputbuf;

__attribute__ ((always_inline))
inline void input(_In_ PINPUT_RECORD x){
    if(x->Event.KeyEvent.bKeyDown == TRUE)
        return;
}