MODULE TestFileRename;

IMPORT Args, Write := Console, p := Platform;

VAR
  filename1, filename2: ARRAY 1024 OF CHAR;

BEGIN
  IF Args.argc = 3 THEN
    Args.Get(1, filename1); Args.Get(2, filename2);
    IF p.RenameFile(filename1, filename2) = 0 THEN
      Write.String("YES!");
    ELSE
      Write.String("NO!");
    END;
  ELSE
    Write.String("TestFileName name1 name2");
  END;
  Write.Ln;
END TestFileRename.
