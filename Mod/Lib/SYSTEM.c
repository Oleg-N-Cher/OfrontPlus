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

#include "SYSTEM.oh"
#include <stdarg.h>
#include <signal.h>
#include <stdlib.h>


INTEGER SYSTEM_ArgCount;
void *SYSTEM_ArgVector;
void (*SYSTEM_AssertFailHandler)(INTEGER n, CHAR *mod, INTEGER pos);
void (*SYSTEM_HaltHandler)(INTEGER n, CHAR *mod, INTEGER pos);
void *SYSTEM_MainStackFrame; // adr of main proc stack frame, used for stack collection


// Procedure verions of SYSTEM.H versions used when a multiply accessed
// parameter has side effects.

LONGINT SYSTEM_XCHK(LONGINT i, LONGINT ub, CHAR *mod, INTEGER pos) {return __X(i, ub, mod, pos);}
LONGINT SYSTEM_RCHK(LONGINT i, LONGINT ub, CHAR *mod, INTEGER pos) {return __R(i, ub, mod, pos);}
INTEGER SYSTEM_ASH (INTEGER x, INTEGER n)  {return __ASH(x, n, INTEGER);}
LONGINT SYSTEM_ASHL(LONGINT x, INTEGER n)  {return __ASH(x, n, LONGINT);}
LONGINT SYSTEM_ABS (LONGINT i)             {return __ABS(i);}
double  SYSTEM_ABSD(double i)              {return __ABS(i);}


INTEGER SYSTEM_DIV(INTEGER x, INTEGER y)
{
  if (y > 0) {
    if (x < 0) return -1 - (-1 - x) / y;
    else       return x / y;
  }
  if (y < 0) {
    if (x > 0) return -1 + (x - 1) / y;
    else       return x / y;
  }
  __HALT(-12, "SYSTEM.c", 50); return 0; // noreturn
}

LONGINT SYSTEM_DIVL(LONGINT x, LONGINT y)
{
  if (y > 0) {
    if (x < 0) return -1 - (-1 - x) / y;
    else       return x / y;
  }
  if (y < 0) {
    if (x > 0) return -1 + (x - 1) / y;
    else       return x / y;
  }
  __HALT(-12, "SYSTEM.c", 63); return 0; // noreturn
}

INTEGER SYSTEM_MOD(INTEGER x, INTEGER y)
{
  if (y > 0) {
    if (x < 0) return y - 1 + (x + 1) % y;
    else       return x % y;
  }
  if (y < 0) {
    if (x > 0) return y + 1 + (x - 1) % y;
    else       return x % y;
  }
  __HALT(-12, "SYSTEM.c", 76); return 0; // noreturn
}

LONGINT SYSTEM_MODL(LONGINT x, LONGINT y)
{
  if (y > 0) {
    if (x < 0) return y - 1 + (x + 1) % y;
    else       return x % y;
  }
  if (y < 0) {
    if (x > 0) return y + 1 + (x - 1) % y;
    else       return x % y;
  }
  __HALT(-12, "SYSTEM.c", 89); return 0; // noreturn
}

INTEGER SYSTEM_ENTIER(REAL x)
{
  INTEGER i;
  i = (INTEGER)x;
  if (i > x) i--;
  return i;
}

LONGINT SYSTEM_ENTIERL(REAL x)
{
  LONGINT i;
  i = (LONGINT)x;
  if (i > x) i--;
  return i;
}

void SYSTEM_UNPK(SHORTREAL *x, INTEGER *exp)
    // real mantissa m of 'x' and an integer exp such that 'x' = m * 2 ** exp
{
    BOOLEAN neg = (*x < (SHORTREAL)0);
    *exp = 0;
    if (neg) *x = -*x;
    if (*x >= (SHORTREAL)1) {
        do { *exp += 1; *x /= (SHORTREAL)2; } while (*x >= (SHORTREAL)1);
    } else if (*x < 0.1 && *x != (SHORTREAL)0) {
        do { *exp -= 1; *x *= (SHORTREAL)2; } while (*x < 0.1);
    }
    if (neg) *x = -*x;
    //*x += *x; (*exp)--;
}

