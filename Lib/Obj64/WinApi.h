/*  Ofront+ 1.0 -xtspkae */

#ifndef WinApi__h
#define WinApi__h

#include "SYSTEM.h"

typedef
	struct WinApi_FILETIME {
		INTEGER dwLowDateTime, dwHighDateTime;
	} WinApi_FILETIME;

typedef
	struct WinApi_BY_HANDLE_FILE_INFORMATION {
		SET dwFileAttributes;
		WinApi_FILETIME ftCreationTime, ftLastAccessTime, ftLastWriteTime;
		INTEGER dwVolumeSerialNumber, nFileSizeHigh, nFileSizeLow, nNumberOfLinks, nFileIndexHigh, nFileIndexLow;
	} WinApi_BY_HANDLE_FILE_INFORMATION;

typedef
	struct WinApi_SECURITY_ATTRIBUTES {
		INTEGER nLength;
		SYSTEM_PTR lpSecurityDescriptor;
		INTEGER bInheritHandle;
	} WinApi_SECURITY_ATTRIBUTES;



import LONGINT *WinApi_SECURITY_ATTRIBUTES__typ;
import LONGINT *WinApi_FILETIME__typ;
import LONGINT *WinApi_BY_HANDLE_FILE_INFORMATION__typ;

import void *WinApi__init(void);

#define WinApi_CloseHandle(hObject)	CloseHandle(hObject)
__EXTERN INTEGER __CALL_1 CloseHandle(SYSTEM_PTR hObject);
#define WinApi_CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, lpSecurityAttributes__typ, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile)	CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile)
__EXTERN SYSTEM_PTR __CALL_1 CreateFileA(SYSTEM_PTR lpFileName, SET dwDesiredAccess, SET dwShareMode, WinApi_SECURITY_ATTRIBUTES *lpSecurityAttributes, INTEGER dwCreationDisposition, SET dwFlagsAndAttributes, SYSTEM_PTR hTemplateFile);
#define WinApi_GetFileInformationByHandle(hFile, lpFileInformation, lpFileInformation__typ)	GetFileInformationByHandle(hFile, lpFileInformation)
__EXTERN INTEGER __CALL_1 GetFileInformationByHandle(SYSTEM_PTR hFile, WinApi_BY_HANDLE_FILE_INFORMATION *lpFileInformation);
#define WinApi__init()	/*-noinit*/

#endif
