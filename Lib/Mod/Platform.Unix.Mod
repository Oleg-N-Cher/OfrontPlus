MODULE Platform; (* Unix *)
IMPORT SYSTEM;


CONST
  StdIn-  = 0;
  StdOut- = 1;
  StdErr- = 2;


TYPE
  ADR = POINTER [1] TO ARRAY [1] 1 OF CHAR; (* 32 or 64 bit *)
  SIZE_T = POINTER [1] TO ARRAY [1] 1 OF CHAR; (* 32 or 64 bit *)
  
  HaltProcedure = PROCEDURE(n: INTEGER);
  SignalHandler = PROCEDURE(signal: INTEGER);

  ErrorCode*  = INTEGER;
  FileHandle* = INTEGER; (* In UNIX, a file handle is an opaque number
    that is used to uniquely identify a file or other file system object.
    The only operations that can be carried out with the file handle in UNIX
    are to copy and compare it for equality with another file handle. *)

  Dev  = LONGINT;  (* 64 bit in Linux 2.2 *)
  Ino  = ADR;      (* 32 or 64 bit *)
  Time = ADR;      (* 32 or 64 bit *)
  
  FileIdentity* = RECORD
    volume: Dev;   (* dev on Unix filesystems, volume serial number on NTFS *)
    index:  Ino;   (* inode on Unix filesystems, file id on NTFS *)
    mtime:  Time;  (* File modification time, value is system dependent *)
  END;

  EnvPtr    = POINTER [1] TO ARRAY [1] 1024 OF CHAR;
  ArgPtr    = POINTER [1] TO ARRAY [1] 1024 OF CHAR;
  ArgVec*   = POINTER [1] TO ARRAY [1] 1024 OF ArgPtr;


VAR
  LittleEndian-:   BOOLEAN;
  MainStackFrame-: ADR;
  HaltCode-:       INTEGER;
  PID-:            INTEGER;    (* Note: Must be updated by Fork implementation *)
  CWD-:            ARRAY 256 OF CHAR;
  ArgCount-:       INTEGER;

  ArgVector-:      ArgVec;
  HaltHandler:     HaltProcedure;
  TimeStart:       INTEGER; (* milliseconds *)

  SeekSet-:        INTEGER;
  SeekCur-:        INTEGER;
  SeekEnd-:        INTEGER;

  nl-:             ARRAY 3 OF CHAR;  (* Platform specific newline representation *)


(* Unix headers to be included *)

PROCEDURE -Aincludesystime  "#include <sys/time.h>";  (* for gettimeofday *)
PROCEDURE -Aincludetime     "#include <time.h>";      (* for localtime *)
PROCEDURE -Aincludesystypes "#include <sys/types.h>";
PROCEDURE -Aincludeunistd   "#include <unistd.h>";
PROCEDURE -Aincludesysstat  "#include <sys/stat.h>";
PROCEDURE -Aincludefcntl    "#include <fcntl.h>";
PROCEDURE -Aincludeerrno    "#include <errno.h>";
PROCEDURE -Astdlib          "#include <stdlib.h>";
PROCEDURE -Astdio           "#include <stdio.h>";
PROCEDURE -Aerrno           "#include <errno.h>";


(* Error code tests *)

PROCEDURE -EMFILE():       ErrorCode "EMFILE";
PROCEDURE -ENFILE():       ErrorCode "ENFILE";
PROCEDURE -ENOENT():       ErrorCode "ENOENT";
PROCEDURE -EXDEV():        ErrorCode "EXDEV";
PROCEDURE -EACCES():       ErrorCode "EACCES";
PROCEDURE -EROFS():        ErrorCode "EROFS";
PROCEDURE -EAGAIN():       ErrorCode "EAGAIN";
PROCEDURE -ETIMEDOUT():    ErrorCode "ETIMEDOUT";
PROCEDURE -ECONNREFUSED(): ErrorCode "ECONNREFUSED";
PROCEDURE -ECONNABORTED(): ErrorCode "ECONNABORTED";
PROCEDURE -ENETUNREACH():  ErrorCode "ENETUNREACH";
PROCEDURE -EHOSTUNREACH(): ErrorCode "EHOSTUNREACH";


PROCEDURE TooManyFiles*(e: ErrorCode): BOOLEAN;
BEGIN RETURN (e = EMFILE()) OR (e = ENFILE()) END TooManyFiles;

