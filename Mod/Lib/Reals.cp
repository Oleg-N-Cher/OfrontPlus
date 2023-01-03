MODULE Reals;
  (* JT, 5.2.90 / RC 9.12.91 conversion between reals and strings for HP-700, MB 9.12.91, JT for Ofront, 16.3. 95*)

  IMPORT S := SYSTEM, bbStrings;

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
  
  PROCEDURE TenL* (n: SHORTINT): REAL; (*compute 10^n *)
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

  PROCEDURE RealToStr* (x: REAL; digits: INTEGER; VAR d: ARRAY OF SHORTCHAR);
    VAR buf: ARRAY 32 OF CHAR;
  BEGIN
    bbStrings.RealToStringForm(x, digits, 0, 0, " ", buf);
    d := SHORT(buf$)
  END RealToStr;

END Reals.
