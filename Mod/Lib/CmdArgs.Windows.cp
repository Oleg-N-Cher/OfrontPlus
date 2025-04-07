MODULE CmdArgs; (** Command line argument handling for MS Windows *)

  IMPORT SYSTEM;

  TYPE PtrSTR = POINTER [notag] TO ARRAY [notag] OF SHORTCHAR;

  VAR
    Count-: INTEGER; (** number of arguments *)
    Truncated-: BOOLEAN; (** set in Get* when processing arguments *)
    cmdline: PtrSTR;
    dummy: ARRAY 1 OF SHORTCHAR;
    current, envCount: INTEGER;
    env: POINTER TO ARRAY OF SHORTCHAR;
    envPtr: POINTER TO ARRAY OF INTEGER;

  PROCEDURE [stdcall] GetCommandLineA ["GetCommandLineA"] (): PtrSTR;

  (* based on Arthur Yefimov's module Args, free.oberon.org *)
  PROCEDURE Get* (n: INTEGER; OUT val: ARRAY OF SHORTCHAR);
    VAR i, j: INTEGER;
  BEGIN
    current := -1; i := 0;
    (* Skip leading whitespaces if any *)
    WHILE (cmdline[i] # 0X) & (cmdline[i] <= " ") DO INC(i) END;
    WHILE cmdline[i] # 0X DO
      j := 0; Truncated := FALSE;
      IF cmdline[i] = '"' THEN
        INC(i);
        WHILE (cmdline[i] # 0X) & (cmdline[i] # '"') DO
          IF j < LEN(val) - 1 THEN
            val[j] := cmdline[i]; INC(j)
          ELSE
            Truncated := TRUE
          END;
          INC(i)
        END;
        IF cmdline[i] = '"' THEN INC(i) END
      ELSE
        WHILE cmdline[i] > " " DO
          IF j < LEN(val) - 1 THEN
            val[j] := cmdline[i]; INC(j)
          ELSE
            Truncated := TRUE
          END;
          INC(i)
        END
      END;
      val[j] := 0X; INC(current);
      IF current = n THEN RETURN END;
      (* Skip whitespaces *)
      WHILE (cmdline[i] # 0X) & (cmdline[i] <= " ") DO INC(i) END
    END;
    val := ""
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
    VAR i: INTEGER; arg: ARRAY 260 (*MAX_PATH*) OF SHORTCHAR;
  BEGIN
    i := 0; Get(i, arg);
    WHILE (i <= Count) & ((s # arg) OR Truncated) DO INC(i); Get(i, arg) END;
    RETURN i
  END Pos;

  (* Program environmet access *)

  PROCEDURE [stdcall] GetEnvironmentVariableA ["GetEnvironmentVariableA"]
    (lpName, lpBuffer: PtrSTR; nSize: INTEGER): INTEGER;

  PROCEDURE [stdcall] GetEnvironmentStringsA ["GetEnvironmentStringsA"]
    (): PtrSTR;

  PROCEDURE [stdcall] FreeEnvironmentStringsA ["FreeEnvironmentStringsA"]
    (env: PtrSTR): BOOLEAN;

  PROCEDURE- ExternPlatformOSAllocate
    "extern SYSTEM_ADRINT Platform_OSAllocate (SYSTEM_ADRINT size);";
  PROCEDURE- ExternPlatformOSFree
    "extern void Platform_OSFree (SYSTEM_ADRINT adr);";

  PROCEDURE- OSAllocate (size: SYSTEM.ADRINT): SYSTEM.ADRINT "Platform_OSAllocate(size)";
  PROCEDURE- OSFree (adr: SYSTEM.ADRINT) "Platform_OSFree(adr)";

  PROCEDURE GetEnv* (IN var: ARRAY OF SHORTCHAR; OUT val: ARRAY OF SHORTCHAR);
    VAR i, len: INTEGER; buf: PtrSTR;
  BEGIN
    Truncated := FALSE;
    len := GetEnvironmentVariableA(var, val, LEN(val));
    IF len = 0 THEN
      val := ""
    ELSIF len >= LEN(val) THEN
      buf := SYSTEM.VAL(PtrSTR, OSAllocate(len * SIZE(SHORTCHAR)));
      IF GetEnvironmentVariableA(var, buf, len) = 0 THEN
        val := ""
      ELSE
        Truncated := TRUE;
        i := 0;
        WHILE i < LEN(val) - 1 DO val[i] := buf[i]; INC(i) END;
        val[i] := 0X
      END;
      OSFree(SYSTEM.VAL(SYSTEM.ADRINT, buf))
    END
  END GetEnv;

  PROCEDURE MaybeLoadEnv;
    VAR p: PtrSTR; i, j: INTEGER;
  BEGIN
    envCount := 0;
    p := GetEnvironmentStringsA();
    IF p # NIL THEN
      i := 0;
      WHILE p[i] # 0X DO
        WHILE p[i] # 0X DO INC(i) END;
        INC(i); INC(envCount)
      END;
      IF envCount > 0 THEN
        NEW(env, i - 1);
        NEW(envPtr, envCount);
        i := 0; j := 0;
        WHILE p[i] # 0X DO
          envPtr[j] := i; INC(j);
          WHILE p[i] # 0X DO env[i] := p[i]; INC(i) END;
          env[i] := 0X; INC(i)
        END
      END;
      IF FreeEnvironmentStringsA(p) THEN END
    END
  END MaybeLoadEnv;

  PROCEDURE GetEnvN* (n: INTEGER; OUT val: ARRAY OF SHORTCHAR);
    VAR p: PtrSTR; i, j: INTEGER;
  BEGIN
    Truncated := FALSE;
    MaybeLoadEnv;
    IF (0 <= n) & (n < envCount) THEN
      i := 0; j := envPtr[n];
      WHILE env[j] # 0X DO
        IF i < LEN(val) - 1 THEN val[i] := env[j]
        ELSE
          val[i] := 0X; Truncated := TRUE;
          env := NIL; envPtr := NIL;
          RETURN
        END;
        INC(i); INC(j)
      END;
      val[i] := 0X
    ELSE
      val := ""
    END;
    env := NIL; envPtr := NIL
  END GetEnvN;

  PROCEDURE EnvCount* (): INTEGER;
  BEGIN
    MaybeLoadEnv; env := NIL; envPtr := NIL;
    RETURN envCount
  END EnvCount;

BEGIN cmdline := GetCommandLineA(); Get(-1, dummy); Count := current;
  envCount := -1
END CmdArgs.
