MODULE TestOoc2RandomNumbers;

IMPORT Platform, Rnd := ooc2RandomNumbers, Write := Console;

VAR
  i: INTEGER;

BEGIN Platform.Delay(100);
Write.Int(SHORT(Platform.Time()), 10);
Write.Int(SHORT(Platform.Time()), 10);
Write.Int(SHORT(Platform.Time()), 10);
  Rnd.PutSeed(SHORT(Platform.Time()));
  FOR i := 1 TO 100 DO Write.Int(Rnd.RND(10), 2) END;
  Write.Ln;
END TestOoc2RandomNumbers.
