MODULE bbMath;
(**
	project	= "armBox"
	organization	= "www.iermakov.ru"
	contributors	= "Ermakov Systima"
	version	= "System/Rsrc/About"
	copyright	= "System/Rsrc/About"
	license	= "GPL 3.0"
	changes	= ""
	issues	= ""

**)

	IMPORT SYSTEM;

	CONST
		pi = 3.141592653589793238462643383279502884197169399375;	(* ? to 64-bits *)
		piInv = 0.31830988618379067154;	(* 1 / pi *)
		piBy2 = 1.5707963267948965579989817342720925807952880859375;	(* ? / 2 *)
		piBy4 = 0.78539816339744827899949086713604629039764404296875;	(* ? / 4 *)
		ln2Inv = 1.44269504088896340735992468100189213;
		ln10 = 2.30258509299404590109361379290930926799774169921875;
		lneps = -36.0436533891171535515240975655615329742431640625;	(* eps = 2^-52 *)

		Large = MAX(REAL);	(* 1.7976931348623157E+308 *)
		Small = 2.2250738585072014/9.9999999999999981E307;	(* /10^308 *)
		ExpoMax = 1023;
		ExpoMin = 1 - ExpoMax;
		ExpOffset = ExpoMax;

		Miny = 1. / Large;	(* Smallest number this package accepts *)
		SqrtHalf = 0.7071067811865475244;
		Limit = 1.0536712E-8;	(* 2**(-MantBits/2) *)

	VAR
		a, b, eps: REAL;
		LnInfinity: REAL;	(* natural log of infinity *)
		LnSmall: REAL;	(* natural log of very small number *)


	PROCEDURE IsNaN (x: REAL): BOOLEAN;	(* by Robert D. Campbell *)
		CONST nanS = {19..30};	(* Detects non-signalling NaNs *)
	BEGIN
		RETURN nanS - BITS(SHORT(ASH(SYSTEM.VAL(LONGINT, x), -32))) = {}
	END IsNaN;


	(** REAL precision **)

	PROCEDURE Pi* (): REAL;
	BEGIN
		RETURN pi
	END Pi;

	PROCEDURE Eps* (): REAL;
	BEGIN
		RETURN eps
	END Eps;


	PROCEDURE^ Exponent* (x: REAL): INTEGER;

	PROCEDURE^ CopySign* (x, y: REAL): REAL;

	PROCEDURE Scale (x: REAL; n: INTEGER): REAL;
	(*
		The value of the call Scale(x, n) shall be the value x*radix^n if such
		a value exists; otherwise an execption shall occur and may be raised.
	*)
		CONST LowExpoMask = {20..30};	(* exponent, {52..62} *)
		VAR exp: INTEGER; lexp: LONGINT;
	BEGIN 
		IF x = 0. THEN RETURN 0.
		ELSE
			exp := Exponent(x) + n;	(* new exponent *)
			IF exp > ExpoMax THEN RETURN CopySign(Large, x)	(* exception raised here *)
			ELSIF exp < ExpoMin THEN RETURN CopySign(Small, x)	(* exception here as well *)
			END;
			lexp := SYSTEM.VAL(LONGINT, x);
			DEC(lexp, ASH(LONG(ORD(BITS(SHORT(ASH(lexp, -32))) * LowExpoMask)), 32));
			INC(lexp, ASH(LONG(exp + ExpOffset), 52));
			RETURN SYSTEM.VAL(REAL, lexp)
		END
	END Scale;


	PROCEDURE^ Mantissa* (r : REAL): REAL;

	PROCEDURE Sqrt* (x: REAL): REAL;	(* Returns the positive square root of x where x >= 0 *)
		CONST P0 = 0.41731; P1 = 0.59016;
		VAR xMant, yEst, z: REAL; xExp: INTEGER;
	BEGIN
		(* optimize zeros and check for illegal negative roots *)
		IF x = 0. THEN RETURN x	(* because the Sqrt of -0. is -0. *)
		ELSE
			ASSERT(x > 0.);
			(* reduce the input number to the range 0.5 <= x <= 1.0 *)
			xMant := Mantissa(x) * 0.5; xExp := Exponent(x) + 1;
			(* initial estimate of the square root *)
			yEst := P0 + P1*xMant;
			(* perform three newtonian iterations *)
			z := (yEst + xMant/yEst); yEst := 0.25*z + xMant/z;
			yEst := 0.5*(yEst + xMant/yEst);
			(* adjust for odd exponents *)
			IF ODD(xExp) THEN yEst := yEst*SqrtHalf; INC(xExp) END;
			(* single Newtonian iteration to produce real number accuracy *)
			RETURN Scale(yEst, xExp DIV 2)
		END
	END Sqrt;


	PROCEDURE Exp* (x: REAL): REAL;	(* Returns the exponential of x for x < Ln(MAX(REAL) *)
		CONST
			c1 = 0.693359375E0;
			c2 = -2.1219444005469058277E-4;
			P0 = 0.249999999999999993E+0;
			P1 = 0.694360001511792852E-2;
			P2 = 0.165203300268279130E-4;
			Q1 = 0.555538666969001188E-1;
			Q2 = 0.495862884905441294E-3;
		VAR xn, g, p, q, z: REAL; n: INTEGER;
	BEGIN
		(* Ensure we detect overflows and return 0 for underflows *)
		ASSERT(x <= LnInfinity);
		IF x < LnSmall THEN RETURN 0.
		ELSIF ABS(x) < eps THEN RETURN 1.
		END;
		(* Decompose and scale the number *)
		IF x >= 0. THEN n := SHORT(ENTIER(ln2Inv*x + 0.5))
		ELSE n := SHORT(ENTIER(ln2Inv*x-0.5))
		END;
		xn := n; g := (x-xn*c1)-xn*c2;
		(* Calculate exp(g)/2 from "Software Manual for the Elementary Functions" *)
		z := g*g; p := ((P2*z + P1)*z + P0)*g; q := (Q2*z + Q1)*z + 0.5;
		RETURN Scale(0.5 + p/(q-p), n + 1)
	END Exp;

	PROCEDURE Ln* (x: REAL): REAL;	(* Returns the natural logarithm of x for x > 0 *)
		CONST
			c1=355./512.;
			c2=-2.121944400546905827679E-4;
			P0=-0.64124943423745581147E+2;
			P1=0.16383943563021534222E+2;
			P2=-0.78956112887491257267E+0;
			Q0=-0.76949932108494879777E+3;
			Q1=0.31203222091924532844E+3;
			Q2=-0.35667977739034646171E+2;
		VAR f, zn, zd, r, z, w, p, q, xn: REAL; n: INTEGER;
	BEGIN
		(* ensure illegal inputs are trapped and handled *)
		ASSERT(x > 0.);
		(* reduce the range of the input *)
		f := Mantissa(x) * 0.5; n := Exponent(x) + 1;
		IF f > SqrtHalf THEN zn := (f - 0.5) - 0.5; zd := f*0.5 + 0.5
		ELSE zn := f - 0.5; zd := zn*0.5 + 0.5; DEC(n)
		END;
		(* evaluate rational approximation from "Software Manual for the Elementary Functions" *)
		z := zn / zd; w := z * z; q := ((w + Q2)*w + Q1)*w + Q0; p := w*((P2*w + P1)*w + P0); r := z + z*(p/q);
		(* scale the output *)
		xn := n;
		RETURN (xn*c2 + r) + xn*c1
	END Ln;

	PROCEDURE Log* (x: REAL): REAL;
	BEGIN
		RETURN Ln(x) / ln10
	END Log;

	PROCEDURE Power* (x, y: REAL): REAL;
	BEGIN
		ASSERT(x >= 0, 20);
		ASSERT((x # 0.0) OR (y # 0.0), 21);
		ASSERT((x # INF) OR (y # 0.0), 22);
		ASSERT((x # 1.0) OR (ABS(y) # INF), 23);
		RETURN Exp(y * Ln(x))
	END Power;

	PROCEDURE IntPower* (x: REAL; n: INTEGER): REAL;
		VAR y: REAL;
	BEGIN
		IF n = MIN(INTEGER) THEN RETURN IntPower(x, n + 1) / x END;
		y := 1.;
		IF n < 0 THEN x := 1. / x; n := -n END;
		WHILE n > 0 DO
			IF ODD(n) THEN y := y * x; DEC(n)
			ELSE x := x * x; n := n DIV 2
			END
		END;
		RETURN y
	END IntPower;


	PROCEDURE SinCos0 (x, y, sign: REAL): REAL;
		CONST
			ymax = 210828714;	(* ENTIER(pi*2**(MantBits/2)) *)
			c1 = 3.1416015625E0;
			c2 = -8.908910206761537356617E-6;
			r1 = -0.16666666666666665052E+0;
			r2 = 0.83333333333331650314E-2;
			r3 = -0.19841269841201840457E-3;
			r4 = 0.27557319210152756119E-5;
			r5 = -0.25052106798274584544E-7;
			r6 = 0.16058936490371589114E-9;
			r7 = -0.76429178068910467734E-12;
			r8 = 0.27204790957888846175E-14;
		VAR n: LONGINT; xn, f, x1, g: REAL;
	BEGIN
		ASSERT(y < ymax, 20);
		(* determine the reduced number *)
		n := ENTIER(y*piInv + 0.5); xn := n;
		IF ODD(n) THEN sign := -sign END;
		x := ABS(x);
		IF x # y THEN xn := xn - 0.5 END;
		(* fractional part of reduced number *)
		x1 := ENTIER(x);
		f := ((x1-xn*c1) + (x-x1))-xn*c2;
		(* Pre: |f| <= pi/2 *)
		IF ABS(f) < Limit THEN RETURN sign*f END;
		(* evaluate polynomial approximation of sin *)
		g := f*f; g := (((((((r8*g + r7)*g + r6)*g + r5)*g + r4)*g + r3)*g + r2)*g + r1)*g;
		g := f + f*g;	(* don't use less accurate f(1 + g) *)
		RETURN sign * g
	END SinCos0;

	PROCEDURE Sin* (x: REAL): REAL;
	BEGIN
		IF x < 0. THEN RETURN SinCos0(x, -x, -1.)
		ELSE RETURN SinCos0(x, x, 1.)
		END
	END Sin;

	PROCEDURE Cos* (x: REAL): REAL;
	BEGIN
		RETURN SinCos0(x, ABS(x) + piBy2, 1.)
	END Cos;

	PROCEDURE SinCos* (x: REAL; OUT sin, cos: REAL);
	BEGIN
		sin := Sin(x); cos := Cos(x)
	END SinCos;

	PROCEDURE Tan* (x: REAL): REAL;	(* Returns the tangent of x where x cannot be an odd multiple of pi/2 *)
		VAR sin, cos: REAL;
	BEGIN
		SinCos(x, sin, cos);
		ASSERT(ABS(cos) >= Miny);
		RETURN sin / cos
	END Tan;

	PROCEDURE^ ArcTan* (x: REAL): REAL;

	PROCEDURE ArcSin* (x: REAL): REAL;
	BEGIN
		ASSERT(ABS(x) <= 1., 20);
		RETURN ArcTan(x / Sqrt(1. - x * x))
	END ArcSin;

	PROCEDURE ArcCos* (x: REAL): REAL;
	BEGIN	(* pi/2 - arcsin(x) *)
		ASSERT(ABS(x) <= 1., 20);
		RETURN ArcTan(Sqrt(1. - x * x) / x)
	END ArcCos;

	PROCEDURE ArcTan* (x: REAL): REAL;	(* Fixed by Robert D. Campbell *)
		(*
			Algorithm and coefficients from:
			"Software manual for the elementary functions"
			by W.J. Cody and W. Waite, Prentice-Hall, 1980
		*)
		CONST
			p0 = -0.13688768894191926929E+02;
			p1 = -0.20505855195861651981E+02;
			p2 = -0.84946240351320683534E+01;
			p3 = -0.83758299368150059274E+00;
			q0 = 0.41066306682575781263E+02;
			q1 = 0.86157349597130242515E+02;
			q2 = 0.59578436142597344465E+02;
			q3 = 0.15024001160028576121E+02;
			q4 = 1.;
		VAR g: REAL; neg: BOOLEAN; n: INTEGER;
	BEGIN
		IF x = 0. THEN RETURN x END;
		neg := FALSE;
		IF x < 0. THEN
			neg := TRUE; x := -x;
		END;
		IF x > 1. THEN
			x := 1./x; n := 2
		ELSE
			n := 0
		END;
		IF x > 0.26794919243112270647	(* 2-sqrt(3) *) THEN
			INC(n);
			x := (((0.73205080756887729353*x-0.5)-0.5)+x) / (1.73205080756887729353 + x)
		END;
		g := x*x;
		x := x + x * g * (((p3*g+p2)*g+p1)*g+p0) / ((((q4*g+q3)*g+q2)*g+q1)*g+q0);
		IF n > 1 THEN x := -x END;
		CASE n OF
		| 1: x := x + 0.52359877559829887307710723554658381	(* pi/6 *)
		| 2: x := x + piBy2
		| 3: x := x + 1.04719755119659774615421446109316763	(* pi/3 *)
		ELSE	(* angle is correct *)
		END;
		IF neg THEN RETURN -x END;
		RETURN x
	END ArcTan;

	PROCEDURE^ Sign* (x: REAL): REAL;

	PROCEDURE ArcTan2* (y, x: REAL): REAL;	(* Fixed by Robert D. Campbell *)
	BEGIN
		IF x = 0. THEN
			IF SYSTEM.VAL(LONGINT, x) = 0 THEN RETURN Sign(y) * piBy2	(* Requires "corrected" Sign *)
			ELSIF y = 0. THEN RETURN CopySign(pi, y)
			ELSE RETURN CopySign(piBy2, y)
			END
		ELSIF y = 0. THEN	(* Optional "speedup" *)
			RETURN (1. - Sign(x)) * CopySign(piBy2, y)
		ELSIF (ABS(x) = INF) & (ABS(y) = INF) THEN
			RETURN (2. - Sign(x)) * CopySign(piBy4, y)
		ELSE
			RETURN ArcTan(y / x) + (1. - Sign(x)) * CopySign(piBy2, y)	(* Requires "corrected" ArcTan *)
		END
	END ArcTan2;


	(* http://mameli.docenti.di.unimi.it/solab/export/HEAD/tags/syscall-add-simple-foo/minix/lib/ack/libm2/Mathlib.mod *)
	PROCEDURE Sinh* (x: REAL): REAL;
	BEGIN
		IF x = 0. THEN RETURN x
		ELSIF ABS(x) < -lneps THEN RETURN (Exp(x) - Exp(-x)) / 2.
		ELSE RETURN Sign(x) * Exp(ABS(x)) / 2.
		END
	END Sinh;

	PROCEDURE Cosh* (x: REAL): REAL;
		VAR expx: REAL;
	BEGIN
		IF ABS(x) < -lneps THEN RETURN (Exp(x) + Exp(-x)) / 2.
		ELSE RETURN Exp(ABS(x)) / 2.
		END
	END Cosh;

	PROCEDURE Tanh* (x: REAL): REAL;
		VAR e1, e2: REAL;
	BEGIN 
		IF x = 0. THEN RETURN x
		ELSIF ABS(x) < -lneps THEN
			e1 := Exp(x); e2 := 1. / e1;
			RETURN (e1 - e2) / (e1 + e2)
		ELSE
			RETURN Sign(x)
		END
	END Tanh;

	PROCEDURE ArcSinh* (x: REAL): REAL;
	BEGIN
		RETURN Sign (x) * Ln(ABS(x) + Sqrt(x * x + 1.))
	END ArcSinh;

	PROCEDURE ArcCosh* (x: REAL): REAL;
	BEGIN
		RETURN Ln(x + Sqrt(x * x - 1.))
	END ArcCosh;

	PROCEDURE ArcTanh* (x: REAL): REAL;
	BEGIN
		ASSERT(ABS(x) <= 1., 20);
		IF x = 0. THEN RETURN x ELSE RETURN Ln((1. + x) / (1. - x)) / 2. END
	END ArcTanh;


	PROCEDURE Floor* (x: REAL): REAL;
	BEGIN
		IF ABS(x) >= 1.0E16 THEN RETURN x
		ELSE RETURN ENTIER(x)
		END
	END Floor;

	PROCEDURE Ceiling* (x: REAL): REAL;
	BEGIN
		IF ABS(x) >= 1.0E16 THEN RETURN x
		ELSE RETURN -ENTIER(-x)
		END
	END Ceiling;

	PROCEDURE Round* (x: REAL): REAL;
	BEGIN
		IF ABS(x) >= 1.0E16 THEN RETURN x
		ELSE RETURN ENTIER(x + 0.5)
		END
	END Round;

	PROCEDURE Trunc* (x: REAL): REAL;
	BEGIN 
		IF ABS(x) >= 1.0E16 THEN RETURN x
		ELSIF x >= 0 THEN RETURN ENTIER(x)
		ELSE RETURN -ENTIER(-x)
		END
	END Trunc;

	PROCEDURE Frac* (x: REAL): REAL;
	BEGIN
		IF ABS(x) >= 1.0E16 THEN RETURN 0.0
		ELSIF x >= 0 THEN RETURN x - ENTIER(x)
		ELSE RETURN x + ENTIER(-x)
		END
	END Frac;

	PROCEDURE Mod1* (x: REAL): REAL;
	BEGIN
		IF ABS(x) >= 1.0E16 THEN RETURN 0.
		ELSE RETURN x - ENTIER(x)
		END
	END Mod1;

	PROCEDURE Sign* (x: REAL): REAL;	(* by Robert D. Campbell *)
	BEGIN
		IF x > 0. THEN RETURN 1.
		ELSIF x < 0. THEN RETURN -1.
		ELSE RETURN x	(* +0.0 or -0.0 *)
		END
	END Sign;

	PROCEDURE SignBit* (x: REAL): BOOLEAN;	(* IEEE 754-2008, section 5.7.2 *)
	BEGIN
		RETURN 31 IN BITS(SHORT(ASH(SYSTEM.VAL(LONGINT, x), -32)))
	END SignBit;

	PROCEDURE CopySign* (x, y: REAL): REAL;	(* IEEE 754-2008, section 5.5.1 *)
	BEGIN
		IF SignBit(y) THEN RETURN -ABS(x) ELSE RETURN ABS(x) END
	END CopySign;

	PROCEDURE Mantissa* (r : REAL): REAL;	(* by Robert D. Campbell *)
		CONST Mask = {0..19, 31}; ZeroExp = {20..29};
		VAR k, msh: LONGINT; a: REAL; exp: INTEGER;
	BEGIN
		IF IsNaN(r) THEN RETURN CopySign(1.5, r)
		ELSIF r = 0. THEN RETURN 0.
		ELSIF r = INF THEN RETURN 1.
		ELSIF r = -INF THEN RETURN -1.
		ELSE
			a := ABS(r);
			exp := SHORT(ASH(SYSTEM.VAL(LONGINT, a), -52));
			WHILE exp = 0 DO
				r := r + r; a := ABS(r);
				exp := SHORT(ASH(SYSTEM.VAL(LONGINT, a), -52))
			END;
			k := SYSTEM.VAL(LONGINT, r);
			msh := ORD(BITS(SHORT(ASH(k, -32))) * Mask + ZeroExp);
			k := ASH(msh, 32) + k MOD 100000000L;
			RETURN SYSTEM.VAL(REAL, k)
		END
	END Mantissa;

	PROCEDURE Exponent* (x: REAL): INTEGER;	(* by Robert D. Campbell *)
		VAR exp, offset: INTEGER;
	BEGIN
		IF IsNaN(x) THEN RETURN MAX(INTEGER)
		ELSIF x = 0. THEN RETURN 0
		ELSE
			x := ABS(x);
			IF x = INF THEN RETURN MAX(INTEGER)
			ELSE exp := SHORT(ASH(SYSTEM.VAL(LONGINT, x), -52))
			END;
			offset := ExpoMax;
			WHILE exp = 0 DO
				x := x + x; exp := SHORT(ASH(SYSTEM.VAL(LONGINT, x), -52)); INC(offset)
			END;
			RETURN exp - offset
		END
	END Exponent;

	(* http://krum.rz.uni-mannheim.de/mas/src/masarith/MathLib.mi.html *)
	PROCEDURE Real* (m: REAL; e: INTEGER): REAL;	(* Returns m * 2 ** e *)
		VAR s: SET; i: INTEGER;
	BEGIN
		IF m = 0 THEN RETURN 0. END;
		ASSERT(~IsNaN(m) & (1 <= ABS(m)) & (ABS(m) < 2), 20);
		IF e = MAX(INTEGER) THEN
			SYSTEM.GET(SYSTEM.ADR(m) + 4, s);
			SYSTEM.PUT(SYSTEM.ADR(m) + 4, s + {20..30});
		ELSE
			IF e > 0 THEN
				FOR i := 1 TO e DO m := m * 2. END
			ELSIF e < 0 THEN
				FOR i := 1 TO -e DO m := m / 2. END
			END
		END;
		RETURN m
	END Real;


BEGIN
	(* determine some fundamental constants used by hyperbolic trig functions *)
	LnInfinity := Ln(Large);
	LnSmall := Ln(Miny);
	(* Calculating epsilon in a way that is portable and reliable is tricky *)
	eps := 1.; a := 1.; b := 1.;	(* https://www.linux.org.ru/forum/general/8089131 *)
	REPEAT eps := eps/2.; a := b + eps/2. UNTIL a <= b
END bbMath.
