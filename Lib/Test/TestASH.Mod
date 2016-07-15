MODULE TestASH; IMPORT SYSTEM;
CONST Ash* = ASH(-1, 30);
VAR
  b: BYTE; s: SHORTINT; i: INTEGER; l: LONGINT;

BEGIN

  s := SHORT(s+i);

  s := SHORT(ASH(s, 2));
  i := ASH(s, 2);
  l := ASH(s, 2);
  i := ASH(i, 2);
  l := ASH(i, 2);
  l := ASH(l, 2);
  (* i := ASH(l, 2); *)
  s := SHORT(ASH(1, b));
  i := ASH(1, b);
  l := ASH(1, b);
  i := ASH(1, b);
  l := ASH(1, b);
  l := ASH(1, b);

  i := SHORT(ASH(LONG(LONG(1)), b));
  l := ASH(LONG(LONG(1)), b);
  l := ASH(LONG(LONG(1)), b);
  l := ASH(LONG(LONG(1)), b);
  l := ASH(LONG(LONG(1)), b);
  l := ASH(LONG(LONG(1)), b);

  s := SHORT(ASH(s, 2)); s := SHORT(ASH(s, -2));
  s := SYSTEM.LSH(s, 2); s := SYSTEM.LSH(s, -2);

  s := s*2; s := SHORT(LONG(s)*2); s := SHORT(SHORT(LONG(LONG(s))*2));
  i := s*2; i := LONG(s)*2;
  i := s*2; i := s DIV 2;
  i := i*2; i := i DIV 2;
  l := s*2; l := l DIV 2;

  i := ASH(1, b);
  l := ASH(LONG(LONG(1)), b);
  i := ASH(b, 30);
  l := ASH(LONG(LONG(s)), 30);
  i := ASH(MAX(INTEGER), b);
  i := ASH(b, b);

  i := ASH(1, s);
  i := ASH(s, 30);
  i := ASH(MAX(INTEGER), s);
  i := ASH(s, s);

  i := ASH(1, i);
  i := ASH(i, 30);
  i := ASH(MAX(INTEGER), i);
  i := ASH(i, i);

  i := ASH(1, SHORT(l));
  l := ASH(l, 30);
  i := ASH(MAX(INTEGER), SHORT(l));
  l := ASH(l, SHORT(l));

  l := ASH(1, b);
  l := ASH(b, 30);
  l := ASH(MAX(LONGINT), b);
  l := ASH(b, b);

  l := ASH(1, s);
  l := ASH(s, 30);
  l := ASH(MAX(LONGINT), s);
  l := ASH(s, s);

  l := ASH(1, i);
  l := ASH(i, 30);
  l := ASH(MAX(LONGINT), i);
  l := ASH(i, i);

  l := ASH(1, SHORT(l));
  l := ASH(l, 30);
  l := ASH(MAX(LONGINT), SHORT(l));
  l := ASH(l, SHORT(l));

  i := ASH(i, 30); i := ASH(i, -30);
  i := ASH(800000H, 7);
  i := ASH(400000H, 8);
(*
  l := ASH(l, 63); l := ASH(l, -63);
*)
END TestASH.
