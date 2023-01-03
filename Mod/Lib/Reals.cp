MODULE Reals;
  (* JT, 5.2.90 / RC 9.12.91 conversion between reals and strings for HP-700, MB 9.12.91, JT for Ofront, 16.3. 95*)

  IMPORT S := SYSTEM, MathL;

  VAR
    maxExp, maxDig: INTEGER;
    factor: REAL; (* 10^maxDig *)

  PROCEDURE Ten* (e: SHORTINT): SHORTREAL;
    VAR r, power: REAL;
  BEGIN r := 1.0;
    power := 10.0;
    WHILE e > 0 DO
      IF ODD(e) THEN r := r * power END;
      power := power * power; e := SHORT(e DIV 2)
    END;
    RETURN SHORT(r)
  END Ten;

  PROCEDURE TenL* (n: INTEGER): REAL; (*compute 10^n *)
    VAR r, power: REAL;
  BEGIN r := 1.0; power := 10.0;
    WHILE n > 0 DO
      IF ODD(n) THEN r := r * power END;
      power := power * power; n := SHORT(n DIV 2)
    END;
    RETURN r
  END TenL;

  PROCEDURE Expo* (x: SHORTREAL): SHORTINT;
  BEGIN
    RETURN SHORT(ASH(S.VAL(INTEGER, x), -23) MOD 256)
  END Expo;

  PROCEDURE ExpoL* (x: REAL): SHORTINT;
    CONST ExpoMax = 1023;
    VAR exp, offset: INTEGER;
  BEGIN
    x := ABS(x);
    exp := SHORT(ASH(S.VAL(S.INT64, x), -52));
    offset := ExpoMax;
    WHILE exp = 0 DO
      x := x + x; exp := SHORT(ASH(S.VAL(S.INT64, x), -52)); INC(offset)
    END;
    RETURN SHORT(exp - offset)
  END ExpoL;

  PROCEDURE SetExpo* (e: SHORTINT; VAR x: SHORTREAL);
    CONST expo = {1..8};
  BEGIN
    x := S.VAL(SHORTREAL, S.VAL(SET, x) - expo + S.VAL(SET, ASH(LONG(e), 23)))
  END SetExpo;

  PROCEDURE SetExpoL* (e: SHORTINT; VAR x: REAL);
    CONST expo = {1..11};
    VAR h: SET;
  BEGIN
    S.GET(S.ADR(x)+4, h);
    h := h - expo + S.VAL(SET, ASH(LONG(e), 20));
    S.PUT(S.ADR(x)+4, h)
  END SetExpoL;

  PROCEDURE Convert* (x: SHORTREAL; n: SHORTINT; VAR d: ARRAY OF SHORTCHAR);
    VAR i, k: INTEGER;
  BEGIN
    i := SHORT(ENTIER(x)); k := 0;
    WHILE k < n DO
      d[k] := SHORT(CHR(i MOD 10 + 48)); i := i DIV 10; INC(k)
    END
  END Convert;

  PROCEDURE Unpack (VAR b, d: ARRAY OF BYTE);
    VAR i, k: BYTE; len: INTEGER;
  BEGIN i := 0; len := LEN(b);
    WHILE i < len DO
      k := SHORT(SHORT(ORD(S.VAL(SHORTCHAR, b[i])) DIV 16));
      IF k > 9 THEN d[i*2] := SHORT(SHORT(k + 55)) ELSE d[i*2] := SHORT(SHORT(k + 48)) END;
      k := SHORT(SHORT(ORD(S.VAL(SHORTCHAR, b[i])) MOD 16));
      IF k > 9 THEN d[i*2+1] := SHORT(SHORT(k + 55)) ELSE d[i*2+1] := SHORT(SHORT(k + 48)) END;
      INC(i)
    END
  END Unpack;

  PROCEDURE ConvertH* (y: SHORTREAL; VAR d: ARRAY OF SHORTCHAR);
  BEGIN
    Unpack(
      S.THISARR(S.ADR(y), SIZE(SHORTREAL)),
      S.THISARR(S.ADR(d), LEN(d)))
  END ConvertH;

  PROCEDURE ConvertHL* (x: REAL; VAR d: ARRAY OF SHORTCHAR);
  BEGIN
    Unpack(
      S.THISARR(S.ADR(x), SIZE(REAL)),
      S.THISARR(S.ADR(d), LEN(d)))
  END ConvertHL;

  PROCEDURE RealToStringForm* (x: REAL; precision, minW, expW: INTEGER; fillCh: SHORTCHAR;
                            OUT s: ARRAY OF SHORTCHAR);
    VAR exp, len, i, j, n, k, p: INTEGER; m: ARRAY 80 OF SHORTCHAR; neg: BOOLEAN;
  BEGIN
    ASSERT((precision > 0) (*& (precision <= 18)*), 20);
    ASSERT((minW >= 0) & (minW < LEN(s)), 21);
    ASSERT((expW > -LEN(s)) & (expW <= 3), 22);
    exp := MathL.exponent(x);
    IF exp = MAX(INTEGER) THEN
      IF fillCh = "0" THEN fillCh := " " END;
      x := MathL.fraction(x);
      IF x = -1 THEN m := "-INF"; n := 4
      ELSIF x = 1 THEN m := "INF"; n := 3
      ELSE m := "NaN"; n := 3
      END;
      i := 0; j := 0;
      WHILE minW > n DO s[i] := fillCh; INC(i); DEC(minW) END;
      WHILE (j <= n) & (i < LEN(s)) DO s[i] := m[j]; INC(i); INC(j) END
    ELSE
      neg := FALSE; len := 1; m := "00";
      IF x < 0 THEN x := -x; neg := TRUE; DEC(minW) END;
      IF x # 0 THEN
        exp := (exp - 8) * 30103 DIV 100000;  (* * log(2) *)
        IF exp > 0 THEN
          n := SHORT(ENTIER(x / TenL(exp)));
          x := x / TenL(exp) - n
        ELSIF exp > -maxExp THEN
          n := SHORT(ENTIER(x * TenL(-exp)));
          x := x * TenL(-exp) - n
        ELSE
          n := SHORT(ENTIER(x * TenL(-exp - 2 * maxDig) * factor * factor));
          x := x * TenL(-exp - 2 * maxDig) * factor * factor - n
        END;
        (* x0 = (n + x) * 10^exp, 200 < n < 5000 *)
        p := precision - 4;
        IF n < 1000 THEN INC(p) END;
        IF (expW < 0) & (p > exp - expW) THEN p := exp - expW END;
        IF p >= 0 THEN
          x := x + 0.5 / TenL(p);  (* rounding correction *)
          IF x >= 1 THEN INC(n); x := x - 1 END
        ELSIF p = -1 THEN INC(n, 5)
        ELSIF p = -2 THEN INC(n, 50)
        ELSIF p = -3 THEN INC(n, 500)
        END;
        i := 0; k := 1000; INC(exp, 3);
        IF n < 1000 THEN k := 100; DEC(exp) END;
        WHILE (i < precision) & ((k > 0) OR (x # 0)) DO
          IF k > 0 THEN p := n DIV k; n := n MOD k; k := k DIV 10
          ELSE x := x * 10; p := SHORT(ENTIER(x)); x := x - p
          END;
          m[i] := SHORT(CHR(p + ORD("0"))); INC(i);
          IF p # 0 THEN len := i END
        END
      END;
      (* x0 = m[0].m[1]...m[len-1] * 10^exp *)
      i := 0;
      IF (expW < 0) OR (expW = 0) & (exp >= -3) & (exp <= len + 1) THEN
        n := exp + 1; k := len - n;
        IF n < 1 THEN n := 1 END;
        IF expW < 0 THEN k := -expW ELSIF k < 1 THEN k := 1 END;
        j := minW - n - k - 1; p := -exp;
        IF neg & (p >= MAX(0, n) + MAX(0, k)) THEN neg := FALSE; INC(j) END
      ELSE
        IF ABS(exp) >= 100 THEN expW := 3
        ELSIF (expW < 2) & (ABS(exp) >= 10) THEN expW := 2
        ELSIF expW < 1 THEN expW := 1
        END;
        IF len < 2 THEN len := 2 END;
        j := minW - len - 3 - expW; k := len;
        IF j > 0 THEN
          k := k + j; j := 0;
          IF k > precision THEN j := k - precision; k := precision END
        END;
        n := 1; DEC(k); p := 0
      END;
      IF neg & (fillCh = "0") THEN s[i] := "-"; INC(i); neg := FALSE END;
      WHILE j > 0 DO s[i] := fillCh; INC(i); DEC(j) END;
      IF neg & (i < LEN(s)) THEN s[i] := "-"; INC(i) END;
      j := 0;
      WHILE (n > 0) & (i < LEN(s)) DO
        IF (p <= 0) & (j < len) THEN s[i] := m[j]; INC(j) ELSE s[i] := "0" END;
        INC(i); DEC(n); DEC(p)
      END;
      IF i < LEN(s) THEN s[i] := "."; INC(i) END;
      WHILE (k > 0) & (i < LEN(s)) DO
        IF (p <= 0) & (j < len) THEN s[i] := m[j]; INC(j) ELSE s[i] := "0" END;
        INC(i); DEC(k); DEC(p)
      END;
      IF expW > 0 THEN
        IF i < LEN(s) THEN s[i] := "E"; INC(i) END;
        IF i < LEN(s) THEN
          IF exp < 0 THEN s[i] := "-"; exp := -exp ELSE s[i] := "+" END;
          INC(i)
        END;
        IF (expW = 3) & (i < LEN(s)) THEN s[i] := SHORT(CHR(exp DIV 100 + ORD("0"))); INC(i) END;
        IF (expW >= 2) & (i < LEN(s)) THEN s[i] := SHORT(CHR(exp DIV 10 MOD 10 + ORD("0"))); INC(i) END;
        IF i < LEN(s) THEN s[i] := SHORT(CHR(exp MOD 10 + ORD("0"))); INC(i) END
      END
    END;
    IF i < LEN(s) THEN s[i] := 0X ELSE HALT(23) END
  END RealToStringForm;

  PROCEDURE RealToStr* (x: REAL; digits: INTEGER; OUT d: ARRAY OF SHORTCHAR);
  BEGIN
    RealToStringForm(x, digits, 0, 0, " ", d)
  END RealToStr;

BEGIN
  maxExp := SHORT(ENTIER(MathL.log(MAX(REAL), 10))) + 1;
  maxDig := SHORT(ENTIER(-MathL.log(MathL.ulp(1.0E0), 10)));
  factor := TenL(maxDig)
END Reals.
