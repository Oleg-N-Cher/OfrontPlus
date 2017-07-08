MODULE TestFileSize;

IMPORT Args, Write := Console, p := Platform;

VAR
  filename: ARRAY 1024 OF CHAR; file: p.FileHandle; filesize: LONGINT;

BEGIN
  IF Args.argc > 0 THEN
    Args.Get(1, filename);
    IF p.OldRO(filename, file) = 0 THEN
      IF p.FileSize(file, filesize) = 0 THEN
        Write.String("file size = "); Write.LongInt(filesize, 0);
      ELSE
        Write.String("error calculating file size");
      END;
      IF p.CloseFile(file) = 0 THEN END;
    ELSE
      Write.String("error opening file");
    END;
    Write.Ln;
  END;
END TestFileSize.
