(*$MAIN*) MODULE OCatCmd;	(* J. Templ, 13-Jan-96 *)

(* looks at the OBERON search path and writes one or more Oberon or ascii texts to standard out *)

	IMPORT Args, Console, Files, Texts;

	PROCEDURE Cat*;
		VAR tab: BOOLEAN; path: ARRAY 128 OF CHAR; arg: INTEGER; T: Texts.Text; R: Texts.Reader; ch: CHAR;

	BEGIN
		path := ""; tab := FALSE; NEW(T);
		arg := 1; Args.Get(1, path);
		IF path = "-t" THEN tab := TRUE; arg := 2; Args.Get(2, path) END ;
		WHILE path # "" DO
			IF Files.Old(path) # NIL THEN 
				Texts.Open(T, path);
				Texts.OpenReader(R, T, 0); Texts.Read(R, ch);
				WHILE ~R.eot DO
					IF ch >= " " THEN Console.Char(ch)
					ELSIF ch = 09X THEN
						IF tab THEN Console.Char(ch) ELSE Console.Char(" "); Console.Char(" ") END
					ELSIF ch = 0DX THEN Console.Char(0AX)
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



ocat [-t] files...

-t   no tab conversion
