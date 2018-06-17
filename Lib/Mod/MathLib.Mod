(* ----------------------------------------------------------------------------
 * $Id: MathLib.mi,v 1.4 1993/03/22 10:17:07 kredel Exp $
 * ----------------------------------------------------------------------------
 * Copyright (c) GMD
 * ----------------------------------------------------------------------------
 * $Log: MathLib.mi,v $
 * Revision 1.4  1993/03/22  10:17:07  kredel
 * This file is not part of MAS
 *
 * Revision 1.3  1992/10/15  16:28:15  kredel
 * Changed rcsid variable
 *
 * Revision 1.2  1992/02/12  13:19:06  pesch
 * Moved CONST Definition to the right place.
 *
 * Revision 1.1  1992/01/22  15:08:34  kredel
 * Initial revision
 *
 * ----------------------------------------------------------------------------
 * http://krum.rz.uni-mannheim.de/mas/src/masarith/MathLib.mi.html 
 * ----------------------------------------------------------------------------
 *)
IMPLEMENTATION MODULE MathLib;

(* GMD Mocka MathLib. *)


  CONST
    log2     =  0.693147180559945309E0;  (* ln (2.0) *)
    pi       = 3.1415926535897932384626434;
    twoopi   = 2.0 / pi;
    pio2     =   pi / 2.0;
    pio4     =   pi / 4.0;
    sqrt2    =  1.4142135623730950488016887;
    MaxCardI = 65535;
    MaxCardR = 65535.0;
    
