MODULE TestLEN;
IMPORT Console;

TYPE
	StrngP = POINTER TO ARRAY OF SHORTCHAR;
VAR
	g, i:INTEGER; s: SET; strng: StrngP; st :ARRAY 15 OF SHORTCHAR;

BEGIN
	NEW (strng, 15);
	g := LEN(strng);
	Console.Int (g, 6); Console.Ln;

	g := LEN(st);
	Console.Int (g, 6); Console.Ln;
	st := 'ABCDEFGH';
	g := LEN(st$);
	Console.Int (g, 6); Console.Ln;

	strng^ := "ABCDEF";
	Console.Int (LEN(strng^$), 6); Console.Ln;

	strng := NIL
		
END TestLEN.
