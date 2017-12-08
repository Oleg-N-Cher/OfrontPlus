MODULE Turtle; (* Turtle Graphic as in LOGO *)
IMPORT G := Graph, Math;

CONST
  ScreenWidth  = 800;
  ScreenHeight = 600;

  Penup   = FALSE;
  Pendown = TRUE;

TYPE
  Color = INTEGER;

VAR
  screen: G.Bitmap; x-, y-, angle: REAL; color: Color;
  penstatus: BOOLEAN; (* TRUE is on, FALSE is off *)
  Black-, Blue-, Green-, Cyan-, Red-, Magenta-, Brown-, LightGray-,
  DarkGray-, LightBlue-, LightGreen-, LightCyan-,
  LightRed-, LightMagenta-, Yellow-, White-, Orange-: Color;

PROCEDURE Home*;
BEGIN
  x := ScreenWidth DIV 2; y := ScreenHeight DIV 2; angle := 0;
END Home;

PROCEDURE Round (x: REAL): INTEGER;
BEGIN
  RETURN SHORT(ENTIER(x + 0.5))
END Round;

PROCEDURE Forward* (distance: REAL);
VAR
  newx, newy: REAL;
BEGIN
  newx := x + distance*Math.cos(angle * Math.pi / 180);
  newy := y + distance*Math.sin(angle * Math.pi / 180);
  IF penstatus = Pendown THEN
    G.Line(screen, Round(x), Round(y), Round(newx), Round(newy), color);
  END;
  x := newx; y := newy;
END Forward;

PROCEDURE Back* (distance: REAL);
BEGIN
  Forward(-distance);
END Back;

PROCEDURE Left* (a: REAL);
BEGIN
  angle := angle - a;
END Left;

PROCEDURE Right* (a: REAL);
BEGIN
  Left(-a);
END Right;

PROCEDURE PenDown*;
BEGIN
  penstatus := Pendown
END PenDown;

PROCEDURE PenUp*;
BEGIN
  penstatus := Penup
END PenUp;

PROCEDURE SetColor* (col: Color);
BEGIN
  color := col
END SetColor;

PROCEDURE Triangle (s: REAL);
CONST
  Bd = 110;
BEGIN
  Left(90); Forward(s / 3); Right(Bd);
  Forward(s); Right(360-Bd-Bd); Forward(s); Right(Bd);
  Forward(s / 3); Right(90);
END Triangle;

PROCEDURE ShowTurtle*;
VAR
  x: INTEGER; oldpenstatus: BOOLEAN;
BEGIN
  oldpenstatus := penstatus;
  PenDown;
  Triangle(30);
  PenUp;
  Left(3);
  Forward(17); PenDown;
  FOR x := 1 TO 10 DO Triangle(x) END;
  Right(3);
  penstatus := oldpenstatus;
END ShowTurtle;

PROCEDURE Quit*;
BEGIN
  G.Flip; G.Pause; G.Close
END Quit;

BEGIN
  G.Settings(ScreenWidth, ScreenHeight, {G.spread});
  screen := G.Init();
  G.SetWindowTitle("Turtle Demo");
  Home; penstatus := Pendown;

  Black        := G.MakeCol(  0,  0,  0);
  Blue         := G.MakeCol(  0,  0,128);
  Green        := G.MakeCol(  0,128,  0);
  Cyan         := G.MakeCol(  0,128,128);
  Red          := G.MakeCol(128,  0,  0);
  Magenta      := G.MakeCol(128,  0,128);
  Brown        := G.MakeCol(128,128,  0);
  LightGray    := G.MakeCol(192,192,192);
  DarkGray     := G.MakeCol(128,128,128);
  LightBlue    := G.MakeCol(  0,  0,255);
  LightGreen   := G.MakeCol(  0,255,  0);
  LightCyan    := G.MakeCol(  0,255,255);
  LightRed     := G.MakeCol(255,  0,  0);
  LightMagenta := G.MakeCol(255,  0,255);
  Yellow       := G.MakeCol(255,255,  0);
  White        := G.MakeCol(255,255,255);
  Orange       := G.MakeCol(255,128,  0);
END Turtle.
