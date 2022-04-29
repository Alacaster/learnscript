#ifndef CONSOLECONTROL
#define CONSOLECONTROL

#define UNICODE

#include <windows.h>
#include <inttypes.h>
#include <varargs.h>

typedef enum {CONSOLE_INITIALIZED_SUCCESS, CONSOLE_ALREADY_INITIALIZED, CALLOC_FAILED} icbe_t;
extern void (*initializeConsoleBuffersError)(icbe_t value);
icbe_t initializeConsoleBuffers(HANDLE hhconout);


typedef int houtfield_t; //unfinished

typedef int hinfield_t;
typedef enum {LINEIN = 0x1, BOXIN = 0x2, MORE_FORMAT = 0x4, CENTER_FIELD = 0x8, FILL = 0x10, SCROLLER = 0x20, CENTER_JUSTIFY = 0x40, SELECTABLE = 0x80} inputfieldstyle_t;

typedef enum {INPUT_FIELD_CREATION_SUCCESS = 1, INPUT_FIELD_OVERLAP, MAX_INPUT_FIELDS_REACHED, BAD_INPUT_FIELD_FORMAT} cife_t, fbe_t;
cife_t createInputField(_In_ inputfieldstyle_t a, _Out_ hinfield_t * b, ...);

typedef enum {INRECTS = 0, OUTRECTS} recttype_t;

//hstructure_t is an array of boxes that define an individual structure
typedef PSMALL_RECT hstructure_t;
typedef enum {HORIZONTAL_PARTITION, VERTICAL_PARTITION} structurestyle_t;

typedef enum {STRUCTURE_SUCCESS, STRUCTURE_OVERLAP} cbse_t;
cbse_t createBoxStructure(SMALL_RECT a)

#endif