MODULE TestASH2;

IMPORT SYSTEM, Write := Console;

VAR
  int: INTEGER; long: LONGINT;

BEGIN
  int := ASH(1, 4); Write.String("ASH(INTEGER, 4): ");
  REPEAT
    int := ASH(int, 4); IF int # 0 THEN Write.Hex(int); Write.Char(" ") END;
  UNTIL int = 0;
  Write.Ln;

  int := 4; Write.String("ASH(1, INTEGER): ");
  REPEAT
    Write.Hex(ASH(1, int)); Write.Char(" "); INC(int, 4);
  UNTIL ASH(1, int) = 1;
  Write.Ln;
(*
  long := ASH(1, 4); Write.String("ASH(LONGINT): ");
  REPEAT
    long := ASH(long, 4); IF long # 0 THEN Write.LongHex(long); Write.Char(" ") END;
  UNTIL long = 0;
  Write.Ln;
*)
  int := 4; Write.String("ASH(LONG(LONG(1)), INTEGER): ");
  REPEAT
    Write.LongHex(ASH(LONG(LONG(1)), int)); Write.Char(" "); INC(int, 4);
  UNTIL ASH(LONG(LONG(1)), int) = 1;
  Write.Ln; Write.Ln;

  int := SYSTEM.LSH(1, 4); Write.String("SYSTEM.LSH(INTEGER): ");
  REPEAT
    int := SYSTEM.LSH(int, 4); IF int # 0 THEN Write.Hex(int); Write.Char(" ") END;
  UNTIL int = 0;
  Write.Ln;

  int := 4; Write.String("SYSTEM.LSH(1, INTEGER): ");
  REPEAT
    Write.Hex(SYSTEM.LSH(1, int)); Write.Char(" "); INC(int, 4);
  UNTIL SYSTEM.LSH(1, int) = 1;
  Write.Ln;

  long := SYSTEM.LSH(1, 4); Write.String("SYSTEM.LSH(LONGINT): ");
  REPEAT
    long := SYSTEM.LSH(long, 4); IF long # 0 THEN Write.LongHex(long); Write.Char(" ") END;
  UNTIL long = 0;
  Write.Ln;

  int := 4; Write.String("SYSTEM.LSH(LONG(LONG(1)), INTEGER): ");
  REPEAT
    Write.LongHex(SYSTEM.LSH(LONG(LONG(1)), int)); Write.Char(" "); INC(int, 4);
  UNTIL SYSTEM.LSH(LONG(LONG(1)), int) = 1;
  Write.Ln;
  
END TestASH2.