CONST rcsidi = "$Id: MathLib.mi,v 1.4 1993/03/22 10:17:07 kredel Exp $";
CONST copyrighti = "Copyright (c) GMD";



  PROCEDURE Trunc (x : LONGREAL) : INTEGER;
  (* software emulation of TRUNC for LONGREAL, x >= 0.0 *)
    VAR res, exp, i, j : INTEGER; xS : REAL; xL : LONGREAL; c : CARDINAL;
  BEGIN
    exp := 0;
    WHILE x >= MaxCardR DO
      x := x / MaxCardR; INC (exp);
    END;
    xS := x;
    res := TRUNC (xS);
    FOR i := 1 TO exp DO
      xS := x;
      xL := FLOAT ( TRUNC (xS));
      x := (x - xL) * MaxCardR;
      xS := x;
      j := TRUNC (xS);
      res := MaxCardI * res + j;
    END;
    RETURN res;
  END Trunc;

  PROCEDURE Float (x : INTEGER) : LONGREAL;
  (* software emulation of FLOAT for LONGREAL, x >= 0.0 *)
    VAR divC, modC : CARDINAL;
        divR, modR : LONGREAL;
  BEGIN
    divC := x DIV MaxCardI; divR := FLOAT (divC);
    modC := x MOD MaxCardI; modR := FLOAT (modC);
    RETURN divR * MaxCardR + modR;
  END Float;

  PROCEDURE frexp (x: LONGREAL; VAR exp: INTEGER): LONGREAL;
  (* Returns the real mantissa m of 'x' and an integer exp *)
  (* such that 'x' = m * 2 ** exp *)
    VAR neg : BOOLEAN;
  BEGIN (* frexp *)
    exp := 0;
    neg := x < 0.0;
    IF neg THEN x := -x; END;
    IF x >= 1.0 THEN
      REPEAT
        INC (exp); x := x / 2.0;
      UNTIL x < 1.0;
    ELSIF (x < 0.1) & (x # 0.0) THEN
      REPEAT
        DEC (exp); x := x * 2.0;
      UNTIL x >= 0.1;
    END;
    IF neg THEN x := -x; END;
    RETURN x
  END frexp;

  PROCEDURE ldexp (value: LONGREAL; exp: INTEGER): LONGREAL;
  (* Returns value * 2 ** exp *)
    VAR i: INTEGER;
  BEGIN
    IF exp > 0 THEN
      FOR i := 1 TO  exp DO value := value * 2.0 END;
    ELSIF exp < 0 THEN
      FOR i := 1 TO -exp DO value := value / 2.0 END;
    END;
    RETURN value;
  END ldexp;

  PROCEDURE modf (value: LONGREAL; VAR int: INTEGER): LONGREAL;
  (* Returns the positive fractional part of value and (by int) *)
  (* the integer part *)
    VAR neg: BOOLEAN; 
  BEGIN
    neg := value < 0.0;
    IF neg THEN value := -value END;
    int := Trunc (value);
    value := value - Float (int);
    IF neg THEN int := -int END;
    RETURN value;
  END modf;

  PROCEDURE sarctan (arg: LONGREAL): LONGREAL;
  (* Reduces its positive argument to the range {0 .. 0.414} and calls xatan *)

    PROCEDURE xatan (arg: LONGREAL): LONGREAL;
      CONST
	p4 = 0.161536412982230228262E2;
	p3 = 0.26842548195503973794141E3;
	p2 = 0.11530293515404850115428136E4;
	p1 = 0.178040631643319697105464587E4;
	p0 = 0.89678597403663861959987488E3;
	q4 = 0.5895697050844462222791E2;
	q3 = 0.536265374031215315104235E3;
	q2 = 0.16667838148816337184521798E4;
	q1 = 0.207933497444540981287275926E4;
	q0 = 0.89678597403663861962481162E3;
      VAR argsq: LONGREAL;
    BEGIN (* xatan *)
      argsq := arg * arg;
      RETURN ((((         p4  * argsq + p3) * argsq + p2)
	       * argsq + p1) * argsq + p0)
	   / (((((argsq + q4) * argsq + q3) * argsq + q2)
	       * argsq + q1) * argsq + q0) * arg;
    END xatan;

    CONST
      sq2m1 = sqrt2 - 1.0;
      sq2p1 = sqrt2 + 1.0;
  BEGIN (* sarctan *)
    IF arg < sq2m1 THEN
      RETURN xatan (arg);
    ELSIF arg > sq2p1 THEN
      RETURN pio2 - xatan (1.0 / arg);
    ELSE
      RETURN pio4 + xatan ((arg-1.0) / (arg+1.0));
    END;
  END sarctan;

  PROCEDURE sinus (x: LONGREAL; quad: INTEGER): LONGREAL;
    VAR y, dummy, ysq: LONGREAL;
      e, f, k: INTEGER;
    CONST
      p0 =  0.1357884097877375669092680E8;
      p1 = -0.4942908100902844161158627E7;
      p2 =  0.4401030535375266501944918E6;
      p3 = -0.1384727249982452873054457E5;
      p4 =  0.1459688406665768722226959E3;
      q0 =  0.8644558652922534429915149E7;
      q1 =  0.4081792252343299749395779E6;
      q2 =  0.9463096101538208180571257E4;
      q3 =  0.1326534908786136358911494E3;
  BEGIN (* sinus *)
    IF x < 0.0 THEN
     x := -x;
     INC (quad, 2);
    END;
    x := x * twoopi;  (* Underflow? *)
    IF x > 32764.0 THEN
      y := modf (x, e);
      INC (e, quad);
      dummy := modf (0.25 * real (e), f);
      quad := e - 4 * f;
    ELSE
      k := entier (x);
      y := x - realL (k);
      quad := (quad + k) MOD 4;
    END;
    IF ODD (quad) THEN y := 1.0 - y END;
    IF quad > 1 THEN y := -y END;
    ysq := y * y;
    RETURN (((((p4 * ysq + p3) * ysq + p2) * ysq + p1) * ysq + p0) * y)
         /  ((((     ysq + q3) * ysq + q2) * ysq + q1) * ysq + q0);
  END sinus;

  PROCEDURE sqrt (arg: REAL): REAL;
  BEGIN 
    RETURN sqrtL (arg)
  END sqrt;

  PROCEDURE sqrtL (arg: LONGREAL): LONGREAL;
    (* Newton's method. Returns zero if arg < 0 *)

    CONST L30 = 1.073741824E9; (* = 1L<<30 *)
    VAR x, temp: LONGREAL; exp: INTEGER; i: [0..4];

    PROCEDURE TwoPower (b: CARDINAL): LONGREAL;
      (* Returns 1L<<b for b < 32 *)
      VAR i: CARDINAL; result: LONGREAL;
    BEGIN (* TwoPower *)
      result := 1.0;
      FOR i := 1 TO b DO result := result * 2.0 END;
      RETURN result;
    END TwoPower;

  BEGIN (* sqrt *)
    IF arg < 0.0 THEN (* Overflow *) RETURN 0.0 END;
    IF arg = 0.0 THEN RETURN 0.0 END;
    x := frexp (arg, exp);
    WHILE x < 0.5 DO
      x := x * 2.0;
      DEC (exp);
    END;
    IF ODD (exp) THEN
      x := x * 2.0;
      DEC (exp);
    END;
    temp := 0.5 * (1.0 + x);
    WHILE exp > 60 DO
      temp := temp * L30;
      DEC (exp, 60);
    END;
    WHILE exp < -60 DO
      temp := temp / L30;
      INC (exp, 60);
    END;
    IF exp >= 0 THEN
      temp := temp * TwoPower ( exp DIV 2);
    ELSE
      temp := temp / TwoPower (-exp DIV 2);
    END;
    FOR i := 0 TO 4 DO temp := 0.5 * (temp + arg / temp) END;
    RETURN temp;
  END sqrtL;

  PROCEDURE exp (arg: REAL): REAL;
  BEGIN 
    RETURN expL (arg)
  END exp;

  PROCEDURE expL (arg: LONGREAL): LONGREAL;
    (* Returns zero for arg out of range *)
    VAR ent: INTEGER;
      fract, xsq, temp1, temp2: LONGREAL;
    CONST
      maxf = 10000.0;
      p0 = 0.2080384346694663001443843411E7;
      p1 = 0.3028697169744036299076048876E5;
      p2 = 0.6061485330061080841615584556E2;
      q0 = 0.6002720360238832528230907598E7;
      q1 = 0.3277251518082914423057964422E6;
      q2 = 0.1749287689093076403844945335E4;
      log2e = 1.0 / log2;
  BEGIN (* exp *)
    IF arg = 0.0 THEN RETURN 1.0 END;
    IF arg < -maxf THEN RETURN 0.0 END;
    IF arg >  maxf THEN (* Overflow *) RETURN 0.0 END;
    arg := arg * log2e;
    ent := entier (arg);
    fract := (arg - realL (ent)) - 0.5;
    xsq := fract * fract;
    temp1 := ((       p2  * xsq + p1) * xsq + p0) * fract;
    temp2 :=  ((xsq + q2) * xsq + q1) * xsq + q0;
    RETURN ldexp (sqrt2 * (temp2 + temp1) / (temp2 - temp1), ent);
  END expL;

  PROCEDURE ln (arg: REAL): REAL;
  BEGIN 
    RETURN lnL (arg)
  END ln;

  PROCEDURE lnL (arg: LONGREAL): LONGREAL;
    (* Returns zero for arg out of range *)
    VAR asq: LONGREAL; exp: INTEGER;
    CONST
      sqrto2 = 1.0 / sqrt2;
      p0 = -0.240139179559210510E2;
      p1 =  0.309572928215376501E2;
      p2 = -0.963769093368686593E1;
      p3 =  0.421087371217979714E0;
      q0 = -0.120069589779605255E2;
      q1 =  0.194809660700889731E2;
      q2 = -0.891110902798312337E1;
  BEGIN (* ln *)
    IF arg <= 0.0 THEN (* Overflow *) RETURN 0.0 END;
    arg := frexp (arg, exp);
    WHILE arg < sqrto2 DO
      arg := arg * 2.0;
      DEC (exp);
    END;
    arg := (arg - 1.0) / (arg + 1.0);
    asq := arg * arg;
    RETURN (((p3 * asq + p2) * asq + p1) * asq + p0)
         / (((     asq + q2) * asq + q1) * asq + q0)
	 * arg + realL (exp) * log2;
  END lnL;

  PROCEDURE sin (arg: REAL): REAL;
  BEGIN 
    RETURN sinL (arg)
  END sin;

  PROCEDURE sinL (arg: LONGREAL): LONGREAL;
  BEGIN
    RETURN sinus (arg, 0);
  END sinL;

  PROCEDURE cos (arg: REAL): REAL;
  BEGIN 
    RETURN cosL (arg)
  END cos;

  PROCEDURE cosL (arg: LONGREAL): LONGREAL;
  BEGIN
    IF arg < 0.0 THEN arg := -arg END;
    RETURN sinus (arg, 1);
  END cosL;

  PROCEDURE arctan (arg: REAL): REAL;
  BEGIN 
    RETURN arctanL (arg)
  END arctan;

  PROCEDURE arctanL (arg: LONGREAL): LONGREAL;
    (* Returns the value of the arctangent of its argument in the
       range {-pi..+pi} *)
  BEGIN (* arctan *)
    IF arg > 0.0 THEN
      RETURN sarctan (arg);
    ELSE
      RETURN -sarctan (-arg);
    END;
  END arctanL;

  PROCEDURE real (arg: INTEGER): REAL;
  BEGIN 
    RETURN realL (arg)
  END real;

  PROCEDURE realL (arg: INTEGER): LONGREAL;
  BEGIN
    IF arg < 0 THEN
      RETURN -Float (-arg);
    ELSE
      RETURN  Float (arg);
    END;
  END realL;

  PROCEDURE entier (arg: REAL): INTEGER;
  BEGIN 
    RETURN entierL (arg)
  END entier;

  PROCEDURE entierL (arg: LONGREAL): INTEGER;
    (* Returns the largest integer <= arg *)
  BEGIN
    IF arg >= 0.0 THEN
      RETURN  Trunc (arg);
    ELSIF -Float (Trunc (-arg)) = arg THEN
      RETURN -Trunc (-arg);
    ELSE
      (*RETURN -Trunc (arg) - 1;   HE 3/90 *)
      RETURN -Trunc (-arg) - 1;
    END;
  END entierL;

END MathLib.
(* -EOF- *)
