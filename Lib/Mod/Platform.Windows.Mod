MODULE Platform;
IMPORT SYSTEM;

(* TODO:
   Use Unicode APIs with manual UTF8 conversion and prepend '\\?\' to
   file paths in order to get 32768 character path length limit (as
   opposed to 256 bytes. *)


TYPE
  HaltProcedure = PROCEDURE(n: INTEGER);
  SignalHandler = PROCEDURE(signal: SHORTINT);

  ErrorCode*  = SHORTINT;
  FileHandle* = INTEGER;

  FileIdentity* = RECORD
    volume:    INTEGER;  (* dev on Unix filesystems, volume serial number on NTFS *)
    indexhigh: INTEGER;  (* inode on Unix filesystems, file id on NTFS *)
    indexlow:  INTEGER;
    mtimehigh: INTEGER;  (* File modification time, value is system dependent *)
    mtimelow:  INTEGER;  (* File modification time, value is system dependent *)
  END;

  ADR       = POINTER [1] TO ARRAY [1] 1 OF CHAR;
  ArgPtr    = POINTER [1] TO ARRAY [1] 1024 OF CHAR;
  ArgVec*   = POINTER [1] TO ARRAY [1] 1024 OF ArgPtr;


VAR
  LittleEndian-:    BOOLEAN;
  MainStackFrame-:  ADR;
  HaltCode-:        INTEGER;
  PID-:             SHORTINT;    (* Note: Must be updated by Fork implementation *)
  CWD-:             ARRAY 4096 OF CHAR;
  ArgCount-:        SHORTINT;

  ArgVector-:       ArgVec;
  HaltHandler:      HaltProcedure;
  TimeStart:        INTEGER;

  SeekSet-:         SHORTINT;
  SeekCur-:         SHORTINT;
  SeekEnd-:         SHORTINT;

  StdIn-:           FileHandle;
  StdOut-:          FileHandle;
  StdErr-:          FileHandle;

  InterruptHandler: SignalHandler;

  nl-:              ARRAY 3 OF CHAR;  (* Platform specific newline representation *)



PROCEDURE -AincludeWindowsWrapper '#include "_windows.h"';


(* Error code tests *)

PROCEDURE -ERRORTOOMANYOPENFILES(): ErrorCode 'ERROR_TOO_MANY_OPEN_FILES';
PROCEDURE -ERRORPATHNOTFOUND():     ErrorCode 'ERROR_PATH_NOT_FOUND';
PROCEDURE -ERRORFILENOTFOUND():     ErrorCode 'ERROR_FILE_NOT_FOUND';
PROCEDURE -ERRORNOTSAMEDEVICE():    ErrorCode 'ERROR_NOT_SAME_DEVICE';
PROCEDURE -ERRORACCESSDENIED():     ErrorCode 'ERROR_ACCESS_DENIED';
PROCEDURE -ERRORWRITEPROTECT():     ErrorCode 'ERROR_WRITE_PROTECT';
PROCEDURE -ERRORSHARINGVIOLATION(): ErrorCode 'ERROR_SHARING_VIOLATION';
PROCEDURE -ERRORNOTREADY():         ErrorCode 'ERROR_NOT_READY';
PROCEDURE -ETIMEDOUT():             ErrorCode 'WSAETIMEDOUT';
PROCEDURE -ECONNREFUSED():          ErrorCode 'WSAECONNREFUSED';
PROCEDURE -ECONNABORTED():          ErrorCode 'WSAECONNABORTED';
PROCEDURE -ENETUNREACH():           ErrorCode 'WSAENETUNREACH';
PROCEDURE -EHOSTUNREACH():          ErrorCode 'WSAEHOSTUNREACH';



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

PROCEDURE -allocate(size: INTEGER): INTEGER "(INTEGER)(SYSTEM_ADR)((void*)HeapAlloc(GetProcessHeap(), 0, (size_t)size))";
PROCEDURE OSAllocate*(size: INTEGER): INTEGER; BEGIN RETURN allocate(size) END OSAllocate;

PROCEDURE -free(address: INTEGER) "HeapFree(GetProcessHeap(), 0, (void*)(SYSTEM_ADR)address)";
PROCEDURE OSFree*(address: INTEGER); BEGIN free(address) END OSFree;




(* Program startup *)

PROCEDURE -ExternInitHeap  "extern void Heap_InitHeap();";
PROCEDURE -HeapInitHeap()  "Heap_InitHeap()";

PROCEDURE Init*(argc: SHORTINT; argvadr: SYSTEM.PTR);
TYPE
  ArgVecPtr = POINTER [1] TO ARRAY [1] 1 OF ArgVec; 
VAR
  av: ArgVecPtr;
BEGIN
  MainStackFrame := SYSTEM.VAL(ADR, argvadr);
  ArgCount       := argc;
  av             := SYSTEM.VAL(ArgVecPtr, argvadr);
  ArgVector      := SYSTEM.VAL(ArgVec, av[0]);
  HaltCode       := -128;

  (* This function (Platform.Init) is called at program startup BEFORE any
     modules have been initalized. In turn we must initialize the heap
     before module startup (xxx__init) code is run. *)
  HeapInitHeap();
END Init;




(* Program arguments and environmet access *)

PROCEDURE -getenv(name: ARRAY OF CHAR; VAR buf: ARRAY OF CHAR): SHORTINT
"(SHORTINT)GetEnvironmentVariable((char*)name, (char*)buf, buf__len)";

PROCEDURE getEnv*(var: ARRAY OF CHAR; VAR val: ARRAY OF CHAR): BOOLEAN;
  VAR
    buf: ARRAY 4096 OF CHAR;
    res: SHORTINT;
BEGIN
  res := getenv(var, buf);
  IF (res > 0) & (res < LEN(buf)) THEN
    COPY(buf, val);
    RETURN TRUE;
  ELSE
    RETURN FALSE;
  END;
END getEnv;

PROCEDURE GetEnv*(var: ARRAY OF CHAR; VAR val: ARRAY OF CHAR);
BEGIN
  IF ~getEnv(var, val) THEN val[0] := 0X END;
END GetEnv;

PROCEDURE GetArg*(n: SHORTINT; VAR val: ARRAY OF CHAR);
BEGIN
  IF n < ArgCount THEN COPY(ArgVector[n]^, val) END
END GetArg;

PROCEDURE GetIntArg*(n: SHORTINT; VAR val: SHORTINT);
  VAR s: ARRAY 64 OF CHAR; k, d, i: SHORTINT;
BEGIN
  s := ""; GetArg(n, s); i := 0;
  IF s[0] = "-" THEN i := 1 END ;
  k := 0; d := ORD(s[i]) - ORD("0");
  WHILE (d >= 0 ) & (d <= 9) DO k := k*10 + d; INC(i); d := ORD(s[i]) - ORD("0") END ;
  IF s[0] = "-" THEN k := -k; DEC(i) END ;
  IF i > 0 THEN val := k END
END GetIntArg;

PROCEDURE ArgPos*(s: ARRAY OF CHAR): SHORTINT;
  VAR i: SHORTINT; arg: ARRAY 256 OF CHAR;
BEGIN
  i := 0; GetArg(i, arg);
  WHILE (i < ArgCount) & (s # arg) DO INC(i); GetArg(i, arg) END ;
  RETURN i
END ArgPos;





(* Signals and traps *)

(* PROCEDURE -signal(sig: INTEGER; func: SignalHandler) "signal(sig, func)"; *)

(* TODO *)

(* Ctrl/c handling *)

PROCEDURE -SetInterruptHandler*(h: SignalHandler) "SystemSetInterruptHandler((SYSTEM_ADR)h)";
PROCEDURE -SetQuitHandler*     (h: SignalHandler) "SystemSetQuitHandler((SYSTEM_ADR)h)";

PROCEDURE SetBadInstructionHandler*(handler: SignalHandler);
BEGIN (* TODO *) END SetBadInstructionHandler;




(* Time of day *)

PROCEDURE -getLocalTime  "SYSTEMTIME st; GetLocalTime(&st)";
PROCEDURE -stmsec(): SHORTINT  "(SHORTINT)st.wMilliseconds";
PROCEDURE -stsec():  SHORTINT  "(SHORTINT)st.wSecond";
PROCEDURE -stmin():  SHORTINT  "(SHORTINT)st.wMinute";
PROCEDURE -sthour(): SHORTINT  "(SHORTINT)st.wHour";
PROCEDURE -stmday(): SHORTINT  "(SHORTINT)st.wDay";
PROCEDURE -stmon():  SHORTINT  "(SHORTINT)st.wMonth";
PROCEDURE -styear(): SHORTINT  "(SHORTINT)st.wYear";

PROCEDURE YMDHMStoClock(ye,mo,da,ho,mi,se: SHORTINT; VAR t, d: INTEGER);
BEGIN
  d := ASH(ye MOD 100, 9) + ASH(mo+1, 5) + da;
  t := ASH(ho, 12)        + ASH(mi, 6)   + se;
END YMDHMStoClock;

PROCEDURE GetClock*(VAR t, d: INTEGER);
BEGIN
  getLocalTime;
  YMDHMStoClock(styear(), stmon(), stmday(), sthour(), stmin(), stsec(), t, d);
END GetClock;

PROCEDURE GetTimeOfDay*(VAR sec, usec: INTEGER);
BEGIN
  getLocalTime; sec := stsec(); usec := LONG(stmsec()) * 1000;
END GetTimeOfDay;

PROCEDURE -GetTickCount(): INTEGER "(INTEGER)(unsigned)GetTickCount()";

PROCEDURE Time*(): INTEGER;
VAR ms: INTEGER;
BEGIN
  ms := GetTickCount();
  RETURN (ms - TimeStart) MOD 7FFFFFFFH;
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
PROCEDURE -createProcess(str: ARRAY OF CHAR): SHORTINT "(SHORTINT)CreateProcess(0, (char*)str, 0,0,0,0,0,0,&si,&pi)";
PROCEDURE -waitForProcess(): SHORTINT                  "(SHORTINT)WaitForSingleObject(pi.hProcess, INFINITE)";
PROCEDURE -getExitCodeProcess(VAR exitcode: SHORTINT)  "GetExitCodeProcess(pi.hProcess, (DWORD*)exitcode);";
PROCEDURE -cleanupProcess                             "CloseHandle(pi.hProcess); CloseHandle(pi.hThread);";
PROCEDURE -err(): SHORTINT                             "(SHORTINT)GetLastError()";

PROCEDURE System*(cmd : ARRAY OF CHAR): SHORTINT;
VAR
  result: SHORTINT;
BEGIN
  result := 127;
  startupInfo; processInfo;
  IF createProcess(cmd) # 0 THEN
    IF waitForProcess() = 0 THEN getExitCodeProcess(result) END;
    cleanupProcess;
  END;
  RETURN result * 256;
END System;

PROCEDURE Error*(): ErrorCode; BEGIN RETURN err() END Error;


(* File system *)

PROCEDURE -invalidHandleValue(): INTEGER "((INTEGER)(SYSTEM_ADR)INVALID_HANDLE_VALUE)";

PROCEDURE -openrw (n: ARRAY OF CHAR): INTEGER
"(INTEGER)(SYSTEM_ADR)CreateFile((char*)n, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)";

PROCEDURE -openro (n: ARRAY OF CHAR): INTEGER
"(INTEGER)(SYSTEM_ADR)CreateFile((char*)n, GENERIC_READ              , FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)";

PROCEDURE -opennew(n: ARRAY OF CHAR): INTEGER
"(INTEGER)(SYSTEM_ADR)CreateFile((char*)n, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0)";




(* File APIs *)

PROCEDURE OldRO*(VAR n: ARRAY OF CHAR; VAR h: FileHandle): ErrorCode;
VAR fd: INTEGER;
BEGIN
  fd := openro(n);
  IF (fd = invalidHandleValue()) THEN RETURN err() ELSE h := fd; RETURN 0 END;
END OldRO;

PROCEDURE OldRW*(VAR n: ARRAY OF CHAR; VAR h: FileHandle): ErrorCode;
VAR fd: INTEGER;
BEGIN
  fd := openrw(n);
  IF (fd = invalidHandleValue()) THEN RETURN err() ELSE h := fd; RETURN 0 END;
END OldRW;

PROCEDURE New*(VAR n: ARRAY OF CHAR; VAR h: FileHandle): ErrorCode;
VAR fd: INTEGER;
BEGIN
  fd := opennew(n);
  IF (fd = invalidHandleValue()) THEN RETURN err() ELSE h := fd; RETURN 0 END;
END New;



PROCEDURE -closeHandle(h: FileHandle): SHORTINT "(SHORTINT)CloseHandle((HANDLE)(SYSTEM_ADR)h)";

PROCEDURE Close*(h: FileHandle): ErrorCode;
BEGIN
  IF closeHandle(h) = 0 THEN RETURN err() ELSE RETURN 0 END
END Close;



PROCEDURE -byHandleFileInformation "BY_HANDLE_FILE_INFORMATION bhfi";
PROCEDURE -getFileInformationByHandle(h: FileHandle): SHORTINT "(SHORTINT)GetFileInformationByHandle((HANDLE)(SYSTEM_ADR)h, &bhfi)";
PROCEDURE -bhfiMtimeHigh(): INTEGER "(INTEGER)bhfi.ftLastWriteTime.dwHighDateTime";
PROCEDURE -bhfiMtimeLow():  INTEGER "(INTEGER)bhfi.ftLastWriteTime.dwLowDateTime";
PROCEDURE -bhfiVsn():       INTEGER "(INTEGER)bhfi.dwVolumeSerialNumber";
PROCEDURE -bhfiIndexHigh(): INTEGER "(INTEGER)bhfi.nFileIndexHigh";
PROCEDURE -bhfiIndexLow():  INTEGER "(INTEGER)bhfi.nFileIndexLow";


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

PROCEDURE IdentifyByName*(n: ARRAY OF CHAR; VAR identity: FileIdentity): ErrorCode;
VAR
  h:   FileHandle;
  e,i: ErrorCode;
BEGIN
  e := OldRO(n, h);
  IF e # 0 THEN RETURN e END;
  e := Identify(h, identity);
  i := Close(h);
  RETURN e;
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

PROCEDURE -largeInteger "LARGE_INTEGER li";
PROCEDURE -liLongint(): INTEGER "(INTEGER)li.QuadPart";
PROCEDURE -getFileSize(h: FileHandle): SHORTINT "(SHORTINT)GetFileSizeEx((HANDLE)(SYSTEM_ADR)h, &li)";

PROCEDURE Size*(h: FileHandle; VAR l: INTEGER): ErrorCode;
BEGIN
  largeInteger;
  IF getFileSize(h) = 0 THEN RETURN err() END;
  l := liLongint();
  RETURN 0;
END Size;


PROCEDURE -readfile (fd: INTEGER; p: INTEGER; l: INTEGER; VAR n: INTEGER): SHORTINT
"(SHORTINT)ReadFile ((HANDLE)(SYSTEM_ADR)fd, (void*)(SYSTEM_ADR)(p), (DWORD)l, (DWORD*)n, 0)";

PROCEDURE Read*(h: FileHandle; p: INTEGER; l: INTEGER; VAR n: INTEGER): ErrorCode;
VAR result: SHORTINT;
BEGIN
  n := 0;  (* Clear n because readfile takes a INTEGER but only updates the bottom 32 bits *)
  result := readfile(h, p, l, n);
  IF result = 0 THEN n := 0; RETURN err() ELSE RETURN 0 END
END Read;

PROCEDURE ReadBuf*(h: FileHandle; VAR b: ARRAY OF BYTE; VAR n: INTEGER): ErrorCode;
VAR result: SHORTINT;
BEGIN
  n := 0;  (* Clear n because readfile takes a INTEGER but only updates the bottom 32 bits *)
  result := readfile(h, SYSTEM.ADR(b), LEN(b), n);
  IF result = 0 THEN n := 0; RETURN err() ELSE RETURN 0 END
END ReadBuf;



PROCEDURE -writefile(fd: INTEGER; p: INTEGER; l: INTEGER): SHORTINT
"(SHORTINT)WriteFile((HANDLE)(SYSTEM_ADR)fd, (void*)(SYSTEM_ADR)(p), (DWORD)l, 0,0)";

PROCEDURE Write*(h: FileHandle; p: INTEGER; l: INTEGER): ErrorCode;
BEGIN
  IF writefile(h, p, l) = 0 THEN RETURN err() ELSE RETURN 0 END
END Write;



PROCEDURE -flushFileBuffers(h: FileHandle): SHORTINT "(SHORTINT)FlushFileBuffers((HANDLE)(SYSTEM_ADR)h)";

PROCEDURE Sync*(h: FileHandle): ErrorCode;
BEGIN
  IF flushFileBuffers(h) = 0 THEN RETURN err() ELSE RETURN 0 END
END Sync;



PROCEDURE -setFilePointerEx(h: FileHandle; o: INTEGER; r: SHORTINT; VAR rc: SHORTINT)
"li.QuadPart=o; *rc = (SHORTINT)SetFilePointerEx((HANDLE)(SYSTEM_ADR)h, li, 0, (DWORD)r)";

PROCEDURE -seekset(): SHORTINT "FILE_BEGIN";
PROCEDURE -seekcur(): SHORTINT "FILE_CURRENT";
PROCEDURE -seekend(): SHORTINT "FILE_END";

PROCEDURE Seek*(h: FileHandle; o: INTEGER; r: SHORTINT): ErrorCode;
VAR rc: SHORTINT;
BEGIN
  largeInteger;
  setFilePointerEx(h, o, r, rc);
  IF rc = 0 THEN RETURN err() ELSE RETURN 0 END
END Seek;



PROCEDURE -setEndOfFile(h: FileHandle): SHORTINT "(SHORTINT)SetEndOfFile((HANDLE)(SYSTEM_ADR)h)";
PROCEDURE -getFilePos(h: FileHandle; VAR r: INTEGER; VAR rc: SHORTINT)
"LARGE_INTEGER liz = {0}; *rc = (SHORTINT)SetFilePointerEx((HANDLE)(SYSTEM_ADR)h, liz, &li, FILE_CURRENT); *r = (INTEGER)li.QuadPart";

PROCEDURE Truncate*(h: FileHandle; limit: INTEGER): ErrorCode;
VAR rc: SHORTINT; oldpos: INTEGER;
BEGIN
  largeInteger;
  getFilePos(h, oldpos, rc);
  IF rc = 0                                          THEN RETURN err() END;
  setFilePointerEx(h, limit, seekset(), rc);
  IF rc = 0                                          THEN RETURN err() END;
  IF setEndOfFile(h) = 0                             THEN RETURN err() END;
  setFilePointerEx(h, oldpos, seekset(), rc);  (* Restore original file position *)
  IF rc = 0                                          THEN RETURN err() END;
  RETURN 0;
END Truncate;



PROCEDURE -deleteFile(n: ARRAY OF CHAR): SHORTINT "(SHORTINT)DeleteFile((char*)n)";

PROCEDURE Unlink*(VAR n: ARRAY OF CHAR): ErrorCode;
BEGIN
  IF deleteFile(n) = 0 THEN RETURN err() ELSE RETURN 0 END
END Unlink;


PROCEDURE -setCurrentDirectory(n: ARRAY OF CHAR): SHORTINT "(SHORTINT)SetCurrentDirectory((char*)n)";
PROCEDURE -getCurrentDirectory(VAR n: ARRAY OF CHAR) "GetCurrentDirectory(n__len, (char*)n)";

PROCEDURE Chdir*(VAR n: ARRAY OF CHAR): ErrorCode;
  VAR r: SHORTINT;
BEGIN
  r := setCurrentDirectory(n);
  IF r = 0 THEN RETURN err() END;
  getCurrentDirectory(CWD);
  RETURN 0;
END Chdir;



PROCEDURE -moveFile(o,n: ARRAY OF CHAR): SHORTINT
"(SHORTINT)MoveFileEx((char*)o, (char*)n, MOVEFILE_REPLACE_EXISTING)";

PROCEDURE Rename*(VAR o,n: ARRAY OF CHAR): ErrorCode;
BEGIN
  IF moveFile(o,n) = 0 THEN RETURN err() ELSE RETURN 0 END
END Rename;




(* Process termination *)

PROCEDURE -exit(code: SHORTINT) "ExitProcess((UINT)code)";
PROCEDURE Exit*(code: SHORTINT);
BEGIN exit(code) END Exit;


PROCEDURE -errstring(s: ARRAY OF CHAR) 'WriteFile((HANDLE)(SYSTEM_ADR)Platform_StdOut, s, s__len-1, 0,0)';
PROCEDURE -errc     (c: CHAR)          'WriteFile((HANDLE)(SYSTEM_ADR)Platform_StdOut, &c, 1, 0,0)';
PROCEDURE errch(c: CHAR); BEGIN errc(c) END errch;
PROCEDURE errln; BEGIN errch(0DX); errch(0AX) END errln;

PROCEDURE errposint(l: INTEGER);
BEGIN IF l>10 THEN errposint(l DIV 10) END; errch(CHR(ORD('0') + (l MOD 10))) END errposint;

PROCEDURE errint(l: INTEGER);
BEGIN IF l<0 THEN errch('-'); l := -l END; errposint(l) END errint;

PROCEDURE DisplayHaltCode(code: INTEGER);
BEGIN
  CASE code OF
  | -1: errstring("Rider ReadBuf/WriteBuf transfer size longer than buffer.")
  | -2: errstring("Index out of range.")
  | -3: errstring("Reached end of function without reaching RETURN.")
  | -4: errstring("CASE statement: no matching label and no ELSE.")
  | -5: errstring("Type guard failed.")
  | -6: errstring("Type equality failed.")
  | -7: errstring("WITH statement type guard failed.")
  | -8: errstring("SHORT: Value too large for shorter type.")
  | -9: errstring("Heap interrupted while locked, but lockdepth = 0 at unlock.")
  |-15: errstring("Type descriptor size mismatch.")
  |-20: errstring("Too many, or negative number of, elements in dynamic array.")
  ELSE
  END
END DisplayHaltCode;

PROCEDURE Halt*(code: INTEGER);
VAR e: ErrorCode;
BEGIN
  HaltCode := code;
  IF HaltHandler # NIL THEN HaltHandler(code) END;
  errstring("Terminated by Halt("); errint(code); errstring("). ");
  IF code < 0 THEN DisplayHaltCode(code) END;
  errln;
  exit(SYSTEM.VAL(SHORTINT,code));
END Halt;

PROCEDURE AssertFail*(code: INTEGER);
VAR e: ErrorCode;
BEGIN
  errstring("Assertion failure.");
  IF code # 0 THEN errstring(" ASSERT code "); errint(code); errstring("."); END;
  errln;
  exit(SYSTEM.VAL(SHORTINT,code));
END AssertFail;

PROCEDURE SetHalt*(p: HaltProcedure);
BEGIN HaltHandler := p; END SetHalt;




PROCEDURE TestLittleEndian;
  VAR i: SHORTINT;
 BEGIN i := 1; SYSTEM.GET(SYSTEM.ADR(i), LittleEndian); END TestLittleEndian;


PROCEDURE -getstdinhandle():  FileHandle "(SYSTEM_ADR)GetStdHandle(STD_INPUT_HANDLE)";
PROCEDURE -getstdouthandle(): FileHandle "(SYSTEM_ADR)GetStdHandle(STD_OUTPUT_HANDLE)";
PROCEDURE -getstderrhandle(): FileHandle "(SYSTEM_ADR)GetStdHandle(STD_ERROR_HANDLE)";
PROCEDURE -getpid():          SHORTINT    "(SHORTINT)GetCurrentProcessId()";

BEGIN
  TestLittleEndian;

  HaltCode    := -128;
  HaltHandler := NIL;
  TimeStart   := Time();
  CWD         := "";  getCurrentDirectory(CWD);
  PID         := getpid();

  SeekSet := seekset();
  SeekCur := seekcur();
  SeekEnd := seekend();

  StdIn  := getstdinhandle();
  StdOut := getstdouthandle();
  StdErr := getstderrhandle();

  nl[0] := 0DX;  (* CR *)
  nl[1] := 0AX;  (* LF *)
  nl[2] := 0X;
END Platform.

