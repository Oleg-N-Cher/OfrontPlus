MODULE Platform; (** Unix *)
IMPORT SYSTEM;


CONST
  StdIn-  = 0;
  StdOut- = 1;
  StdErr- = 2;


TYPE
  Int8* = BYTE; Int16* = SHORTINT; Int32* = INTEGER; Int64* = LONGINT;
  MemAdr* = POINTER [1] TO ARRAY [1] 1 OF CHAR;  (* 32 or 64 bit *)
  
  TIME_T = POINTER [1] TO ARRAY [1] 1 OF CHAR;  (* time_t type is used
    instead of long, but which is also a 32-bit representation on some systems
    while there are even 64-bit representations on other systems. *)
  DEV_T = LONGINT;  (* 64 bit in Linux 2.2 *)
  INO_T = POINTER [1] TO ARRAY [1] 1 OF CHAR;  (* 32 or 64 bit *)

  HaltProcedure = PROCEDURE(n: INTEGER);
  SignalHandler = PROCEDURE(signal: INTEGER);

  ErrorCode*  = INTEGER;
  FileHandle* = INTEGER;  (* In UNIX, a file handle is an opaque number
    that is used to uniquely identify a file or other file system object.
    The only operations that can be carried out with the file handle in UNIX
    are to copy and compare it for equality with another file handle. *)

  FileIdentity* = RECORD
    volume: DEV_T;   (* dev on Unix filesystems, volume serial number on NTFS *)
    index:  INO_T;   (* inode on Unix filesystems, file id on NTFS *)
    mtime:  TIME_T;  (* File modification time, value is system dependent *)
  END;

  EnvPtr  = POINTER [1] TO ARRAY [1] 1024 OF CHAR;
  ArgPtr  = POINTER [1] TO ARRAY [1] 1024 OF CHAR;
  ArgVec* = POINTER [1] TO ARRAY [1] 1024 OF ArgPtr;


VAR
  LittleEndian-:   BOOLEAN;
  MainStackFrame-: MemAdr;
  HaltCode-:       INTEGER;
  PID-:            INTEGER;  (* Note: Must be updated by Fork implementation *)
  CWD-:            ARRAY 256 OF CHAR;
  ArgCount-:       INTEGER;

  ArgVector-:      ArgVec;
  HaltHandler:     HaltProcedure;
  TimeStart:       INTEGER;  (* milliseconds *)

  SeekSet-:        INTEGER;
  SeekCur-:        INTEGER;
  SeekEnd-:        INTEGER;

  nl-:  ARRAY 2 OF CHAR;  (* Platform specific newline representation *)


(* Some unsigned arithmetic *)

PROCEDURE -UBYTE*(b: BYTE): SHORTINT "((SHORTINT)(unsigned char)(b))";
PROCEDURE -USHORT*(s: SHORTINT): INTEGER "((INTEGER)(unsigned short)(s))";
PROCEDURE -UINT*(i: INTEGER): LONGINT "((LONGINT)(unsigned int)(i))";


(* Unix headers to be included *)

(*PROCEDURE -AAincludeSystime  "#include <sys/time.h>";  (* for gettimeofday *)*)
PROCEDURE -AAincludeTime     "#include <time.h>";      (* for localtime *)
PROCEDURE -AAincludeSystypes "#include <sys/types.h>";
PROCEDURE -AAincludeUnistd   "#include <unistd.h>";
PROCEDURE -AAincludeSysstat  "#include <sys/stat.h>";
PROCEDURE -AAincludeFcntl    "#include <fcntl.h>";
PROCEDURE -AAincludeErrno    "#include <errno.h>";
PROCEDURE -AAincludeStdlib   "#include <stdlib.h>";
PROCEDURE -AAincludeStdio    "#include <stdio.h>";


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

PROCEDURE ^ Halt*(code: INTEGER);

PROCEDURE -allocate(size: INTEGER): MemAdr "((Platform_MemAdr)malloc((size_t)(size)))";
PROCEDURE OSAllocate*(size: INTEGER): MemAdr;
BEGIN
  IF size > 0 THEN
    RETURN allocate(size)  (* malloc will return the null pointer when it fails. *)
  END;
  Halt(-25); RETURN NIL (* To remove __RETCHK *)
