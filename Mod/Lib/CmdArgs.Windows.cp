MODULE CmdArgs; (** Command line argument handling for MS Windows *)

  IMPORT SYSTEM;

  TYPE PtrSTR = POINTER [notag] TO ARRAY [notag] OF SHORTCHAR;

  VAR
    Count-, current: INTEGER;
    cmdline: PtrSTR;
    env: POINTER TO ARRAY OF SHORTCHAR;
    envPtr: POINTER TO ARRAY OF INTEGER;
    envCount: INTEGER;
    dummy: ARRAY 1 OF SHORTCHAR;

  PROCEDURE [stdcall] GetCommandLine ["GetCommandLineA"] (): PtrSTR;

  (* based on Arthur Yefimov's module Args, free.oberon.org *)
  PROCEDURE Get* (n: INTEGER; VAR val: ARRAY OF SHORTCHAR);
    VAR i, j: INTEGER; param: ARRAY 260 OF SHORTCHAR;
  BEGIN
    current := -1; i := 0;
    (* Skip leading whitespaces if any *)
    WHILE (cmdline[i] # 0X) & (cmdline[i] <= " ") DO INC(i) END;
    WHILE cmdline[i] # 0X DO
      IF cmdline[i] = '"' THEN
        INC(i); j := 0;
        WHILE (cmdline[i] # 0X) & (cmdline[i] # '"') DO
          IF j < LEN(param) - 1 THEN param[j] := cmdline[i]; INC(j) END;
          INC(i)
        END;
        IF cmdline[i] = '"' THEN INC(i) END
      ELSE
        j := 0;
        WHILE cmdline[i] > " " DO
          IF j < LEN(param) - 1 THEN param[j] := cmdline[i]; INC(j) END;
          INC(i)
        END
      END;
      param[j] := 0X; INC(current);
      IF current = n THEN
        IF LEN(val) > LEN(param$) THEN val := param$ END;
        RETURN
      END;
      (* Skip whitespaces *)
      WHILE (cmdline[i] # 0X) & (cmdline[i] <= " ") DO INC(i) END
    END
  END Get;

  PROCEDURE GetInt* (n: INTEGER; VAR val: INTEGER);
    VAR s: ARRAY 64 OF SHORTCHAR; k, d, i: INTEGER;
  BEGIN
    s := ""; Get(n, s); i := 0;
    IF s[0] = "-" THEN i := 1 END;
    k := 0; d := ORD(s[i]) - ORD("0");
    WHILE (d >= 0 ) & (d <= 9) DO k := k*10 + d; INC(i); d := ORD(s[i]) - ORD("0") END;
    IF s[0] = "-" THEN k := -k; DEC(i) END;
    IF i > 0 THEN val := k END
  END GetInt;

  PROCEDURE Pos* (IN s: ARRAY OF SHORTCHAR): INTEGER;
    VAR i: INTEGER; arg: ARRAY 256 OF SHORTCHAR;
  BEGIN
    i := 0; Get(i, arg);
    WHILE (i <= Count) & (s # arg) DO INC(i); Get(i, arg) END;
    RETURN i
  END Pos;

  (* Program environmet access *)

  PROCEDURE [stdcall] GetEnvironmentVariable ["GetEnvironmentVariableA"]
    (lpName, lpBuffer: PtrSTR; nSize: INTEGER): INTEGER;

  PROCEDURE [stdcall] GetEnvironmentStringsA ["GetEnvironmentStringsA"]
    (): PtrSTR;

  PROCEDURE [stdcall] FreeEnvironmentStringsA ["FreeEnvironmentStringsA"]
    (env: PtrSTR): BOOLEAN;

  PROCEDURE GetEnv* (IN var: ARRAY OF SHORTCHAR; OUT val: ARRAY OF SHORTCHAR);
    VAR buf: ARRAY 4096 OF SHORTCHAR; res: INTEGER;
  BEGIN
    res := GetEnvironmentVariable(var, buf, LEN(buf));
    IF (res > 0) & (res < LEN(buf)) THEN val := buf$ ELSE val := "" END
  END GetEnv;

  PROCEDURE MaybeLoadEnv;
    VAR p: PtrSTR;
      i, j, len: INTEGER;
  BEGIN
    IF envCount = -1 THEN
      envCount := 0;
      p := GetEnvironmentStringsA();
      IF p # NIL THEN
        i := 0;
        WHILE p[i] # 0X DO
          WHILE p[i] # 0X DO INC(i) END;
          INC(i); INC(envCount)
        END;

        NEW(env, i - 1);
        NEW(envPtr, envCount);
        i := 0; j := 0;
        WHILE p[i] # 0X DO
          envPtr[j] := i; INC(j);
          WHILE p[i] # 0X DO env[i] := p[i]; INC(i) END;
          env[i] := 0X; INC(i)
        END;
        IF FreeEnvironmentStringsA(p) THEN END
      END
    END
  END MaybeLoadEnv;

  PROCEDURE GetEnvN* (n: INTEGER; VAR s: ARRAY OF SHORTCHAR);
    VAR p: PtrSTR; i, j: INTEGER;
  BEGIN
    MaybeLoadEnv;
    IF (0 <= n) & (n < envCount) THEN
      i := 0; j := envPtr[n];
      WHILE (i < LEN(s) - 1) & (env[j] # 0X) DO
        s[i] := env[j]; INC(i); INC(j)
      END;
      s[i] := 0X
    ELSE s := "" END
  END GetEnvN;

  PROCEDURE EnvCount* (): INTEGER;
  BEGIN
    MaybeLoadEnv;
    RETURN envCount
  END EnvCount;

BEGIN cmdline := GetCommandLine(); Get(-1, dummy); Count := current;
  envCount := -1
END CmdArgs.
