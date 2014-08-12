MODULE Console;	(* J. Templ, 29-June-96 *)

	(* output to Unix/Windows standard output device based printf *)

	IMPORT SYSTEM;

	PROCEDURE -includestdio()	"#include <stdio.h>";
	PROCEDURE -fflushstdout()	"fflush(stdout)";
	PROCEDURE -writeCh(ch: CHAR)	'printf("%c", ch)';
	PROCEDURE -writeStr(str: ARRAY OF CHAR)	'printf("%s", str)';
	PROCEDURE -writeLn()	'printf("\n")';

	PROCEDURE -read(VAR ch: CHAR): LONGINT
		"read(0/*stdin*/, ch, 1)";

	PROCEDURE Flush*();
	BEGIN fflushstdout()
	END Flush;

	PROCEDURE Char*(ch: CHAR);
	BEGIN writeCh(ch); IF ch = 0AX THEN fflushstdout() END 
	END Char;

	PROCEDURE String*(s: ARRAY[1] OF CHAR);
	(** UNSAFE! Make sure that value of array 'str' is null-terminated. *)
	BEGIN writeStr(s)
	END String;

	PROCEDURE Int*(i, n: LONGINT);
		VAR s: ARRAY 32 OF CHAR; i1, k: LONGINT;
	BEGIN
		IF i = SYSTEM.LSH(LONG(LONG(1)), SIZE(LONGINT)*8 - 1) THEN
			IF SIZE(LONGINT) = 8 THEN s := "8085774586302733229"; k := 19
			ELSE s := "8463847412"; k := 10
			END
		ELSE
			i1 := ABS(i);
			s[0] := CHR(i1 MOD 10 + ORD("0")); i1 := i1 DIV 10; k := 1;
			WHILE i1 > 0 DO s[k] := CHR(i1 MOD 10 + ORD("0")); i1 := i1 DIV 10; INC(k) END
		END ;
		IF i < 0 THEN s[k] := "-"; INC(k) END ;
		WHILE n > k DO Char(" "); DEC(n) END ;
		WHILE k > 0 DO	DEC(k); Char(s[k]) END
	END Int;

	PROCEDURE Ln*;
	BEGIN writeLn; fflushstdout()
	END Ln;

	PROCEDURE Bool*(b: BOOLEAN);
	BEGIN IF b THEN String("TRUE") ELSE String("FALSE") END
	END Bool;

	PROCEDURE Hex*(i: LONGINT);
		VAR k, n: LONGINT;
	BEGIN
		k := -28;
		WHILE k <= 0 DO
			n := ASH(i, k) MOD 16;
			IF n <= 9 THEN Char(CHR(ORD("0") + n)) ELSE Char(CHR(ORD("A") - 10 + n)) END ;
			INC(k, 4)
		END
	END Hex;

	PROCEDURE Read*(VAR ch: CHAR);
		VAR n: LONGINT;
	BEGIN Flush();
		n := read(ch);
		IF n # 1 THEN ch := 0X END
	END Read;

	PROCEDURE ReadLine*(VAR line: ARRAY OF CHAR);
		VAR i: LONGINT; ch: CHAR;
	BEGIN Flush();
		i := 0; Read(ch); 
		WHILE (i < LEN(line) - 1) & (ch # 0AX) & (ch # 0X) DO line[i] := ch; INC(i); Read(ch) END ;
		line[i] := 0X
	END ReadLine;

(*PROCEDURE- _init* "/*-noinit*/";*)

END Console.
