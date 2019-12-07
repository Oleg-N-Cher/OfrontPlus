MODULE TestTypesCP; IMPORT SYSTEM;
VAR
	b: BYTE; s: SHORTINT; i: INTEGER; l: LONGINT; bool: BOOLEAN;
	p: SYSTEM.PTR; a: SYSTEM.ADRINT; c: SHORTCHAR; set: SET; r: REAL;
	arr1: ARRAY 10 OF SHORTCHAR; arr2: POINTER TO ARRAY OF SHORTCHAR;
	arrOfArr: ARRAY 10, 10, 10 OF BOOLEAN;
BEGIN
	(* SYSTEM.ADR *)
	b := SHORT(SYSTEM.ADR(l));
	s := SYSTEM.ADR(l);
	i := LONG(SYSTEM.ADR(l));
	l := LONG(LONG(SYSTEM.ADR(l)));
	(* SYSTEM.LSH *)
	b := SYSTEM.LSH(b, b);
	s := SYSTEM.LSH(s, s);
	i := SYSTEM.LSH(i, i);
	l := SYSTEM.LSH(l, l);
	(* SYSTEM.ROT *)
	b := SYSTEM.ROT(b, b);
	s := SYSTEM.ROT(s, s);
	i := SYSTEM.ROT(i, i);
	l := SYSTEM.ROT(l, l);
	(* SYSTEM.GET *)
	SYSTEM.GET(s, b); SYSTEM.GET(s, s); SYSTEM.GET(s, i); SYSTEM.GET(s, l);
	SYSTEM.GET(p, b); SYSTEM.GET(p, s); SYSTEM.GET(p, i); SYSTEM.GET(p, l);
	SYSTEM.GET(a, b); SYSTEM.GET(a, s); SYSTEM.GET(a, i); SYSTEM.GET(a, l);
	(* SYSTEM.PUT *)
	SYSTEM.PUT(s, b); SYSTEM.PUT(s, s); SYSTEM.PUT(s, i); SYSTEM.PUT(s, l);
	SYSTEM.PUT(p, b); SYSTEM.PUT(p, s); SYSTEM.PUT(p, i); SYSTEM.PUT(p, l);
	SYSTEM.PUT(a, b); SYSTEM.PUT(a, s); SYSTEM.PUT(a, i); SYSTEM.PUT(a, l);
	SYSTEM.PUT(a, SHORT(0));
	SYSTEM.PUT(a, 0);
	SYSTEM.PUT(a, LONG(0));
	SYSTEM.PUT(a, LONG(LONG(0)));
	(* SYSTEM.BIT *)
	bool := SYSTEM.BIT(a,i);
	(* SYSTEM.VAL *)
	b := SYSTEM.VAL(BYTE, l); s := SYSTEM.VAL(SHORTINT, i);
	i := SYSTEM.VAL(INTEGER, s); l := SYSTEM.VAL(LONGINT, b);
	(* SYSTEM.MOVE *)
	SYSTEM.MOVE(a, a, b);
	(* *)
	b := SHORT(0); s := 0; i := LONG(0); l := LONG(LONG(0));
	(* HALT *)
	HALT(254 + 1); HALT(1-1);
	(* ABS *)
	b := SHORT(ABS(b));	s := ABS(s); i := ABS(i); l := ABS(l);
	(* CAP *)
	c := CAP(c);
	(* ORD *)
	b := SHORT(ORD(c)); s := ORD(c); i := LONG(ORD(c)); l := LONG(ORD(c));
	(* ENTIER *)
	b := SHORT(SHORT(ENTIER(r)));
	(* ODD *)
	bool := ODD(b); bool := ODD(s); bool := ODD(i); bool := ODD(l);
	(* MIN *)
	b := MIN(BYTE); s := MIN(SHORTINT); i := MIN(INTEGER); l := MIN(LONGINT);
	(* MAX *)
	b := MAX(BYTE); s := MAX(SHORTINT); i := MAX(INTEGER); l := MAX(LONGINT);
	(* CHR *)
	c := CHR(b); c := CHR(s); c := CHR(i); c := CHR(l);
	(* INC *)
	INC(b, b); INC(s, s); INC(i, i); INC(l, l);
	(* DEC *)
	DEC(b, b); DEC(s, s); DEC(i, i); DEC(l, l);
	(* LEN *)
	b := LEN(arr1);
	b := SHORT(LEN(arr2));
	b := LEN(arrOfArr, 0); b := LEN(arrOfArr, 1); b := LEN(arrOfArr, 2);
	(* ASH *)
	b := SHORT(SHORT(ASH(b,b))); s := SHORT(ASH(s,s)); i := ASH(i,i);
	l := ASH(l,SHORT(l));
	(* BITS *)
	set := BITS(b); set := BITS(s); set := BITS(i); set := BITS(l);
	(* + - * DIV MOD DIV0 REM0 *)
	b := SHORT(b+b); s := s+s; i := i+i; l := l+l;
	b := SHORT(b-b); s := SHORT(s-s); i := i-i; l := l-l;
	b := SHORT(b*b); s := SHORT(s*s); i := i*i; l := l*l;
	b := SHORT(b DIV b); s := SHORT(s DIV s); i := i DIV i; l := l DIV l;
	b := SHORT(b DIV0 b); s := SHORT(s DIV0 s); i := i DIV0 i; l := l DIV0 l;
	b := SHORT(b MOD b); s := SHORT(s MOD s); i := i MOD i; l := l MOD l;
	b := SHORT(b REM0 b); s := SHORT(s REM0 s); i := i REM0 i; l := l REM0 l;
	b := SHORT(-b); s := -s; i := -i; l := -l;
END TestTypesCP.
