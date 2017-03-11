/*
*   The body prefix file of the voc(jet backend) runtime system, Version 1.0
*
*   Copyright (c) Software Templ, 1994, 1995
*
*   Module SYSTEM is subject to change any time without prior notification.
*   Software Templ disclaims all warranties with regard to module SYSTEM,
*   in particular shall Software Templ not be liable for any damage resulting
*   from inappropriate use or modification of module SYSTEM.
*
*   Version 1.1  jt, 24.11.95  fixes for correct pointer arithmetic on Cray computers
*   jt 31.1.2007  ANSI prototypes for malloc and exit in order to avoid cc warnings
*
*/

#include "SYSTEM.h"
#include "stdarg.h"
#include <signal.h>
#include <stdlib.h>


INTEGER SYSTEM_ArgCount;
void   *SYSTEM_ArgVector;
void  (*SYSTEM_AssertFailHandler)(INTEGER code);
void  (*SYSTEM_HaltHandler)(INTEGER code);
void   *SYSTEM_MainStackFrame; /* adr of main proc stack frame, used for stack collection */


LONGINT SYSTEM_XCHK(LONGINT i, LONGINT ub) {return __X(i, ub);}
LONGINT SYSTEM_RCHK(LONGINT i, LONGINT ub) {return __R(i, ub);}
INTEGER SYSTEM_ASH (INTEGER x, INTEGER n)  {return __ASH(x, n, INTEGER);}
LONGINT SYSTEM_ASHL(LONGINT x, INTEGER n)  {return __ASH(x, n, LONGINT);}
LONGINT SYSTEM_ABS (LONGINT i)             {return __ABS(i);}
double  SYSTEM_ABSD(double i)              {return __ABS(i);}

void SYSTEM_INHERIT(LONGINT *t, LONGINT *t0)
{
    t -= __TPROC0OFF;
    t0 -= __TPROC0OFF;
    while (*t0 != __EOM) {*t = *t0; t--; t0--;}
}


void SYSTEM_ENUMP(void *adr, LONGINT n, void (*P)())
{
    while (n > 0) {
        P((LONGINT)(SYSTEM_ADR)(*((void**)(adr))));
        adr = ((void**)adr) + 1;
        n--;
    }
}

void SYSTEM_ENUMR(void *adr, LONGINT *typ, LONGINT size, LONGINT n, void (*P)())
{
    LONGINT *t, off;
    typ++;
    while (n > 0) {
        t = typ;
        off = *t;
        while (off >= 0) {P(*(LONGINT*)((char*)adr+off)); t++; off = *t;}
        adr = ((char*)adr) + size;
        n--;
    }
}

LONGINT SYSTEM_DIV(U_LONGINT x, U_LONGINT y)
{   if ((LONGINT) x >= 0) return (x / y);
    else return -((y - 1 - x) / y);
}

LONGINT SYSTEM_MOD(U_LONGINT x, U_LONGINT y)
{   U_LONGINT m;
    if ((LONGINT) x >= 0) return (x % y);
    else { m = (-x) % y;
        if (m != 0) return (y - m); else return 0;
    }
}

INTEGER SYSTEM_ENTIER(LONGREAL x)
{
    INTEGER i;
    i = (INTEGER)x;
    if (i > x) i--;
    return i;
}

LONGINT SYSTEM_ENTIERL(LONGREAL x)
{
    LONGINT i;
    i = (LONGINT)x;
    if (i > x) i--;
    return i;
}

extern void Heap_Lock();
extern void Heap_Unlock();


void SYSTEM_HALT(INTEGER code)
{
  if (SYSTEM_HaltHandler!=0) SYSTEM_HaltHandler(code);
  exit(code);
}

void SYSTEM_ASSERT_FAIL(INTEGER code)
{
  if (SYSTEM_AssertFailHandler!=0) SYSTEM_AssertFailHandler(code);
  exit(code);
}


// Program startup

extern void Heap_InitHeap();

void SYSTEM_INIT(INTEGER argc, void *argvadr)
{
  SYSTEM_MainStackFrame = argvadr;
  SYSTEM_ArgCount = argc;
  SYSTEM_ArgVector = *(void**)argvadr;
  SYSTEM_AssertFailHandler = 0;
  SYSTEM_HaltHandler = 0;
  // This function (SYSTEM_INIT) is called at program startup BEFORE any
  // modules have been initalized. In turn we must initialize the heap
  // before module startup (xxx__init) code is run.
  Heap_InitHeap();
}

