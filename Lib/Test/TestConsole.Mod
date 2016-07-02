MODULE TestConsole;

IMPORT Write := Console;

VAR
  int: INTEGER; long: LONGINT;

BEGIN
  Write.LongInt(0, 0); Write.Char(" "); Write.LongHex(0); Write.Ln;
  (* INTEGER *)
  int := MIN(INTEGER);
  Write.String("MIN(INTEGER)="); Write.Int(int, 0); Write.Char(" ");
  Write.Hex(int); Write.Char(" ");
  INC(int); Write.Int(int, 0); Write.Char(" "); Write.Hex(int); Write.Ln;
  int := MAX(INTEGER);
  Write.String("MAX(INTEGER)="); Write.Int(int, 0); Write.Char(" ");
  Write.Hex(int); Write.Char(" ");
  DEC(int); Write.Int(int, 0); Write.Char(" "); Write.Hex(int); Write.Ln;
  (* LONGINT *)
  long := MIN(LONGINT);
  Write.String("MIN(LONGINT)="); Write.LongInt(long, 0); Write.Char(" ");
  Write.LongHex(long); Write.Char(" ");
  INC(long); Write.LongInt(long, 0); Write.Char(" "); Write.LongHex(long);
  Write.Ln;
  long := MAX(LONGINT);
  Write.String("MAX(LONGINT)="); Write.LongInt(long, 0); Write.Char(" ");
  Write.LongHex(long); Write.Char(" ");
  DEC(long); Write.LongInt(long, 0); Write.Char(" "); Write.LongHex(long);
  Write.Ln;
END TestConsole.