END OSAllocate;

PROCEDURE -free(address: MemAdr) "free((void*)address)";
PROCEDURE OSFree*(address: MemAdr); BEGIN free(address) END OSFree;


(* Program startup *)

PROCEDURE -ExternInitHeap  "extern void Heap_InitHeap();";
PROCEDURE -HeapInitHeap()  "Heap_InitHeap()";

PROCEDURE Init*(argc: INTEGER; argvadr: MemAdr);
TYPE
  ArgVecPtr = POINTER [1] TO ARRAY [1] 1 OF ArgVec;
VAR
  av: ArgVecPtr;
BEGIN
  MainStackFrame := argvadr;
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

PROCEDURE GetArg*(n: INTEGER; VAR val: ARRAY OF CHAR);
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

(* struct timeval
   The struct timeval structure represents an elapsed time. It is declared
   in sys/time.h and has the following members:

      time_t tv_sec
      This represents the number of whole seconds of elapsed time.

      suseconds_t tv_usec
      This is the rest of the elapsed time (a fraction of a second), represented
      as the number of microseconds. It is always less than one million.

   Using long int for tv_sec will work until year 2038, and after that the
   tv_sec will overflow on machines where long is 4 bytes.
    
   That is why some are using unsigned long, as it will stop the overflow
   until year 2100+. You should use the time_t type instead, and you won't need
   to think about how long your program is supposed to run for in the future.
*)
PROCEDURE -gettimeval          "struct timeval tv; gettimeofday(&tv,0)";
PROCEDURE -tvsec():  TIME_T    "(Platform_TIME_T)tv.tv_sec";
PROCEDURE -tvusec(): INTEGER   "(INTEGER)tv.tv_usec";
PROCEDURE -sectotm(s: TIME_T)  "struct tm *time = localtime((time_t*)&s)";
PROCEDURE -tmsec():  INTEGER   "time->tm_sec";
PROCEDURE -tmmin():  INTEGER   "time->tm_min";
PROCEDURE -tmhour(): INTEGER   "time->tm_hour";
PROCEDURE -tmmday(): INTEGER   "time->tm_mday";
PROCEDURE -tmmon():  INTEGER   "time->tm_mon";
PROCEDURE -tmyear(): INTEGER   "time->tm_year";

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

PROCEDURE -timeint(time: TIME_T): INTEGER "((INTEGER)(time))";

PROCEDURE Time*(): INTEGER;
VAR ms: INTEGER;
BEGIN
  gettimeval;
  ms := (tvusec() DIV 1000) + (timeint(tvsec()) * 1000);
  RETURN (ms - TimeStart) MOD 7FFFFFFFH
END Time;

(* struct timespec
   Structure holding an interval broken down into seconds and nanoseconds.

      time_t tv_sec  whole seconds (valid values are >= 0)
      long tv_nsec   nanoseconds (valid values are [0, 999999999])
*)
PROCEDURE -nanosleep(s, ns: INTEGER) "struct timespec req, rem; req.tv_sec = (time_t)s; req.tv_nsec = ns; nanosleep(&req, &rem)";

PROCEDURE Delay*(ms: INTEGER);
VAR s, ns: INTEGER;
BEGIN
  s  :=  ms DIV 1000;
  ns := (ms MOD 1000) * 1000000;
  nanosleep(s, ns);
END Delay;


(* System call *)

PROCEDURE -system(str: ARRAY OF CHAR): INTEGER "system((char*)str)";
PROCEDURE -err(): INTEGER "errno";

PROCEDURE System*(cmd : ARRAY OF CHAR): INTEGER;
BEGIN RETURN system(cmd) END System;

PROCEDURE Error*(): ErrorCode; BEGIN RETURN err() END Error;


(* File system *)

PROCEDURE -InvalidHandleValue*(): FileHandle "(-1)";

