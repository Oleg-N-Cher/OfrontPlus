MODULE Platform; (** Windows *)
IMPORT SYSTEM;

(* Based on Vishap Oberon (voc) runtime by David C W Brown, 2016-2018
   Improvements and Windows 9x compatibility by Oleg N. Cher, 2019-2020

   TODO:
   Use Unicode APIs with manual UTF-8 conversion and prepend "\\?\" to
   file paths in order to get 32768 character path length limit (as
   opposed to 256 bytes. *)


CONST
  MAX_PATH = 260;

  PathDelimiter* = "\";
  PathSeparator* = ";";

  Unix*    = FALSE;
  Windows* = TRUE;

  NewLine* = 0DX + 0AX;  (* Platform specific new line representation *)


TYPE
  CHAR = SHORTCHAR; BOOL = INTEGER; DWORD = INTEGER;
  ADRINT = SYSTEM.ADRINT; (* 32 or 64 bits *)
  LARGE_INTEGER = RECORD [notag] low, high: INTEGER END;

  ErrorCode*  = INTEGER;
  FileHandle* = ADRINT; (* 32 or 64 bits *)
  (* In Windows, the file handle is used to identify a file. When a file is
     opened by a process using the CreateFile function, a file handle is
     associated with it until either the process terminates or the handle is
     closed by using the CloseHandle function. But a HANDLE is not a memory
     address. Although HANDLE is typedef'd as void *, that's just to make it
     more opaque because it prohibits arithmetic. *)

  FileIdentity* = RECORD [notag]
    volume:    DWORD;(* dev on Unix filesystems, volume serial number on NTFS *)
    indexhigh: DWORD;  (* inode on Unix filesystems, file id on NTFS *)
    indexlow:  DWORD;
    mtimehigh: DWORD;  (* File modification time, value is system dependent *)
    mtimelow:  DWORD;  (* File modification time, value is system dependent *)
  END;

  ArgPtr* = POINTER [notag] TO ARRAY 1024 OF CHAR;
  ArgVec* = POINTER [notag] TO ARRAY 1024 OF ArgPtr;


VAR
  LittleEndian-:    BOOLEAN;
  PID-:             DWORD;  (* Note: Must be updated by Fork implementation *)
  CWD-:             ARRAY MAX_PATH OF CHAR;

  TimeStart:        DWORD;  (* milliseconds *)

  SeekSet-:         INTEGER;
  SeekCur-:         INTEGER;
  SeekEnd-:         INTEGER;

  StdIn-:           FileHandle;
  StdOut-:          FileHandle;
  StdErr-:          FileHandle;


(* Some unsigned arithmetic *)

PROCEDURE -ToBYTE*(s: SHORTINT): BYTE "((BYTE)(s))";
PROCEDURE -ToSHORT*(i: INTEGER): SHORTINT "((SHORTINT)(i))";
PROCEDURE -ToINT*(l: LONGINT): INTEGER "((INTEGER)(l))";


PROCEDURE -AAincludeWindowsWrapper '#include "_windows.h"';


(* Error code tests *)

PROCEDURE -ERRORCALLNOTIMPLEMENTED(): ErrorCode "ERROR_CALL_NOT_IMPLEMENTED";
PROCEDURE -ERRORTOOMANYOPENFILES():   ErrorCode "ERROR_TOO_MANY_OPEN_FILES";
PROCEDURE -ERRORPATHNOTFOUND():       ErrorCode "ERROR_PATH_NOT_FOUND";
PROCEDURE -ERRORFILENOTFOUND():       ErrorCode "ERROR_FILE_NOT_FOUND";
PROCEDURE -ERRORNOTSAMEDEVICE():      ErrorCode "ERROR_NOT_SAME_DEVICE";
PROCEDURE -ERRORACCESSDENIED():       ErrorCode "ERROR_ACCESS_DENIED";
PROCEDURE -ERRORWRITEPROTECT():       ErrorCode "ERROR_WRITE_PROTECT";
PROCEDURE -ERRORSHARINGVIOLATION():   ErrorCode "ERROR_SHARING_VIOLATION";
PROCEDURE -ERRORNOTREADY():           ErrorCode "ERROR_NOT_READY";
PROCEDURE -ETIMEDOUT():               ErrorCode "WSAETIMEDOUT";
PROCEDURE -ECONNREFUSED():            ErrorCode "WSAECONNREFUSED";
PROCEDURE -ECONNABORTED():            ErrorCode "WSAECONNABORTED";
PROCEDURE -ENETUNREACH():             ErrorCode "WSAENETUNREACH";
PROCEDURE -EHOSTUNREACH():            ErrorCode "WSAEHOSTUNREACH";
PROCEDURE -INVALIDFILEATTRIBUTES():   ErrorCode "INVALID_FILE_ATTRIBUTES";
PROCEDURE -INVALIDSETFILEPOINTER():   ErrorCode "INVALID_SET_FILE_POINTER";
PROCEDURE -INVALIDFILESIZE():         ErrorCode "INVALID_FILE_SIZE";
PROCEDURE -NOERROR():                 ErrorCode "NO_ERROR";


PROCEDURE TooManyFiles*(e: ErrorCode): BOOLEAN;
BEGIN RETURN e = ERRORTOOMANYOPENFILES() END TooManyFiles;

PROCEDURE NoSuchDirectory*(e: ErrorCode): BOOLEAN;
BEGIN RETURN e = ERRORPATHNOTFOUND() END NoSuchDirectory;

PROCEDURE DifferentFilesystems*(e: ErrorCode): BOOLEAN;
BEGIN RETURN e = ERRORNOTSAMEDEVICE() END DifferentFilesystems;

PROCEDURE Inaccessible*(e: ErrorCode): BOOLEAN;
BEGIN
  RETURN (e = ERRORACCESSDENIED()) OR (e = ERRORWRITEPROTECT())
      OR (e = ERRORNOTREADY())     OR (e = ERRORSHARINGVIOLATION());
END Inaccessible;

PROCEDURE Absent*(e: ErrorCode): BOOLEAN;
BEGIN RETURN (e = ERRORFILENOTFOUND()) OR (e = ERRORPATHNOTFOUND()) END Absent;

PROCEDURE TimedOut*(e: ErrorCode): BOOLEAN;
BEGIN RETURN (e = ETIMEDOUT()) END TimedOut;

PROCEDURE ConnectionFailed*(e: ErrorCode): BOOLEAN;
BEGIN RETURN (e = ECONNREFUSED()) OR (e = ECONNABORTED())
          OR (e = ENETUNREACH())  OR (e = EHOSTUNREACH()) END ConnectionFailed;


(* OS memory allocaton *)

PROCEDURE -allocate(size: ADRINT): ADRINT "((SYSTEM_ADRINT)HeapAlloc(GetProcessHeap(), 0, (size_t)(size)))";

PROCEDURE -SystemHalt(code: INTEGER; mod: ARRAY OF SHORTCHAR; pos: INTEGER) "__HALT(code, mod, pos)";

PROCEDURE OSAllocate*(size: ADRINT): ADRINT;
BEGIN
  IF size > 0 THEN
    RETURN allocate(size)  (* If the function HeapAlloc fails and you have not specified HEAP_GENERATE_EXCEPTIONS, the return value is NULL. *)
  END;
  SystemHalt(-25, "Platform.Mod", 133); RETURN 0 (* To remove __RETCHK *)
END OSAllocate;

PROCEDURE -free(address: ADRINT) "HeapFree(GetProcessHeap(), 0, (void*)address)";

PROCEDURE OSFree*(address: ADRINT); BEGIN free(address) END OSFree;


(* Program arguments and environmet access *)

PROCEDURE -getenv(name: ARRAY OF CHAR; VAR buf: ARRAY OF CHAR): DWORD
  "(INTEGER)GetEnvironmentVariable((char*)name, (char*)buf, buf__len)";

PROCEDURE getEnv*(IN var: ARRAY OF CHAR; VAR val: ARRAY OF CHAR): BOOLEAN;
  VAR
    buf: ARRAY 4096 OF CHAR;
    res: INTEGER;
BEGIN
  res := getenv(var, buf);
  IF (res > 0) & (res < LEN(buf)) THEN
    val := buf$;
    RETURN TRUE
  ELSE
    RETURN FALSE
  END
END getEnv;

PROCEDURE GetEnv*(IN var: ARRAY OF CHAR; VAR val: ARRAY OF CHAR);
BEGIN
  IF ~ getEnv(var, val) THEN val[0] := 0X END;
END GetEnv;

PROCEDURE -AAExternArgCount    "extern INTEGER SYSTEM_ArgCount;";
PROCEDURE -AAExternArgVector   "extern void *SYSTEM_ArgVector;";
PROCEDURE -ArgCount(): INTEGER "SYSTEM_ArgCount";
PROCEDURE -ArgVector(): ArgVec "(Platform_ArgVec)SYSTEM_ArgVector";

PROCEDURE GetArg*(n: INTEGER; VAR val: ARRAY OF CHAR);
VAR
  av: ArgVec;
BEGIN
  IF n < ArgCount() THEN av := ArgVector(); val := av[n]^$ END
END GetArg;

PROCEDURE GetIntArg*(n: INTEGER; VAR val: INTEGER);
  VAR s: ARRAY 64 OF CHAR; k, d, i: INTEGER;
BEGIN
  s := ""; GetArg(n, s); i := 0;
  IF s[0] = "-" THEN i := 1 END ;
  k := 0; d := ORD(s[i]) - ORD("0");
  WHILE (d >= 0 ) & (d <= 9) DO k := k*10 + d; INC(i); d := ORD(s[i]) - ORD("0") END ;
  IF s[0] = "-" THEN k := -k; DEC(i) END ;
  IF i > 0 THEN val := k END
END GetIntArg;

PROCEDURE ArgPos*(IN s: ARRAY OF CHAR): INTEGER;
  VAR i: INTEGER; arg: ARRAY 256 OF CHAR;
BEGIN
  i := 0; GetArg(i, arg);
  WHILE (i < ArgCount()) & (s # arg) DO INC(i); GetArg(i, arg) END;
  RETURN i
END ArgPos;


(* Time of day *)

PROCEDURE -getLocalTime  "SYSTEMTIME st; GetLocalTime(&st)";
PROCEDURE -stsec():  INTEGER  "(INTEGER)st.wSecond";
PROCEDURE -stmin():  INTEGER  "(INTEGER)st.wMinute";
PROCEDURE -sthour(): INTEGER  "(INTEGER)st.wHour";
PROCEDURE -stmday(): INTEGER  "(INTEGER)st.wDay";
PROCEDURE -stmon():  INTEGER  "(INTEGER)st.wMonth";
PROCEDURE -styear(): INTEGER  "(INTEGER)st.wYear";

PROCEDURE YMDHMStoClock(ye, mo, da, ho, mi, se: INTEGER; VAR t, d: INTEGER);
BEGIN
  d := ASH(ye MOD 100, 9) + ASH(mo+1, 5) + da;
  t := ASH(ho, 12)        + ASH(mi, 6)   + se;
END YMDHMStoClock;

PROCEDURE GetClock*(VAR t, d: INTEGER);
BEGIN
  getLocalTime;
  YMDHMStoClock(styear(), stmon(), stmday(), sthour(), stmin(), stsec(), t, d);
END GetClock;

PROCEDURE -GetTickCount(): DWORD "(INTEGER)GetTickCount()";

PROCEDURE Time*(): INTEGER;
VAR ms: DWORD;
BEGIN
  ms := GetTickCount();
  RETURN (ms - TimeStart) MOD 7FFFFFFFH
END Time;

PROCEDURE -sleep(ms: INTEGER) "Sleep((DWORD)ms)";

PROCEDURE Delay*(ms: INTEGER);
BEGIN
  WHILE ms > 30000 DO sleep(30000); ms := ms-30000 END;
  IF ms > 0 THEN sleep(ms) END;
END Delay;


(* System call *)

PROCEDURE -startupInfo                                "STARTUPINFO si = {0}; si.cb = sizeof(si);";
PROCEDURE -processInfo                                "PROCESS_INFORMATION pi = {0};";
PROCEDURE -createProcess(str: ARRAY OF CHAR): BOOL    "(INTEGER)CreateProcess(0, (char*)str, 0,0,0,0,0,0,&si,&pi)";
PROCEDURE -waitForProcess(): DWORD                    "(INTEGER)WaitForSingleObject(pi.hProcess, INFINITE)";
PROCEDURE -getExitCodeProcess(VAR exitcode: INTEGER)  "GetExitCodeProcess(pi.hProcess, (DWORD*)exitcode);";
PROCEDURE -cleanupProcess                             "CloseHandle(pi.hProcess); CloseHandle(pi.hThread);";
PROCEDURE -err(): DWORD                               "(INTEGER)GetLastError()";

PROCEDURE System*(IN cmd : ARRAY OF CHAR): INTEGER;
VAR
  result: INTEGER;
BEGIN
  result := 127;
  startupInfo; processInfo;
  IF createProcess(cmd) # 0 THEN
    IF waitForProcess() = 0 THEN getExitCodeProcess(result) END;
    cleanupProcess;
  END;
  RETURN result * 256
END System;

PROCEDURE Error*(): ErrorCode; BEGIN RETURN err() END Error;


(* Expose file and path name length limits (same on Windows) *)

PROCEDURE -MAXPATH(): INTEGER 'MAX_PATH';

PROCEDURE MaxNameLength*(): INTEGER; BEGIN RETURN MAXPATH() END MaxNameLength;
PROCEDURE MaxPathLength*(): INTEGER; BEGIN RETURN MAXPATH() END MaxPathLength; 


(* File system *)

PROCEDURE -InvalidHandleValue*(): FileHandle "(-1)";

PROCEDURE -openrw (n: ARRAY OF CHAR): FileHandle
"(SYSTEM_ADRINT)CreateFile((char*)n, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)";

PROCEDURE -openro (n: ARRAY OF CHAR): FileHandle
"(SYSTEM_ADRINT)CreateFile((char*)n, GENERIC_READ              , FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)";

PROCEDURE -opennew(n: ARRAY OF CHAR): FileHandle
"(SYSTEM_ADRINT)CreateFile((char*)n, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0)";

PROCEDURE -createDirectory(lpPathName: ARRAY OF CHAR): BOOL
"CreateDirectory((char*)lpPathName, (LPSECURITY_ATTRIBUTES)0)";


(* File APIs *)

PROCEDURE OldRO*(IN pathname: ARRAY OF CHAR; VAR h: FileHandle): ErrorCode;
VAR fd: FileHandle;
BEGIN
  fd := openro(pathname);
  IF fd = InvalidHandleValue() THEN RETURN err() ELSE h := fd; RETURN 0 END
END OldRO;

PROCEDURE OldRW*(IN pathname: ARRAY OF CHAR; VAR h: FileHandle): ErrorCode;
VAR fd: FileHandle;
BEGIN
  fd := openrw(pathname);
  IF fd = InvalidHandleValue() THEN RETURN err() ELSE h := fd; RETURN 0 END
END OldRW;

PROCEDURE NewFile*(IN pathname: ARRAY OF CHAR; VAR h: FileHandle): ErrorCode;
VAR fd: FileHandle;
BEGIN
  fd := opennew(pathname);
  IF fd = InvalidHandleValue() THEN RETURN err() ELSE h := fd; RETURN 0 END
END NewFile;

PROCEDURE NewDir*(IN pathname: ARRAY OF CHAR): ErrorCode;
BEGIN
  IF createDirectory(pathname) = 0 THEN RETURN err() ELSE RETURN 0 END
END NewDir;


PROCEDURE -closeHandle(h: FileHandle): BOOL "(INTEGER)CloseHandle((HANDLE)h)";

PROCEDURE CloseFile*(h: FileHandle): ErrorCode;
BEGIN
  IF closeHandle(h) = 0 THEN RETURN err() ELSE RETURN 0 END
END CloseFile;


PROCEDURE -byHandleFileInformation "BY_HANDLE_FILE_INFORMATION bhfi";
PROCEDURE -getFileInformationByHandle(h: FileHandle): BOOL "(INTEGER)GetFileInformationByHandle((HANDLE)h, &bhfi)";
PROCEDURE -bhfiMtimeHigh(): DWORD "(INTEGER)bhfi.ftLastWriteTime.dwHighDateTime";
PROCEDURE -bhfiMtimeLow():  DWORD "(INTEGER)bhfi.ftLastWriteTime.dwLowDateTime";
PROCEDURE -bhfiVsn():       DWORD "(INTEGER)bhfi.dwVolumeSerialNumber";
PROCEDURE -bhfiIndexHigh(): DWORD "(INTEGER)bhfi.nFileIndexHigh";
PROCEDURE -bhfiIndexLow():  DWORD "(INTEGER)bhfi.nFileIndexLow";


PROCEDURE Identify*(h: FileHandle; VAR identity: FileIdentity): ErrorCode;
BEGIN
  byHandleFileInformation;
  IF getFileInformationByHandle(h) = 0 THEN RETURN err() END;
  identity.volume    := bhfiVsn();
  identity.indexhigh := bhfiIndexHigh();
  identity.indexlow  := bhfiIndexLow();
  identity.mtimehigh := bhfiMtimeHigh();
  identity.mtimelow  := bhfiMtimeLow();
  RETURN 0
END Identify;

PROCEDURE IdentifyByName*(IN n: ARRAY OF CHAR; VAR identity: FileIdentity): ErrorCode;
VAR
  h:   FileHandle;
  e,i: ErrorCode;
BEGIN
  e := OldRO(n, h);
  IF e # 0 THEN RETURN e END;
  e := Identify(h, identity);
  i := CloseFile(h);
  RETURN e
END IdentifyByName;


PROCEDURE SameFile*(i1, i2: FileIdentity): BOOLEAN;
BEGIN RETURN (i1.indexhigh = i2.indexhigh) & (i1.indexlow = i2.indexlow) & (i1.volume = i2.volume)
END SameFile;

PROCEDURE SameFileTime*(i1, i2: FileIdentity): BOOLEAN;
BEGIN RETURN (i1.mtimehigh = i2.mtimehigh) & (i1.mtimelow = i2.mtimelow)
END SameFileTime;

PROCEDURE SetMTime*(VAR target: FileIdentity; source: FileIdentity);
BEGIN target.mtimehigh := source.mtimehigh;  target.mtimelow := source.mtimelow;
END SetMTime;

PROCEDURE -identityToFileTime(i: FileIdentity)
"FILETIME ft; ft.dwHighDateTime = i.mtimehigh; ft.dwLowDateTime = i.mtimelow";

PROCEDURE -fileTimeToSysTime
"SYSTEMTIME st; FileTimeToSystemTime(&ft, &st)";

PROCEDURE MTimeAsClock*(i: FileIdentity; VAR t, d: INTEGER);
BEGIN
  identityToFileTime(i); fileTimeToSysTime;
  YMDHMStoClock(styear(), stmon(), stmday(), sthour(), stmin(), stsec(), t, d);
END MTimeAsClock;


PROCEDURE -getFileSize(h: FileHandle; VAR sizehigh: DWORD): DWORD "(INTEGER)GetFileSize((HANDLE)h, (LPDWORD)sizehigh)";

PROCEDURE FileSize*(h: FileHandle; VAR len: LONGINT): ErrorCode;
VAR size: RECORD [notag] low, high: DWORD END; error: ErrorCode;
BEGIN
  size.low := getFileSize(h, size.high);
  IF size.low = INVALIDFILESIZE() THEN
    error := err(); IF error # NOERROR() THEN RETURN error END;
  END;
  len := SYSTEM.VAL(LONGINT, size);
  RETURN 0
END FileSize;


PROCEDURE -readfile (fd: FileHandle; p: ADRINT; l: INTEGER; VAR n: INTEGER): BOOL
"(INTEGER)ReadFile ((HANDLE)fd, (void*)(p), (DWORD)l, (DWORD*)n, 0)";

PROCEDURE Read*(h: FileHandle; p: ADRINT; l: INTEGER; VAR n: INTEGER): ErrorCode;
VAR result: INTEGER;
BEGIN
  n := 0;  (* Clear n because readfile takes a INTEGER but only updates the bottom 32 bits *)
  result := readfile(h, p, l, n);
  IF result = 0 THEN n := 0; RETURN err() ELSE RETURN 0 END
END Read;

PROCEDURE ReadBuf*(h: FileHandle; VAR b: ARRAY OF BYTE; VAR n: INTEGER): ErrorCode;
VAR result: INTEGER;
BEGIN
  n := 0;  (* Clear n because readfile takes a INTEGER but only updates the bottom 32 bits *)
  result := readfile(h, SYSTEM.ADR(b), LEN(b), n);
  IF result = 0 THEN n := 0; RETURN err() ELSE RETURN 0 END
END ReadBuf;


PROCEDURE -writefile(fd: FileHandle; p: ADRINT; l: INTEGER; VAR dummy: DWORD): BOOL
"(INTEGER)WriteFile((HANDLE)fd, (void*)(p), (DWORD)l, (LPDWORD)dummy, 0)";

PROCEDURE Write*(h: FileHandle; p: ADRINT; l: INTEGER): ErrorCode;
VAR dummy: DWORD;
BEGIN
  IF writefile(h, p, l, dummy) = 0 THEN RETURN err() ELSE RETURN 0 END
END Write;


PROCEDURE -flushFileBuffers(h: FileHandle): BOOL "(INTEGER)FlushFileBuffers((HANDLE)h)";

PROCEDURE Sync*(h: FileHandle): ErrorCode;
BEGIN
  IF flushFileBuffers(h) = 0 THEN RETURN err() ELSE RETURN 0 END
END Sync;


PROCEDURE -setFilePointer(hFile: FileHandle; lDistanceToMove: INTEGER;
  VAR lpDistanceToMoveHigh: INTEGER; dwMoveMethod: DWORD): DWORD "(INTEGER)SetFilePointer((HANDLE)hFile, (LONG)lDistanceToMove, (PLONG)lpDistanceToMoveHigh, (DWORD)dwMoveMethod)";

PROCEDURE -seekset(): INTEGER "FILE_BEGIN";
PROCEDURE -seekcur(): INTEGER "FILE_CURRENT";
PROCEDURE -seekend(): INTEGER "FILE_END";

PROCEDURE Seek*(h: FileHandle; offset: LONGINT; r: INTEGER): ErrorCode;
VAR pos: LARGE_INTEGER; error: ErrorCode;
BEGIN
  pos := SYSTEM.VAL(LARGE_INTEGER, offset);
  IF setFilePointer(h, pos.low, pos.high, r) = INVALIDSETFILEPOINTER() THEN
    error := err(); IF error # NOERROR() THEN RETURN error END;
  END;
  RETURN 0
END Seek;


PROCEDURE -setEndOfFile(h: FileHandle): BOOL "(INTEGER)SetEndOfFile((HANDLE)h)";

PROCEDURE GetFilePos(h: FileHandle; VAR r: LONGINT): ErrorCode;
VAR pos: LARGE_INTEGER; error: ErrorCode;
BEGIN
  pos.high := 0;
  pos.low := setFilePointer(h, 0, pos.high, seekcur());
  IF pos.low = INVALIDSETFILEPOINTER() THEN
    error := err(); IF error # NOERROR() THEN RETURN error END;
  END;
  r := SYSTEM.VAL(LONGINT, pos);
  RETURN 0
END GetFilePos;

PROCEDURE TruncateFile*(h: FileHandle; limit: LONGINT): ErrorCode;
VAR pos: LARGE_INTEGER; oldpos: LONGINT; error: ErrorCode;
BEGIN
  error := GetFilePos(h, oldpos);
  IF error # 0                           THEN RETURN error END;
  pos := SYSTEM.VAL(LARGE_INTEGER, limit);
  IF setFilePointer(h, pos.low, pos.high, seekset()) = INVALIDSETFILEPOINTER()
  THEN
    error := err(); IF error # NOERROR() THEN RETURN error END;
  END;
  IF setEndOfFile(h) = 0                 THEN RETURN err() END;
  pos := SYSTEM.VAL(LARGE_INTEGER, oldpos); (* Restore original file position *)
  IF setFilePointer(h, pos.low, pos.high, seekset()) = INVALIDSETFILEPOINTER()
  THEN
    error := err(); IF error # NOERROR() THEN RETURN error END;
  END;
  RETURN 0
END TruncateFile;


PROCEDURE -deleteFile(n: ARRAY OF CHAR): BOOL "(INTEGER)DeleteFile((char*)n)";

PROCEDURE DeleteFile*(IN n: ARRAY OF CHAR): ErrorCode;
BEGIN
  IF deleteFile(n) = 0 THEN RETURN err() ELSE RETURN 0 END
END DeleteFile;


PROCEDURE -setCurrentDirectory(n: ARRAY OF CHAR): BOOL "(INTEGER)SetCurrentDirectory((char*)n)";
PROCEDURE -getCurrentDirectory(VAR n: ARRAY OF CHAR) "GetCurrentDirectory((DWORD)n__len, (char*)n)";

PROCEDURE ChDir*(IN n: ARRAY OF CHAR): ErrorCode;
  VAR r: BOOL;
BEGIN
  r := setCurrentDirectory(n);
  IF r = 0 THEN RETURN err() END;
  getCurrentDirectory(CWD);
  RETURN 0
END ChDir;

PROCEDURE -getModuleFileName(hModule: FileHandle; lpFilename: ARRAY OF CHAR; nSize: DWORD): DWORD
"(INTEGER)GetModuleFileNameA((HMODULE)hModule, (LPTSTR)lpFilename, nSize)";

PROCEDURE GetStartDir* (VAR dir: ARRAY MAX_PATH OF CHAR); (* get the start directory with ending "\" *)
  VAR i: INTEGER;
BEGIN
  dir[MAX_PATH - 1] := 0X;
  i := getModuleFileName(0, dir, MAX_PATH - 1);
  WHILE (i # 0) & (dir[i] # "\") DO DEC(i) END;
  IF i # 0 THEN dir[i+1] := 0X END
END GetStartDir;


PROCEDURE -getFileAttributes(name: ARRAY OF CHAR): DWORD
"(INTEGER)GetFileAttributes((LPCTSTR)name)";
PROCEDURE -FILEATTRIBUTEDIRECTORY(): SET "(SET)FILE_ATTRIBUTE_DIRECTORY";

PROCEDURE FileExists*(IN name: ARRAY OF CHAR): BOOLEAN;
VAR
  dwAttrib: DWORD;
BEGIN
  dwAttrib := getFileAttributes(name);
  RETURN (dwAttrib # INVALIDFILEATTRIBUTES()) (* Wrong path! *)
    & (SYSTEM.VAL(SET, dwAttrib) * FILEATTRIBUTEDIRECTORY() = {})
END FileExists;

PROCEDURE DirExists*(IN name: ARRAY OF CHAR): BOOLEAN;
VAR
  dwAttrib: DWORD;
BEGIN
  dwAttrib := getFileAttributes(name);
  RETURN (dwAttrib # INVALIDFILEATTRIBUTES()) (* Wrong path! *)
    & (SYSTEM.VAL(SET, dwAttrib) * FILEATTRIBUTEDIRECTORY() # {})
END DirExists;

PROCEDURE -moveFileEx(src, dest: ARRAY OF CHAR): BOOL
  "(INTEGER)MoveFileEx((LPCTSTR)src, (LPCTSTR)dest, MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)";
PROCEDURE -copyFile(src, dest: ARRAY OF CHAR): BOOL
  "(INTEGER)CopyFile((LPCTSTR)src, (LPCTSTR)dest, FALSE)";

(* This version of RenameFile requires a closed file,
   as FILE_SHARE_DELETE cannot be used due to compatibility issues.
*)
PROCEDURE RenameFile*(IN src, dest: ARRAY OF CHAR): ErrorCode;
VAR
  src_id, dest_id: FileIdentity; error: ErrorCode;
BEGIN
  IF moveFileEx(src, dest) # 0 THEN RETURN 0 END;
  error := err();
  IF (error # ERRORCALLNOTIMPLEMENTED()) & (error # ERRORNOTSAMEDEVICE()) THEN
    RETURN error
  END;
  IF FileExists(dest) & (FileExists(src)
    & (IdentifyByName(src, src_id) = 0) & (IdentifyByName(dest, dest_id) = 0)
    & SameFile(src_id, dest_id))
  THEN
    RETURN 0
  END;
  IF copyFile(src, dest) = 0 THEN RETURN err() END;
  RETURN DeleteFile(src)
END RenameFile;


(* Process termination *)

PROCEDURE -exit(code: INTEGER) "ExitProcess((UINT)code)";
PROCEDURE ExitOS*(code: INTEGER);
BEGIN exit(code) END ExitOS;


PROCEDURE -getConsoleMode(h: FileHandle; VAR m: INTEGER): BOOLEAN "GetConsoleMode((HANDLE)h, (DWORD*)m)"; 

PROCEDURE IsConsole*(h: FileHandle): BOOLEAN;
VAR mode: INTEGER;
BEGIN RETURN getConsoleMode(StdOut, mode)
END IsConsole;


PROCEDURE TestLittleEndian;
  VAR i: INTEGER;
 BEGIN i := 1; SYSTEM.GET(SYSTEM.ADR(i), LittleEndian); END TestLittleEndian;


PROCEDURE -getstdinhandle():  FileHandle "(SYSTEM_ADRINT)GetStdHandle(STD_INPUT_HANDLE)";
PROCEDURE -getstdouthandle(): FileHandle "(SYSTEM_ADRINT)GetStdHandle(STD_OUTPUT_HANDLE)";
PROCEDURE -getstderrhandle(): FileHandle "(SYSTEM_ADRINT)GetStdHandle(STD_ERROR_HANDLE)";
PROCEDURE -getpid():          DWORD      "(INTEGER)GetCurrentProcessId()";

BEGIN
  TestLittleEndian;

  TimeStart := 0;  TimeStart := Time();  (* Time() uses TimeStart *)
  CWD       := "";  getCurrentDirectory(CWD);
  PID       := getpid();

  SeekSet := seekset();
  SeekCur := seekcur();
  SeekEnd := seekend();

  StdIn  := getstdinhandle();
  StdOut := getstdouthandle();
  StdErr := getstderrhandle();
END Platform.
