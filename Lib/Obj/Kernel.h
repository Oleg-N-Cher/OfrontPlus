/*  Ofront 1.2 -xtspkae */

#ifndef Kernel__h
#define Kernel__h

#include "SYSTEM.h"
#include "Unix.h"

typedef
	void (*Kernel_KeyCmd)(void);

typedef
	void (*Kernel_ObjFinalizer)(SYSTEM_PTR);


import Unix_JmpBuf Kernel_trapEnv;
import LONGINT Kernel_nofiles;
import Unix_FdSet Kernel_readSet, Kernel_readySet;
import Kernel_KeyCmd Kernel_FKey[16];
import BOOLEAN Kernel_littleEndian;
import LONGINT Kernel_TimeUnit;
import CHAR Kernel_LIB[256], Kernel_CWD[256];
import CHAR Kernel_OBERON[1024];


import void Kernel_GetClock (LONGINT *t, LONGINT *d);
import void Kernel_InstallTermHandler (void (*p)(void));
import LONGINT Kernel_LargestAvailable (void);
import void Kernel_SetClock (LONGINT t, LONGINT d);
import LONGINT Kernel_Time (void);
import void *Kernel__init(void);

#define Kernel_Exit(n)	exit(n)
#define Kernel_GC(markStack)	SYSTEM_GC(markStack)
#define Kernel_Lock()	SYSTEM_lock++
#define Kernel_RegisterObject(obj, finalize)	SYSTEM_REGFIN(obj, finalize)
#define Kernel_SetHalt(p)	SYSTEM_Halt = p
#define Kernel_Unlock()	SYSTEM_lock--; if (SYSTEM_interrupted && SYSTEM_lock == 0) __HALT(-9)
#define Kernel_allocated()	SYSTEM_allocated
#define Kernel_free(adr)	(void)free(adr)
#define Kernel_heapsize()	SYSTEM_heapsize
#define Kernel_malloc(size)	(LONGINT)malloc(size)
#define Kernel_siglongjmp(env, env__typ, val)	siglongjmp(env, val)
#define Kernel_sigsetjmp(env, env__typ, savemask)	sigsetjmp(env, savemask)

#endif
