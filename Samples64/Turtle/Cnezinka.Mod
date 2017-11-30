MODULE Cnezinka;
IMPORT t := Turtle;
VAR
  x: INTEGER; s: REAL;
BEGIN
  s := 1;
  FOR x := 1 TO 990 DO 
    t.SetColor(t.Blue); t.Forward(s); t.Right(100);
    s := s + 0.1;
    t.SetColor(t.LightBlue); t.Forward(s); t.Right(100);
    s := s + 0.1;
    t.SetColor(t.Cyan); t.Forward(s); t.Right(100);
    s := s + 0.1;
    t.SetColor(t.LightCyan); t.Forward(s); t.Right(100);
    s := s + 0.01;
    t.SetColor(t.White); t.Forward(s); t.Right(100);
    s := s + 0.01;
  END;
  t.ShowTurtle;
  t.Quit;
END Cnezinka.