void SYSTEM_PACK(SHORTREAL *x, INTEGER exp) // x * 2 ** exp
{
    INTEGER i = 1;
    if (exp > 0) {
        while (i <=  exp) { *x *= (SHORTREAL)2; i += 1; }
    } else if (exp < 0) {
        while (i <= -exp) { *x /= (SHORTREAL)2; i += 1; }
    }
}


void SYSTEM_INHERIT(SYSTEM_ADRINT *t, SYSTEM_ADRINT *t0)
{
    t -= __TPROC0OFF;
    t0 -= __TPROC0OFF;
    while (*t0 != __EOM) {*t = *t0; t--; t0--;}
}


void SYSTEM_ENUMP(void *adr, SYSTEM_ADRINT n, void (*P)())
{
    while (n > 0) {
        P((SYSTEM_ADRINT)(*((void**)(adr))));
        adr = ((void**)adr) + 1;
        n--;
    }
}

void SYSTEM_ENUMR(void *adr, SYSTEM_ADRINT *typ, SYSTEM_ADRINT size, SYSTEM_ADRINT n, void (*P)())
{
    SYSTEM_ADRINT *t, off;
    if (typ == 0) return; // not yet initialized
    typ++;
    while (n > 0) {
        t = typ;
        off = *t;
        while (off >= 0) {P(*(SYSTEM_ADRINT*)((char*)adr+off)); t++; off = *t;}
        adr = ((char*)adr) + size;
        n--;
    }
}

extern void Heap_Lock();
extern void Heap_Unlock();


// Program startup

extern void Heap_InitHeap();

void SYSTEM_INIT(INTEGER argc, void *argv, void *stktop)
{
  SYSTEM_MainStackFrame = stktop; // MUST be aligned
  SYSTEM_ArgCount = argc;
  SYSTEM_ArgVector = argv;
  SYSTEM_AssertFailHandler = 0;
  SYSTEM_HaltHandler = 0;
  // This function (SYSTEM_INIT) is called at program startup BEFORE any
  // modules have been initalized. In turn we must initialize the heap
  // before module startup (xxx__init) code is run.
  Heap_InitHeap();
}

