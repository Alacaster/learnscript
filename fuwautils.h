#ifndef FUWAUTILS
#define FUWAUTILS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cpuid.h>
// warning this flushes the input buffer
void waitForKey();
int custom_strncmp(char *positiveString, char *negativeString, size_t size);
int custom_wstrncmp(wchar_t *positiveLString, wchar_t *negativeLString, size_t size);
extern long long int (*rand64)(void);
void printBits(size_t const size, void const * const ptr);

#endif