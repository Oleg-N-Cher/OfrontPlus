MODULE Reals;
  (* JT, 5.2.90 / RC 9.12.91 conversion between reals and strings for HP-700, MB 9.12.91, JT for Ofront, 16.3. 95*)

  IMPORT S := SYSTEM;

  TYPE
    SHORTINT = S.INT8; INTEGER = S.INT16; LONGINT = S.INT32; ADRINT = S.ADRINT;
    CHAR = S.CHAR8; REAL = S.REAL32; LONGREAL = S.REAL64;


  PROCEDURE -Offset(adr: ADRINT; offset: LONGINT): ADRINT
      "((SYSTEM_ADRINT)adr + offset)";
  PROCEDURE -AAincludeStdio "#include <stdio.h>"; (* sprintf() *)
  PROCEDURE -sprintf (buf, format: ADRINT; x: LONGREAL): LONGINT
      "sprintf ((char*)buf, (char*)format, x)";
  PROCEDURE -sprintd (buf, format: ADRINT; x: LONGINT): LONGINT
      "sprintf ((char*)buf, (char*)format, x)";

  PROCEDURE Ten*(e: INTEGER): REAL;
    VAR r, power: LONGREAL;
  BEGIN r := 1.0;
    power := 10.0;
    WHILE e > 0 DO
      IF ODD(e) THEN r := r * power END;
      power := power * power; e := SHORT(e DIV 2)
    END;
    RETURN SHORT(r)
  END Ten;
  
  PROCEDURE TenL* (n: INTEGER): LONGREAL; (*compute 10^n *)
    VAR r, power: LONGREAL;
  BEGIN r := 1.0; power := 10.0;
    WHILE n > 0 DO
      IF ODD(n) THEN r := r * power END;
      power := power * power; n := SHORT(n DIV 2)
    END;
    RETURN r
  END TenL;
  
  PROCEDURE Expo*(x: REAL): INTEGER;
  BEGIN
    RETURN SHORT(ASH(S.VAL(LONGINT, x), -23) MOD 256)
  END Expo;
  
  PROCEDURE ExpoL* (x: LONGREAL): INTEGER;
    CONST ExpoMax = 1023;
    VAR exp, offset: LONGINT;
  BEGIN
    x := ABS(x);
    exp := SHORT(ASH(S.VAL(S.INT64, x), -52));
    offset := ExpoMax;
    WHILE exp = 0 DO
      x := x + x; exp := SHORT(ASH(S.VAL(S.INT64, x), -52)); INC(offset)
    END;
    RETURN SHORT(exp - offset)
  END ExpoL;

  PROCEDURE SetExpo*(e: INTEGER; VAR x: REAL);
    CONST expo = {1..8};
  BEGIN
    x := S.VAL(REAL, S.VAL(SET, x) - expo + S.VAL(SET, ASH(LONG(e), 23)))
  END SetExpo;
  
  PROCEDURE SetExpoL*(e: INTEGER; VAR x: LONGREAL);
    CONST expo = {1..11};
    VAR h: SET;
  BEGIN
    S.GET(S.ADR(x)+4, h);
    h := h - expo + S.VAL(SET, ASH(LONG(e), 20));
    S.PUT(S.ADR(x)+4, h)
  END SetExpoL;
  
  PROCEDURE Convert*(x: REAL; n: INTEGER; VAR d: ARRAY OF CHAR);
    VAR i, k: LONGINT;
  BEGIN
    i := SHORT(ENTIER(x)); k := 0;
    WHILE k < n DO
      d[k] := CHR(i MOD 10 + 48); i := i DIV 10; INC(k)
    END
  END Convert;
  
  PROCEDURE Unpack(VAR b, d: ARRAY OF BYTE);
    VAR i, k: SHORTINT; len: LONGINT;
  BEGIN i := 0; len := LEN(b);
    WHILE i < len DO
      k := SHORT(SHORT(ORD(S.VAL(CHAR, b[i])) DIV 16));
      IF k > 9 THEN d[i*2] := SHORT(SHORT(k + 55)) ELSE d[i*2] := SHORT(SHORT(k + 48)) END;
      k := SHORT(SHORT(ORD(S.VAL(CHAR, b[i])) MOD 16));
      IF k > 9 THEN d[i*2+1] := SHORT(SHORT(k + 55)) ELSE d[i*2+1] := SHORT(SHORT(k + 48)) END;
      INC(i)
    END
  END Unpack;
  
  PROCEDURE ConvertH* (y: REAL; VAR d: ARRAY OF CHAR);
  BEGIN Unpack(y, d)
  END ConvertH;
  
  PROCEDURE ConvertHL* (x: LONGREAL; VAR d: ARRAY OF CHAR);
  BEGIN Unpack(x, d)
  END ConvertHL;

  PROCEDURE RealToStr* (x: LONGREAL; digits: LONGINT; VAR d: ARRAY OF CHAR);
    VAR buf: ARRAY 28 OF CHAR; format: ARRAY 8 OF CHAR;
  BEGIN
    format[0] := "%"; format[1] := ".";
    ASSERT((0 < digits) & (digits <= 17));
    ASSERT(sprintd(S.ADR(format)+2, S.ADR("%dG"), digits) < LEN(format)-2);
    ASSERT(sprintf(S.ADR(buf), S.ADR(format), x) < LEN(buf));
    d := buf$
  END RealToStr;

END Reals.
