/* Ofront+ 0.9 -ske */
#include "SYSTEM.h"

typedef
	CHAR (*Platform_ArgPtr)[1024];

typedef
	Platform_ArgPtr (*Platform_ArgVec)[1024];

typedef
	CHAR (*Platform_FileHandle)[1];

typedef
	struct Platform_FileIdentity {
		INTEGER volume, indexhigh, indexlow, mtimehigh, mtimelow;
	} Platform_FileIdentity;

typedef
	void (*Platform_HaltProcedure)(INTEGER);

typedef
	CHAR (*Platform_MemAdr)[1];

typedef
	void (*Platform_SignalHandler)(INTEGER);


export BOOLEAN Platform_LittleEndian;
export Platform_MemAdr Platform_MainStackFrame;
export INTEGER Platform_HaltCode, Platform_PID;
export CHAR Platform_CWD[4096];
export INTEGER Platform_ArgCount;
export Platform_ArgVec Platform_ArgVector;
static Platform_HaltProcedure Platform_HaltHandler;
static INTEGER Platform_TimeStart;
export INTEGER Platform_SeekSet, Platform_SeekCur, Platform_SeekEnd;
export Platform_FileHandle Platform_StdIn, Platform_StdOut, Platform_StdErr;
export CHAR Platform_newLine[3];

export LONGINT *Platform_FileIdentity__typ;

export BOOLEAN Platform_Absent (INTEGER e);
export INTEGER Platform_ArgPos (CHAR *s, INTEGER s__len);
export void Platform_AssertFail (INTEGER code);
export INTEGER Platform_Chdir (CHAR *n, INTEGER n__len);
export INTEGER Platform_Close (Platform_FileHandle h);
export BOOLEAN Platform_ConnectionFailed (INTEGER e);
export void Platform_Delay (INTEGER ms);
export BOOLEAN Platform_DifferentFilesystems (INTEGER e);
static void Platform_DisplayHaltCode (INTEGER code);
export INTEGER Platform_Error (void);
export void Platform_Exit (INTEGER code);
export void Platform_GetArg (INTEGER n, CHAR *val, INTEGER val__len);
export void Platform_GetClock (INTEGER *t, INTEGER *d);
export void Platform_GetEnv (CHAR *var, INTEGER var__len, CHAR *val, INTEGER val__len);
export void Platform_GetIntArg (INTEGER n, INTEGER *val);
export void Platform_Halt (INTEGER code);
export INTEGER Platform_Identify (Platform_FileHandle h, Platform_FileIdentity *identity, LONGINT *identity__typ);
export INTEGER Platform_IdentifyByName (CHAR *n, INTEGER n__len, Platform_FileIdentity *identity, LONGINT *identity__typ);
export BOOLEAN Platform_Inaccessible (INTEGER e);
export void Platform_Init (INTEGER argc, Platform_MemAdr argvadr);
export void Platform_MTimeAsClock (Platform_FileIdentity i, INTEGER *t, INTEGER *d);
export INTEGER Platform_New (CHAR *n, INTEGER n__len, Platform_FileHandle *h);
export BOOLEAN Platform_NoSuchDirectory (INTEGER e);
export Platform_MemAdr Platform_OSAllocate (INTEGER size);
export void Platform_OSFree (Platform_MemAdr address);
export INTEGER Platform_OldRO (CHAR *n, INTEGER n__len, Platform_FileHandle *h);
export INTEGER Platform_OldRW (CHAR *n, INTEGER n__len, Platform_FileHandle *h);
export INTEGER Platform_Read (Platform_FileHandle h, Platform_MemAdr p, INTEGER l, INTEGER *n);
export INTEGER Platform_ReadBuf (Platform_FileHandle h, BYTE *b, INTEGER b__len, INTEGER *n);
export INTEGER Platform_Rename (CHAR *o, INTEGER o__len, CHAR *n, INTEGER n__len);
export BOOLEAN Platform_SameFile (Platform_FileIdentity i1, Platform_FileIdentity i2);
export BOOLEAN Platform_SameFileTime (Platform_FileIdentity i1, Platform_FileIdentity i2);
export INTEGER Platform_Seek (Platform_FileHandle h, LONGINT o, INTEGER r);
export void Platform_SetBadInstructionHandler (Platform_SignalHandler handler);
export void Platform_SetHalt (Platform_HaltProcedure p);
export void Platform_SetMTime (Platform_FileIdentity *target, LONGINT *target__typ, Platform_FileIdentity source);
export INTEGER Platform_Size (Platform_FileHandle h, LONGINT *len);
export INTEGER Platform_Sync (Platform_FileHandle h);
export INTEGER Platform_System (CHAR *cmd, INTEGER cmd__len);
static void Platform_TestLittleEndian (void);
export INTEGER Platform_Time (void);
export BOOLEAN Platform_TimedOut (INTEGER e);
export BOOLEAN Platform_TooManyFiles (INTEGER e);
export INTEGER Platform_Truncate (Platform_FileHandle h, LONGINT limit);
export INTEGER Platform_Unlink (CHAR *n, INTEGER n__len);
export INTEGER Platform_Write (Platform_FileHandle h, Platform_MemAdr p, INTEGER l);
static void Platform_YMDHMStoClock (INTEGER ye, INTEGER mo, INTEGER da, INTEGER ho, INTEGER mi, INTEGER se, INTEGER *t, INTEGER *d);
static void Platform_errch (CHAR c);
static void Platform_errint (INTEGER l);
static void Platform_errln (void);
static void Platform_errposint (INTEGER l);
export BOOLEAN Platform_getEnv (CHAR *var, INTEGER var__len, CHAR *val, INTEGER val__len);

