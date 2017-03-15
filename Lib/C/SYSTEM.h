#ifndef SYSTEM__h
#define SYSTEM__h

/*

  the Ofront+ runtime system interface and macro library
  based on SYSTEM.h by Josef Templ and voc team

*/


#if defined __linux__ || defined __unix__
#  include <alloca.h>
#else
#  define errno __errno__ /* to avoid of implicit inclusion "errno" */
#  include <malloc.h>
#  undef errno
#endif

// The compiler uses 'import' and 'export' which translate to 'extern' and
// nothing respectively.

#define import extern
#define export



// Known constants

#define NIL          ((void*)0)
#define __MAXEXT     16
#define POINTER__typ ((SYSTEM_ADR*)(1))  // not NIL and not a valid type


// Oberon types

typedef char          BOOLEAN;
typedef signed char   BYTE;
typedef unsigned char CHAR;
typedef short int     SHORTINT;
typedef int           INTEGER;   // INTEGER is 32 bit.
typedef float         REAL;
typedef double        LONGREAL;
typedef void*         SYSTEM_PTR;

// Unsigned variants are for use by shift and rotate macros.

typedef unsigned char      U_BYTE;
typedef unsigned char      U_CHAR;
typedef unsigned short int U_SHORTINT;
typedef unsigned int       U_INTEGER;
typedef unsigned int       U_SET;

// For 32 bit builds, the size of LONGINT depends on a make option:

#if (__SIZEOF_POINTER__ == 8) || defined(_LONGINT64) || defined(_WIN64)
  typedef long long          LONGINT;   // LONGINT is 64 bit. (long long is always 64 bits, while long can be 32 bits e.g. under MSC/MingW)
  typedef unsigned long long U_LONGINT;
#else
  typedef long               LONGINT;   // LONGINT is 32 bit.
  typedef unsigned long      U_LONGINT;
#endif

typedef U_INTEGER SET;   // SET is 32 bit.

// SYSTEM_ADR is an unsigned integer of pointer size

#if defined __amd64__ || defined __x86_64__
  typedef unsigned long long SYSTEM_ADR;
#else
  typedef unsigned int SYSTEM_ADR;
#endif


// Run time system routines in SYSTEM.c

extern int     SYSTEM_STRCMP (CHAR *x, CHAR *y);
extern void   *SYSTEM_MEMCPY (void *dest, void *src, SYSTEM_ADR n);
extern LONGINT SYSTEM_XCHK   (LONGINT i, LONGINT ub);
extern LONGINT SYSTEM_RCHK   (LONGINT i, LONGINT ub);
extern INTEGER SYSTEM_ASH    (INTEGER x, INTEGER n);
extern LONGINT SYSTEM_ASHL   (LONGINT x, INTEGER n);
extern LONGINT SYSTEM_ABS    (LONGINT i);
extern double  SYSTEM_ABSD   (double i);
extern void    SYSTEM_INHERIT(SYSTEM_ADR *t, SYSTEM_ADR *t0);
extern void    SYSTEM_ENUMP  (void *adr, SYSTEM_ADR n, void (*P)());
extern void    SYSTEM_ENUMR  (void *adr, SYSTEM_ADR *typ, SYSTEM_ADR size, SYSTEM_ADR n, void (*P)());
extern LONGINT SYSTEM_DIV    (LONGINT x, LONGINT y);
extern LONGINT SYSTEM_MOD    (LONGINT x, LONGINT y);
extern INTEGER SYSTEM_ENTIER (LONGREAL x);
extern LONGINT SYSTEM_ENTIERL(LONGREAL x);


// Signal handling in SYSTEM.c

extern void SystemSetInterruptHandler     (SYSTEM_ADR h);
extern void SystemSetQuitHandler          (SYSTEM_ADR h);
extern void SystemSetBadInstructionHandler(SYSTEM_ADR h);


// String comparison

#define __STRCMP(a, b)  SYSTEM_STRCMP((CHAR*)(a), (CHAR*)(b))


// Inline string, record and array copy

#define __COPY(s, d, n) {char*_a=(void*)s,*_b=(void*)d; INTEGER _i=0,_t=n-1; \
                         while(_i<_t&&((_b[_i]=_a[_i])!=0)){_i++;};_b[_i]=0;}
#define __DUP(x, l)     x=(void*)__MEMCPY(alloca(l*sizeof(*x)),x,l*sizeof(*x))
#define __DUPARR(v)     v=(void*)__MEMCPY(v##__copy,v,sizeof(v##__copy))
#define __DEL(x)        /* DUP with alloca frees storage automatically */


/* SYSTEM ops */

#define __VAL(t, x)     (*(t*)&(x))
#define __VALP(t, x)    ((t)(SYSTEM_ADR)(x))