PROCEDURE NoSuchDirectory*(e: ErrorCode): BOOLEAN;
BEGIN RETURN e = ENOENT() END NoSuchDirectory;

PROCEDURE DifferentFilesystems*(e: ErrorCode): BOOLEAN;
BEGIN RETURN e = EXDEV() END DifferentFilesystems;

PROCEDURE Inaccessible*(e: ErrorCode): BOOLEAN;
BEGIN RETURN (e = EACCES()) OR (e = EROFS()) OR (e = EAGAIN()) END Inaccessible;

PROCEDURE Absent*(e: ErrorCode): BOOLEAN;
BEGIN RETURN (e = ENOENT()) END Absent;

PROCEDURE TimedOut*(e: ErrorCode): BOOLEAN;
BEGIN RETURN (e = ETIMEDOUT()) END TimedOut;

PROCEDURE ConnectionFailed*(e: ErrorCode): BOOLEAN;
BEGIN RETURN (e = ECONNREFUSED()) OR (e = ECONNABORTED())
          OR (e = ENETUNREACH())  OR (e = EHOSTUNREACH()) END ConnectionFailed;


(* OS memory allocaton *)

PROCEDURE -allocate(size: SIZE_T): ADR "((Platform_ADR)malloc((size_t)size))";
PROCEDURE OSAllocate*(size: SIZE_T): ADR; BEGIN RETURN allocate(size) END OSAllocate;

PROCEDURE -free(address: ADR) "free((void*)address)";
PROCEDURE OSFree*(address: ADR); BEGIN free(address) END OSFree;


(* Program startup *)

PROCEDURE -ExternInitHeap  "extern void Heap_InitHeap();";
PROCEDURE -HeapInitHeap()  "Heap_InitHeap()";

PROCEDURE Init*(argc: INTEGER; argvadr: SYSTEM.PTR);
TYPE
  ArgVecPtr = POINTER [1] TO ARRAY [1] 1 OF ArgVec;
VAR
  av: ArgVecPtr;
BEGIN
  MainStackFrame := SYSTEM.VAL(ADR, argvadr);
  ArgCount       := argc;
  av             := SYSTEM.VAL(ArgVecPtr, argvadr);
  ArgVector      := av[0];
  HaltCode       := -128;

  (* This function (Platform.Init) is called at program startup BEFORE any
     modules have been initalized. In turn we must initialize the heap
     before module startup (xxx__init) code is run. *)
  HeapInitHeap();
END Init;


(* Program arguments and environment access *)

PROCEDURE -getenv(var: ARRAY OF CHAR): EnvPtr "(Platform_EnvPtr)getenv((char*)var)";

PROCEDURE getEnv*(var: ARRAY OF CHAR; VAR val: ARRAY OF CHAR): BOOLEAN;
  VAR p: EnvPtr;
BEGIN
  p := getenv(var);
  IF p # NIL THEN COPY(p^, val) END;
  RETURN p # NIL
END getEnv;

PROCEDURE GetEnv*(var: ARRAY OF CHAR; VAR val: ARRAY OF CHAR);
BEGIN
  IF ~ getEnv(var, val) THEN val[0] := 0X END;
END GetEnv;

PROCEDURE GetArg*(n: SHORTINT; VAR val: ARRAY OF CHAR);
BEGIN
  IF n < ArgCount THEN COPY(ArgVector[n]^, val) END
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

PROCEDURE ArgPos*(s: ARRAY OF CHAR): INTEGER;
  VAR i: INTEGER; arg: ARRAY 256 OF CHAR;
