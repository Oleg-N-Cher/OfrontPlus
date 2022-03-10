MODULE CmdArgs; (* jt, 8.12.94 *)

  (* Command line argument handling for UNIX/Linux *)

  IMPORT SYSTEM;

  TYPE
    ArgPtr = POINTER [notag] TO ARRAY 2048 OF SHORTCHAR;
    ArgVec = POINTER [notag] TO ARRAY 2048 OF ArgPtr;
    EnvPtr = POINTER [notag] TO ARRAY 2048 OF SHORTCHAR;
    Environ = POINTER [notag] TO RECORD [notag] e: EnvPtr END;

  VAR
    Count-: INTEGER;

  PROCEDURE- includeStdlib      "#include <stdlib.h>";
  PROCEDURE- ExternArgc         "extern INTEGER SYSTEM_argc;";
  PROCEDURE- ExternArgv         "extern void *SYSTEM_argv;";
  PROCEDURE- ExternEnviron      "extern void **environ;";
  PROCEDURE- argc(): INTEGER    "SYSTEM_argc";
  PROCEDURE- argv(): ArgVec     "(CmdArgs_ArgVec)SYSTEM_argv";
  PROCEDURE- environ(): Environ "(CmdArgs_Environ)environ";

  PROCEDURE Get* (n: INTEGER; VAR val: ARRAY OF SHORTCHAR);
    VAR av: ArgVec;
  BEGIN
    IF n < argc() THEN av := argv();
      IF LEN(val) > LEN(av[n]^$) THEN val := av[n]^$ END
    END
  END Get;

  PROCEDURE GetInt* (n: INTEGER; VAR val: INTEGER);
    VAR s: ARRAY 64 OF SHORTCHAR; k, d, i: INTEGER;
  BEGIN
    s := ""; Get(n, s); i := 0;
    IF s[0] = "-" THEN i := 1 END ;
    k := 0; d := ORD(s[i]) - ORD("0");
    WHILE (d >= 0 ) & (d <= 9) DO k := k*10 + d; INC(i); d := ORD(s[i]) - ORD("0") END;
    IF s[0] = "-" THEN k := -k; DEC(i) END;
    IF i > 0 THEN val := k END
  END GetInt;

  PROCEDURE Pos* (IN s: ARRAY OF SHORTCHAR): INTEGER;
    VAR i: INTEGER; arg: ARRAY 256 OF SHORTCHAR;
  BEGIN
    i := 0; Get(i, arg);
    WHILE (i < argc()) & (s # arg) DO INC(i); Get(i, arg) END;
    RETURN i
  END Pos;

  (* Program environmet access *)

  PROCEDURE- getenv (var: ARRAY OF SHORTCHAR): EnvPtr "(CmdArgs_EnvPtr)getenv((char*)var)";

  PROCEDURE GetEnv* (IN var: ARRAY OF SHORTCHAR; OUT val: ARRAY OF SHORTCHAR);
    VAR p: EnvPtr;
  BEGIN
    p := getenv(var);
    IF p # NIL THEN val := p^$ ELSE val := "" END
  END GetEnv;

  PROCEDURE- nextenviron(p: Environ; n: INTEGER): Environ "p + n";

  PROCEDURE GetEnvCount*(): INTEGER;
    VAR count: INTEGER;
      p: Environ;
  BEGIN
    p := environ();
    count := 0;
    WHILE p.e # NIL DO
      INC(count);
      p := nextenviron(p, 1)
    END;
    RETURN count
  END GetEnvCount;

  PROCEDURE GetEnvN*(n: INTEGER; VAR s: ARRAY OF SHORTCHAR);
    VAR p: Environ;
  BEGIN
    p := environ();
    p := nextenviron(p, n);
    IF p.e # NIL THEN s := p.e^$ ELSE s := "" END
  END GetEnvN;

BEGIN Count := argc() - 1
END CmdArgs.