(* Note: Consider also using flags O_SYNC and O_DIRECT as we do buffering *)
PROCEDURE -openrw (n: ARRAY OF CHAR): INTEGER "open((char*)n, O_RDWR)";
PROCEDURE -openro (n: ARRAY OF CHAR): INTEGER "open((char*)n, O_RDONLY)";
PROCEDURE -opennew(n: ARRAY OF CHAR): INTEGER "open((char*)n, O_CREAT | O_TRUNC | O_RDWR, 0664)";

(* File APIs *)

PROCEDURE OldRO*(VAR n: ARRAY OF CHAR; VAR h: FileHandle): ErrorCode;
VAR fd: FileHandle;
BEGIN
  fd := openro(n);
  IF fd = InvalidHandleValue() THEN RETURN err() ELSE h := fd; RETURN 0 END;
END OldRO;

PROCEDURE OldRW*(VAR n: ARRAY OF CHAR; VAR h: FileHandle): ErrorCode;
VAR fd: FileHandle;
BEGIN
  fd := openrw(n);
  IF fd = InvalidHandleValue() THEN RETURN err() ELSE h := fd; RETURN 0 END;
END OldRW;

PROCEDURE New*(VAR n: ARRAY OF CHAR; VAR h: FileHandle): ErrorCode;
VAR fd: FileHandle;
BEGIN
  fd := opennew(n);
  IF fd = InvalidHandleValue() THEN RETURN err() ELSE h := fd; RETURN 0 END;
END New;


PROCEDURE -closefile (fd: FileHandle): INTEGER "close(fd)";

PROCEDURE Close*(h: FileHandle): ErrorCode;
BEGIN
  IF closefile(h) < 0 THEN RETURN err() ELSE RETURN 0 END
END Close;


PROCEDURE -fstat(fd: FileHandle):  INTEGER "fstat(fd, &s)";
PROCEDURE -stat(n: ARRAY OF CHAR): INTEGER "stat((char*)n, &s)";
PROCEDURE -structstats                     "struct stat s";
PROCEDURE -statdev():              DEV_T   "(LONGINT)s.st_dev";
PROCEDURE -statino():              INO_T   "(Platform_INO_T)s.st_ino";
PROCEDURE -statmtime():            TIME_T  "(Platform_TIME_T)s.st_mtime";
PROCEDURE -statsize():             LONGINT "(LONGINT)s.st_size";

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


PROCEDURE Size*(h: FileHandle; VAR l: LONGINT): ErrorCode;
BEGIN
  structstats;
  IF fstat(h) < 0 THEN RETURN err() END;
  l := statsize();
  RETURN 0
END Size;


PROCEDURE -readfile (fd: INTEGER; p: MemAdr; l: INTEGER): INTEGER
"read(fd, (void*)(SYSTEM_ADR)(p), l)";

PROCEDURE Read*(h: FileHandle; p: MemAdr; l: INTEGER; VAR n: INTEGER): ErrorCode;
BEGIN
  n := readfile(h, p, l);
  IF n < 0 THEN n := 0; RETURN err() ELSE RETURN 0 END
END Read;

PROCEDURE ReadBuf*(h: FileHandle; VAR b: ARRAY OF BYTE; VAR n: INTEGER): ErrorCode;
BEGIN
  n := readfile(h, SYSTEM.VAL(MemAdr, SYSTEM.ADR(b)), SHORT(LEN(b)));
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
  VAR i: INTEGER;
 BEGIN i := 1; SYSTEM.GET(SYSTEM.ADR(i), LittleEndian); END TestLittleEndian;


PROCEDURE -getpid(): INTEGER   "(INTEGER)getpid()";

BEGIN
  TestLittleEndian;

  HaltCode    := -128;
  HaltHandler := NIL;
  TimeStart   := 0;  TimeStart := Time();  (* Time() uses TimeStart *)
  CWD         := "";  getcwd(CWD);
  PID         := getpid();

  SeekSet := seekset();
  SeekCur := seekcur();
  SeekEnd := seekend();

  nl[0] := 0AX; (* LF *)
  nl[1] := 0X;
END Platform.

