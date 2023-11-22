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


LONGINT SYSTEM_INF = 0x7FF0000000000000L;
INTEGER SYSTEM_INFS = 0x7F800000;
#ifndef _WIN32
  INTEGER SYSTEM_argc;
  void *SYSTEM_argv;
#endif
void (*SYSTEM_AssertHandler) (INTEGER n, CHAR *mod, INTEGER pos);
void (*SYSTEM_HaltHandler) (INTEGER n, CHAR *mod, INTEGER pos);
void *SYSTEM_MainStackFrame; // adr of main proc stack frame, used for stack collection


// Procedure verions of SYSTEM.H versions used when a multiply accessed
// parameter has side effects.

INTEGER SYSTEM_XCHK(INTEGER i, INTEGER ub, CHAR *mod, INTEGER pos) {return __X(i, ub, mod, pos);}
LONGINT SYSTEM_RCHK(LONGINT i, LONGINT ub, CHAR *mod, INTEGER pos) {return __R(i, ub, mod, pos);}
INTEGER SYSTEM_ASH (INTEGER x, INTEGER n)  {return __ASH(x, n, INTEGER);}
LONGINT SYSTEM_ASHL(LONGINT x, INTEGER n)  {return __ASH(x, n, LONGINT);}

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
  __HALT(-12, "SYSTEM.c", 48); return 0; // noreturn
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
  __HALT(-12, "SYSTEM.c", 61); return 0; // noreturn
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
  __HALT(-12, "SYSTEM.c", 74); return 0; // noreturn
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
  __HALT(-12, "SYSTEM.c", 87); return 0; // noreturn
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


BOOLEAN isNaN (SHORTREAL s) // By Robert D Campbell
{
  return (0x7fc00000 & ~(SET)(__VAL(INTEGER, s))) == 0x0;
}

SHORTREAL ldexpf (SHORTREAL mant, INTEGER exp) // By Robert D Campbell
{
  INTEGER m;
  if (isNaN(mant) || __ABS(mant) == __INFS) return mant;
  __ASSERT(exp <= 128 && exp >= -148, 45, "SYSTEM.c", 119);
  if (exp == 128) { exp -= 1; mant = mant * (SHORTREAL)2; }
  else if (exp <= -127) {
    exp += 22; mant = mant * ((SHORTREAL)1 / __ASHL(1, 22, INTEGER));
  }
  m = __ASHL(exp + 127, 23, INTEGER);
  return __VAL(SHORTREAL, m) * mant;
}

void SYSTEM_PACK (SHORTREAL *x, INTEGER n) // x * 2 ** n
{
  *x = ldexpf(*x, n);
}

SHORTREAL frexpf (SHORTREAL x, INTEGER *exp) // By Robert D Campbell
{
  if (isNaN(x)) { *exp = 2147483647; return x; }
  else if (x == (SHORTREAL)0) { *exp = 0; return x; }
  {
    REAL xx = __ABS(x);
    if (xx == __INF) { *exp = 2147483647; return x; }
    *exp = (INTEGER)__ASHR(__VAL(LONGINT, xx), 52, LONGINT);
    xx = x;
    {
      LONGINT k = __VAL(LONGINT, xx);
      LONGINT msh = (((SET)((INTEGER)__ASHR(k, 32, LONGINT)) & 0x800fffff) | 0x3fe00000);
      k = __ASHL(msh, 32, LONGINT) + __MASK(k, -4294967296LL);
      *exp -= 1022;
      return (SHORTREAL)(__VAL(REAL, k));
    }
  }
}

// real mantissa m of 'x' and an integer n such that 'x' = m * 2 ** n
void SYSTEM_UNPK (SHORTREAL *x, INTEGER *n)
{
  *x = frexpf(*x, n); *x += *x; (*n)--;
}


void SYSTEM_INHERIT(SYSTEM_ADRINT *t, SYSTEM_ADRINT *t0)
{
    t -= __TPROC0OFF;
    t0 -= __TPROC0OFF;
    while (*t0 != __EOM) {*t = *t0; t--; t0--;}
}


void SYSTEM_ENUMP(void *adr, SYSTEM_ADRINT n, void (*P)(void*))
{
    while (n > 0) {
        P(*((void**)(adr)));
        adr = ((void**)adr) + 1;
        n--;
    }
}

void SYSTEM_ENUMR(void *adr, SYSTEM_ADRINT *typ, SYSTEM_ADRINT size, SYSTEM_ADRINT n, void (*P)(void*))
{
    SYSTEM_ADRINT *t, off;
    if (typ == 0) return; // not yet initialized
    typ++;
    while (n > 0) {
        t = typ;
        off = *t;
        while (off >= 0) {P(*(void**)((char*)adr+off)); t++; off = *t;}
        adr = ((char*)adr) + size;
        n--;
    }
}

extern void Heap_Lock (void);
extern void Heap_Unlock (void);


// Program startup

extern void Heap_InitHeap (void);

void SYSTEM_INIT (void *stktop)
{
  SYSTEM_MainStackFrame = stktop; // MUST be aligned
  SYSTEM_AssertHandler = 0;
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
        if (nofelems <= 0) __HALT(-20, "SYSTEM.c", 178);
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


typedef void (*SystemSignalHandler)(INTEGER); // = Platform_SignalHandler

#ifndef _WIN32

    void SYSTEM_HALT (INTEGER n, CHAR *mod, INTEGER pos) {
      if (SYSTEM_HaltHandler != 0) SYSTEM_HaltHandler(n, mod, pos);
      exit(n);
    }

    void SYSTEM_ASSERT (INTEGER n, CHAR *mod, INTEGER pos) {
      if (SYSTEM_AssertHandler != 0) SYSTEM_AssertHandler(n, mod, pos);
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

    void SYSTEM_HALT (INTEGER n, CHAR *mod, INTEGER pos) {
      if (SYSTEM_HaltHandler != 0) SYSTEM_HaltHandler(n, mod, pos);
      ExitProcess((UINT)(n));
    }

    void SYSTEM_ASSERT (INTEGER n, CHAR *mod, INTEGER pos) {
      if (SYSTEM_AssertHandler != 0) SYSTEM_AssertHandler(n, mod, pos);
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

    void EnsureConsoleCtrlHandler (void) {
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
