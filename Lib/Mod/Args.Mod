MODULE Args;	(* jt, 8.12.94 *)

	(* command line argument handling for ofront *)


	IMPORT SYSTEM, Platform;

	VAR argc-: INTEGER; argv-: Platform.ArgVec;

	PROCEDURE -AAExternArgCount    "extern INTEGER SYSTEM_ArgCount;";
	PROCEDURE -AAExternArgVector   "extern void *SYSTEM_ArgVector;";
	PROCEDURE -ArgCount(): INTEGER "SYSTEM_ArgCount";
	PROCEDURE -ArgVector(): Platform.ArgVec "(Platform_ArgVec)SYSTEM_ArgVector";

	PROCEDURE Get*(n: INTEGER; VAR val: ARRAY OF CHAR);
	BEGIN
		Platform.GetArg(n, val)
	END Get;

	PROCEDURE GetInt*(n: INTEGER; VAR val: INTEGER);
	BEGIN
		Platform.GetIntArg(n, val)
	END GetInt;

	PROCEDURE Pos*(s: ARRAY [1] OF CHAR): INTEGER;
	BEGIN
		RETURN Platform.ArgPos(s)
	END Pos;

	PROCEDURE GetEnv*(var: ARRAY [1] OF CHAR; VAR val: ARRAY OF CHAR);
	BEGIN
		Platform.GetEnv(var, val)
	END GetEnv;

BEGIN argc := ArgCount(); argv := ArgVector()
END Args.