#define __GET(a, x, t)  x= *(t*)(a)
#define __PUT(a, x, t)  *(t*)(a)=(t)x

#define __LSHL(x, n, t) ((t)((U_##t)(x)<<(n)))
#define __LSHR(x, n, t) ((t)((U_##t)(x)>>(n)))
#define __LSH(x, n, t)  ((n)>=0? __LSHL(x, n, t): __LSHR(x, -(n), t))

#define __ASH(x, n, t)  ((n)>=0?__ASHL(x,n,t):__ASHR(x,-(n),t))
#define __ASHL(x, n, t) ((t)(x)<<(n))
#define __ASHR(x, n, t) ((t)(x)>>(n))
#define __ASHF(x, n, t)  SYSTEM_ASH(x, n)
#define __ASHFL(x, n, t) SYSTEM_ASHL(x, n)

#define __ROTL(x, n, t) ((t)((U_##t)(x)<<(n)|(U_##t)(x)>>(8*sizeof(t)-(n))))
#define __ROTR(x, n, t) ((t)((U_##t)(x)>>(n)|(U_##t)(x)<<(8*sizeof(t)-(n))))
#define __ROT(x, n, t)  ((n)>=0? __ROTL(x, n, t): __ROTR(x, -(n), t))

#define __BIT(x, n)     (*(U_LONGINT*)(x)>>(n)&1)
#define __MEMCPY        SYSTEM_MEMCPY
#define __MOVE(s, d, n) __MEMCPY((char*)(d),(char*)(s),n)
#define __SHORT(x, y)   ((int)((U_LONGINT)(x)+(y)<(y)+(y)?(x):(__HALT(-8),0)))
#define __SHORTF(x, y)  ((int)(__RF((x)+(y),(y)+(y))-(y)))
#define __CHR(x)        ((CHAR)__R(x, 256))
#define __CHRF(x)       ((CHAR)__RF(x, 256))
#define __DIV(x, y)     ((x)>=0?(x)/(y):-(((y)-1-(x))/(y)))
#define __DIVF(x, y)    SYSTEM_DIV(x, y)
#define __MOD(x, y)     ((x)>=0?(x)%(y):__MODF(x,y))
#define __MODF(x, y)    SYSTEM_MOD(x, y)
#define __ENTIER(x)     SYSTEM_ENTIER(x)
#define __ENTIERL(x)    SYSTEM_ENTIERL(x)
#define __ABS(x)        (((x)<0)?-(x):(x))
#define __ABSF(x)       SYSTEM_ABS(x)
#define __ABSFD(x)      SYSTEM_ABSD(x)
#define __CAP(ch)       ((CHAR)((ch)&0x5f))
#define __ODD(x)        ((x)&1)
#define __IN(x, s)      (((s)>>(x))&1)
#define __SETOF(x)      ((SET)1<<(x))
#define __SETRNG(l, h)  ((~(SET)0<<(l))&~(SET)0>>(8*sizeof(SET)-1-(h)))
#define __MASK(x, m)    ((x)&~(m))



// Runtime checks

#define __X(i, ub)   (((U_LONGINT)(i)<(U_LONGINT)(ub))?i:(__HALT(-2),0))
#define __XF(i, ub)  SYSTEM_XCHK((LONGINT)(i), (LONGINT)(ub))
#define __R(i, ub)   (((U_LONGINT)(i)<(U_LONGINT)(ub))?i:(__HALT(-8),0))
#define __RF(i, ub)  SYSTEM_RCHK((LONGINT)(i),(LONGINT)(ub))
#define __RETCHK     __retchk: __HALT(-3); return 0;
#define __CASECHK    __HALT(-4)
#define __WITHCHK    __HALT(-7)

#define __GUARDP(p, typ, level)    ((typ*)(__ISP(p,typ,level)?p:(__HALT(-5),p)))
#define __GUARDR(r, typ, level)    (*((typ*)(__IS(r##__typ,typ,level)?r:(__HALT(-5),r))))
#define __GUARDA(p, typ, level)    ((struct typ*)(__IS(__TYPEOF(p),typ,level)?p:(__HALT(-5),p)))
#define __GUARDEQR(p, dyntyp, typ) if(dyntyp!=typ##__typ) __HALT(-6);*(p)
#define __GUARDEQP(p, typ)         if(__TYPEOF(p)!=typ##__typ)__HALT(-6);*(p)



// Module entry/registration/exit

extern void       Heap_REGCMD();
extern SYSTEM_PTR Heap_REGMOD();
extern void       Heap_REGTYP();
extern void       Heap_INCREF();

