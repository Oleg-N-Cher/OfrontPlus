MODULE arArgs;	(* jt, 8.12.94 *)

	IMPORT SYSTEM, Platform;
	
	TYPE
		ArgPtr* = POINTER [notag] TO ARRAY 1024 OF CHAR;
		ArgVec* = POINTER [notag] TO ARRAY 1024 OF ArgPtr;

	VAR argc-: LONGINT; argv-: ArgVec;

	PROCEDURE -Argc(): INTEGER "SYSTEM_argc";
	PROCEDURE -Argv(): ArgVec "(arArgs_ArgVec)SYSTEM_argv";
	PROCEDURE getmainargs* ["__getmainargs"] (VAR argc: LONGINT; VAR argv: ArgVec;
		VAR env: SYSTEM.PTR; wildcard: LONGINT; VAR startupinfo: SYSTEM.PTR): LONGINT;
	PROCEDURE -getMainArgs
		"void *tmp; __getmainargs(&arArgs_argc, &arArgs_argv, &tmp, 0, &tmp)";

BEGIN
	IF Platform.Windows THEN getMainArgs
	ELSE argc := Argc(); argv := Argv()
	END
END arArgs.
