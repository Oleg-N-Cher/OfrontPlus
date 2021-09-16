MODULE bbStrings;
(**
	project	= "BlackBox"
	organization	= "www.oberon.ch"
	contributors	= "Oberon microsystems"
	version	= "System/Rsrc/About"
	copyright	= "System/Rsrc/About"
	license	= "Docu/BB-License"
	changes	= ""
	issues	= ""

**)

	IMPORT Kernel := bbKernel, Math := bbMath;

	CONST
		charCode* = -1; decimal* = 10; hexadecimal* = -2; roman*= -3;
		digitspace* = 08FX;
		showBase* = TRUE; hideBase* = FALSE;
		minLongIntRev = "8085774586302733229";	(* reversed string of -MIN(LONGINT) *)

	VAR
		maxExp: INTEGER;
		maxDig: INTEGER;
		factor: REAL;	(* 10^maxDig *)
		digits: ARRAY 17 OF CHAR;
		toUpper, toLower: ARRAY 256 OF CHAR;


	(* integer conversions *)

	PROCEDURE IntToString* (x: LONGINT; OUT s: ARRAY OF CHAR);
		VAR j, k: INTEGER; ch: CHAR; a: ARRAY 32 OF CHAR;
	BEGIN
		IF x # MIN(LONGINT) THEN
			IF x < 0 THEN s[0] := "-"; k := 1; x := -x ELSE k := 0 END;
			j := 0; REPEAT a[j] := CHR(x MOD 10 + ORD("0")); x := x DIV 10; INC(j) UNTIL x = 0
		ELSE
			a := minLongIntRev; s[0] := "-"; k := 1;
			j := 0; WHILE a[j] # 0X DO INC(j) END
		END;
		ASSERT(k + j < LEN(s), 23);
		REPEAT DEC(j); ch := a[j]; s[k] := ch; INC(k) UNTIL j = 0;
		s[k] := 0X
	END IntToString;

	PROCEDURE IntToStringForm* (x: LONGINT; form, minWidth: INTEGER; fillCh: CHAR;
														showBase: BOOLEAN; OUT s: ARRAY OF CHAR);
		VAR base, i, j, k, si: INTEGER; mSign: BOOLEAN; a: ARRAY 128 OF CHAR; c1, c5, c10: CHAR;
	BEGIN
		ASSERT((form = charCode) OR (form = hexadecimal) OR (form = roman) OR ((form >= 2) & (form <= 16)), 20);
		ASSERT(minWidth >= 0, 22);
		IF form = charCode THEN base := 16
		ELSIF form = hexadecimal THEN base := 16
		ELSE base := form
		END;
		
		IF form = roman THEN
			ASSERT((x > 0) & (x < 3999), 21); 
			base := 1000; i := 0; mSign := FALSE;
			WHILE (base > 0) & (x > 0) DO
				IF base = 1 THEN c1 := "I"; c5 := "V"; c10 := "X"
				ELSIF base = 10 THEN c1 := "X"; c5 := "L"; c10 := "C"
				ELSIF base = 100 THEN c1 := "C"; c5 := "D"; c10 := "M"
				ELSE c1 := "M"
				END;
				k := SHORT(x DIV base); x := x MOD base;
				IF k IN {4, 9} THEN a[i] := c1; INC(i) END;
				IF k IN {4 .. 8} THEN a[i] := c5; INC(i) END;
				IF k = 9 THEN a[i] := c10; INC(i)
				ELSIF k IN {1 .. 3, 6 .. 8} THEN
					j := k MOD 5;
					REPEAT a[i] := c1; INC(i); DEC(j) UNTIL j = 0
				END;
				base := base DIV 10
			END
		ELSIF (form = hexadecimal) OR (form = charCode) THEN
			i := 0; mSign := FALSE;
			IF showBase THEN DEC(minWidth) END;
			REPEAT
				a[i] := digits[x MOD base]; x := x DIV base; INC(i)
			UNTIL (x = 0) OR (x = -1) OR (i = LEN(a));
			IF x = -1 THEN fillCh := "F" END
		ELSE
			IF x < 0 THEN
				i := 0; mSign := TRUE; DEC(minWidth);
				REPEAT
					IF x MOD base = 0 THEN
						a[i] := digits[0]; x := x DIV base
					ELSE
						a[i] := digits[base - x MOD base]; x := x DIV base + 1
					END;
					INC(i)
				UNTIL (x = 0) OR (i = LEN(a))
			ELSE
				i := 0; mSign := FALSE;
				REPEAT
					a[i] := digits[x MOD base]; x := x DIV base; INC(i)
				UNTIL (x = 0) OR (i = LEN(a))
			END;
			IF showBase THEN DEC(minWidth);
				IF base < 10 THEN DEC(minWidth) ELSE DEC(minWidth,2) END
			END
		END;
		si := 0;
		IF mSign & (fillCh = "0") & (si < LEN(s)) THEN s[si] := "-"; INC(si); mSign := FALSE END;
		WHILE minWidth > i DO
			IF si < LEN(s) THEN s[si] := fillCh; INC(si) END;
			DEC(minWidth)
		END;
		IF mSign & (si < LEN(s)) THEN s[si] := "-"; INC(si) END;
		IF form = roman THEN
			j := 0;
			WHILE j < i DO 
				IF si < LEN(s) THEN s[si] := a[j]; INC(si) END; 
				INC(j)
			END
		ELSE
			REPEAT DEC(i);
				IF si < LEN(s) THEN s[si] := a[i]; INC(si) END
			UNTIL i = 0
		END;
		IF showBase & (form # roman) THEN
			IF (form = charCode) & (si < LEN(s)) THEN s[si] := "X"; INC(si)
			ELSIF (form = hexadecimal) & (si < LEN(s)) THEN s[si] := "H"; INC(si)
			ELSIF (form < 10) & (si < LEN(s)-1) THEN s[si] := "%"; s[si+1] := digits[base]; INC(si, 2)
			ELSIF (si < LEN(s) - 2) THEN
				s[si] := "%"; s[si+1] := digits[base DIV 10]; s[si+2] := digits[base MOD 10]; INC(si, 3)
			END
		END;
		IF si < LEN(s) THEN s[si] := 0X ELSE HALT(23) END
	END IntToStringForm;

	PROCEDURE StringToInt* (IN s: ARRAY OF CHAR; OUT x: INTEGER; OUT res: INTEGER);
		CONST hexLimit = MAX(INTEGER) DIV 8 + 1;
		VAR i, j, k, digits: INTEGER; ch, top: CHAR; neg: BOOLEAN; base: INTEGER;
	BEGIN
		res := 0; i := 0; ch := s[0];
		WHILE (ch # 0X) & (ch <= " ") OR (ch = 8BX) OR (ch = 8FX) OR (ch = 0A0X) DO	(* ignore leading blanks *)
			INC(i); ch := s[i]
		END;
		j := i; top := "0";
		WHILE (ch # 0X) & (ch # "H") & (ch # "X") & (ch # "%") DO 
			IF ch > top THEN top := ch END;
			INC(j); ch := s[j]
		END;
		IF (ch = "H") OR (ch = "X") THEN
			x := 0; ch := s[i];
			IF ("0" <= ch) & (ch <= "9") OR ("A" <= ch) & (ch <= "F") THEN
				WHILE ch = "0" DO INC(i); ch := s[i] END;
				digits := 0;
				WHILE (res = 0) & (("0" <= ch) & (ch <= "9") OR ("A" <= ch) & (ch <= "F")) DO
					IF ch < "A" THEN k := ORD(ch) - ORD("0")
					ELSE k := ORD(ch) - ORD("A") + 10
					END;
					IF digits < 8 THEN
						x := x MOD hexLimit;
						IF x >= hexLimit DIV 2 THEN x := x - hexLimit END;
						x := x * 16 + k; INC(i); ch := s[i]
					ELSE res := 1
					END;
					INC(digits)
				END;
				IF res = 0 THEN
					IF (ch # "H") & (ch # "X") OR (s[i+1] # 0X) THEN res := 2 END
				END
			ELSE res := 2
			END
		ELSE
			IF ch = "%" THEN
				INC(j); ch := s[j]; base := 0;
				IF ("0" <= ch) & (ch <= "9") THEN
					k := ORD(ch) - ORD("0");
					REPEAT
						base := base * 10 + k;
						INC(j); ch := s[j]; k := ORD(ch) - ORD("0")
					UNTIL (ch < "0") OR (ch > "9") OR (base > (MAX(INTEGER) - k) DIV 10);
					IF ("0" <= ch) & (ch <= "9") THEN base := 0 END
				END
			ELSE
				base := 10
			END;
			
			IF (base < 2) OR (base > 16) THEN
				res := 2
			ELSIF (base <= 10) & (ORD(top) < base + ORD("0"))
			OR (base > 10) & (ORD(top) < base - 10 + ORD("A")) THEN
				x := 0; ch := s[i]; neg := FALSE;
				IF ch = "-" THEN INC(i); ch := s[i]; neg := TRUE ELSIF ch = "+" THEN INC(i); ch := s[i] END;
				WHILE (ch # 0X) & (ch <= " ") DO INC(i); ch := s[i] END; 
				IF ("0" <= ch) & (ch <= "9") OR ("A" <= ch) & (ch <= "F") THEN
					IF ch <= "9" THEN k := ORD(ch) - ORD("0") ELSE k := ORD(ch) - ORD("A") + 10 END;
					WHILE (("0" <= ch) & (ch <= "9") OR ("A" <= ch) & (ch <= "F")) & (res = 0) DO
						IF x >= (MIN(INTEGER) + (base - 1) + k) DIV base THEN
							x := x * base - k; INC(i); ch := s[i];
							IF ch <= "9" THEN k := ORD(ch) - ORD("0") ELSE k := ORD(ch) - ORD("A") + 10 END
						ELSE res := 1
						END
					END
				ELSE res := 2
				END;
				IF res = 0 THEN
					IF ~neg THEN
						IF x > MIN(INTEGER) THEN x := -x ELSE res := 1 END
					END;
					IF (ch # 0X) & (ch # "%") THEN res := 2 END
				END
			ELSE
				res := 2
			END
		END
	END StringToInt;

	PROCEDURE StringToLInt* (IN s: ARRAY OF CHAR; OUT x: LONGINT; OUT res: INTEGER);
		CONST hexLimit = MAX(LONGINT) DIV 8 + 1;
		VAR i, j, k, digits: INTEGER; ch, top: CHAR; neg: BOOLEAN; base: INTEGER;
	BEGIN
		res := 0; i := 0; ch := s[0];
		WHILE (ch # 0X) & (ch <= " ") OR (ch = 8BX) OR (ch = 8FX) OR (ch = 0A0X) DO	(* ignore leading blanks *)
			INC(i); ch := s[i]
		END;
		j := i; top := "0";
		WHILE (ch # 0X) & (ch # "H") & (ch # "X") & (ch # "%") DO 
			IF ch > top THEN top := ch END;
			INC(j); ch := s[j]
		END;
		IF (ch = "H") OR (ch = "X") THEN
			x := 0; ch := s[i];
			IF ("0" <= ch) & (ch <= "9") OR ("A" <= ch) & (ch <= "F") THEN
				WHILE ch = "0" DO INC(i); ch := s[i] END;
				digits := 0;
				WHILE (res = 0) & (("0" <= ch) & (ch <= "9") OR ("A" <= ch) & (ch <= "F")) DO
					IF ch < "A" THEN k := ORD(ch) - ORD("0")
					ELSE k := ORD(ch) - ORD("A") + 10
					END;
					IF digits < 16 THEN
						x := x MOD hexLimit;
						IF x >= hexLimit DIV 2 THEN x := x - hexLimit END;
						x := x * 16 + k; INC(i); ch := s[i]
					ELSE res := 1
					END;
					INC(digits)
				END;
				IF res = 0 THEN
					IF (ch # "H") & (ch # "X") OR (s[i+1] # 0X) THEN res := 2 END
				END
			ELSE res := 2
			END
		ELSE
			IF ch = "%" THEN
				INC(j); ch := s[j]; base := 0;
				IF ("0" <= ch) & (ch <= "9") THEN
					k := ORD(ch) - ORD("0");
					REPEAT
						base := base * 10 + k;
						INC(j); ch := s[j]; k := ORD(ch) - ORD("0")
					UNTIL (ch < "0") OR (ch > "9") OR (base > (MAX(INTEGER) - k) DIV 10);
					IF ("0" <= ch) & (ch <= "9") THEN base := 0 END
				END
			ELSE
				base := 10
			END;
			
			IF (base < 2) OR (base > 16) THEN
				res := 2
			ELSIF (base <= 10) & (ORD(top) < base + ORD("0"))
			OR (base > 10) & (ORD(top) < base -10 + ORD("A")) THEN
				x := 0; ch := s[i]; neg := FALSE;
				IF ch = "-" THEN INC(i); ch := s[i]; neg := TRUE ELSIF ch = "+" THEN INC(i); ch := s[i] END;
				WHILE (ch # 0X) & (ch <= " ") DO INC(i); ch := s[i] END; 
				IF ("0" <= ch) & (ch <= "9") OR ("A" <= ch) & (ch <= "F") THEN
					IF ch <= "9" THEN k := ORD(ch) - ORD("0") ELSE k := ORD(ch) - ORD("A") + 10 END;
					WHILE (("0" <= ch) & (ch <= "9") OR ("A" <= ch) & (ch <= "F")) & (res = 0) DO
						IF x >= (MIN(LONGINT) + (base - 1) + k) DIV base THEN
							x := x * base - k; INC(i); ch := s[i];
							IF ch <= "9" THEN k := ORD(ch) - ORD("0") ELSE k := ORD(ch) - ORD("A") + 10 END
						ELSE res := 1
						END
					END
				ELSE res := 2
				END;
				IF res = 0 THEN
					IF ~neg THEN
						IF x > MIN(LONGINT) THEN x := -x ELSE res := 1 END
					END;
					IF (ch # 0X) & (ch # "%") THEN res := 2 END
				END
			ELSE
				res := 2
			END
		END
	END StringToLInt;


	(* real conversions *)

	PROCEDURE RealToStringForm* (x: REAL; precision, minW, expW: INTEGER; fillCh: CHAR;
														OUT s: ARRAY OF CHAR);
		VAR exp, len, i, j, n, k, p: INTEGER; m: ARRAY 80 OF CHAR; neg: BOOLEAN;
	BEGIN
		ASSERT((precision > 0) (*& (precision <= 18)*), 20);
		ASSERT((minW >= 0) & (minW < LEN(s)), 21);
		ASSERT((expW > -LEN(s)) & (expW <= 3), 22);
		exp := Math.Exponent(x);
		IF exp = MAX(INTEGER) THEN
			IF fillCh = "0" THEN fillCh := digitspace END;
			x := Math.Mantissa(x);
			IF x = -1 THEN m := "-inf"; n := 4
			ELSIF x = 1 THEN m := "inf"; n := 3
			ELSE m := "nan"; n := 3
			END;
			i := 0; j := 0;
			WHILE minW > n DO s[i] := fillCh; INC(i); DEC(minW) END;
			WHILE (j <= n) & (i < LEN(s)) DO s[i] := m[j]; INC(i); INC(j) END
		ELSE
			neg := FALSE; len := 1; m := "00";
			IF x < 0 THEN x := -x; neg := TRUE; DEC(minW) END;
			IF x # 0 THEN
				exp := (exp - 8) * 30103 DIV 100000;	(* * log(2) *)
				IF exp > 0 THEN
					n := SHORT(ENTIER(x / Math.IntPower(10, exp)));
					x := x / Math.IntPower(10, exp) - n
				ELSIF exp > -maxExp THEN
					n := SHORT(ENTIER(x * Math.IntPower(10, -exp)));
					x := x * Math.IntPower(10, -exp) - n
				ELSE
					n := SHORT(ENTIER(x * Math.IntPower(10, -exp - 2 * maxDig) * factor * factor));
					x := x * Math.IntPower(10, -exp - 2 * maxDig) * factor * factor - n
				END;
				(* x0 = (n + x) * 10^exp, 200 < n < 5000 *)
				p := precision - 4;
				IF n < 1000 THEN INC(p) END;
				IF (expW < 0) & (p > exp - expW) THEN p := exp - expW END;
				IF p >= 0 THEN
					x := x + 0.5 / Math.IntPower(10, p);	(* rounding correction *)
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
					m[i] := CHR(p + ORD("0")); INC(i);
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
				IF (expW = 3) & (i < LEN(s)) THEN s[i] := CHR(exp DIV 100 + ORD("0")); INC(i) END;
				IF (expW >= 2) & (i < LEN(s)) THEN s[i] := CHR(exp DIV 10 MOD 10 + ORD("0")); INC(i) END;
				IF i < LEN(s) THEN s[i] := CHR(exp MOD 10 + ORD("0")); INC(i) END
			END
		END;
		IF i < LEN(s) THEN s[i] := 0X ELSE HALT(23) END
	END RealToStringForm;

	PROCEDURE RealToString* (x: REAL; OUT s: ARRAY OF CHAR);
	BEGIN
		RealToStringForm(x, 16, 0, 0, digitspace, s)
	END RealToString;


	PROCEDURE StringToReal* (IN s: ARRAY OF CHAR; OUT x: REAL; OUT res: INTEGER);
		VAR first, last, point, e, n, i, exp: INTEGER; y: REAL; ch: CHAR; neg, negExp, dig: BOOLEAN;
	BEGIN
		res := 0; i := 0; ch := s[0]; dig := FALSE;
		WHILE (ch # 0X) & (ch <= " ") OR (ch = 8BX) OR (ch = 8FX) OR (ch = 0A0X) DO INC(i); ch := s[i] END;
		IF ch = "+" THEN
			neg := FALSE; INC(i); ch := s[i]
		ELSIF ch = "-" THEN
			neg := TRUE; INC(i); ch := s[i]
		ELSE
			neg := FALSE
		END;
		WHILE ch = "0" DO INC(i); ch := s[i]; dig := TRUE END;
		first := i; e := 0;
		WHILE ("0" <= ch) & (ch <= "9") DO INC(i); ch := s[i]; INC(e); dig := TRUE END;
		point := i;
		IF ch = "." THEN
			INC(i); ch := s[i];
			IF e = 0 THEN
				WHILE ch = "0" DO INC(i); ch := s[i]; DEC(e); dig := TRUE END;
				first := i
			END;
			WHILE ("0" <= ch) & (ch <= "9") DO INC(i); ch := s[i]; dig := TRUE END
		END;
		last := i - 1; exp := 0;
		IF (ch = "E") OR (ch = "D") THEN
			INC(i); ch := s[i]; negExp := FALSE;
			IF ch = "-" THEN negExp := TRUE; INC(i); ch := s[i]
			ELSIF ch = "+" THEN INC(i); ch := s[i]
			END;
			WHILE ("0" <= ch) & (ch <= "9") & (exp < 1000) DO
				exp := exp * 10 + (ORD(ch) - ORD("0"));
				INC(i); ch := s[i]
			END;
			IF negExp THEN exp := -exp END
		END;
		exp := exp + e; x := 0; y := 0; n := 0; 
		WHILE (n < maxDig) & (first <= last) DO
			IF first # point THEN x := x * 10 + (ORD(s[first]) - ORD("0")); INC(n) END;
			INC(first)
		END;
		WHILE last >= first DO
			IF last # point THEN y := (y + (ORD(s[last]) - ORD("0"))) / 10 END;
			DEC(last)
		END;
		IF ~dig OR (ch # 0X) THEN res := 2	(* syntax error *)
		ELSIF exp < -maxExp - maxDig THEN
			x := 0.0
		ELSIF exp < -maxExp + maxDig THEN
			x := (x + y) / Math.IntPower(10, n - exp - 2 * maxDig) / factor / factor
		ELSIF exp < n THEN
			x := (x + y) / Math.IntPower(10, n - exp)
		ELSIF exp < maxExp THEN
			x := (x + y) * Math.IntPower(10, exp - n)
		ELSIF exp = maxExp THEN
			x := (x + y) * (Math.IntPower(10, exp - n) / 16);
			IF x <= MAX(REAL) / 16 THEN x := x * 16
			ELSE res := 1	(* overflow *)
			END
		ELSE res := 1	(* overflow *)
		END;
		IF neg THEN x := -x END
	END StringToReal;


	(* set conversions *)

	PROCEDURE SetToString* (x: SET; OUT str: ARRAY OF CHAR);
		VAR
			len, i: INTEGER;

		PROCEDURE AppendChar(ch: CHAR);
		BEGIN ASSERT(len < LEN(str), 23); str[len] := ch; INC(len)
		END AppendChar;

		PROCEDURE AppendInt (x: INTEGER);
			VAR i: INTEGER; ch: CHAR; buf: ARRAY 32 OF CHAR;
		BEGIN
			IntToString(x, buf);
			ch := buf[0]; i := 0;
			WHILE ch # 0X DO AppendChar(ch); INC(i); ch := buf[i] END
		END AppendInt;

	BEGIN
		len := 0; AppendChar('{'); i := MIN(SET);
		WHILE x # {} DO
			IF i IN x THEN AppendInt(i); EXCL(x, i);
				IF (i <= MAX(SET) - 2) & (i+1 IN x) & (i+2 IN x) THEN AppendChar('.'); AppendChar('.');
					x := x - {i+1, i+2}; INC(i, 3);
					WHILE (i <= MAX(SET)) & (i IN x) DO EXCL(x, i); INC(i) END;
					AppendInt(i-1)
				END;
				IF x # {} THEN AppendChar(","); AppendChar(" ") END
			END;
			INC(i)
		END;
		AppendChar("}"); AppendChar(0X)
	END SetToString;

	PROCEDURE StringToSet* (IN s: ARRAY OF CHAR; OUT x: SET; OUT res: INTEGER);
		VAR
			next, x1: INTEGER; ch: CHAR;
		
		PROCEDURE NextChar;
		BEGIN ch := s[next]; 
			WHILE (ch <= " ") & (ch # 0X) DO INC(next); ch := s[next] END ;
			IF ch # 0X THEN INC(next) END
		END NextChar;
		
		PROCEDURE ParseInt(): INTEGER;
			VAR intval: INTEGER;
		BEGIN
			intval := ORD(ch) - ORD("0");
			ch := s[next];
			WHILE ("0" <= ch) & (ch <= "9") DO
				intval := intval * 10 + ORD(ch) - ORD('0');
				IF intval > MAX(SET) THEN res := 1; intval := 0 END ;
				INC(next); ch := s[next]
			END ;
			NextChar;
			RETURN intval
		END ParseInt;
		
		PROCEDURE ParseRange();
			VAR x2, i: INTEGER;
		BEGIN
			ch := s[next]; INC(next);
			IF ch = "." THEN NextChar; 
				IF ("0" <= ch) & (ch <= "9") THEN x2 := ParseInt();
					IF res = 0 THEN
						IF x2 >= x1 THEN FOR i := x1 TO x2 DO INCL(x, i) END
						ELSE res := 2
						END
					END
				ELSE res := 2
				END
			ELSE res := 2
			END
		END ParseRange;
		
	BEGIN
		x := {}; res := 0; next := 0; NextChar;
		IF ch = "{" THEN
			NextChar;
			WHILE ("0" <= ch) & (ch <= "9") & (res # 2) DO
				x1 := ParseInt();
				IF ch = "." THEN ParseRange() ELSIF res = 0 THEN INCL(x, x1) END ;
				IF ch = "," THEN NextChar END
			END ;
			IF ch = "}" THEN NextChar;
				IF ch # 0X THEN res := 2 END
			ELSE res := 2
			END
		ELSE res := 2
		END
	END StringToSet;

	(* ----------------------------- general purpose character classes and conversions --------------------------- *)
	
	PROCEDURE IsUpper* (ch: CHAR): BOOLEAN;
	BEGIN
		IF ORD(ch) < 256 THEN RETURN toLower[ORD(ch)] # ch ELSE RETURN Kernel.IsUpper(ch) END
	END IsUpper;

	PROCEDURE Upper* (ch: CHAR): CHAR;
	BEGIN
		IF ORD(ch) < 256 THEN RETURN toUpper[ORD(ch)] ELSE RETURN Kernel.Upper(ch) END
	END Upper;
	
	PROCEDURE IsLower* (ch: CHAR): BOOLEAN;
	BEGIN
		IF ORD(ch) < 256 THEN RETURN toUpper[ORD(ch)] # ch ELSE RETURN Kernel.IsLower(ch) END
	END IsLower;

	PROCEDURE Lower* (ch: CHAR): CHAR;
	BEGIN 
		IF ORD(ch) < 256 THEN RETURN toLower[ORD(ch)] ELSE RETURN Kernel.Lower(ch) END
	END Lower;

	PROCEDURE IsAlpha* (ch: CHAR): BOOLEAN;
	BEGIN
		CASE ch OF
		"a".."z", "A".."Z": RETURN TRUE
		ELSE
			IF ch > 7FX THEN RETURN Kernel.IsAlpha(ch) ELSE RETURN FALSE END
		END
	END IsAlpha;

	PROCEDURE IsNumeric* (ch: CHAR): BOOLEAN;
	BEGIN
		RETURN ("0" <= ch) & (ch <= "9")
	END IsNumeric;

	PROCEDURE IsAlphaNumeric* (ch: CHAR): BOOLEAN;
	BEGIN
		RETURN IsAlpha(ch) OR IsNumeric(ch)
	END IsAlphaNumeric;

	(* ----------------------------- Component Pascal character classes --------------------------- *)

	PROCEDURE IsIdentStart* (ch: CHAR): BOOLEAN;
	BEGIN
		CASE ch OF
		"a".."z", "A".."Z", "_": RETURN TRUE
		ELSE
			IF ch > 7FX THEN RETURN Kernel.IsAlpha(ch) ELSE RETURN FALSE END
		END
	END IsIdentStart;

	PROCEDURE IsIdent* (ch: CHAR): BOOLEAN;
	BEGIN
		(* returns IsIdentStart(ch) OR IsNumeric(ch); optimized because heavily used in the compiler *)
		CASE ch OF
		"a".."z", "A".."Z", "_", "0".."9": RETURN TRUE
		ELSE
			IF ch > 7FX THEN RETURN Kernel.IsAlpha(ch) ELSE RETURN FALSE END
		END
	END IsIdent;

	(* ----------------------------- string manipulation routines --------------------------- *)

	PROCEDURE Valid* (IN s: ARRAY OF CHAR): BOOLEAN;
		VAR i: INTEGER;
	BEGIN i := 0;
		WHILE (i < LEN(s)) & (s[i] # 0X) DO INC(i) END;
		RETURN i < LEN(s)
	END Valid;

	PROCEDURE ToUpper* (IN in: ARRAY OF CHAR; OUT out: ARRAY OF CHAR);
		VAR i, max: INTEGER;
	BEGIN i := 0; max := LEN(out)-1;
		WHILE (in[i] # 0X) & (i < max) DO
			IF ORD(in[i]) < 256 THEN out[i] := toUpper[ORD(in[i])] ELSE out[i] := Kernel.Upper(in[i]) END;
			INC(i)
		END;
		out[i] := 0X
	END ToUpper;

	PROCEDURE ToLower* (IN in: ARRAY OF CHAR; OUT out: ARRAY OF CHAR);
		VAR i, max: INTEGER;
	BEGIN i := 0; max := LEN(out)-1;
		WHILE (in[i] # 0X) & (i < max) DO
			IF ORD(in[i]) < 256 THEN out[i] := toLower[ORD(in[i])] ELSE out[i] := Kernel.Lower(in[i]) END;
			INC(i)
		END;
		out[i] := 0X
	END ToLower;

	PROCEDURE Utf8ToString* (IN in: ARRAY OF SHORTCHAR; OUT out: ARRAY OF CHAR; OUT res: INTEGER);
	BEGIN
		Kernel.Utf8ToString(in, out, res)
	END Utf8ToString;

	PROCEDURE StringToUtf8* (IN in: ARRAY OF CHAR; OUT out: ARRAY OF SHORTCHAR; OUT res: INTEGER);
	BEGIN
		Kernel.StringToUtf8(in, out, res)
	END StringToUtf8;

	PROCEDURE Replace* (VAR s: ARRAY OF CHAR; pos, len: INTEGER; IN rep: ARRAY OF CHAR);
		(* replace stretch s[pos]..s[pos+len-1] with rep *)
		(* insert semantics if len = 0; delete semantics if LEN(rep$) = 0 *)
		VAR i, j, k, max, lenS: INTEGER; ch: CHAR;
	BEGIN
		ASSERT(len >= 0, 20); ASSERT(pos >= 0, 21);
		lenS := LEN(s$); max := LEN(s) - 1;
		IF pos <= lenS THEN i := pos; j := 0; ch := rep[0];
			IF pos+len > lenS THEN len := lenS - pos END;
			WHILE (ch # 0X) & (len > 0) DO
				s[i] := ch; INC(i); INC(j); DEC(len); ch := rep[j]
			END;
			IF len > 0 THEN (* delete the remaining part of the stretch [pos, pos+len') *)
				REPEAT ch := s[i+len]; s[i] := ch; INC(i) UNTIL ch = 0X
			ELSIF ch # 0X THEN (* insert the remaining part of rep *)
				len := LEN(rep$) - j; k := lenS + len;
				IF k > max THEN k := max END;
				s[k] := 0X; DEC(k);
				WHILE k - len >= i DO s[k] := s[k-len]; DEC(k) END;
				WHILE (rep[j] # 0X) & (i < max) DO s[i] := rep[j]; INC(i); INC(j) END
			END
		END
	END Replace;

	PROCEDURE Extract* (s: ARRAY OF CHAR; pos, len: INTEGER; OUT res: ARRAY OF CHAR);
		VAR i, j, max: INTEGER;
	BEGIN
		ASSERT(len >= 0, 20); ASSERT(pos >= 0, 21);
		i := 0; j := 0; max := LEN(res) - 1;
		WHILE (i < pos) & (s[i] # 0X) DO INC(i) END;
		WHILE (j < len) & (j < max) & (s[i] # 0X) DO res[j] := s[i]; INC(j); INC(i) END;
		res[j] := 0X
	END Extract;

	PROCEDURE Find* (IN s: ARRAY OF CHAR; IN pat: ARRAY OF CHAR; start: INTEGER; OUT pos: INTEGER);
		VAR j: INTEGER;
	BEGIN
		ASSERT(start >= 0, 20);
		IF (start = 0) OR (start <= LEN(s$) - LEN(pat$)) THEN
			(* start = 0 is optimization: need not call Len *)
			pos := start;
			WHILE s[pos] # 0X DO j := 0;
				WHILE (s[pos+j] = pat[j]) & (pat[j] # 0X) DO INC(j) END;
				IF pat[j] = 0X THEN RETURN END;
				INC(pos)
			END
		END;
		pos := -1	(* pattern not found *)
	END Find;

	PROCEDURE Init;
		VAR i: INTEGER;
	BEGIN
		FOR i := 0 TO 255 DO toUpper[i] :=  CHR(i); toLower[i] := CHR(i) END;
		FOR i := ORD("A") TO ORD("Z") DO toLower[i] := CHR(i + 32); toUpper[i + 32] := CHR(i) END;
		FOR i := ORD("À") TO ORD ("Ö") DO toLower[i] := CHR(i + 32); toUpper[i + 32] := CHR(i) END;
		FOR i := ORD("Ø") TO ORD ("Þ") DO toLower[i] := CHR(i + 32); toUpper[i + 32] := CHR(i) END;
		toUpper[ORD("ÿ")] := "Ÿ";
		digits := "0123456789ABCDEF"; 
		maxExp := SHORT(ENTIER(Math.Log(MAX(REAL)))) + 1;
		maxDig := SHORT(ENTIER(-Math.Log(Math.Eps())));
		factor := Math.IntPower(10, maxDig)
	END Init;

BEGIN
	Init
END bbStrings.
