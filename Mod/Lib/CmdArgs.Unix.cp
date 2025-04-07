MODULE CmdArgs; (* jt, 8.12.94 *)

  (* Command line argument handling for UNIX/Linux *)

  IMPORT SYSTEM;

  TYPE
    ArgPtr = POINTER [notag] TO ARRAY [notag] OF SHORTCHAR;
    ArgVec = POINTER [notag] TO ARRAY [notag] OF ArgPtr;
    EnvPtr = POINTER [notag] TO ARRAY [notag] OF SHORTCHAR;
    Environ = POINTER [notag] TO RECORD [notag] e: EnvPtr END;

  VAR
    Count-: INTEGER;
    Truncated-: BOOLEAN; (** set in Get* when processing arguments *)

  PROCEDURE- includeStdlib      "#include <stdlib.h>";
  PROCEDURE- ExternArgc         "extern INTEGER SYSTEM_argc;";
  PROCEDURE- ExternArgv         "extern void *SYSTEM_argv;";
  PROCEDURE- ExternEnviron      "extern void **environ;";
  PROCEDURE- argc(): INTEGER    "SYSTEM_argc";
  PROCEDURE- argv(): ArgVec     "(CmdArgs_ArgVec)SYSTEM_argv";
  PROCEDURE- environ(): Environ "(CmdArgs_Environ)environ";

  PROCEDURE Get* (n: INTEGER; OUT val: ARRAY OF SHORTCHAR);
    VAR av: ArgVec; i: INTEGER;
  BEGIN
    val := ""; Truncated := FALSE;
    IF n < argc() THEN av := argv(); i := 0;
      WHILE av[n]^[i] # 0X DO
        IF i < LEN(val) - 1 THEN val[i] := av[n]^[i]
        ELSE val[i] := 0X; Truncated := TRUE; RETURN
        END;
        INC(i)
      END;
      val[i] := 0X
    END
  END Get;

  PROCEDURE GetInt* (n: INTEGER; OUT val: INTEGER);
    VAR s: ARRAY 16 OF SHORTCHAR; d, i: INTEGER;
  BEGIN
    Get(n, s); i := 0;
    IF s[0] = "-" THEN i := 1 END;
    WHILE s[i] = "0" DO INC(i) END;
    val := 0; d := ORD(s[i]) - ORD("0");
    WHILE (d >= 0 ) & (d <= 9) DO
      val := val*10 + d; INC(i); d := ORD(s[i]) - ORD("0")
    END;
    IF s[0] = "-" THEN val := -val END
  END GetInt;

  PROCEDURE Pos* (IN s: ARRAY OF SHORTCHAR): INTEGER;
    VAR i: INTEGER; arg: ARRAY 1024 OF SHORTCHAR;
  BEGIN
    i := 0; Get(i, arg);
    WHILE (i < argc()) & ((s # arg) OR Truncated) DO INC(i); Get(i, arg) END;
    RETURN i
  END Pos;

  (* Program environmet access *)

  PROCEDURE- getenv (var: ARRAY OF SHORTCHAR): EnvPtr "(CmdArgs_EnvPtr)getenv((char*)var)";

  PROCEDURE GetEnv* (IN var: ARRAY OF SHORTCHAR; OUT val: ARRAY OF SHORTCHAR);
    VAR p: EnvPtr; i: INTEGER;
  BEGIN
    Truncated := FALSE;
    p := getenv(var);
    IF p # NIL THEN i := 0;
      WHILE p^[i] # 0X DO
        IF i < LEN(val) - 1 THEN val[i] := p^[i]
        ELSE val[i] := 0X; Truncated := TRUE; RETURN
        END;
        INC(i)
      END;
      val[i] := 0X
    ELSE
      val := ""
    END
  END GetEnv;

  PROCEDURE- nextenviron (p: Environ; n: INTEGER): Environ "p + n";

  PROCEDURE EnvCount* (): INTEGER;
    VAR count: INTEGER; p: Environ;
  BEGIN
    p := environ();
    count := 0;
    WHILE p.e # NIL DO
      INC(count);
      p := nextenviron(p, 1)
    END;
    RETURN count
  END EnvCount;

  PROCEDURE GetEnvN* (n: INTEGER; OUT val: ARRAY OF SHORTCHAR);
    VAR p: Environ; i: INTEGER;
  BEGIN
    Truncated := FALSE;
    p := environ();
    p := nextenviron(p, n);
    IF p.e # NIL THEN i := 0;
      WHILE p.e^[i] # 0X DO
        IF i < LEN(val) - 1 THEN val[i] := p.e^[i]
        ELSE val[i] := 0X; Truncated := TRUE; RETURN
        END;
        INC(i)
      END;
      val[i] := 0X
    ELSE
      val := ""
    END
  END GetEnvN;

BEGIN Count := argc() - 1
END CmdArgs.
