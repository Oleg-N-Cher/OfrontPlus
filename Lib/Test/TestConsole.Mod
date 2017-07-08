MODULE TestConsole;

IMPORT SYSTEM, Write := Console, p := Platform;

VAR
  byte: BYTE; short: SHORTINT; int: INTEGER; long: LONGINT;

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
  long := SYSTEM.LSH(LONG(LONG(LONG(1))), 63);
  Write.String("MIN(LONGINT) = "); Write.LongInt(long, 0); Write.Char(" ");
  Write.LongHex(long); Write.Char("H"); Write.Ln;
  INC(long); Write.String("MIN(LONGINT)+1 = "); Write.LongInt(long, 0);
  Write.Char(" "); Write.LongHex(long); Write.Char("H"); Write.Ln;

  long := -(SYSTEM.LSH(LONG(LONG(LONG(1))), 63) + 1);
  Write.String("MAX(LONGINT) = "); Write.LongInt(long, 0); Write.Char(" ");
  Write.LongHex(long); Write.Char("H"); Write.Ln;
  DEC(long); Write.String("MAX(LONGINT)-1 = "); Write.LongInt(long, 0);
  Write.Char(" "); Write.LongHex(long); Write.Char("H"); Write.Ln;

  Write.String("Byte: ");
  byte := 127; Write.Int(p.UBYTE(byte), 4); (*Write.Int(p.USHORT(byte), 4); Write.Int(p.UINT(byte), 4);*)
  byte := -128;; Write.Int(p.UBYTE(byte), 4); (*Write.Int(p.USHORT(byte), 4); Write.Int(p.UINT(byte), 4);*)
  byte := -1; Write.Int(p.UBYTE(byte), 4); (*Write.Int(p.USHORT(byte), 4); Write.Int(p.UINT(byte), 4);*)
  Write.Ln;
  
  Write.String("Short: ");
  short := 32767; Write.Int(p.UBYTE(SHORT(short)), 3); Write.Int(p.USHORT(short), 6); (*Write.LongInt(p.UINT(short), 7);*)
  INC(short); Write.Int(p.UBYTE(SHORT(short)), 2); Write.Int(p.USHORT(short), 6); (*Write.LongInt(p.UINT(short), 11);*)
  short := -1; Write.Int(p.UBYTE(SHORT(short)), 4); Write.Int(p.USHORT(short), 6); (*Write.LongInt(p.UINT(short), 6);*)
  Write.Ln;
  
  Write.String("Int: ");
  int := MAX(INTEGER); Write.Int(p.UBYTE(SHORT(SHORT(int))), 5); Write.Int(p.USHORT(SHORT(int)), 6); Write.LongInt(p.UINT(int), 11);
  INC(int); Write.Int(p.UBYTE(SHORT(SHORT(int))), 2); Write.Int(p.USHORT(SHORT(int)), 2); Write.LongInt(p.UINT(int), 11);
  int := -1; Write.Int(p.UBYTE(SHORT(SHORT(int))), 4); Write.Int(p.USHORT(SHORT(int)), 6); Write.LongInt(p.UINT(int), 11);
  Write.Ln;

END TestConsole.
