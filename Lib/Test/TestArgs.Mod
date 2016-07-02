MODULE TestArgs;

IMPORT Args, Write := Console;

VAR
  i: LONGINT; n: SHORTINT; arg, var: ARRAY 1024 OF CHAR;

PROCEDURE Length (VAR str: ARRAY OF CHAR): LONGINT;
VAR
  len: LONGINT;
BEGIN
  len := 0;
  WHILE (len < SHORT(LEN(str))) & (str[len] # 0X) DO INC(len) END;
  RETURN len
END Length;

BEGIN
  Write.String("Args.argc = "); Write.Int(Args.argc, 0); Write.Ln; Write.Ln;
  FOR n := 1 TO SHORT(Args.argc - 1) DO
    Args.Get(n, arg);
    Write.Int(n, 0); Write.Char(":"); Write.String(arg); Write.Ln;
    FOR i := 0 TO SHORT(Length(arg)) - 1 DO
      Write.Int(ORD(arg[i]), 0); Write.Char(",")
    END;
    Write.Ln
  END;

  Args.GetEnv("OBERON", var); Write.Ln;
  IF var # "" THEN Write.String("OBERON="); Write.String(var); Write.Ln END;
  var := "";
  Args.GetEnv("PATH", var); Write.Ln;
  IF var # "" THEN Write.String("PATH="); Write.String(var); Write.Ln END;
  var := "";
  Args.GetEnv("HOME", var); Write.Ln;
  IF var # "" THEN Write.String("HOME="); Write.String(var); Write.Ln END;
  var := "";

END TestArgs.
