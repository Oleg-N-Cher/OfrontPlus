MODULE Gradient24;
IMPORT G := Graph, Out;
VAR screen: G.Bitmap;
    y, b: INTEGER;
BEGIN
  screen := G.Init();
  IF screen = NIL THEN Out.String("Couldn't init graphic"); Out.Ln; HALT(1) END;
  y := 0;
  REPEAT
    b := 255 - 255 * y DIV screen.h;
    G.Line(screen, 0, y, screen.w - 1, y, G.MakeCol(b, 0, b));
    INC(y)
  UNTIL b = 0;
  G.Flip;
  G.Pause;
  G.Close
END Gradient24.