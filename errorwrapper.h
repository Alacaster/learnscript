// It will tell u if a function has been defined, edit switches.c to add more cases.
// Needs to come after UNICODE and whatnot
#ifndef ERRORWRAPPER
#define ERRORWRAPPER

#include <stddef.h>

void winErrorCheck8(int line, char *filename, void * value, char *functionname);
void winErrorCheck16(int line, wchar_t *filename, void * value, wchar_t *functionname);

#ifndef REMOVE_ERROR_WRAPPERS

#ifdef UNICODE
#define E E_16
#else
#define E E_8
#endif

#define E_8(function) ({typeof(function) __a_; __a_ = function; winErrorCheck8(__LINE__, __FILE__, (void*)&__a_, #function); __a_;})
// one day this looming error "ISO C forbids braced-groups within expressions" will be resolved
#define E_16(function) ({typeof(function) __a_; __a_ = function; winErrorCheck16_interum(__LINE__, __FILE__, (void*)&__a_, #function); __a_;})
#define winErrorCheck16_interum(a, b, c, d) winErrorCheck16_interum2(a, b, c, d)
#define winErrorCheck16_interum2(a, b, c, d) winErrorCheck16(a, L##b, c, L##d)

#else
#define E
#endif
#endif