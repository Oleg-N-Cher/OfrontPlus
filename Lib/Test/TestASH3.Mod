MODULE TestASH3;

IMPORT SYSTEM, Write := Console;

VAR
  s: SHORTINT; i: INTEGER;

BEGIN
  s := 32767;
  REPEAT
    i := s*2;
    s := s*2;
    Write.Int(s, 0); Write.Char(":"); Write.Int(i, 0); Write.Char(" ");
    Write.Hex(s); Write.Char(":"); Write.Hex(i); Write.Ln;
  UNTIL i = 0;

  Write.Ln;
END TestASH3.
