/* Ofront 1.2 -xtspkael */
#include "SYSTEM.h"
#include "Args.h"
#include "Unix.h"

typedef
	void (*Kernel_KeyCmd)(void);

typedef
	void (*Kernel_ObjFinalizer)(SYSTEM_PTR);

typedef
	struct Kernel_TimeDesc *Kernel_RealTime;

typedef
	struct Kernel_TimeDesc {
		LONGINT sec, min, hour, mday, mon, year, wday, isdst, zone, gmtoff;
	} Kernel_TimeDesc;


export Unix_JmpBuf Kernel_trapEnv;
export LONGINT Kernel_nofiles;
export Unix_FdSet Kernel_readSet, Kernel_readySet;
export Kernel_KeyCmd Kernel_FKey[16];
export BOOLEAN Kernel_littleEndian;
export LONGINT Kernel_TimeUnit;
export CHAR Kernel_LIB[256], Kernel_CWD[256];
export CHAR Kernel_OBERON[1024];
static LONGINT Kernel_timeStart;

export LONGINT *Kernel_TimeDesc__typ;

static void Kernel_EndianTest (void);
export void Kernel_Exit (INTEGER n);
export void Kernel_GetClock (LONGINT *t, LONGINT *d);
static void Kernel_Halt (LONGINT n);
export void Kernel_InstallTermHandler (void (*p)(void));
static void Kernel_JmpBufCheck (void);
export LONGINT Kernel_LargestAvailable (void);
export void Kernel_Select (LONGINT delay);
export void Kernel_SetClock (LONGINT t, LONGINT d);
export LONGINT Kernel_Time (void);

#define Kernel_Error(msg, msg__len, len)	write(1/*stdout*/, msg, len); char ch = 0xa; write(1, &ch, 1)
#define Kernel_GC(markStack)	SYSTEM_GC(markStack)
#define Kernel_Lock()	SYSTEM_lock++
#define Kernel_RegisterObject(obj, finalize)	SYSTEM_REGFIN(obj, finalize)
#define Kernel_SetHalt(p)	SYSTEM_Halt = p
#define Kernel_SizeofSigJmpBuf()	sizeof(sigjmp_buf)
#define Kernel_SizeofUnixJmpBuf()	sizeof(Unix_JmpBuf)
#define Kernel_Unlock()	SYSTEM_lock--; if (SYSTEM_interrupted && SYSTEM_lock == 0) __HALT(-9)
#define Kernel_allocated()	SYSTEM_allocated
#define Kernel_free(adr)	(void)free(adr)
#define Kernel_getcwd(cwd, cwd__len)	getcwd(cwd, cwd__len)
#define Kernel_heapsize()	SYSTEM_heapsize
#include <setjmp.h>
#define Kernel_localtime(clock)	(Kernel_RealTime)localtime(clock)
#define Kernel_malloc(size)	(LONGINT)malloc(size)
#define Kernel_siglongjmp(env, env__typ, val)	siglongjmp(env, val)
#define Kernel_sigsetjmp(env, env__typ, savemask)	__sigsetjmp(env, savemask)

void Kernel_Exit (INTEGER n)
{
	Unix_Exit(n);
}

void Kernel_GetClock (LONGINT *t, LONGINT *d)
{
	Unix_Timeval tv;
	Unix_Timezone tz;
	Kernel_RealTime time = NIL;
	Unix_Gettimeofday(&tv, Unix_Timeval__typ, &tz, Unix_Timezone__typ);
	time = Kernel_localtime(&tv.sec);
	*t = (time->sec + __ASHL(time->min, 6, LONGINT)) + __ASHL(time->hour, 12, LONGINT);
	*d = (time->mday + __ASHL(time->mon + 1, 5, LONGINT)) + __ASHL(__MOD(time->year, 100), 9, LONGINT);
}

void Kernel_SetClock (LONGINT t, LONGINT d)
{
	CHAR err[25];
	__MOVE("not yet implemented", err, 20);
	__HALT(99);
}

LONGINT Kernel_Time (void)
{
	Unix_Timeval timeval;
	Unix_Timezone timezone;
	Unix_Gettimeofday(&timeval, Unix_Timeval__typ, &timezone, Unix_Timezone__typ);
	return __MOD((__DIV(timeval.usec, 1000) + timeval.sec * 1000) - Kernel_timeStart, 2147483647);
}

void Kernel_Select (LONGINT delay)
{
	Unix_FdSet rs, ws, xs;
	LONGINT n;
	Unix_Timeval tv;
	__MOVE(Kernel_readSet, rs, 32);
	n = 0;
	while (n <= 7) {
		ws[__X(n, 8)] = 0x0;
		xs[__X(n, 8)] = 0x0;
		Kernel_readySet[__X(n, 8)] = 0x0;
		n += 1;
	}
	if (delay < 0) {
		delay = 0;
	}
	tv.sec = __DIV(delay, 1000);
	tv.usec = __MOD(delay, 1000) * 1000;
	n = Unix_Select(256, rs, ws, xs, &tv, Unix_Timeval__typ);
	if (n >= 0) {
		__MOVE(rs, Kernel_readySet, 32);
	}
}

void Kernel_InstallTermHandler (void (*p)(void))
{
}

LONGINT Kernel_LargestAvailable (void)
{
	return 2147483647;
}

static void Kernel_Halt (LONGINT n)
{
	LONGINT res;
	res = Unix_Kill(Unix_Getpid(), 4);
}

static void Kernel_EndianTest (void)
{
	LONGINT i;
	INTEGER dmy;
	dmy = 1;
	i = (LONGINT)&dmy;
	__GET(i, Kernel_littleEndian, BOOLEAN);
}

static void Kernel_JmpBufCheck (void)
{
	LONGINT x, y;
	x = Kernel_SizeofUnixJmpBuf();
	y = Kernel_SizeofSigJmpBuf();
	if (x < y) {
		Kernel_Error((CHAR*)"Kernel.JmpBufCheck: inconsistent usage of sigjmp_buf", (LONGINT)53, 52);
		Kernel_Exit(1);
	}
}

__TDESC(Kernel_TimeDesc__desc, 1, 0) = {__TDFLDS("TimeDesc", 40), {-4}};

export void *Kernel__init(void)
{
	__DEFMOD;
	__IMPORT(Args__init);
	__IMPORT(Unix__init);
	__REGMOD("Kernel", 0);
	__INITYP(Kernel_TimeDesc, Kernel_TimeDesc, 0);
/* BEGIN */
	Kernel_EndianTest();
	Kernel_SetHalt(Kernel_Halt);
	Kernel_CWD[0] = 0x00;
	__MOVE(".", Kernel_OBERON, 2);
	Kernel_LIB[0] = 0x00;
	Kernel_getcwd((void*)Kernel_CWD, 256);
	Args_GetEnv((CHAR*)"OBERON", (LONGINT)7, (void*)Kernel_OBERON, 1024);
	Args_GetEnv((CHAR*)"OBERON_LIB", (LONGINT)11, (void*)Kernel_LIB, 256);
	Kernel_TimeUnit = 1000;
	Kernel_timeStart = 0;
	Kernel_timeStart = Kernel_Time();
	Kernel_JmpBufCheck();
	__ENDMOD;
}