#include "_windows.h"
#define Platform_ECONNABORTED()	WSAECONNABORTED
#define Platform_ECONNREFUSED()	WSAECONNREFUSED
#define Platform_EHOSTUNREACH()	WSAEHOSTUNREACH
#define Platform_ENETUNREACH()	WSAENETUNREACH
#define Platform_ERRORACCESSDENIED()	ERROR_ACCESS_DENIED
#define Platform_ERRORFILENOTFOUND()	ERROR_FILE_NOT_FOUND
#define Platform_ERRORNOTREADY()	ERROR_NOT_READY
#define Platform_ERRORNOTSAMEDEVICE()	ERROR_NOT_SAME_DEVICE
#define Platform_ERRORPATHNOTFOUND()	ERROR_PATH_NOT_FOUND
#define Platform_ERRORSHARINGVIOLATION()	ERROR_SHARING_VIOLATION
#define Platform_ERRORTOOMANYOPENFILES()	ERROR_TOO_MANY_OPEN_FILES
#define Platform_ERRORWRITEPROTECT()	ERROR_WRITE_PROTECT
#define Platform_ETIMEDOUT()	WSAETIMEDOUT
extern void Heap_InitHeap();
#define Platform_GetTickCount()	(INTEGER)GetTickCount()
#define Platform_HeapInitHeap()	Heap_InitHeap()
#define Platform_InvalidHandleValue()	((Platform_FileHandle)(SYSTEM_ADR)-1)
#define Platform_SetInterruptHandler(h)	SystemSetInterruptHandler((SYSTEM_ADR)h)
#define Platform_SetQuitHandler(h)	SystemSetQuitHandler((SYSTEM_ADR)h)
#define Platform_ToBYTE(s)	((BYTE)(s))
#define Platform_ToINT(l)	((INTEGER)(l))
#define Platform_ToSHORT(i)	((SHORTINT)(i))
#define Platform_UBYTE(b)	((SHORTINT)(unsigned char)(b))
#define Platform_UINT(i)	((LONGINT)(unsigned int)(i))
#define Platform_USHORT(s)	((INTEGER)(unsigned short)(s))
#define Platform_allocate(size)	((Platform_MemAdr)HeapAlloc(GetProcessHeap(), 0, (size_t)(size)))
#define Platform_bhfiIndexHigh()	(INTEGER)bhfi.nFileIndexHigh
#define Platform_bhfiIndexLow()	(INTEGER)bhfi.nFileIndexLow
#define Platform_bhfiMtimeHigh()	(INTEGER)bhfi.ftLastWriteTime.dwHighDateTime
#define Platform_bhfiMtimeLow()	(INTEGER)bhfi.ftLastWriteTime.dwLowDateTime
#define Platform_bhfiVsn()	(INTEGER)bhfi.dwVolumeSerialNumber
#define Platform_byHandleFileInformation()	BY_HANDLE_FILE_INFORMATION bhfi
#define Platform_cleanupProcess()	CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
#define Platform_closeHandle(h)	(INTEGER)CloseHandle((HANDLE)h)
#define Platform_createProcess(str, str__len)	(INTEGER)CreateProcess(0, (char*)str, 0,0,0,0,0,0,&si,&pi)
#define Platform_deleteFile(n, n__len)	(INTEGER)DeleteFile((char*)n)
#define Platform_err()	(INTEGER)GetLastError()
#define Platform_errc(c)	{ DWORD dummy; WriteFile((HANDLE)Platform_StdOut, &c, 1, &dummy, 0); }
#define Platform_errstring(s, s__len)	{ DWORD dummy; WriteFile((HANDLE)Platform_StdOut, s, s__len-1, &dummy, 0); }
#define Platform_exit(code)	ExitProcess((UINT)code)
#define Platform_fileTimeToSysTime()	SYSTEMTIME st; FileTimeToSystemTime(&ft, &st)
#define Platform_flushFileBuffers(h)	(INTEGER)FlushFileBuffers((HANDLE)h)
#define Platform_free(address)	HeapFree(GetProcessHeap(), 0, (void*)address)
#define Platform_getCurrentDirectory(n, n__len)	GetCurrentDirectory((DWORD)n__len, (char*)n)
#define Platform_getExitCodeProcess(exitcode)	GetExitCodeProcess(pi.hProcess, (DWORD*)exitcode);
#define Platform_getFileInformationByHandle(h)	(INTEGER)GetFileInformationByHandle((HANDLE)h, &bhfi)
#define Platform_getFilePos(h, r, rc)	LARGE_INTEGER liz = {0}; *rc = (INTEGER)SetFilePointerEx((HANDLE)h, liz, &li, FILE_CURRENT); *r = (LONGINT)li.QuadPart
#define Platform_getFileSize(h)	(INTEGER)GetFileSizeEx((HANDLE)h, &li)
#define Platform_getLocalTime()	SYSTEMTIME st; GetLocalTime(&st)
#define Platform_getenv(name, name__len, buf, buf__len)	(INTEGER)GetEnvironmentVariable((char*)name, (char*)buf, buf__len)
#define Platform_getpid()	(INTEGER)GetCurrentProcessId()
#define Platform_getstderrhandle()	(Platform_FileHandle)GetStdHandle(STD_ERROR_HANDLE)
#define Platform_getstdinhandle()	(Platform_FileHandle)GetStdHandle(STD_INPUT_HANDLE)
#define Platform_getstdouthandle()	(Platform_FileHandle)GetStdHandle(STD_OUTPUT_HANDLE)
#define Platform_identityToFileTime(i)	FILETIME ft; ft.dwHighDateTime = i.mtimehigh; ft.dwLowDateTime = i.mtimelow
#define Platform_largeInteger()	LARGE_INTEGER li
#define Platform_liLongint()	(LONGINT)li.QuadPart
#define Platform_moveFile(o, o__len, n, n__len)	(INTEGER)MoveFileEx((char*)o, (char*)n, MOVEFILE_REPLACE_EXISTING)
#define Platform_opennew(n, n__len)	(Platform_FileHandle)CreateFile((char*)n, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0)
#define Platform_openro(n, n__len)	(Platform_FileHandle)CreateFile((char*)n, GENERIC_READ              , FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)
#define Platform_openrw(n, n__len)	(Platform_FileHandle)CreateFile((char*)n, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)
#define Platform_processInfo()	PROCESS_INFORMATION pi = {0};
#define Platform_readfile(fd, p, l, n)	(INTEGER)ReadFile ((HANDLE)fd, (void*)(p), (DWORD)l, (DWORD*)n, 0)
#define Platform_seekcur()	FILE_CURRENT
#define Platform_seekend()	FILE_END
#define Platform_seekset()	FILE_BEGIN
#define Platform_setCurrentDirectory(n, n__len)	(INTEGER)SetCurrentDirectory((char*)n)
#define Platform_setEndOfFile(h)	(INTEGER)SetEndOfFile((HANDLE)h)
#define Platform_setFilePointerEx(h, o, r, rc)	li.QuadPart=o; *rc = (INTEGER)SetFilePointerEx((HANDLE)h, li, 0, (DWORD)r)
#define Platform_sleep(ms)	Sleep((DWORD)ms)
#define Platform_startupInfo()	STARTUPINFO si = {0}; si.cb = sizeof(si);
#define Platform_sthour()	(INTEGER)st.wHour
#define Platform_stmday()	(INTEGER)st.wDay
#define Platform_stmin()	(INTEGER)st.wMinute
#define Platform_stmon()	(INTEGER)st.wMonth
#define Platform_stsec()	(INTEGER)st.wSecond
#define Platform_styear()	(INTEGER)st.wYear
#define Platform_waitForProcess()	(INTEGER)WaitForSingleObject(pi.hProcess, INFINITE)
#define Platform_writefile(fd, p, l, dummy)	(INTEGER)WriteFile((HANDLE)fd, (void*)(p), (DWORD)l, (LPDWORD)dummy, 0)

