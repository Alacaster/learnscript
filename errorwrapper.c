#include "errorwrapper.h"

// SETH if String Equal Then Return
// Currently fails if you change the console output buffer
//TODO: update to use WriteConsole instead

void winErrorCheck8(int line, ...);
void winErrorCheck16(int line, ...);
int getFunctionID8(char *name);
int getFunctionID16(wchar_t *name);
char *validateFunctionName8(char *hi);
wchar_t *validateFunctionName16(wchar_t *hi);

#define SWITCHSELECTION winErrorCheck
#define test(testt) \
    if (testt)      \
    errtrue = 1

void winErrorCheck8(int line, ...)
{
    va_list a;
    va_start(a, line);
    char *filename = va_arg(a, char *);
    long long int value = *((long long int *)va_arg(a, void *));
    char *functionname = va_arg(a, char *);
    va_end(a);

    if (!(functionname = validateFunctionName8(functionname)))
    {
        fprintf(stderr, "\nE MACRO FAILED TO VERIFY FUNCTON NAME STRING. REALLY YOU SHOULDN'T EVER SEE THIS MESSAGE");
        exit(0);
    }
    int errtrue = 0;
    int funcid = getFunctionID8(functionname);
    if (funcid == -1)
    {
        fprintf(stderr, "\n\nFAILED TO ACERTAIN FUNCTION ID FOR %s\nRead errorwrapper.c for more details!", functionname);
    }
#include "snippets/switches.c" //INCLUDE STUFF IMPORTANT LONG SPACER FILLER TEXT VISUAL REPRESENTATION
    if (!errtrue)
        return;
    // the following is the default case
    DWORD error = GetLastError();
    LPSTR message;
    int chars;
    if (0 == (chars = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_USER_DEFAULT, SUBLANG_DEFAULT), (LPSTR)&message, 0, NULL)))
    {
        printf("\nINVALID ERROR MESSAGE FATAL");
        exit(0);
    }
    fprintf_s(stderr, "Bad return value is indicating error in %d: %s\n\n%s", line, filename, message);
    LocalFree(message);
    return;
}

void winErrorCheck16(int line, ...)
{
    va_list a;
    va_start(a, line);
    wchar_t *filename = va_arg(a, wchar_t *);
    long long int value = *((long long int *)va_arg(a, void *));
    wchar_t *functionname = va_arg(a, wchar_t *);
    va_end(a);

    if (!(functionname = validateFunctionName16(functionname)))
    {
        fwprintf(stderr, L"\nE MACRO FAILED TO VERIFY FUNCTON NAME STRING. REALLY YOU SHOULDN'T EVER SEE THIS MESSAGE");
        exit(0);
    }
    int errtrue = 0;
    int funcid = getFunctionID16(functionname);
    if (funcid == -1)
    {
        fwprintf(stderr, L"\n\nFAILED TO ACERTAIN FUNCTION ID FOR %ls\nRead errorwrapper.c for more details!", functionname);
    }
#include "snippets/switches.c" //INCLUDE STUFF IMPORTANT LONG SPACER FILLER TEXT VISUAL REPRESENTATION
    if (!errtrue)
        return;
    DWORD error = GetLastError();
    LPWSTR message;
    int chars;
    if (0 == (chars = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_USER_DEFAULT, SUBLANG_DEFAULT), (LPWSTR)&message, 0, NULL)))
    {
        printf("\nINVALID ERROR MESSAGE FATAL");
        exit(0);
    }
    fwprintf_s(stderr, L"Bad return value is indicating error in %d: %ls\n\n%ls", line, filename, message);
    LocalFree(message);
    // here is the default case
    return;
}

#undef SWITCHSELECTION
#define SWITCHSELECTION getFunctionID
#define SETH(funcname, ID)                       \
    if (!custom_strncmp(name, #funcname, i - 1)) \
    {                                            \
        return ID;                               \
    }

int getFunctionID8(char *name)
{ // do a wee bit of hashing
    int i = 0;
    int sum = 0;
    while (name[i] != '(')
    {
        sum += (int)name[i++];
    }
    sum %= 20;
#include "snippets/switches.c"
    return -1;
}
#undef SETH
#define SETH(funcname, ID)                            \
    if (!custom_wstrncmp_interum(name, #funcname, i)) \
    {                                                 \
        return ID;                                    \
    }
#define custom_wstrncmp_interum(a, b, c) custom_wstrncmp(a, L##b, c - 1)

int getFunctionID16(wchar_t *name)
{
    int i = 0;
    int sum = 0;
    while (name[i] != u'(')
    {
        sum += (int)name[i++];
    }
    sum %= 20;
#include "snippets/switches.c"
    return -1;
}

char *validateFunctionName8(char *hi)
{ // re-returns the pointer but skipping white space, idk why.
    int i = -1;
    int start;
    while (hi[++i] == ' ')
        ; // inc i if character is space
    start = i;
    if (!__iscsymf(hi[i]))
    {
        return NULL;
    } // test first character
    do
    {
        i++;
    } while (__iscsym(hi[i]));
    if (hi[i] != '(')
    {
        return NULL;
    }
    return (char *)((long long)hi + start);
}

wchar_t *validateFunctionName16(wchar_t *hi)
{
    int i = -1;
    int start;
    while (hi[++i] == u' ')
        ; // inc i if character is space
    start = i;
    if (!__iswcsymf(hi[i]))
    {
        return NULL;
    } // test first character
    do
    {
        i++;
    } while (__iswcsym(hi[i]));
    if (hi[i] != u'(')
    {
        return NULL;
    }
    return (wchar_t *)((long long)hi + start);
}