SYSTEM_PTR SYSTEM_NEWARR(LONGINT *typ, LONGINT elemsz, int elemalgn, int nofdim, int nofdyn, ...)
{
    LONGINT nofelems, size, dataoff, n, nptr, *x, *p, nofptrs, i, *ptab, off;
    va_list ap;
    va_start(ap, nofdyn);
    nofelems = 1;
    while (nofdim > 0) {
        nofelems = nofelems * va_arg(ap, LONGINT); nofdim--;
        if (nofelems <= 0) __HALT(-20);
    }
    va_end(ap);
    dataoff = nofdyn * sizeof(LONGINT);
    if (elemalgn > sizeof(LONGINT)) {
        n = dataoff % elemalgn;
        if (n != 0) dataoff += elemalgn - n;
    }
    size = dataoff + nofelems * elemsz;
    Heap_Lock();
    if (typ == NIL) {
        /* element typ does not contain pointers */
        x = Heap_NEWBLK(size);
    }
    else if (typ == (LONGINT*)POINTER__typ) {
        /* element type is a pointer */
        x = Heap_NEWBLK(size + nofelems * sizeof(LONGINT));
        p = (LONGINT*)(SYSTEM_ADR)x[-1];
        p[-nofelems] = *p;  /* build new type desc in situ: 1. copy block size; 2. setup ptr tab; 3. set sentinel; 4. patch tag */
        p -= nofelems - 1; n = 1;   /* n =1 for skipping the size field */
        while (n <= nofelems) {*p = n*sizeof(LONGINT); p++; n++;}
        *p = - (nofelems + 1) * sizeof(LONGINT);    /* sentinel */
        x[-1] -= nofelems * sizeof(LONGINT);
    }
    else {
        /* element type is a record that contains pointers */
        ptab = typ + 1; nofptrs = 0;
        while (ptab[nofptrs] >= 0) {nofptrs++;} /* number of pointers per element */
        nptr = nofelems * nofptrs;  /* total number of pointers */
        x = Heap_NEWBLK(size + nptr * sizeof(LONGINT));
        p = (LONGINT*)(SYSTEM_ADR)x[- 1];
        p[-nptr] = *p;  /* build new type desc in situ; 1. copy block size; 2. setup ptr tab; 3. set sentinel; 4. patch tag */
        p -= nptr - 1; n = 0; off = dataoff;
        while (n < nofelems) {i = 0;
            while (i < nofptrs) {*p = off + ptab[i]; p++; i++;}
            off += elemsz; n++;
        }
        *p = - (nptr + 1) * sizeof(LONGINT);    /* sentinel */
        x[-1] -= nptr * sizeof(LONGINT);
    }
    if (nofdyn != 0) {
        /* setup len vector for index checks */
        va_start(ap, nofdyn);
        p = x;
        while (nofdyn > 0) {*p = va_arg(ap, LONGINT); p++, nofdyn--;}
        va_end(ap);
    }
    Heap_Unlock();
    return x;
}


// String comparison

int SYSTEM_STRCMP(CHAR *x, CHAR *y)
{
  int i = 0;
  CHAR ch1, ch2;
  do {ch1 = x[i]; ch2 = y[i]; i++;
    if (!ch1) return -(int)ch2;
  } while (ch1==ch2);
  return (int)ch1 - (int)ch2;
}


// Copy memory

#ifndef _WIN32
  // Building for a Unix/Linux based system
#  include <string.h>  // For memcpy ...
#else
  // Building for Windows platform with either mingw under cygwin, or the MS C compiler
  void * __cdecl memcpy(void * dest, const void * source, SYSTEM_ADR size);
#endif

void *SYSTEM_MEMCPY (void *dest, void *src, SYSTEM_ADR n)
{
  return memcpy(dest, (const void*)src, n);
}


typedef void (*SystemSignalHandler)(INTEGER); // = Platform_SignalHandler

#ifndef _WIN32

    SystemSignalHandler handler[3] = {0};

    // Provide signal handling for Unix based systems
    void signalHandler(int s) {
        if (s >= 2  &&  s <= 4) handler[s-2](s);
        // (Ignore other signals)
    }

    void SystemSetHandler(int s, SYSTEM_ADR h) {
        if (s >= 2 && s <= 4) {
            int needtosetsystemhandler = handler[s-2] == 0;
            handler[s-2] = (SystemSignalHandler)h;
            if (needtosetsystemhandler) {signal(s, signalHandler);}
        }
    }

    void SystemSetInterruptHandler(SYSTEM_ADR h) {
        SystemSetHandler(2, h);
    }

    void SystemSetQuitHandler(SYSTEM_ADR h) {
        SystemSetHandler(3, h);
    }

    void SystemSetBadInstructionHandler(SYSTEM_ADR h) {
        SystemSetHandler(4, h);
    }

#else

    // Provides Windows callback handlers for signal-like scenarios
#   include "_windows.h"

    SystemSignalHandler SystemInterruptHandler = 0;
    SystemSignalHandler SystemQuitHandler      = 0;
    BOOL ConsoleCtrlHandlerSet = FALSE;

    BOOL WINAPI SystemConsoleCtrlHandler(DWORD ctrlType) {
        if ((ctrlType == CTRL_C_EVENT) || (ctrlType == CTRL_BREAK_EVENT)) {
            if (SystemInterruptHandler) {
                SystemInterruptHandler(2); // SIGINT
                return TRUE;
            }
        } else { // Close, logoff or shutdown
            if (SystemQuitHandler) {
                SystemQuitHandler(3); // SIGQUIT
                return TRUE;
            }
        }
        return FALSE;
    }

    void EnsureConsoleCtrlHandler() {
        if (!ConsoleCtrlHandlerSet) {
        SetConsoleCtrlHandler(SystemConsoleCtrlHandler, TRUE);
            ConsoleCtrlHandlerSet = TRUE;
        }
    }

    void SystemSetInterruptHandler(SYSTEM_ADR h) {
        EnsureConsoleCtrlHandler();
        SystemInterruptHandler = (SystemSignalHandler)h;
    }

    void SystemSetQuitHandler(SYSTEM_ADR h) {
        EnsureConsoleCtrlHandler();
        SystemQuitHandler = (SystemSignalHandler)h;
    }

    void SystemSetBadInstructionHandler(SYSTEM_ADR h) {
    }

#endif