SYSTEM_PTR SYSTEM_NEWARR(SYSTEM_ADRINT *typ, SYSTEM_ARRLEN elemsz, int elemalgn, int nofdim, int nofdyn, ...)
{
    SYSTEM_ADRINT *x, *p, *ptab;
    SYSTEM_ARRLEN nofelems, size, dataoff, n, nptr, nofptrs, i, *pi, off;
    va_list ap;
    va_start(ap, nofdyn);
    nofelems = 1;
    while (nofdim > 0) {
        nofelems = nofelems * va_arg(ap, SYSTEM_ARRLEN); nofdim--;
        if (nofelems <= 0) __HALT(-20, "SYSTEM.c", 171);
    }
    va_end(ap);
    dataoff = nofdyn * sizeof(SYSTEM_ARRLEN);
    if (elemalgn > sizeof(SYSTEM_ARRLEN)) {
        n = dataoff % elemalgn;
        if (n != 0) dataoff += elemalgn - n;
    }
    size = dataoff + nofelems * elemsz;
    Heap_Lock();
    if (typ == NIL) {
        /* element typ does not contain pointers */
        x = Heap_NEWBLK(size);
    }
    else if (typ == (SYSTEM_ADRINT*)POINTER__typ) {
        /* element type is a pointer */
        x = Heap_NEWBLK(size + nofelems * sizeof(SYSTEM_ADRINT));
        p = (SYSTEM_ADRINT*)x[-1];
        p[-nofelems] = *p;  /* build new type desc in situ: 1. copy block size; 2. setup ptr tab; 3. set sentinel; 4. patch tag */
        p -= nofelems - 1;
		for (n=0; n<nofelems; n++, p++)
			*p = dataoff + n*sizeof(SYSTEM_ADRINT);
        *p = - (nofelems + 1) * sizeof(SYSTEM_ADRINT);    /* sentinel */
        x[-1] -= nofelems * sizeof(SYSTEM_ADRINT);
    }
    else {
        /* element type is a record that contains pointers */
        ptab = typ + 1; nofptrs = 0;
        while (ptab[nofptrs] >= 0) {nofptrs++;} /* number of pointers per element */
        nptr = nofelems * nofptrs;  /* total number of pointers */
        x = Heap_NEWBLK(size + nptr * sizeof(SYSTEM_ADRINT));
        p = (SYSTEM_ADRINT*)x[-1];
        p[-nptr] = *p;  /* build new type desc in situ; 1. copy block size; 2. setup ptr tab; 3. set sentinel; 4. patch tag */
        p -= nptr - 1; n = 0; off = dataoff;
        while (n < nofelems) {i = 0;
            while (i < nofptrs) {*p = off + ptab[i]; p++; i++;}
            off += elemsz; n++;
        }
        *p = - (nptr + 1) * sizeof(SYSTEM_ADRINT);    /* sentinel */
        x[-1] -= nptr * sizeof(SYSTEM_ADRINT);
    }
    if (nofdyn != 0) {
        /* setup len vector for index checks */
        va_start(ap, nofdyn);
        pi = (SYSTEM_ARRLEN*)x;
        while (nofdyn > 0) {*pi = va_arg(ap, SYSTEM_ARRLEN); pi++, nofdyn--;}
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


typedef void (*SystemSignalHandler)(INTEGER); // = Platform_SignalHandler

#ifndef _WIN32

    void SYSTEM_HALT(INTEGER n, CHAR *mod, INTEGER pos) {
      if (SYSTEM_HaltHandler != 0) SYSTEM_HaltHandler(n, mod, pos);
      exit(n);
    }

    void SYSTEM_ASSERT_FAIL(INTEGER n, CHAR *mod, INTEGER pos) {
      if (SYSTEM_AssertFailHandler != 0) SYSTEM_AssertFailHandler(n, mod, pos);
      exit(n);
    }


    SystemSignalHandler handler[3] = {0};

    // Provide signal handling for Unix based systems
    void signalHandler(int s) {
        if (s >= 2  &&  s <= 4) handler[s-2](s);
        // (Ignore other signals)
    }

    void SystemSetHandler(int s, SYSTEM_ADRINT h) {
        if (s >= 2 && s <= 4) {
            int needtosetsystemhandler = handler[s-2] == 0;
            handler[s-2] = (SystemSignalHandler)h;
            if (needtosetsystemhandler) {signal(s, signalHandler);}
        }
    }

    void SystemSetInterruptHandler(SYSTEM_ADRINT h) {
        SystemSetHandler(2, h);
    }

    void SystemSetQuitHandler(SYSTEM_ADRINT h) {
        SystemSetHandler(3, h);
    }

    void SystemSetBadInstructionHandler(SYSTEM_ADRINT h) {
        SystemSetHandler(4, h);
    }

#else

#   include "_windows.h"

    void SYSTEM_HALT(INTEGER n, CHAR *mod, INTEGER pos) {
      if (SYSTEM_HaltHandler != 0) SYSTEM_HaltHandler(n, mod, pos);
      ExitProcess((UINT)(n));
    }

    void SYSTEM_ASSERT_FAIL(INTEGER n, CHAR *mod, INTEGER pos) {
      if (SYSTEM_AssertFailHandler != 0) SYSTEM_AssertFailHandler(n, mod, pos);
      ExitProcess((UINT)(n));
    }


    // Provides Windows callback handlers for signal-like scenarios

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

    void SystemSetInterruptHandler(SYSTEM_ADRINT h) {
        EnsureConsoleCtrlHandler();
        SystemInterruptHandler = (SystemSignalHandler)h;
    }

    void SystemSetQuitHandler(SYSTEM_ADRINT h) {
        EnsureConsoleCtrlHandler();
        SystemQuitHandler = (SystemSignalHandler)h;
    }

    void SystemSetBadInstructionHandler(SYSTEM_ADRINT h) {
    }

#endif
