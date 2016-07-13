MODULE TestASH;
CONST Ash* = ASH(-1, 30);
VAR
  b: BYTE; s: SHORTINT; i: INTEGER; l: LONGINT;

BEGIN

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

END TestASH.
