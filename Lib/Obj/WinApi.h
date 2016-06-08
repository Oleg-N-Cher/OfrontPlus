/*  Ofront 1.2 -xtspkae */

#ifndef WinApi__h
#define WinApi__h

#include "SYSTEM.h"

typedef
	struct WinApi_FILETIME {
		LONGINT dwLowDateTime, dwHighDateTime;
	} WinApi_FILETIME;

typedef
	struct WinApi_BY_HANDLE_FILE_INFORMATION {
		SET dwFileAttributes;
		WinApi_FILETIME ftCreationTime, ftLastAccessTime, ftLastWriteTime;
		LONGINT dwVolumeSerialNumber, nFileSizeHigh, nFileSizeLow, nNumberOfLinks, nFileIndexHigh, nFileIndexLow;
	} WinApi_BY_HANDLE_FILE_INFORMATION;

typedef
	struct WinApi_SECURITY_ATTRIBUTES {
		LONGINT nLength;
		SYSTEM_PTR lpSecurityDescriptor;
		LONGINT bInheritHandle;
	} WinApi_SECURITY_ATTRIBUTES;



import LONGINT *WinApi_SECURITY_ATTRIBUTES__typ;
import LONGINT *WinApi_FILETIME__typ;
import LONGINT *WinApi_BY_HANDLE_FILE_INFORMATION__typ;

import void *WinApi__init(void);

#define WinApi_CloseHandle(hObject)	CloseHandle(hObject)
__EXTERN LONGINT __CALL_1 CloseHandle(SYSTEM_PTR hObject);
#define WinApi_CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, lpSecurityAttributes__typ, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile)	CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile)
__EXTERN SYSTEM_PTR __CALL_1 CreateFileA(SYSTEM_PTR lpFileName, SET dwDesiredAccess, SET dwShareMode, WinApi_SECURITY_ATTRIBUTES *lpSecurityAttributes, LONGINT dwCreationDisposition, SET dwFlagsAndAttributes, SYSTEM_PTR hTemplateFile);
#define WinApi_GetFileInformationByHandle(hFile, lpFileInformation, lpFileInformation__typ)	GetFileInformationByHandle(hFile, lpFileInformation)
__EXTERN LONGINT __CALL_1 GetFileInformationByHandle(SYSTEM_PTR hFile, WinApi_BY_HANDLE_FILE_INFORMATION *lpFileInformation);
#define WinApi__init()	/*-noinit*/

#endif
