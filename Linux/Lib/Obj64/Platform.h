/* Ofront+ 0.9 -xtspkae */

#ifndef Platform__h
#define Platform__h

#include "SYSTEM.h"

typedef
	CHAR (*Platform_ArgPtr)[1024];

typedef
	Platform_ArgPtr (*Platform_ArgVec)[1024];

typedef
	struct Platform_FileIdentity {
		LONGINT _prvt0;
		char _prvt1[16];
	} Platform_FileIdentity;

typedef
	void (*Platform_HaltProcedure)(INTEGER);

typedef
	CHAR (*Platform_MemAdr)[1];

typedef
	void (*Platform_SignalHandler)(INTEGER);


import BOOLEAN Platform_LittleEndian;
import Platform_MemAdr Platform_MainStackFrame;
import INTEGER Platform_HaltCode, Platform_PID;
import CHAR Platform_CWD[256];
import INTEGER Platform_ArgCount;
import Platform_ArgVec Platform_ArgVector;
import INTEGER Platform_SeekSet, Platform_SeekCur, Platform_SeekEnd;
import CHAR Platform_newLine[2];

import LONGINT *Platform_FileIdentity__typ;

import BOOLEAN Platform_Absent (INTEGER e);
import INTEGER Platform_ArgPos (CHAR *s, INTEGER s__len);
import void Platform_AssertFail (INTEGER code);
import INTEGER Platform_Chdir (CHAR *n, INTEGER n__len);
import INTEGER Platform_Close (INTEGER h);
import BOOLEAN Platform_ConnectionFailed (INTEGER e);
import void Platform_Delay (INTEGER ms);
import BOOLEAN Platform_DifferentFilesystems (INTEGER e);
import INTEGER Platform_Error (void);
import void Platform_Exit (INTEGER code);
import void Platform_GetArg (INTEGER n, CHAR *val, INTEGER val__len);
import void Platform_GetClock (INTEGER *t, INTEGER *d);
import void Platform_GetEnv (CHAR *var, INTEGER var__len, CHAR *val, INTEGER val__len);
import void Platform_GetIntArg (INTEGER n, INTEGER *val);
import void Platform_Halt (INTEGER code);
import INTEGER Platform_Identify (INTEGER h, Platform_FileIdentity *identity, LONGINT *identity__typ);
import INTEGER Platform_IdentifyByName (CHAR *n, INTEGER n__len, Platform_FileIdentity *identity, LONGINT *identity__typ);
import BOOLEAN Platform_Inaccessible (INTEGER e);
import void Platform_Init (INTEGER argc, Platform_MemAdr argvadr);
import void Platform_MTimeAsClock (Platform_FileIdentity i, INTEGER *t, INTEGER *d);
import INTEGER Platform_New (CHAR *n, INTEGER n__len, INTEGER *h);
import BOOLEAN Platform_NoSuchDirectory (INTEGER e);
import Platform_MemAdr Platform_OSAllocate (INTEGER size);
import void Platform_OSFree (Platform_MemAdr address);
import INTEGER Platform_OldRO (CHAR *n, INTEGER n__len, INTEGER *h);
import INTEGER Platform_OldRW (CHAR *n, INTEGER n__len, INTEGER *h);
import INTEGER Platform_Read (INTEGER h, Platform_MemAdr p, INTEGER l, INTEGER *n);
import INTEGER Platform_ReadBuf (INTEGER h, BYTE *b, INTEGER b__len, INTEGER *n);
import INTEGER Platform_Rename (CHAR *o, INTEGER o__len, CHAR *n, INTEGER n__len);
import BOOLEAN Platform_SameFile (Platform_FileIdentity i1, Platform_FileIdentity i2);
import BOOLEAN Platform_SameFileTime (Platform_FileIdentity i1, Platform_FileIdentity i2);
import INTEGER Platform_Seek (INTEGER h, LONGINT offset, INTEGER whence);
import void Platform_SetBadInstructionHandler (Platform_SignalHandler handler);
import void Platform_SetHalt (Platform_HaltProcedure p);
import void Platform_SetInterruptHandler (Platform_SignalHandler handler);
import void Platform_SetMTime (Platform_FileIdentity *target, LONGINT *target__typ, Platform_FileIdentity source);
import void Platform_SetQuitHandler (Platform_SignalHandler handler);
import INTEGER Platform_Size (INTEGER h, LONGINT *len);
import INTEGER Platform_Sync (INTEGER h);
import INTEGER Platform_System (CHAR *cmd, INTEGER cmd__len);
import INTEGER Platform_Time (void);
import BOOLEAN Platform_TimedOut (INTEGER e);
import BOOLEAN Platform_TooManyFiles (INTEGER e);
import INTEGER Platform_Truncate (INTEGER h, INTEGER l);
import INTEGER Platform_Unlink (CHAR *n, INTEGER n__len);
import INTEGER Platform_Write (INTEGER h, Platform_MemAdr p, INTEGER l);
import BOOLEAN Platform_getEnv (CHAR *var, INTEGER var__len, CHAR *val, INTEGER val__len);
import void *Platform__init(void);

#define Platform_InvalidHandleValue()	(-1)
#define Platform_ToBYTE(s)	((BYTE)(s))
#define Platform_ToINT(l)	((INTEGER)(l))
#define Platform_ToSHORT(i)	((SHORTINT)(i))
#define Platform_UBYTE(b)	((SHORTINT)(unsigned char)(b))
#define Platform_UINT(i)	((LONGINT)(unsigned int)(i))
#define Platform_USHORT(s)	((INTEGER)(unsigned short)(s))

#endif
