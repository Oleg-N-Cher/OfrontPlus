/*  Ofront 1.2 -xtspkae */

#ifndef Platform__h
#define Platform__h

#include "SYSTEM.h"

typedef
	CHAR (*Platform_ArgPtr)[1024];

typedef
	Platform_ArgPtr (*Platform_ArgVec)[1024];

typedef
	CHAR (*Platform_FileHandle)[1];

typedef
	struct Platform_FileIdentity {
		INTEGER _prvt0;
		char _prvt1[16];
	} Platform_FileIdentity;

typedef
	void (*Platform_HaltProcedure)(INTEGER);

typedef
	void (*Platform_SignalHandler)(INTEGER);


import BOOLEAN Platform_LittleEndian;
import SYSTEM_PTR Platform_MainStackFrame;
import INTEGER Platform_HaltCode, Platform_PID;
import CHAR Platform_CWD[4096];
import INTEGER Platform_ArgCount;
import Platform_ArgVec Platform_ArgVector;
import INTEGER Platform_SeekSet, Platform_SeekCur, Platform_SeekEnd;
import Platform_FileHandle Platform_StdIn, Platform_StdOut, Platform_StdErr;
import CHAR Platform_nl[3];

import LONGINT *Platform_FileIdentity__typ;

import BOOLEAN Platform_Absent (INTEGER e);
import INTEGER Platform_ArgPos (CHAR *s, LONGINT s__len);
import void Platform_AssertFail (INTEGER code);
import INTEGER Platform_Chdir (CHAR *n, LONGINT n__len);
import INTEGER Platform_Close (Platform_FileHandle h);
import BOOLEAN Platform_ConnectionFailed (INTEGER e);
import void Platform_Delay (INTEGER ms);
import BOOLEAN Platform_DifferentFilesystems (INTEGER e);
import INTEGER Platform_Error (void);
import void Platform_Exit (INTEGER code);
import void Platform_GetArg (INTEGER n, CHAR *val, LONGINT val__len);
import void Platform_GetClock (INTEGER *t, INTEGER *d);
import void Platform_GetEnv (CHAR *var, LONGINT var__len, CHAR *val, LONGINT val__len);
import void Platform_GetIntArg (INTEGER n, INTEGER *val);
import void Platform_Halt (INTEGER code);
import INTEGER Platform_Identify (Platform_FileHandle h, Platform_FileIdentity *identity, LONGINT *identity__typ);
import INTEGER Platform_IdentifyByName (CHAR *n, LONGINT n__len, Platform_FileIdentity *identity, LONGINT *identity__typ);
import BOOLEAN Platform_Inaccessible (INTEGER e);
import void Platform_Init (INTEGER argc, SYSTEM_PTR argvadr);
import void Platform_MTimeAsClock (Platform_FileIdentity i, INTEGER *t, INTEGER *d);
import INTEGER Platform_New (CHAR *n, LONGINT n__len, Platform_FileHandle *h);
import BOOLEAN Platform_NoSuchDirectory (INTEGER e);
import SYSTEM_PTR Platform_OSAllocate (INTEGER size);
import void Platform_OSFree (SYSTEM_PTR address);
import INTEGER Platform_OldRO (CHAR *n, LONGINT n__len, Platform_FileHandle *h);
import INTEGER Platform_OldRW (CHAR *n, LONGINT n__len, Platform_FileHandle *h);
import INTEGER Platform_Read (Platform_FileHandle h, SYSTEM_PTR p, INTEGER l, INTEGER *n);
import INTEGER Platform_ReadBuf (Platform_FileHandle h, BYTE *b, LONGINT b__len, INTEGER *n);
import INTEGER Platform_Rename (CHAR *o, LONGINT o__len, CHAR *n, LONGINT n__len);
import BOOLEAN Platform_SameFile (Platform_FileIdentity i1, Platform_FileIdentity i2);
import BOOLEAN Platform_SameFileTime (Platform_FileIdentity i1, Platform_FileIdentity i2);
import INTEGER Platform_Seek (Platform_FileHandle h, LONGINT o, INTEGER r);
import void Platform_SetBadInstructionHandler (Platform_SignalHandler handler);
import void Platform_SetHalt (Platform_HaltProcedure p);
import void Platform_SetMTime (Platform_FileIdentity *target, LONGINT *target__typ, Platform_FileIdentity source);
import INTEGER Platform_Size (Platform_FileHandle h, LONGINT *l);
import INTEGER Platform_Sync (Platform_FileHandle h);
import INTEGER Platform_System (CHAR *cmd, LONGINT cmd__len);
import INTEGER Platform_Time (void);
import BOOLEAN Platform_TimedOut (INTEGER e);
import BOOLEAN Platform_TooManyFiles (INTEGER e);
import INTEGER Platform_Truncate (Platform_FileHandle h, LONGINT limit);
import INTEGER Platform_Unlink (CHAR *n, LONGINT n__len);
import INTEGER Platform_Write (Platform_FileHandle h, SYSTEM_PTR p, INTEGER l);
import BOOLEAN Platform_getEnv (CHAR *var, LONGINT var__len, CHAR *val, LONGINT val__len);
import void *Platform__init(void);

#define Platform_SetInterruptHandler(h)	SystemSetInterruptHandler((SYSTEM_ADR)h)
#define Platform_SetQuitHandler(h)	SystemSetQuitHandler((SYSTEM_ADR)h)

#endif