#define __DEFMOD              static void *m; if (m!=0) {return m;}
#define __REGCMD(name, cmd)   // Heap_REGCMD(m, (CHAR*)name, cmd)
#define __REGMOD(name, enum)  if (m==0) {m = Heap_REGMOD((CHAR*)name,enum);}
#define __ENDMOD              return m
#define __IMPORT(name__init)  Heap_INCREF(name__init())

#define __EXTERN __attribute__((dllimport))
#define __CALL_1 __attribute__((__stdcall__))

// Main module initialization, registration and finalization

extern void SYSTEM_INIT(INTEGER argc, void *argvadr);
extern void Heap_FINALL();

#define __INIT(argc, argv)    static void *m; SYSTEM_INIT(argc, &argv);
#define __REGMAIN(name, enum) m = Heap_REGMOD((CHAR*)name,enum)
#define __FINI                Heap_FINALL(); return 0


// Assertions and Halts

extern void SYSTEM_HALT(INTEGER code);
extern void SYSTEM_ASSERT_FAIL(INTEGER code);

#define __HALT(code)         SYSTEM_HALT(code)
#define __ASSERT(cond, code) if (!(cond)) SYSTEM_ASSERT_FAIL(code)


// Memory allocation

extern SYSTEM_PTR Heap_NEWBLK (SYSTEM_ADR size);
extern SYSTEM_PTR Heap_NEWREC (SYSTEM_ADR tag);
extern SYSTEM_PTR SYSTEM_NEWARR(SYSTEM_ADR*, SYSTEM_ADR, int, int, int, ...);

#define __SYSNEW(p, len) p = Heap_NEWBLK((SYSTEM_ADR)(len))
#define __NEW(p, t)      p = Heap_NEWREC((SYSTEM_ADR)t##__typ)
#define __NEWARR         SYSTEM_NEWARR



/* Type handling */

#define __TDESC(t__desc, m, n)                                          \
  static struct t__desc {                                               \
    SYSTEM_ADR  tproc[m];         /* Proc for each ptr field            */ \
    SYSTEM_ADR  tag;                                                       \
    SYSTEM_ADR  next;             /* Module table type list points here */ \
    SYSTEM_ADR  level;                                                     \
    SYSTEM_ADR  module;                                                    \
    char        name[24];                                                  \
    SYSTEM_ADR  basep[__MAXEXT];  /* List of bases this extends         */ \
    SYSTEM_ADR  reserved;                                                  \
    SYSTEM_ADR  blksz;            /* xxx_typ points here                */ \
    SYSTEM_ADR  ptr[n+1];         /* Offsets of ptrs up to -ve sentinel */ \
  } t__desc

#define __BASEOFF   (__MAXEXT+1)                           // blksz as index to base.
#define __TPROC0OFF (__BASEOFF+24/sizeof(SYSTEM_ADR)+5)    // blksz as index to tproc IFF m=1.
#define __EOM 1
#define __TDFLDS(name, size)          {__EOM}, 1, 0, 0, 0, name, {0}, 0, size
#define __ENUMP(adr, n, P)            SYSTEM_ENUMP(adr, (SYSTEM_ADR)(n), P)
#define __ENUMR(adr, typ, size, n, P) SYSTEM_ENUMR(adr, typ, (SYSTEM_ADR)(size), (SYSTEM_ADR)(n), P)

#define __INITYP(t, t0, level) \
  t##__typ               = (SYSTEM_ADR*)&t##__desc.blksz;                                                    \
  __MEMCPY(t##__desc.basep, t0##__typ - __BASEOFF, level*sizeof(SYSTEM_ADR));                                \
  t##__desc.basep[level] = (SYSTEM_ADR)t##__typ;                                                 \
  t##__desc.module       = (SYSTEM_ADR)m;                                                        \
  if(t##__desc.blksz!=sizeof(struct t)) __HALT(-15);                                                      \
  t##__desc.blksz        = (t##__desc.blksz+5*sizeof(SYSTEM_ADR)-1)/(4*sizeof(SYSTEM_ADR))*(4*sizeof(SYSTEM_ADR)); \
  Heap_REGTYP(m, (SYSTEM_ADR)&t##__desc.next);                                                   \
  SYSTEM_INHERIT(t##__typ, t0##__typ)

#define __IS(tag, typ, level) (*(tag-(__BASEOFF-level))==(SYSTEM_ADR)typ##__typ)
#define __TYPEOF(p)           (*(((SYSTEM_ADR**)(p))-1))
#define __ISP(p, typ, level)  __IS(__TYPEOF(p),typ,level)

// Oberon-2 type bound procedures support
#define __INITBP(t, proc, num)            *(t##__typ-(__TPROC0OFF+num))=(SYSTEM_ADR)proc
#define __SEND(typ, num, funtyp, parlist) ((funtyp)((SYSTEM_ADR)*(typ-(__TPROC0OFF+num))))parlist




#endif
