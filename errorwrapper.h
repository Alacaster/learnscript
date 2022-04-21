// It will tell u if a function has been defined, edit switches.c to add more cases.
// Needs to come after UNICODE and whatnot
#ifndef ERRORWRAPPER
#define ERRORWRAPPER

#include "fuwautils.h"
#include <wchar.h>
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>

void winErrorCheck8(int line, ...);
void winErrorCheck16(int line, ...);

#ifndef REMOVE_ERROR_WRAPPERS

#ifdef UNICODE
#define E E_16
#else
#define E E_8
#endif

#define E_8(function) ({union {typeof(function) temp; long long int temp0;} a; a.temp0 = 0; a.temp = function; winErrorCheck8(__LINE__, __FILE__, (void*)&a.temp, #function); a.temp; })
// one day this looming error "ISO C forbids braced-groups within expressions" will be resolved
#define E_16(function) ({union {typeof(function) temp; long long int temp0;} a; a.temp0 = 0; a.temp = function; winErrorCheck16_interum(__LINE__, __FILE__, (void*)&a.temp, #function); a.temp; })
#define winErrorCheck16_interum(a, b, c, d) winErrorCheck16_interum2(a, b, c, d)
#define winErrorCheck16_interum2(a, b, c, d) winErrorCheck16(a, L##b, c, L##d)

#else
#define E
#endif
#endif