/*============================================================================*/

BOOLEAN Platform_TooManyFiles (INTEGER e)
{
	return e == Platform_ERRORTOOMANYOPENFILES();
}

/*----------------------------------------------------------------------------*/
BOOLEAN Platform_NoSuchDirectory (INTEGER e)
{
	return e == Platform_ERRORPATHNOTFOUND();
}

/*----------------------------------------------------------------------------*/
BOOLEAN Platform_DifferentFilesystems (INTEGER e)
{
	return e == Platform_ERRORNOTSAMEDEVICE();
}

/*----------------------------------------------------------------------------*/
BOOLEAN Platform_Inaccessible (INTEGER e)
{
	return ((e == Platform_ERRORACCESSDENIED() || e == Platform_ERRORWRITEPROTECT()) || e == Platform_ERRORNOTREADY()) || e == Platform_ERRORSHARINGVIOLATION();
}

/*----------------------------------------------------------------------------*/
BOOLEAN Platform_Absent (INTEGER e)
{
	return e == Platform_ERRORFILENOTFOUND() || e == Platform_ERRORPATHNOTFOUND();
}

/*----------------------------------------------------------------------------*/
BOOLEAN Platform_TimedOut (INTEGER e)
{
	return e == Platform_ETIMEDOUT();
}

/*----------------------------------------------------------------------------*/
BOOLEAN Platform_ConnectionFailed (INTEGER e)
{
	return ((e == Platform_ECONNREFUSED() || e == Platform_ECONNABORTED()) || e == Platform_ENETUNREACH()) || e == Platform_EHOSTUNREACH();
}

