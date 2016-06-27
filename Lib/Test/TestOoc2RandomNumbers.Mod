MODULE TestOoc2RandomNumbers;

IMPORT Kernel, Rnd := ooc2RandomNumbers, Write := Console;

VAR
  i: INTEGER;

BEGIN
  Rnd.PutSeed(SHORT(Kernel.Time()));
  FOR i := 1 TO 100 DO Write.Int(Rnd.RND(10), 2) END;
  Write.Ln;
END TestOoc2RandomNumbers.
