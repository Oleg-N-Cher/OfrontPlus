MODULE WatsonCmd;	(* RC 29.10.93 *)	(* command line version onc 21.6.16 *)

	IMPORT
		Texts, Watson0;

	PROCEDURE ShowDef;
		VAR T: Texts.Text; mod: ARRAY 32 OF CHAR;
	BEGIN
		mod := "Watson0";
		NEW(T); Texts.Open(T, "");
		Watson0.ShowDef(mod, T)
	END ShowDef;

BEGIN (*$MAIN*)
	ShowDef
END WatsonCmd.