/*----------------------------------------------------------------------------*/
Platform_MemAdr Platform_OSAllocate (INTEGER size)
{
	if (size > 0) {
		return Platform_allocate(size);
	}
	Platform_Halt(-25);
	return NIL;
}

/*----------------------------------------------------------------------------*/
void Platform_OSFree (Platform_MemAdr address)
{
	Platform_free(address);
}

/*----------------------------------------------------------------------------*/
typedef
	Platform_ArgVec (*ArgVecPtr__38)[1];

void Platform_Init (INTEGER argc, Platform_MemAdr argvadr)
{
	ArgVecPtr__38 av;
	Platform_MainStackFrame = argvadr;
	Platform_ArgCount = argc;
	av = (ArgVecPtr__38)argvadr;
	Platform_ArgVector = (*av)[0];
	Platform_HaltCode = -128;
	Platform_HeapInitHeap();
}

/*----------------------------------------------------------------------------*/
BOOLEAN Platform_getEnv (CHAR *var, INTEGER var__len, CHAR *val, INTEGER val__len)
{
	CHAR buf[4096];
	INTEGER res;
	__DUP(var, var__len);
	res = Platform_getenv(var, var__len, (void*)buf, 4096);
	if (res > 0 && res < 4096) {
		__COPY(buf, val, val__len);
		__DEL(var);
		return 1;
	} else {
		__DEL(var);
		return 0;
	}
	__RETCHK;
}

