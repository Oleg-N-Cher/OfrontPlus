(*
    (c) Destr
*)

MODULE Elo4ka; (** portable *)
IMPORT G := Graph, Math;

CONST
  Xoff = 256 DIV 2; Yoff = 192-48; R = 64;

TYPE
  Color = INTEGER;

VAR
  screen: G.Bitmap;
  a, b, alpha: INTEGER; angle: SHORTREAL; xn, yn: INTEGER;
  color: Color;

PROCEDURE NextColor(): Color;
VAR
  res: Color;
BEGIN
  CASE color OF
  | 0: res := G.MakeCol(0, 255, 0); (* LightGreen *)
  | 1: res := G.MakeCol(255, 0, 0); (* LightRed   *)
  | 2: res := G.MakeCol(0, 0, 255); (* LightBlue  *)
  | 3: res := G.MakeCol(170, 170, 170);  (* Gray  *)
  END;
  color := (color + 1) MOD 4;
  RETURN res
END NextColor;

BEGIN
  G.Settings(256, 192, {G.fullscreen, G.sharpPixels});
  screen := G.Init();
  alpha := 0;
  color := 0;
  REPEAT
    G.ClearScreen;
    FOR b := 0 TO 64 DO
      FOR a := 0 TO 255 BY 8 DO
        angle := SHORT(Math.pi/128)*a+alpha+b;
        yn := SHORT(ENTIER(Math.cos(angle)*(R-b)));
        IF yn > 0 THEN
          xn := SHORT(ENTIER(Math.sin(angle)*(R-b)));
          G.PutPixel(screen, Xoff+xn, Yoff+yn DIV 2-(b*2), NextColor());
        END;
      END(*a*);
    END(*b*);
    alpha := (alpha + 1) MOD 256;
    G.Flip; G.Delay(60);
  UNTIL G.KeyPressed();
  G.Close
END Elo4ka.
