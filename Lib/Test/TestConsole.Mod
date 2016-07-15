MODULE TestConsole;

IMPORT SYSTEM, Write := Console;

VAR
  int: INTEGER; long: LONGINT;

BEGIN
  Write.LongInt(0, 0); Write.Char(" "); Write.LongHex(0); Write.Char("H"); Write.Ln;

  (* INTEGER *)
  int := MIN(INTEGER);
  Write.String("MIN(INTEGER) = "); Write.Int(int, 0); Write.Char(" ");
  Write.Hex(int); Write.String("H "); Write.Ln;
  INC(int); Write.String("MIN(INTEGER)+1 = "); Write.Int(int, 0);
  Write.Char(" "); Write.Hex(int); Write.Char("H"); Write.Ln;

  int := MAX(INTEGER);
  Write.String("MAX(INTEGER) = "); Write.Int(int, 0); Write.Char(" ");
  Write.Hex(int); Write.String("H "); Write.Ln;
  DEC(int); Write.String("MAX(INTEGER)-1 = "); Write.Int(int, 0);
  Write.Char(" "); Write.Hex(int); Write.Char("H"); Write.Ln;

  (* LONGINT *)
  long := SYSTEM.LSH(LONG(LONG(1)), 63);
  Write.String("MIN(LONGINT) = "); Write.LongInt(long, 0); Write.Char(" ");
  Write.LongHex(long); Write.Char("H"); Write.Ln;
  INC(long); Write.String("MIN(LONGINT)+1 = "); Write.LongInt(long, 0);
  Write.Char(" "); Write.LongHex(long); Write.Char("H"); Write.Ln;

  long := -(SYSTEM.LSH(LONG(LONG(1)), 63) + 1);
  Write.String("MAX(LONGINT) = "); Write.LongInt(long, 0); Write.Char(" ");
  Write.LongHex(long); Write.Char("H"); Write.Ln;
  DEC(long); Write.String("MAX(LONGINT)-1 = "); Write.LongInt(long, 0);
  Write.Char(" "); Write.LongHex(long); Write.Char("H"); Write.Ln;
END TestConsole.