BEGIN
  i := 0; GetArg(i, arg);
  WHILE (i < ArgCount) & (s # arg) DO INC(i); GetArg(i, arg) END ;
  RETURN i
END ArgPos;


(* Signals and traps *)

PROCEDURE -sethandler(s: INTEGER; h: SignalHandler) "SystemSetHandler(s, (SYSTEM_ADR)h)";

PROCEDURE SetInterruptHandler*(handler: SignalHandler);
BEGIN sethandler(2, handler); END SetInterruptHandler;

PROCEDURE SetQuitHandler*(handler: SignalHandler);
BEGIN sethandler(3, handler); END SetQuitHandler;

PROCEDURE SetBadInstructionHandler*(handler: SignalHandler);
BEGIN sethandler(4, handler); END SetBadInstructionHandler;


(* Time of day *)

PROCEDURE -gettimeval          "struct timeval tv; gettimeofday(&tv,0)";
PROCEDURE -tvsec():  INTEGER   "tv.tv_sec";
PROCEDURE -tvusec(): INTEGER   "tv.tv_usec";
PROCEDURE -sectotm(s: INTEGER) "struct tm *time = localtime((time_t*)&s)";
PROCEDURE -tmsec():  INTEGER   "(INTEGER)time->tm_sec";
PROCEDURE -tmmin():  INTEGER   "(INTEGER)time->tm_min";
PROCEDURE -tmhour(): INTEGER   "(INTEGER)time->tm_hour";
PROCEDURE -tmmday(): INTEGER   "(INTEGER)time->tm_mday";
PROCEDURE -tmmon():  INTEGER   "(INTEGER)time->tm_mon";
PROCEDURE -tmyear(): INTEGER   "(INTEGER)time->tm_year";

PROCEDURE YMDHMStoClock(ye, mo, da, ho, mi, se: INTEGER; VAR t, d: INTEGER);
BEGIN
  d := ASH(ye MOD 100, 9) + ASH(mo+1, 5) + da;
  t := ASH(ho, 12)        + ASH(mi, 6)   + se;
END YMDHMStoClock;

PROCEDURE GetClock*(VAR t, d: INTEGER);
BEGIN
  gettimeval; sectotm(tvsec());
  YMDHMStoClock(tmyear(), tmmon(), tmmday(), tmhour(), tmmin(), tmsec(), t, d);
END GetClock;

PROCEDURE GetTimeOfDay*(VAR sec, usec: INTEGER);
BEGIN
  gettimeval; sec := tvsec(); usec := tvusec();
END GetTimeOfDay;

PROCEDURE Time*(): INTEGER;
VAR ms: INTEGER;
BEGIN
  gettimeval;
  ms := (tvusec() DIV 1000) + (tvsec() * 1000);
  RETURN (ms - TimeStart) MOD 7FFFFFFFH
END Time;

PROCEDURE -nanosleep(s: INTEGER; ns: INTEGER) "struct timespec req, rem; req.tv_sec = s; req.tv_nsec = ns; nanosleep(&req, &rem)";

PROCEDURE Delay*(ms: INTEGER);
VAR s, ns: INTEGER;
BEGIN
  s  :=  ms DIV 1000;
  ns := (ms MOD 1000) * 1000000;
  nanosleep(s, ns);
END Delay;


(* System call *)

PROCEDURE -system(str: ARRAY OF CHAR): SHORTINT "system((char*)str)";
PROCEDURE -err(): SHORTINT "errno";


PROCEDURE System*(cmd : ARRAY OF CHAR): SHORTINT;
BEGIN RETURN system(cmd) END System;

PROCEDURE Error*(): ErrorCode; BEGIN RETURN err() END Error;




(* File system *)

(* Note: Consider also using flags O_SYNC and O_DIRECT as we do buffering *)
PROCEDURE -openrw (n: ARRAY OF CHAR): SHORTINT "open((char*)n, O_RDWR)";
PROCEDURE -openro (n: ARRAY OF CHAR): SHORTINT "open((char*)n, O_RDONLY)";
PROCEDURE -opennew(n: ARRAY OF CHAR): SHORTINT "open((char*)n, O_CREAT | O_TRUNC | O_RDWR, 0664)";

(* File APIs *)

PROCEDURE OldRO*(VAR n: ARRAY OF CHAR; VAR h: FileHandle): ErrorCode;
VAR fd: SHORTINT;
BEGIN
  fd := openro(n);
  IF (fd < 0) THEN RETURN err() ELSE h := fd; RETURN 0 END;
END OldRO;

PROCEDURE OldRW*(VAR n: ARRAY OF CHAR; VAR h: FileHandle): ErrorCode;
VAR fd: SHORTINT;
BEGIN
  fd := openrw(n);
  IF (fd < 0) THEN RETURN err() ELSE h := fd; RETURN 0 END;
END OldRW;

PROCEDURE New*(VAR n: ARRAY OF CHAR; VAR h: FileHandle): ErrorCode;
VAR fd: SHORTINT;
BEGIN
  fd := opennew(n);
  IF (fd < 0) THEN RETURN err() ELSE h := fd; RETURN 0 END;
END New;



PROCEDURE -closefile (fd: INTEGER): SHORTINT "close(fd)";

PROCEDURE Close*(h: FileHandle): ErrorCode;
BEGIN
  IF closefile(h) < 0 THEN RETURN err() ELSE RETURN 0 END
END Close;



PROCEDURE -fstat(fd: INTEGER):     SHORTINT "fstat(fd, &s)";
PROCEDURE -stat(n: ARRAY OF CHAR): SHORTINT "stat((char*)n, &s)";
PROCEDURE -structstats                     "struct stat s";
PROCEDURE -statdev():              INTEGER "(INTEGER)s.st_dev";
PROCEDURE -statino():              INTEGER "(INTEGER)s.st_ino";
PROCEDURE -statmtime():            INTEGER "(INTEGER)s.st_mtime";
PROCEDURE -statsize():             INTEGER "(INTEGER)s.st_size";

PROCEDURE Identify*(h: FileHandle; VAR identity: FileIdentity): ErrorCode;
BEGIN
  structstats;
  IF fstat(h) < 0 THEN RETURN err() END;
  identity.volume := statdev();
  identity.index  := statino();
  identity.mtime  := statmtime();
  RETURN 0
END Identify;

PROCEDURE IdentifyByName*(n: ARRAY OF CHAR; VAR identity: FileIdentity): ErrorCode;
BEGIN
  structstats;
  IF stat(n) < 0 THEN RETURN err() END;
  identity.volume := statdev();
  identity.index  := statino();
  identity.mtime  := statmtime();
  RETURN 0
END IdentifyByName;


PROCEDURE SameFile*(i1, i2: FileIdentity): BOOLEAN;
BEGIN RETURN (i1.index = i2.index) & (i1.volume = i2.volume)
END SameFile;

PROCEDURE SameFileTime*(i1, i2: FileIdentity): BOOLEAN;
BEGIN RETURN i1.mtime = i2.mtime
END SameFileTime;

PROCEDURE SetMTime*(VAR target: FileIdentity; source: FileIdentity);
BEGIN target.mtime := source.mtime;
END SetMTime;

PROCEDURE MTimeAsClock*(i: FileIdentity; VAR t, d: INTEGER);
BEGIN
  sectotm(i.mtime);
  YMDHMStoClock(tmyear(), tmmon(), tmmday(), tmhour(), tmmin(), tmsec(), t, d);
END MTimeAsClock;


PROCEDURE Size*(h: FileHandle; VAR l: INTEGER): ErrorCode;
BEGIN
  structstats;
  IF fstat(h) < 0 THEN RETURN err() END;
  l := statsize();
  RETURN 0
END Size;



PROCEDURE -readfile (fd: INTEGER; p: INTEGER; l: INTEGER): INTEGER
"read(fd, (void*)(SYSTEM_ADR)(p), l)";

PROCEDURE Read*(h: FileHandle; p: INTEGER; l: INTEGER; VAR n: INTEGER): ErrorCode;
BEGIN
  n := readfile(h, p, l);
  IF n < 0 THEN n := 0; RETURN err() ELSE RETURN 0 END
END Read;

PROCEDURE ReadBuf*(h: FileHandle; VAR b: ARRAY OF BYTE; VAR n: INTEGER): ErrorCode;
BEGIN
  n := readfile(h, SYSTEM.ADR(b), LEN(b));
  IF n < 0 THEN n := 0; RETURN err() ELSE RETURN 0 END
END ReadBuf;



PROCEDURE -writefile(fd: INTEGER; p: INTEGER; l: INTEGER): INTEGER
"write(fd, (void*)(SYSTEM_ADR)(p), l)";

PROCEDURE Write*(h: FileHandle; p: INTEGER; l: INTEGER): ErrorCode;
  VAR written: INTEGER;
BEGIN
  written := writefile(h, p, l);
  IF written < 0 THEN RETURN err() ELSE RETURN 0 END
END Write;



PROCEDURE -fsync(fd: INTEGER): SHORTINT "fsync(fd)";

PROCEDURE Sync*(h: FileHandle): ErrorCode;
BEGIN
  IF fsync(h) < 0 THEN RETURN err() ELSE RETURN 0 END
END Sync;



PROCEDURE -lseek(fd: INTEGER; o: INTEGER; w: SHORTINT): SHORTINT "lseek(fd, o, w)";
PROCEDURE -seekset(): SHORTINT "SEEK_SET";
PROCEDURE -seekcur(): SHORTINT "SEEK_CUR";
PROCEDURE -seekend(): SHORTINT "SEEK_END";

PROCEDURE Seek*(h: FileHandle; offset: INTEGER; whence: SHORTINT): ErrorCode;
BEGIN
  IF lseek(h, offset, whence) < 0 THEN RETURN err() ELSE RETURN 0 END
END Seek;



PROCEDURE -ftruncate(fd: INTEGER; l: INTEGER): SHORTINT "ftruncate(fd, l)";

PROCEDURE Truncate*(h: FileHandle; l: INTEGER): ErrorCode;
BEGIN
  IF (ftruncate(h, l) < 0) THEN RETURN err() ELSE RETURN 0 END;
END Truncate;



PROCEDURE -unlink(n: ARRAY OF CHAR): SHORTINT "unlink((char*)n)";

PROCEDURE Unlink*(VAR n: ARRAY OF CHAR): ErrorCode;
BEGIN
  IF unlink(n) < 0 THEN RETURN err() ELSE RETURN 0 END
END Unlink;



PROCEDURE -chdir(n: ARRAY OF CHAR): SHORTINT "chdir((char*)n)";
PROCEDURE -getcwd(VAR cwd: ARRAY OF CHAR) "getcwd((char*)cwd, cwd__len)";

PROCEDURE Chdir*(VAR n: ARRAY OF CHAR): ErrorCode;
  VAR r: SHORTINT;
BEGIN
  r := chdir(n);  getcwd(CWD);
  IF r < 0 THEN RETURN err() ELSE RETURN 0 END
END Chdir;



PROCEDURE -rename(o,n: ARRAY OF CHAR): SHORTINT "rename((char*)o, (char*)n)";

PROCEDURE Rename*(VAR o,n: ARRAY OF CHAR): ErrorCode;
BEGIN
  IF rename(o,n) < 0 THEN RETURN err() ELSE RETURN 0 END
END Rename;




(* Process termination *)

PROCEDURE -exit(code: SHORTINT) "exit(code)";
PROCEDURE Exit*(code: SHORTINT);
BEGIN exit(code) END Exit;

PROCEDURE -errstring(s: ARRAY OF CHAR) "write(1, s, s__len-1)";
PROCEDURE -errc     (c: CHAR)          "write(1, &c, 1)";
PROCEDURE errch(c: CHAR); BEGIN errc(c) END errch;
PROCEDURE errln; BEGIN errch(0DX); errch(0AX) END errln;

PROCEDURE errposint(l: INTEGER);
BEGIN IF l>10 THEN errposint(l DIV 10) END; errch(CHR(ORD("0") + (l MOD 10))) END errposint;

PROCEDURE errint(l: INTEGER);
BEGIN IF l<0 THEN errch("-"); l := -l END; errposint(l) END errint;

PROCEDURE DisplayHaltCode(code: INTEGER);
BEGIN
  CASE code OF
  | -1: errstring("Assertion failure.")
  | -2: errstring("Index out of range.")
  | -3: errstring("Reached end of function without reaching RETURN.")
  | -4: errstring("CASE statement: no matching label and no ELSE.")
  | -5: errstring("Type guard failed.")
  | -6: errstring("Implicit type guard in record assignment failed.")
  | -7: errstring("Invalid case in WITH statement.")
  | -8: errstring("Value out of range.")
  | -9: errstring("Heap interrupted while locked, but lockdepth = 0 at unlock.")
  |-10: errstring("NIL access.");
  |-11: errstring("Alignment error.");
  |-12: errstring("Divide by zero.");
  |-13: errstring("Arithmetic overflow/underflow.");
  |-14: errstring("Invalid function argument.");
  |-15: errstring("Internal error, e.g. Type descriptor size mismatch.")
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


PROCEDURE -getpid(): SHORTINT   "(SHORTINT)getpid()";

BEGIN
  TestLittleEndian;

  HaltCode    := -128;
  HaltHandler := NIL;
  TimeStart   := Time();
  CWD         := "";  getcwd(CWD);
  PID         := getpid();

  SeekSet := seekset();
  SeekCur := seekcur();
  SeekEnd := seekend();

  nl[0] := 0AX; (* LF *)
  nl[1] := 0X;
END Platform.

