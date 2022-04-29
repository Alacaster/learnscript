#include "fuwautils.h"

void waitForKey()
{
    HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
    FlushConsoleInputBuffer(in);
    INPUT_RECORD a;
    DWORD g;
    while (1)
    {
        ReadConsoleInputW(in, &a, 1, &g);
        if (a.EventType == KEY_EVENT)
        {
            return;
        }
    }
}

int custom_strncmp(char *positiveString, char *negativeString, size_t size)
{
    int diff = 0;
    for (int i = 0; (size_t)i < size; i++)
    {
        diff += positiveString[i] - negativeString[i];
    }
    return diff;
}

int custom_wstrncmp(wchar_t *positiveLString, wchar_t *negativeLString, size_t size)
{
    int diff = 0;
    for (int i = 0; (size_t)i < size; i++)
    {
        diff += positiveLString[i] - negativeLString[i];
    }
    return diff;
}

static long long int initrand64(); //the first call to rand64 will set the seed and calculate
static long long int randrand64(); //the necessary shift constant then reroute the pointer permanently
static long long int goodrand64(); //hardware random number generator
static char nRandNeeded;
static char cbitsOfRand;
long long int (*rand64)() = &initrand64;

static long long int initrand64(){
    register unsigned int ecx asm ("ecx");
    asm volatile(
        "movq $1, %%rax\n\t"
        "cpuid"
        : "+r" (ecx)
        :: "eax", "ebx", "edx", "cc"
    );
    if(ecx & bit_RDRND){
        rand64 = &goodrand64;
    }else{
        rand64 = &randrand64;
        cbitsOfRand = (int)ceil(log2(RAND_MAX));
        nRandNeeded = 64 / cbitsOfRand + 1;
    }
    return rand64();
}
static long long int randrand64(){
    long long int theNumber = 0;
    for(int i = 0; i < nRandNeeded; i++){
        theNumber |= (rand() << (cbitsOfRand * i));
    }
    return theNumber;
}
static long long int goodrand64(){
    long long int r;
    asm volatile("rdrand %0":"=r"(r)::"cc");
    return r;
}

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
}

long long countbits(long long x){
        x -= ((x >> 1) & 0x5555555555555555);
        x = (((x >> 2) & 0x3333333333333333) + (x & 0x3333333333333333));
        x = (((x >> 4) + x) & 0x0f0f0f0f0f0f0f0f);
        x += (x >> 8);
        x += (x >> 16);
        x += (x >> 32);
        return(x & 0xff);
}