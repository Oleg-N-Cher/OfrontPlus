MODULE OCatCmd;	(* J. Templ, 13-Jan-96 *)

	(* looks at the OBERON search path and writes one or more Oberon or ascii texts to standard out.
	Horizontal tabulator characters are converted to a pair of spaces by default.
	CR is converted to LF.

	SYNOPSIS: ocat [-t] files...

	-t   no tabulator conversion

	*)

	IMPORT Args, Console, Files, Texts;

	CONST TAB = 09X; CR = 0DX; LF = 0AX;

	PROCEDURE Cat*;
		VAR optTab: BOOLEAN; path: ARRAY 128 OF CHAR; arg: INTEGER;
			T: Texts.Text; R: Texts.Reader; ch: CHAR;
	BEGIN
		optTab := FALSE; path := ""; arg := 1; NEW(T);
		Args.Get(1, path);
		IF path = "-t" THEN optTab := TRUE; arg := 2; path := ""; Args.Get(2, path) END ;
		WHILE path # "" DO
			IF Files.Old(path) # NIL THEN
				Texts.Open(T, path);
				Texts.OpenReader(R, T, 0); Texts.Read(R, ch);
				WHILE ~R.eot DO
					IF ch >= " " THEN Console.Char(ch)
					ELSIF ch = TAB THEN
						IF optTab THEN Console.Char(TAB) ELSE Console.String("  ") END
					ELSIF ch = CR THEN Console.Char(LF)
					END ;
					Texts.Read(R, ch)
				END
			ELSE
				Console.String("OCat: cannot open "); Console.String(path); Console.Ln
			END ;
			INC(arg); path := "";
			Args.Get(arg, path)
		END ;
		Console.Flush
	END Cat;

BEGIN Cat
END OCatCmd.
