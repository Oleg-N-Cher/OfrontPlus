MODULE TestFileExists;

IMPORT Args, Write := Console, p := Platform;

VAR
  filename: ARRAY 1024 OF CHAR;

BEGIN
  IF Args.argc > 0 THEN
    Args.Get(1, filename);
    IF p.FileExists(filename) THEN
      Write.String("YES!");
    ELSE
      Write.String("NO!");
    END;
    Write.Ln;
  END;
END TestFileExists.
