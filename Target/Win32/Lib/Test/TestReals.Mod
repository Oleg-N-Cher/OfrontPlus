MODULE TestReals;

IMPORT SYSTEM, Write := Console, Reals, Texts, Kernel;

CONST
  asdf* = 1.000000001E+308;
  a* = 1.7976931348623E+308;
  max* = MAX(REAL); min* = MIN(REAL);
(*
  max1* = 1.7976931348623157E+308;
*)
  _2* = MAX(REAL) / 2;
  _4* = MAX(REAL) / 4;
  _8* = MAX(REAL) / 8;
  _16* = MAX(REAL) / 16;
  maxs* = MAX(SHORTREAL); mins* = MIN(SHORTREAL);
  B* = 1.5;
  C* = 0.0;

VAR r: REAL; s: SHORTREAL;

PROCEDURE Wr (x: REAL);
VAR
  W: Texts.Writer; T: Texts.Text; R: Texts.Reader; ch: SHORTCHAR;
BEGIN
  Texts.OpenWriter(W);
  Texts.WriteLongReal(W, x, 17);
  NEW(T); Texts.Open(T, ""); Texts.Append(T, W.buf);
  Texts.OpenReader(R, T, 0); Texts.Read(R, ch);
  WHILE ch # 0X DO Write.Char(ch); Texts.Read(R, ch) END;
  Write.Ln
END Wr;

PROCEDURE Wr2 (str: ARRAY OF SHORTCHAR; x: REAL);
VAR
  a: ARRAY 80 OF SHORTCHAR; n: SHORTINT;
BEGIN
  Write.String(str); Write.String(" = ");
  Reals.RealToStr(x, 17, a);
  Write.String(a); Write.Ln
END Wr2;

(*
  Wr(2.2204460492503131E-016);
  Wr(0.33287108371666779);
  Wr(244.69193226422038);
  Wr(12.24744871391589);
  Wr(0.95892427466313834);
  Wr(0.7568024953079282);
  Wr(-0.14112000805986721);
  Wr(-0.9092974268256816);
  Wr(-0.84147098480789639);
  Wr(64078097265623867.84147098480789639);
  Wr(340365272637.9092974268256816);
  Wr(45709845079.14112000805986721);
  Wr(-0.7568024953079282);
  Wr(-0.95892427466313834);
  Wr(0.28366218546322625);
  Wr(-0.65364362086361194);
  Wr(-0.9899924966004453);
  Wr(-0.41614683654714241);
  Wr(0.54030230586813965);
  Wr(0.99999999999999978);
  Wr(0.54030230586813965);
  Wr(-0.41614683654714241);
  Wr(-0.9899924966004453);
  Wr(-0.65364362086361194);
  Wr(0.28366218546322625);
*)

PROCEDURE Abc;
BEGIN
  r := asdf; (*r := a;*) r := min; r := max;
END Abc;

BEGIN
  Wr(0); Wr(1.0);
  Wr(asdf); Wr(a); Wr(min); Wr(max);
  Wr(1.79769313486231571E+38);
  Wr(1.797693134862315E+38);(*
  Wr(1.79769313486231571E+0308);*)
  Write.Ln; Write.Ln;

  Wr2("asdf", 1.000000001E+308);
  Wr2("a", 1.7976931348623E+308);
  Wr2("max", MAX(REAL));
  Wr2("_2", MAX(REAL) / 2);
  Wr2("_4", MAX(REAL) / 4);
  Wr2("_8", MAX(REAL) / 8);
  Wr2("_16", MAX(REAL) / 16);
  Wr2("123.1", 123.1);
  Wr2("MAX(SHORTREAL)", MAX(SHORTREAL));
  Wr2("MIN(SHORTREAL)", MIN(SHORTREAL));
  Write.Ln;

  (* 7f7f ffff ? 3.4028234 ? 38  (максимальное одинарной точности) *)
  (*s := 3.4028235E+38;*)
END TestReals.