/*----------------------------------------------------------------------------*/
void Platform_GetEnv (CHAR *var, INTEGER var__len, CHAR *val, INTEGER val__len)
{
	__DUP(var, var__len);
	if (!Platform_getEnv(var, var__len, (void*)val, val__len)) {
		val[0] = 0x00;
	}
	__DEL(var);
}

/*----------------------------------------------------------------------------*/
void Platform_GetArg (INTEGER n, CHAR *val, INTEGER val__len)
{
	if (n < Platform_ArgCount) {
		__COPY(*(*Platform_ArgVector)[n], val, val__len);
	}
}

/*----------------------------------------------------------------------------*/
void Platform_GetIntArg (INTEGER n, INTEGER *val)
{
	CHAR s[64];
	INTEGER k, d, i;
	s[0] = 0x00;
	Platform_GetArg(n, (void*)s, 64);
	i = 0;
	if (s[0] == '-') {
		i = 1;
	}
	k = 0;
	d = (SHORTINT)s[i] - 48;
	while (d >= 0 && d <= 9) {
		k = k * 10 + d;
		i += 1;
		d = (SHORTINT)s[i] - 48;
	}
	if (s[0] == '-') {
		k = -k;
		i -= 1;
	}
	if (i > 0) {
		*val = k;
	}
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_ArgPos (CHAR *s, INTEGER s__len)
{
	INTEGER i;
	CHAR arg[256];
	__DUP(s, s__len);
	i = 0;
	Platform_GetArg(i, (void*)arg, 256);
	while (i < Platform_ArgCount && __STRCMP(s, arg) != 0) {
		i += 1;
		Platform_GetArg(i, (void*)arg, 256);
	}
	__DEL(s);
	return i;
}

/*----------------------------------------------------------------------------*/
void Platform_SetBadInstructionHandler (Platform_SignalHandler handler)
{
}

/*----------------------------------------------------------------------------*/
static void Platform_YMDHMStoClock (INTEGER ye, INTEGER mo, INTEGER da, INTEGER ho, INTEGER mi, INTEGER se, INTEGER *t, INTEGER *d)
{
	*d = (__ASHL((INTEGER)__MOD(ye, 100), 9, INTEGER) + __ASHL(mo + 1, 5, INTEGER)) + da;
	*t = (__ASHL(ho, 12, INTEGER) + __ASHL(mi, 6, INTEGER)) + se;
}

void Platform_GetClock (INTEGER *t, INTEGER *d)
{
	Platform_getLocalTime();
	Platform_YMDHMStoClock(Platform_styear(), Platform_stmon(), Platform_stmday(), Platform_sthour(), Platform_stmin(), Platform_stsec(), &*t, &*d);
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_Time (void)
{
	INTEGER ms;
	ms = Platform_GetTickCount();
	return (INTEGER)__MOD(ms - Platform_TimeStart, 2147483647);
}

/*----------------------------------------------------------------------------*/
void Platform_Delay (INTEGER ms)
{
	while (ms > 30000) {
		Platform_sleep(30000);
		ms = ms - 30000;
	}
	if (ms > 0) {
		Platform_sleep(ms);
	}
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_System (CHAR *cmd, INTEGER cmd__len)
{
	INTEGER result;
	__DUP(cmd, cmd__len);
	result = 127;
	Platform_startupInfo();
	Platform_processInfo();
	if (Platform_createProcess(cmd, cmd__len) != 0) {
		if (Platform_waitForProcess() == 0) {
			Platform_getExitCodeProcess(&result);
		}
		Platform_cleanupProcess();
	}
	__DEL(cmd);
	return __ASHL(result, 8, INTEGER);
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_Error (void)
{
	return Platform_err();
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_OldRO (CHAR *n, INTEGER n__len, Platform_FileHandle *h)
{
	Platform_FileHandle fd;
	fd = Platform_openro(n, n__len);
	if (fd == Platform_InvalidHandleValue()) {
		return Platform_err();
	} else {
		*h = fd;
		return 0;
	}
	__RETCHK;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_OldRW (CHAR *n, INTEGER n__len, Platform_FileHandle *h)
{
	Platform_FileHandle fd;
	fd = Platform_openrw(n, n__len);
	if (fd == Platform_InvalidHandleValue()) {
		return Platform_err();
	} else {
		*h = fd;
		return 0;
	}
	__RETCHK;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_New (CHAR *n, INTEGER n__len, Platform_FileHandle *h)
{
	Platform_FileHandle fd;
	fd = Platform_opennew(n, n__len);
	if (fd == Platform_InvalidHandleValue()) {
		return Platform_err();
	} else {
		*h = fd;
		return 0;
	}
	__RETCHK;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_Close (Platform_FileHandle h)
{
	if (Platform_closeHandle(h) == 0) {
		return Platform_err();
	} else {
		return 0;
	}
	__RETCHK;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_Identify (Platform_FileHandle h, Platform_FileIdentity *identity, LONGINT *identity__typ)
{
	Platform_byHandleFileInformation();
	if (Platform_getFileInformationByHandle(h) == 0) {
		return Platform_err();
	}
	(*identity).volume = Platform_bhfiVsn();
	(*identity).indexhigh = Platform_bhfiIndexHigh();
	(*identity).indexlow = Platform_bhfiIndexLow();
	(*identity).mtimehigh = Platform_bhfiMtimeHigh();
	(*identity).mtimelow = Platform_bhfiMtimeLow();
	return 0;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_IdentifyByName (CHAR *n, INTEGER n__len, Platform_FileIdentity *identity, LONGINT *identity__typ)
{
	Platform_FileHandle h;
	INTEGER e, i;
	__DUP(n, n__len);
	e = Platform_OldRO((void*)n, n__len, &h);
	if (e != 0) {
		__DEL(n);
		return e;
	}
	e = Platform_Identify(h, &*identity, identity__typ);
	i = Platform_Close(h);
	__DEL(n);
	return e;
}

/*----------------------------------------------------------------------------*/
BOOLEAN Platform_SameFile (Platform_FileIdentity i1, Platform_FileIdentity i2)
{
	return (i1.indexhigh == i2.indexhigh && i1.indexlow == i2.indexlow) && i1.volume == i2.volume;
}

/*----------------------------------------------------------------------------*/
BOOLEAN Platform_SameFileTime (Platform_FileIdentity i1, Platform_FileIdentity i2)
{
	return i1.mtimehigh == i2.mtimehigh && i1.mtimelow == i2.mtimelow;
}

/*----------------------------------------------------------------------------*/
void Platform_SetMTime (Platform_FileIdentity *target, LONGINT *target__typ, Platform_FileIdentity source)
{
	(*target).mtimehigh = source.mtimehigh;
	(*target).mtimelow = source.mtimelow;
}

/*----------------------------------------------------------------------------*/
void Platform_MTimeAsClock (Platform_FileIdentity i, INTEGER *t, INTEGER *d)
{
	Platform_identityToFileTime(i);
	Platform_fileTimeToSysTime();
	Platform_YMDHMStoClock(Platform_styear(), Platform_stmon(), Platform_stmday(), Platform_sthour(), Platform_stmin(), Platform_stsec(), &*t, &*d);
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_Size (Platform_FileHandle h, LONGINT *len)
{
	Platform_largeInteger();
	if (Platform_getFileSize(h) == 0) {
		return Platform_err();
	}
	*len = Platform_liLongint();
	return 0;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_Read (Platform_FileHandle h, Platform_MemAdr p, INTEGER l, INTEGER *n)
{
	INTEGER result;
	*n = 0;
	result = Platform_readfile(h, p, l, &*n);
	if (result == 0) {
		*n = 0;
		return Platform_err();
	} else {
		return 0;
	}
	__RETCHK;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_ReadBuf (Platform_FileHandle h, BYTE *b, INTEGER b__len, INTEGER *n)
{
	INTEGER result;
	*n = 0;
	result = Platform_readfile(h, (Platform_MemAdr)((LONGINT)b), b__len, &*n);
	if (result == 0) {
		*n = 0;
		return Platform_err();
	} else {
		return 0;
	}
	__RETCHK;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_Write (Platform_FileHandle h, Platform_MemAdr p, INTEGER l)
{
	INTEGER dummy;
	if (Platform_writefile(h, p, l, &dummy) == 0) {
		return Platform_err();
	} else {
		return 0;
	}
	__RETCHK;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_Sync (Platform_FileHandle h)
{
	if (Platform_flushFileBuffers(h) == 0) {
		return Platform_err();
	} else {
		return 0;
	}
	__RETCHK;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_Seek (Platform_FileHandle h, LONGINT o, INTEGER r)
{
	INTEGER rc;
	Platform_largeInteger();
	Platform_setFilePointerEx(h, o, r, &rc);
	if (rc == 0) {
		return Platform_err();
	} else {
		return 0;
	}
	__RETCHK;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_Truncate (Platform_FileHandle h, LONGINT limit)
{
	INTEGER rc;
	LONGINT oldpos;
	Platform_largeInteger();
	Platform_getFilePos(h, &oldpos, &rc);
	if (rc == 0) {
		return Platform_err();
	}
	Platform_setFilePointerEx(h, limit, Platform_seekset(), &rc);
	if (rc == 0) {
		return Platform_err();
	}
	if (Platform_setEndOfFile(h) == 0) {
		return Platform_err();
	}
	Platform_setFilePointerEx(h, oldpos, Platform_seekset(), &rc);
	if (rc == 0) {
		return Platform_err();
	}
	return 0;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_Unlink (CHAR *n, INTEGER n__len)
{
	if (Platform_deleteFile(n, n__len) == 0) {
		return Platform_err();
	} else {
		return 0;
	}
	__RETCHK;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_Chdir (CHAR *n, INTEGER n__len)
{
	INTEGER r;
	r = Platform_setCurrentDirectory(n, n__len);
	if (r == 0) {
		return Platform_err();
	}
	Platform_getCurrentDirectory((void*)Platform_CWD, 4096);
	return 0;
}

/*----------------------------------------------------------------------------*/
INTEGER Platform_Rename (CHAR *o, INTEGER o__len, CHAR *n, INTEGER n__len)
{
	if (Platform_moveFile(o, o__len, n, n__len) == 0) {
		return Platform_err();
	} else {
		return 0;
	}
	__RETCHK;
}

/*----------------------------------------------------------------------------*/
void Platform_Exit (INTEGER code)
{
	Platform_exit(code);
}

/*----------------------------------------------------------------------------*/
static void Platform_errch (CHAR c)
{
	Platform_errc(c);
}

static void Platform_errln (void)
{
	Platform_errch(0x0d);
	Platform_errch(0x0a);
}

static void Platform_errposint (INTEGER l)
{
	if (l > 10) {
		Platform_errposint(__DIV(l, 10));
	}
	Platform_errch((CHAR)(48 + (INTEGER)__MOD(l, 10)));
}

static void Platform_errint (INTEGER l)
{
	if (l < 0) {
		Platform_errch('-');
		l = -l;
	}
	Platform_errposint(l);
}

static void Platform_DisplayHaltCode (INTEGER code)
{
	switch (code) {
		case -1: 
			Platform_errstring((CHAR*)"Rider ReadBuf/WriteBuf transfer size longer than buffer.", 57);
			break;
		case -2: 
			Platform_errstring((CHAR*)"Index out of range.", 20);
			break;
		case -3: 
			Platform_errstring((CHAR*)"Reached end of function without reaching RETURN.", 49);
			break;
		case -4: 
			Platform_errstring((CHAR*)"CASE statement: no matching label and no ELSE.", 47);
			break;
		case -5: 
			Platform_errstring((CHAR*)"Type guard failed.", 19);
			break;
		case -6: 
			Platform_errstring((CHAR*)"Type equality failed.", 22);
			break;
		case -7: 
			Platform_errstring((CHAR*)"WITH statement type guard failed.", 34);
			break;
		case -8: 
			Platform_errstring((CHAR*)"SHORT: Value too large for shorter type.", 41);
			break;
		case -9: 
			Platform_errstring((CHAR*)"Heap interrupted while locked, but lockdepth = 0 at unlock.", 60);
			break;
		case -15: 
			Platform_errstring((CHAR*)"Type descriptor size mismatch.", 31);
			break;
		case -20: 
			Platform_errstring((CHAR*)"Too many, or negative number of, elements in dynamic array.", 60);
			break;
		case -25: 
			Platform_errstring((CHAR*)"Memory allocation error.", 25);
			break;
		default: 
			break;
	}
}

void Platform_Halt (INTEGER code)
{
	INTEGER e;
	Platform_HaltCode = code;
	if (Platform_HaltHandler != NIL) {
		(*Platform_HaltHandler)(code);
	}
	Platform_errstring((CHAR*)"Terminated by Halt(", 20);
	Platform_errint(code);
	Platform_errstring((CHAR*)"). ", 4);
	if (code < 0) {
		Platform_DisplayHaltCode(code);
	}
	Platform_errln();
	Platform_exit(code);
}

/*----------------------------------------------------------------------------*/
void Platform_AssertFail (INTEGER code)
{
	INTEGER e;
	Platform_errstring((CHAR*)"Assertion failure.", 19);
	if (code != 0) {
		Platform_errstring((CHAR*)" ASSERT code ", 14);
		Platform_errint(code);
		Platform_errstring((CHAR*)".", 2);
	}
	Platform_errln();
	Platform_exit(code);
}

/*----------------------------------------------------------------------------*/
void Platform_SetHalt (Platform_HaltProcedure p)
{
	Platform_HaltHandler = p;
}

/*----------------------------------------------------------------------------*/
static void Platform_TestLittleEndian (void)
{
	INTEGER i;
	i = 1;
	__GET((LONGINT)&i, Platform_LittleEndian, BOOLEAN);
}

__TDESC(Platform_FileIdentity__desc, 1, 0) = {__TDFLDS("FileIdentity", 20), {-8}};

export void *Platform__init(void)
{
	__DEFMOD;
	__REGMOD("Platform", 0);
	__INITYP(Platform_FileIdentity, Platform_FileIdentity, 0);
/* BEGIN */
	Platform_TestLittleEndian();
	Platform_HaltCode = -128;
	Platform_HaltHandler = NIL;
	Platform_TimeStart = 0;
	Platform_TimeStart = Platform_Time();
	Platform_CWD[0] = 0x00;
	Platform_getCurrentDirectory((void*)Platform_CWD, 4096);
	Platform_PID = Platform_getpid();
	Platform_SeekSet = Platform_seekset();
	Platform_SeekCur = Platform_seekcur();
	Platform_SeekEnd = Platform_seekend();
	Platform_StdIn = Platform_getstdinhandle();
	Platform_StdOut = Platform_getstdouthandle();
	Platform_StdErr = Platform_getstderrhandle();
	Platform_newLine[0] = 0x0d;
	Platform_newLine[1] = 0x0a;
	Platform_newLine[2] = 0x00;
	__ENDMOD;
}
